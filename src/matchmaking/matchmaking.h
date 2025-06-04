#pragma once

#include <queue>
#include <vector>

#include "player/player.h"
#include "game_room/game_room.h"

class Matchmaking {
    std::queue<Player*> waiting_list;

public:
    void start_new_game(const std::vector<Player*>& players);
    void add_to_matchmaking(Player* p);
};
