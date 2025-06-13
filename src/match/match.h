#pragma once

#include "player/player.h"
#include "game_room/game_room.h"

#include <queue>
#include <vector>

class Match {
    std::queue<Player*> waiting_list;

public:
    void start_new_game(const std::vector<Player*>& players);
    void add_to_match(Player* p);
};
