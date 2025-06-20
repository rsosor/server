#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

// class Client {
// public:
//     Client(boost::asio::io_context& io, const std::string& host, unsigned short port);
//     void send_message(const std::string& message);
//     std::string receive_response();
// private:
//     boost::asio::ip::tcp::socket socket_;
// };

class Client {
public:
    Client(const std::string& host, const std::string& port);

    Client();

    bool login(const std::string& host, const std::string& port, 
        const std::string& username, const std::string& password);
private:
    bool post_login(const std::string& host, const std::string& port, 
        const std::string& username, const std::string& password);

    bool connect(const std::string& host, const std::string& port);

    bool send(const std::string& message);

    std::string receive(size_t max_length = 1024);

    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::socket socket_;
    std::string host_;
    std::string port_;
};

class AsyncClient : public std::enable_shared_from_this<AsyncClient> {
public:
    AsyncClient(net::io_context& ioc, const std::string host, 
                const std::string port, std::string body);

    void async_login();
private:
    void connect();

    void send_request();

    void read_response();
private:
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string host_, port_, body_;
    http::request<http::string_body> req_;
    beast::flat_buffer buffer_;
    http::response<http::string_body> res_;
};
