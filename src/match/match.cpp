#include "match/match.h"

#include <iostream>

using namespace rsosor::game::player;

namespace rsosor {
namespace match {

    void Match::start_new_game(const std::vector<Player*>& players) {
        std::cout << "Starting a new game with " << players.size() << " players." << '\n';

        for (auto p : players) {
            std::cout << "Player: " << p->get_name() << '\n';
        }

        // 這裡可以放遊戲開始的其他邏輯
    }

    void Match::add_to_match(Player* p) {
        waiting_list.push(p);
        if (waiting_list.size() >= 4) {
            std::vector<Player*> players;
            for (int i = 0; i < 4; ++i) {
                players.push_back(std::move(waiting_list.front()));
                waiting_list.pop();
            }
            start_new_game(players);
        }
    }

}   // match
}   // rsosor