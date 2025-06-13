#pragma once

#include "player/player.h"
#include "game/card.h"

#include <vector>
#include <map>
#include <string>

#include <boost/asio.hpp>

class GameRoom {
    std::vector<Player*> players;
    boost::asio::io_context& io_context_;
    int current_turn;
    std::vector<std::string> last_played_cards;
    std::map<Player*, std::vector<std::string>> hands;
public:
    GameRoom(const std::vector<Player*>& players, boost::asio::io_context& io);

    void start();
    bool is_valid_play(Player* p, const std::vector<std::string>& cards);
    void handle_play_card(Player* p, const std::vector<std::string>& cards);
    Player* get_player_by_id(int id);
    void start_turn_timer(Player& player);
    void handle_player_move(Player& player);
};
