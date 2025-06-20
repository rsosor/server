/**
 * ✅ boost::asio::io_context: I/O 核心，負責處理異步事件與處理 handlers。
 * ✅ boost::asio::ip::tcp::socket: TCP socket，透由它與 server 連接。
 * ✅ boost::asio::ip::tcp::resolver: 解析 "hostname" + "port" 為 endpoint。
 * ✅ boost::asio::connect: 依序嘗試 connect 每一個解析出的 endpoint。
 * ✅ boost::asio::write: 向 socket 中寫入資料。
 * ✅ socket.read_some: 由 socket 中讀取最多指定量的数据。
 */
#include "login/login.h"

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = net::ip::tcp;

// Client::Client(boost::asio::io_context& io, const std::string& host, unsigned short port) 
//         : socket_(io) {
//     boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
//     socket_.connect(endpoint);
// }

// void Client::send_message(const std::string& message) {
//     boost::asio::write(socket_, boost::asio::buffer(message)); 
// }

// std::string Client::receive_response() {
//     std::array<char, 1024> buffer;
//     size_t len = socket_.read_some(boost::asio::buffer(buffer)); 
//     return std::string(buffer.data(), len);
// }

Client::Client(const std::string& host, const std::string& port)
    : ioc_(), socket_(ioc_), host_(host), port_(port) {}

Client::Client() : ioc_(), socket_(ioc_) {}

bool Client::login(const std::string& host, const std::string& port, 
        const std::string& username, const std::string& password) {
    return post_login(host, port, username, password);
}

// http post login：沒有使用到 Client 的 ioc_、socket_、connect()、send()、receive()
bool Client::post_login(const std::string& host, const std::string& port, 
               const std::string& username, const std::string& password) {
    try {
        // Prepare context, resolver, socket
        net::io_context ioc;
        tcp::resolver resolver{ioc};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Establish a connection
        tcp::socket socket{ioc};

        net::connect(socket, results.begin(), results.end());

        // Prepare the HTTP request
        std::string body = "{\"username\": \"" + username + "\", \"password\": \"" + password + "\"}";
        std::cout << "Body length = " << body.size() << '\n';
        std::cout << "Body = " << body << '\n';

        http::request<http::string_body> req{http::verb::post, "/login", 11};

        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");

        req.body() = body;
        req.prepare_payload();

        // Send the request
        http::write(socket, req);

        std::cout << "BIO Request:\n" << req << '\n';

        // Buffer for the response
        beast::flat_buffer buffer;

        // Container for the response
        http::response<http::string_body> res;

        // Receive the response
        http::read(socket, buffer, res);

        std::cout << "BIO Response:\n" << res << '\n';

        return res.result() == http::status::ok;
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return false;
    }
}

bool Client::connect(const std::string& host, const std::string& port) {
    try {
        boost::asio::ip::tcp::resolver resolver(ioc_);
        auto endpoints = resolver.resolve(host, port);
        boost::asio::connect(socket_, endpoints);
        std::cout << "Client connected to " << host << ":" << port << '\n';
        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Error during connect: " << e.what() << '\n';
        return false;
    }
}

bool Client::send(const std::string& message) {
    try {
        boost::asio::write(socket_, boost::asio::buffer(message)); 
        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Error during send: " << e.what() << '\n';
        return false;
    }
}

std::string Client::receive(size_t max_length) {
    try {
        char data[1024] = {0};

        size_t length = socket_.read_some(boost::asio::buffer(data, max_length));

        return std::string(data, length);
    }
    catch (std::exception& e) {
        std::cerr << "Error during receive: " << e.what() << '\n';
        return "";
    }
}

AsyncClient::AsyncClient(net::io_context& ioc, const std::string host, 
                        const std::string port, std::string body)
    : resolver_(ioc), socket_(ioc), host_(std::move(host)), port_(std::move(port)), body_(std::move(body)) {}

void AsyncClient::async_login() {
    connect();
}

void AsyncClient::connect() {
    resolver_.async_resolve(host_, port_, 
        [self = shared_from_this()](boost::system::error_code ec, tcp::resolver::results_type results) {
            if (ec) {
                std::cerr << "Resolve failed: " << ec.message() << '\n';
                return;
            }
            
            net::async_connect(self->socket_, results,
                                [self](boost::system::error_code ec, tcp::endpoint) {
                                    if (ec) {
                                        std::cerr << "Connect failed: " << ec.message() << '\n';
                                        return;
                                    }
                                    self->send_request();
                                });
        });
}

void AsyncClient::send_request() {
    // Prepare the request
    req_.version(11);
    req_.method(http::verb::post);
    req_.target("/login");

    req_.set(http::field::host, host_);
    req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req_.set(http::field::content_type, "application/json");

    req_.body() = body_;
    req_.prepare_payload();

    // Send the request
    http::async_write(socket_, req_, 
        [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
            if (ec) {
                std::cerr << "Write failed: " << ec.message() << '\n';
                return;
            }
            std::cout << "AIO Request:\n" << self->req_ << '\n';
            self->read_response();
        });
}

void AsyncClient::read_response() {
    http::async_read(socket_, buffer_, res_, 
        [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
            if (ec) {
                std::cerr << "Read failed: " << ec.message() << '\n';
                return;
            }
            std::cout << "AIO Response:\n" << self->res_ << '\n';
        });
}
