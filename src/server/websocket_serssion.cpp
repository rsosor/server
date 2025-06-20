/**
 * | 名稱    | 用途說明                                             | 發生時機                                          |
 * | ------ | ---------------------------------------------------- | ------------------------------------------------ |
 * | `ec`   | 非同步操作的主錯誤碼（如 `async_read`, `async_write`）  | lambda 傳入的錯誤結果                             |
 * | `ec2`  | 嘗試關閉 TCP socket 時的錯誤碼                         | `shutdown()` 或 `close()` 返回                   |
 * | `ec3`  | SSL 的 `async_shutdown()` 錯誤碼                      | SSL/TLS 關閉握手時的錯誤（如 WebSocket over TLS）  |
 */
#include "server/websocket_session.h"

#include <iostream>
#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

WebSocketSession::WebSocketSession(tcp::socket socket, ssl::context& ctx)
    : ws_(std::move(socket), ctx) {}

void WebSocketSession::run() {
    // 開始 SSL 握手
    ws_.next_layer().async_handshake(ssl::stream_base::server,
        [self = shared_from_this()](beast::error_code ec) {
            if (ec) return self->fail(ec, "ssl_handshake");
            self->do_accept();
        });
}

void WebSocketSession::do_accept() {
    ws_.async_accept([self = shared_from_this()](beast::error_code ec) {
        if (ec) return self->fail(ec, "ws_accept");
        self->do_read();
    });
}

void WebSocketSession::do_read() {
    ws_.async_read(buffer_, [self = shared_from_this()](beast::error_code ec, std::size_t) {
        if (ec) {
            // WebSocket 正常關閉，不應呼叫 fail
            if (ec == websocket::error::closed) {
                std::cout << "WebSocket closed normally" << '\n';
                return;
            }
            self->fail(ec, "read");
            return;
        }

        std::string msg = beast::buffers_to_string(self->buffer_.data());
        std::cout << "Received: " << msg << '\n';

        // self->ws_.text(true);
        self->ws_.text(self->ws_.got_text());
        self->ws_.async_write(
            net::buffer("Echo: " + msg),
            [self](beast::error_code ec, std::size_t) {
                if (ec) {
                    self->fail(ec, "write");
                    return;
                }
                self->buffer_.consume(self->buffer_.size());
                self->do_read();
            }
        );
    });
}

// void WebSocketSession::fail(beast::error_code ec, const char* what) {
//     std::cerr << what << ": " << ec.message() << "\n";

//     // 嘗試關閉連線
//     beast::error_code ec2;
//     ws_.next_layer().next_layer().shutdown(tcp::socket::shutdown_both, ec2);
//     ws_.next_layer().next_layer().close(ec2);
// }

// TLS version
void WebSocketSession::fail(beast::error_code ec, const char* what) {
    if (ec == net::error::operation_aborted)
        return; // 非異常，操作被取消

    std::cerr << what << ": " << ec.message() << "\n";

    // 避免多次關閉 socket（可能需要加個 "is_closed" 標誌）

    // 非同步 TLS shutdown
    ws_.next_layer().async_shutdown (
        [self = shared_from_this()](beast::error_code ec2) {
            if (ec2 && ec2 != boost::asio::error::eof)
                std::cerr << "TLS shutdown error: " << ec2.message() << '\n';
            beast::error_code ec3;
            self->ws_.next_layer().next_layer().shutdown(tcp::socket::shutdown_both, ec3);
            self->ws_.next_layer().next_layer().close(ec3);
        }
    );
}