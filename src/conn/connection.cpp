/**
 * 🧪 若你想知道對方是正常 FIN 還是異常 RST 斷開：
 * 方式 1️⃣：看 recv() 結果
 * 對方行為	            你這邊的 recv() 回傳	WSAGetLastError()
 * 傳送 FIN	            0	                   無錯誤
 * TCP RST（強制關閉）	-1	                    10054（WSAECONNRESET）
 * 
 * 方式 2️⃣：用 Wireshark 觀察封包
 * 若你要進一步觀察 TCP 細節（例如看到 SYN/ACK/FIN/RST），你可以使用 Wireshark 抓封包：
 * - 看你程式的 recv() 時 TCP 封包是不是 FIN 還是 RST
 * - 可視化誰先關閉連線、是否正常四次揮手（FIN-ACK-FIN-ACK）
 */
#include "conn/connection.h"
#include "util/d2d.h"

using namespace rsosor::game::player;
using boost::asio::ip::tcp;
/**
 * Connection::start()
 * └── read_name()             // async_read_some
 *     └── send_welcome()     // async_write
 *         └── do_read()      // async_read_until
 *             └── process_command(line)
 *                 ├── 錯誤 -> async_write -> socket.close()
 *                 └── 成功 -> 回應 + 加入等待區
 */
// extern int next_client_id;
// extern std::unordered_map<int, Player> player_registry;
// extern std::vector<Player> waiting_room;
// extern std::mutex waiting_room_mutex;
// extern const std::string hardcoded_token;

int next_client_id = 1;
std::unordered_map<int, Player> player_registry;

const std::string hardcoded_token = "letmein";

// 等待區容器與鎖
std::vector<Player> waiting_room;
std::mutex waiting_room_mutex;

Connection::Connection(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {
    std::cout << "New player connected.\n";
}

std::string Connection::get_time_string(std::time_t t) {
    char buf[32];
    std::strftime(buf, sizeof(buf), "%F, %T", std::localtime(&t));
    return std::string(buf);
}

void Connection::start() {
    read_name();
}

void Connection::read_name() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, 1024),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                if (length == 0) {
                    std::cout << "Client sent FIN (connection closed by peer)\n";
                    // 可以選擇關閉 socket
                } else {
                    player_name_ = std::string(data_, length);
                    std::cout << "Player name: " << player_name_ << "\n";
                    send_welcome();
                }
            } else {
                std::cout << "Read error: " << ec.message() << "\n";
            }
        });
}

void Connection::send_welcome() {
    auto self(shared_from_this());
    std::string message = "Welcome, " + player_name_ + '\n';
    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                std::cout << "send_welcome async_write success" << '\n';
                do_read();
            } else {
                std::cout << "Write error: " << ec.message() << "\n";
            }
        });
    std::cout << "send_welcome 處理中" << '\n';

    int n = 0;
    while (n < 5) {
        Sleep(1000);
        std::cout << ++n << '\n';
    }
}

void Connection::do_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);
                // 認證、加入等待區
                process_command(line);  // 處理輸入
                do_read();              // 持續等待接收指令
            } else {
                std::cout << "Disconnected: " << ansi_to_utf8(ec.message()) << '\n';
            }
        });
}

bool Connection::process_command(const std::string& line) {
    std::istringstream iss(line);
    std::string command, name, token;
    iss >> command >> name >> token;

    if (command != "LOGIN" || token != hardcoded_token) {
        std::string msg = "AUTH FAILED\n";
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(msg),
            [this, self, name](boost::system::error_code, std::size_t) {
                boost::system::error_code ignored_ec;
                socket_.shutdown(tcp::socket::shutdown_both, ignored_ec);
                socket_.close(ignored_ec);
                std::cout << "Authentication failed for: " << name << '\n';
            });
        return false;
    }

    Player p;
    p.name = name;
    p.ip = socket_.remote_endpoint().address().to_string();
    p.login_time = std::time(nullptr);
    p.authenticated = true;

    int client_id = next_client_id++;
    player_registry[client_id] = std::move(p);

    {
        std::lock_guard<std::mutex> lock(waiting_room_mutex);
        waiting_room.push_back(std::move(p));
        std::cout << "Player " << p.name << " 加入等待區，目前等待人數: "
                  << waiting_room.size() << '\n';
    }

    std::ostringstream welcome;
    welcome << "Welcome " << p.name << "！ Your IP: " << p.ip
            << ", Login Time: " << get_time_string(p.login_time) << '\n';
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(welcome.str()),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                std::cout << "Write error (post login): " << ec.message() << '\n';
            }
        });
    std::cout << "Client " << client_id << " logged in: " << p.name << '\n';
    return true;
}