#pragma once

#include <string>
#include <memory>
#include <ctime>

#include <boost/asio.hpp>

class Player {
public:
    int id;
    std::string name;
    std::string ip;
    std::string token;
    bool connected;
    std::time_t login_time;
    bool authenticated = false;

    Player() : id(-1), name("Unknown"), connected(true), login_time(0) {};
    Player(int id, std::string name) : id(id), name(name), connected(true) {}

    // ✅ Move constructor/assignment
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;
    // ❌ 禁止 copy constructor/assignment（因為 unique_ptr 不允許 copy）
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    std::string get_name() const { return name; }
    std::unique_ptr<boost::asio::steady_timer> turn_timer;
};
