/**
 * TCP 預設開啟 Nagle’s algorithm，會合併小封包，導致「延遲但節省頻寬」的行為。
 * 當你重新編譯產生新的 client，因為初始快取尚未建好，或連線重置，導致發送快。
 * 但後續 reuse socket 資源或 OS 快取時，反而會開啟延遲演算法。
 * 
 * socket.set_option(boost::asio::ip::tcp::no_delay(true));  // 關閉 Nagle
 */
#include "server/server.h"
#include "conn/connection.h"
#include "server/http_session.h"
#include "server/websocket_session.h"

#include <iostream>
#include <thread>
#include <filesystem>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;
using tcp = net::ip::tcp;

/**
 * | 選項                    | 說明                                                          |
 * | -----------------------| --------------------------------------------------------------|
 * | `default_workarounds`  | 啟用一些歷史相容性 workaround（OpenSSL 官方建議設定）            |
 * | `no_sslv2`             | 明確禁止 SSLv2（這個版本已被破解）                               |
 * | `single_dh_use`        | 每次握手都用不同的 Diffie-Hellman 參數，提升前向保密性（PFS）     |
 */
void setup_ssl_context(ssl::context& ctx, const std::string& cert_path, const std::string& key_path) {
    if (!std::filesystem::exists(cert_path) || !std::filesystem::exists(key_path)) {
        // std::ostringstream oss;
        // oss << "憑證或私鑰檔案不存在: cert_path: " << cert_path << ", key_path: " << key_path;
        // throw std::runtime_error(oss.str());
        std::cerr << "警告：SSL 憑證或私鑰不存在，跳過 SSL 設定" << '\n';
        return;
    }

    ctx.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::single_dh_use);

    ctx.use_certificate_chain_file(cert_path);
    ctx.use_private_key_file(key_path, ssl::context::pem);
    std::cout << "SSL 憑證載入成功" << '\n';
}

Server::Server(boost::asio::io_context& ioc, unsigned short port)
    : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
      ioc_(ioc) {
    std::cout << "TCP server is listening on port：" << acceptor_.local_endpoint().port() << '\n';
    do_accept();
}

/**
 * 適合遊戲伺服器的做法是：
 * 主線程使用 ioc.run() 運行事件迴圈。
 * 每次有新連線時，用 async_accept() 非同步處理。
 * 新的 socket 交給邏輯層去驗證、廣播、加房間、傳資料等。
 * 
 * 優點：
 * 不會阻塞主線程：適合大型伺服器，效率高。
 * 能利用 boost::asio::io_context 內建的事件迴圈處理大量連線。
 * 更容易整合遊戲邏輯與資料同步（如 timer、狀態廣播、聊天室）。
 * 
 * 2. 非阻塞式：server.h / Server::do_accept() + Connection::start()
 * 使用 boost::asio::async_accept。
 * 每個 client 透過 Connection 類別處理。
 * 高效、可擴充、可併發。
 */
// 非同步事件監聽 async_accept
void Server::do_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioc_);
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<Connection>(std::move(*socket))->start();
        }
        do_accept();
    });
}

// HttpServer::HttpServer(net::io_context& ioc, unsigned short port,
//               std::function<void(tcp::socket)> session_handler)
//         : ioc_(ioc),
//           acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
//           session_handler_(std::move(session_handler)) {}

HttpServer::HttpServer(net::io_context& ioc, unsigned short port)
        : ioc_(ioc),
          acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {}

void HttpServer::run() {
    std::cout << "Http Server is listening on port：" << acceptor_.local_endpoint().port() << '\n';
    do_accept();
    // 同步阻塞 sync accept
    // std::cout << "Server is listening on port：" << acceptor_.local_endpoint().port() << std::endl;
    // for (;;) {
    //     tcp::socket socket{ioc_};
    //     acceptor_.accept(socket);

    //     // 用 thread 執行 session_handler，並 detach
    //     std::thread(session_handler_, std::move(socket)).detach();
    // }
}

// 非同步事件監聽 async_accept
void HttpServer::do_accept() {
    acceptor_.async_accept(
        [self = shared_from_this()](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                // 非同步 accept 完，啟動 session 處理，完全自己控制後續處理
                // self->session_handler_(std::move(socket));

                // Boost::beast 常用寫法，HttpSession 這種 session 類負責每一條連接的數據傳輸，做到程式結構更加清晰、符合異步模式
                std::make_shared<HttpSession>(std::move(socket))->start();
            } else {
                std::cerr << "Accept error: " << ec.message() << '\n';
            }
            // 繼續 accept 下一個連線
            self->do_accept();
        });
}

WebSocketServer::WebSocketServer(net::io_context& ioc, ssl::context& ctx, unsigned short port)
        : ioc_(ioc), ctx_(ctx), acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {}

void WebSocketServer::run() {
    do_accept();
}

void WebSocketServer::do_accept() {
    acceptor_.async_accept(
        [self = shared_from_this()](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<WebSocketSession>(std::move(socket), self->ctx_)->run();
            } else {
                std::cerr << "WebSocket Accept error: " << ec.message() << '\n';
            }

            self->do_accept();  // 繼續接受下一個連線
        });
}
