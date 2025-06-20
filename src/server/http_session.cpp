#include "server/http_session.h"

#include <iostream>
#include <memory>

#ifndef CXX17_OPEN_VARIANT

// 每一條連線由一個 session 處理
HttpSession::HttpSession(tcp::socket socket, ssl::context& ctx) : stream_(std::move(socket), ctx) {}
// HttpSession::HttpSession(tcp::socket socket) : socket_(std::move(socket)) {}

void HttpSession::start() {
    do_read();
}

// async read
void HttpSession::do_read() {
    auto self = shared_from_this();

    http::async_read(socket_, buffer_, req_,
        [self](beast::error_code ec, std::size_t) {
            if (ec) {
                std::cerr << "Read error: " << ec.message() << '\n';
                return;
            }
            self->handle_request();
        }
    );
}

void HttpSession::handle_request() {
    if (req_.method() == http::verb::post && req_.target() == "/login") {
        // Here we simply parse the body manually
        std::string body = req_.body();

        // Naive parsing (for demonstration only!)
        if (body == R"({"username": "Alice", "password": "mypassword"})") {
            std::cout << body << '\n';
            http::response<http::string_body> res{http::status::ok, req_.version()};
            res.set(http::field::server, "Boost Beast Server");
            res.set(http::field::content_type, "application/json");

            res.body() = "Login successful.";
            res.prepare_payload();

            // synchronous
            // http::write(socket_, res);
            do_write(res);
            return;
        } else {
            http::response<http::string_body> res{http::status::unauthorized, req_.version()};
            res.set(http::field::server, "Boost Beast Server");

            res.body() = "Invalid credentials.";
            res.prepare_payload();

            // synchronous
            // http::write(socket_, res);
            do_write(res);
            return;
        }
    } else {
        http::response<http::string_body> res{http::status::not_found, req_.version()};
        res.set(http::field::server, "Boost Beast Server");

        res.body() = "Not found.";
        res.prepare_payload();

        // synchronous
        // http::write(socket_, res);
        do_write(res);
    }
}
  
// async write
void HttpSession::do_write(http::response<http::string_body> res) {
    auto self = shared_from_this();

    http::async_write(
        socket_,
        res,
        [self](beast::error_code ec, std::size_t) {
            if (ec) {
                std::cerr << "Write error: " << ec.message() << '\n';
            }
            
            // 儲點：我們只處理一個請求，然後關掉 socket
            self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        }
    );
}

#else

// 非同步 read、write 搭配 TLS
HttpSession::HttpSession(PlainSocket&& socket) 
    : stream_(std::move(socket)) {}

HttpSession::HttpSession(SslSocket&& stream) 
    : stream_(std::move(stream)) {}

void HttpSession::start() {
    do_read();
}

void HttpSession::do_read() {
    auto self = shared_from_this();

    std::visit([this, self](auto& s) {
        http::async_read(s, buffer_, req_, 
            [self](beast::error_code ec, std::size_t) {
                std::cout << "HttpSession::do_read(): ec: " << ec << '\n';
                if (ec) {
                    std::cerr << "HttpSession::do_read(): Read error: " << ec.message() << '\n';
                    return;
                }
                self->handle_request();
            });
    }, stream_);
}

void HttpSession::handle_request() {
    http::response<http::string_body> res {
        http::status::ok, req_.version()
    };
    res.set(http::field::server, "Boost Beast Server");
    res.set(http::field::content_type, "application/json");
    res.body() = "Login successful.";
    res.prepare_payload();

    do_write(std::move(res));
}

void HttpSession::do_write(http::response<http::string_body> res) {
    auto self = shared_from_this();

    std::visit([self, res = std::move(res)](auto& s) mutable {
        http::async_write(s, res, 
            [self, &s](beast::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "Write error: " << ec.message() << '\n';
                    return;
                }

                std::cout << "request keep_alive: " << self->req_.keep_alive() << '\n';
                // 根據是否 Keep-Alive 決定是否繼續
                if (self->req_.keep_alive()) {
                    self->do_read();    // 繼續處理下一個請求
                } else {
                    // 關閉 socket
                    // 平台安全地關閉 socket
                    if constexpr (std::is_same_v<std::decay_t<decltype(s)>, tcp::socket>) {
                        beast::error_code ec2;
                        s.shutdown(tcp::socket::shutdown_both, ec2);
                        s.close(ec2);
                    } else {
                        s.async_shutdown([&s](beast::error_code ec3) {
                            if (ec3) std::cerr << "SSL shutdown error: " << ec3.message() << '\n';
                            
                            beast::error_code ec2;
                            // 假設 s 是 ssl::stream<tcp::socket>
                            s.next_layer().shutdown(tcp::socket::shutdown_both, ec2);
                            s.next_layer().close(ec2);
                        });
                    }
                }
            });
    }, stream_);
}

#endif