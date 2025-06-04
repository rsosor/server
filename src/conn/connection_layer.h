#pragma once

#include <string>
#include <memory>

#include "game_room/game_room.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ConnectionLayer {
    std::shared_ptr<GameRoom> game_room;

public:
    ConnectionLayer(std::shared_ptr<GameRoom> gr) : game_room(gr) {}

    void handle_message(int player_id, const std::string& msg);
};
