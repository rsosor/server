#pragma once

#define CXX17_OPEN_VARIANT

#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

#ifdef CXX17_OPEN_VARIANT
#include <variant>

using PlainSocket = tcp::socket;
using SslSocket = ssl::stream<tcp::socket>;
using StreamVariant = std::variant<PlainSocket, SslSocket>;
#endif

#ifndef CXX17_OPEN_VARIANT

struct HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    explicit HttpSession(tcp::socket socket, ssl::context& ctx);
    // explicit HttpSession(tcp::socket socket);

    void start();

private:
    void do_read();
    void handle_request();
    void do_write(http::response<http::string_body> res);

    /**
     * 方案 1. 只保留 stream_，不保留 socket_
     * 建議保留一個 socket，使用 ssl 就不要使用 socket
     * 
     * 方案 2. 使用 std::variant
     * 如果你要支援 HTTP（純 TCP）與 HTTPS（SSL 包裝），則建議如下：
     * std::variant<tcp::socket, ssl::stream<tcp::socket>> stream_;
     * 
     * 並在 constructor 中只初始化其中一種：
     * HttpSession::HttpSession(tcp::socket socket)
     * : stream_(std::move(socket)) {}
     * 
     * HttpSession::HttpSession(tcp::socket socket, ssl::context& ctx)
     * : stream_(ssl::stream<tcp::socket>(std::move(socket), ctx)) {}
     * 
     * 方案 3. 使用 std::optional
     * 如果你只想在 run-time 選擇 HTTP or HTTPS 但不要 std::variant：
     * std::optional<tcp::socket> socket_;
     * std::optional<ssl::stream<tcp::socket>> stream_;
     * - 只初始化其中一個即可
     */
    // tcp::socket socket_;
    // error C2512: 'boost::asio::ssl::stream<boost::asio::ip::tcp::socket>': 
    // 沒有適當的預設建構函式 [D:\rsosor\cpp\game\build\game.vcxproj]
    // ssl::stream<tcp::socket> stream_;
    std::variant<tcp::socket, ssl::stream<tcp::socket>> stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
};

#else

/**
 * C++17 variant version
 */
class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    // constructor for plain
    explicit HttpSession(PlainSocket&& socket);

    // constructor for ssl
    HttpSession(SslSocket&& stream);

    void start();
private:
    void do_read();
    void handle_request();
    void do_write(http::response<http::string_body> res);

    StreamVariant stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
};

#endif