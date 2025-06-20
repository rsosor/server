// #include "server/core_server.h"
// #include "game/player/player.h"

// #include <iostream>
// #include <unordered_map>
// #include <memory>
// #include <ctime>
// #include <sstream>
// #include <mutex>

// using boost::asio::ip::tcp;

// int next_client_id = 1;
// std::unordered_map<int, Player> player_registry;

// const std::string hardcoded_token = "letmein";

// // 等待區容器與鎖
// std::vector<Player> waiting_room;
// std::mutex waiting_room_mutex;

// std::string get_time_string(std::time_t t) {
//     char buf[32];
//     std::strftime(buf, sizeof(buf), "%F, %T", std::localtime(&t));
//     return std::string(buf);
// }

// /**
//  *  1. 阻塞式：core_server.h / handle_client()
//  * 用 boost::asio::read_until + getline 阻塞等待。
//  * 一次處理一個連線。
//  * 用 std::vector<Player> 做等待區，外加 mutex。
//  */
// void handle_client(std::shared_ptr<tcp::socket> sock) {
//     try {
//         boost::asio::streambuf buf;
//         boost::asio::read_until(*sock, buf, "\n");
//         std::istream is(&buf);
//         std::string line;
//         std::getline(is, line);

//         std::cout << "Received from client: " << line << '\n';

//         std::istringstream iss(line);
//         std::string command, name, token;
//         iss >> command >> name >> token;

//         if (command != "LOGIN" || token != hardcoded_token) {
//             std::string msg = "AUTH FAILED\n";
//             boost::asio::write(*sock, boost::asio::buffer(msg));
//             sock->close();
//             std::cout << "Authentication failed for client: " << name << '\n';
//             return;
//         }

//         Player p;
//         p.name = name;
//         p.ip = sock->remote_endpoint().address().to_string();
//         p.login_time = std::time(nullptr);
//         p.authenticated = true;

//         // player_registry[sock.get()] = p;
//         int client_id = next_client_id++;
//         player_registry[client_id] = p;

//         // 加入等待區加上鎖
//         {
//             std::lock_guard<std::mutex> lock(waiting_room_mutex);
//             waiting_room.push_back(p);
//             std::cout << "Player " << p.name << " 加入等待區，目前等待人數: "
//                       << waiting_room.size() << '\n';
//         }

//         std::ostringstream welcome;
//         welcome << "Welcome " << p.name << "！ Your IP: " << p.ip
//                 << ", Login Time: " << get_time_string(p.login_time) << '\n';
//         boost::asio::write(*sock, boost::asio::buffer(welcome.str()));
//         std::cout << "Client " << client_id << " logged in: " << p.name << '\n';
//     } catch (std::exception& e) {
//         std::cerr << "Client error: " << e.what() << '\n';
//     }
// }