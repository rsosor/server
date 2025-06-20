#include <iostream>
#include <string>

// #include <boost/beast/core.hpp>
// #include <boost/beast/websocket.hpp>
// #include <boost/beast/websocket/ssl.hpp>
// #include <boost/asio/ssl.hpp>
// #include <boost/asio/connect.hpp>
// #include <boost/asio/ip/tcp.hpp>

// namespace beast = boost::beast;
// namespace net = boost::asio;
// namespace ssl = net::ssl;
// namespace websocket = beast::websocket;
// using tcp = net::ip::tcp;

void run_tls_websocket_client(const std::string& host, const std::string& port, const std::string& text);