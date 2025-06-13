#include "server.h"

Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      io_context_(io_context) {
    do_accept();
}

/**
 * 適合遊戲伺服器的做法是：
 * 主線程使用 io_context.run() 運行事件迴圈。
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
void Server::do_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<Connection>(std::move(*socket))->start();
        }
        do_accept();
    });
}
