#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

void setup_ssl_context(ssl::context& ctx, const std::string& cert_path, const std::string& key_path);

class Server {
public:
    Server(boost::asio::io_context& ioc, unsigned short port);

private:
    void do_accept();

    tcp::acceptor acceptor_;
    net::io_context& ioc_;
};

class HttpServer : public std::enable_shared_from_this<HttpServer> {
public:
    // HttpServer(net::io_context& ioc, unsigned short port, std::function<void(tcp::socket)> session_handler);
    HttpServer(net::io_context& ioc, unsigned short port);
    void run();
private:
    void do_accept();
    // void handle_session(tcp::socket socket);
    // void handle_request(http::request<http::string_body> req, tcp::socket& socket);
    // void write(http::response<http::string_body> res);

    tcp::acceptor acceptor_;
    net::io_context& ioc_;
    // 
    std::function<void(tcp::socket)> session_handler_;
};

class WebSocketServer : public std::enable_shared_from_this<WebSocketServer> {
public:
    WebSocketServer(net::io_context& ioc, ssl::context& ctx, unsigned short port);

    void run();
private:
    void do_accept();

    tcp::acceptor acceptor_;
    net::io_context& ioc_;
    ssl::context& ctx_;
    // void run_server(unsigned short port, const std::string& cert_file, const std::string& key_file);
};
