#include <iostream>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace net = boost::asio;
namespace ssl = net::ssl;
namespace websocket = beast::websocket;
using tcp = net::ip::tcp;

void run_tls_websocket_client(const std::string& host, const std::string& port, const std::string& text) {
    try {
        net::io_context ioc;
        ssl::context ctx(ssl::context::tlsv12_client);

        // 可加載 CA 憑證
        ctx.set_default_verify_paths();

        // 建立 socket 與 resolver
        tcp::resolver resolver(ioc);
        websocket::stream<ssl::stream<tcp::socket>> ws(ioc, ctx);

        // DNS 解析
        auto const results = resolver.resolve(host, port);

        // TCP 連線
        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());

        // TLS 握手
        ws.next_layer().handshake(ssl::stream_base::client);

        // WebSocket 握手
        ws.handshake(host, "/");

        // 傳送訊息
        ws.write(net::buffer(text));

        // 讀取回應
        beast::flat_buffer buffer;
        ws.read(buffer);

        // 讀取 buffer
        std::cout << "Received: " << beast::make_printable(buffer.data()) << '\n';

        // 正常關閉 WebSocket
        ws.close(websocket::close_code::normal);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}
