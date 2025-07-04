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
#include "generated/bigtwo.pb.h"
#include <grpcpp/grpcpp.h>  // gRPC 的 C++ 頭檔，會定義 grpc::ServerWriter

#include <vector>
#include <map>
#include <string>

#include <boost/asio.hpp>

namespace player = rsosor::game::player;
namespace card = rsosor::game::card;
namespace gen = rsosor::generated;

namespace rsosor {
namespace game {
namespace game_room {

    // ServerWriter 只能在訂閱的 RPC 服務方法所屬的 thread（那個 function）裡使用，不能跨 thread 呼叫（這是 gRPC 本身的限制）。
    // 你要為每個連線（玩家）在他呼叫訂閱的 RPC 時，在該 RPC handler 裡取得 ServerWriter，並在同一 thread 裡維護這個 ServerWriter 的生命週期和寫入。
    struct PlayerConnection {
        std::string player_id;
        grpc::ServerWriter<gen::GameState>* stream_writer; // 推播對象
        bool connected;

        PlayerConnection() = default;
    };
    
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

        std::string room_id;
        std::vector<card::Card> last_played;
        std::map<std::string, PlayerConnection> players_mp;
        std::vector<std::string> play_history;
    };

}   // game_room
}   // game
}   // rsosor