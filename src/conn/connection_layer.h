/**
 * 日後希望讓 Connection 專心處理「接收 / 發送」資料，而不介入遊戲邏輯
 * 那 ConnectionLayer 是合理存在的：
 */
#pragma once

#include "game/game_room/game_room.h"

#include <string>
#include <memory>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace game_room = rsosor::game::game_room;

class ConnectionLayer {
    std::shared_ptr<game_room::GameRoom> game_room;

public:
    ConnectionLayer(std::shared_ptr<game_room::GameRoom> gr) : game_room(gr) {}

    void handle_message(int player_id, const std::string& msg);
};
