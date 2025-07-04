/**
 * GameStateData 遊戲核心資料
 * 
 * 代表遊戲流程的進度與狀態機
 * 
 * 負責：
 * 控制階段：WaitingStart、Dealing、Playing、Ended
 * 處理輪轉邏輯：誰先出、誰贏、誰跳過
 * 更新是否可以出牌、是否可結束
 * 判斷遊戲是否結束
 * 
 * 抽象層比 GameRoom 更高，屬於控制流程的邏輯核心
 * 
 * 擁有 std::unordered_map<int, Player>（或 shared_ptr<Player>）
 * 
 * 擁有 std::unordered_map<int, GameRoom> 或類似
 * 
 * 提供 thread-safe 訪問、更新方法（可加 mutex）
 */
// game/state/game_state_data.h
#pragma once

#include "game/game_room/game_room.h"
#include "game/player/player.h"

#include <unordered_map>
#include <memory>
#include <mutex>

namespace game_room = rsosor::game::game_room;
namespace player = rsosor::game::player;
namespace card = rsosor::game::card;

namespace rsosor {
namespace game {
namespace state {

    class GameStateData {
    public:
        int register_player(const int player_id, const std::string& name);
        void remove_player(int id);

        std::shared_ptr<player::Player> get_player(int id);
        std::shared_ptr<game_room::GameRoom> get_room(int id);
        std::vector<card::Card> get_player_hand(int player_id);

        // 其他狀態、遊戲邏輯

    private:
        std::mutex mutex_;
        int next_player_id_ = 1;
        std::unordered_map<int, std::shared_ptr<player::Player>> players_;
        std::unordered_map<int, std::shared_ptr<game_room::GameRoom>> rooms_;
        std::unordered_map<int, std::vector<card::Card>> hands_;
    };

}   // state
}   // game
}   // rsosor