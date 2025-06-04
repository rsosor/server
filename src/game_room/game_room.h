#pragma once

#include <vector>
#include <map>
#include <string>

#include "player/player.h"
#include "game/card.h"

class GameRoom {
    std::vector<Player*> players;
    int current_turn;
    std::vector<std::string> last_played_cards;
    std::map<Player*, std::vector<std::string>> hands;

public:
    GameRoom(const std::vector<Player*>& players);

    void start();
    bool is_valid_play(Player* p, const std::vector<std::string>& cards);
    void handle_play_card(Player* p, const std::vector<std::string>& cards);
    Player* get_player_by_id(int id);
};
