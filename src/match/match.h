#pragma once

#include "game/player/player.h"
#include "game/game_room/game_room.h"

#include <queue>
#include <vector>

namespace player = rsosor::game::player;

namespace rsosor {
namespace match {

    class Match {
        std::queue<player::Player*> waiting_list;
        
        public:
        void start_new_game(const std::vector<player::Player*>& players);
        void add_to_match(player::Player* p);
    };
    
}   // match
}   // rsosor