/**
 * 代表單一玩家的遊戲狀態
 * 
 * 應包含：
 * int id
 * std::string name
 * std::vector<Card> hand
 * 是否已 Pass、本輪出牌紀錄等
 * 
 * 可包含：
 * bool isAI、得分、連線狀態等
 */
#pragma once

#include <string>
#include <memory>
#include <ctime>

#include <boost/asio.hpp>

namespace rsosor {
namespace game {
namespace player {
    
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
    
}   // player
}   // game
}   // rsosor