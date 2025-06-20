/**
 * 代表一場正在進行的大老二遊戲房間
 * 
 * 應包含：
 * std::vector<Player> 玩家清單
 * int current_turn_player_id
 * std::vector<Card> last_player_cards
 * 遊戲房間設定（最大人數、規則模式等）
 * 
 * 負責協調多個 Player，並與 GameState 結合完成遊戲流程控制。
 */
#pragma once

#include "game/player/player.h"
#include "game/card/card.h"

#include <vector>
#include <map>
#include <string>

#include <boost/asio.hpp>

namespace player = rsosor::game::player;

namespace rsosor {
namespace game {
namespace game_room {

    class GameRoom {
        std::vector<std::shared_ptr<player::Player>> players;
        boost::asio::io_context& ioc_;
        int current_turn;
        std::vector<std::string> last_played_cards;
        std::map<std::shared_ptr<player::Player>, std::vector<std::string>> hands;
        public:
        GameRoom(const std::vector<std::shared_ptr<player::Player>>& players, boost::asio::io_context& io);
        
        void start();
        bool is_valid_play(std::shared_ptr<player::Player> p, const std::vector<std::string>& cards);
        void handle_play_card(std::shared_ptr<player::Player> p, const std::vector<std::string>& cards);
        std::shared_ptr<player::Player> get_player_by_id(int id);
        void start_turn_timer(player::Player& player);
        void handle_player_move(player::Player& player);
    };

}   // game_room
}   // game
}   // rsosor