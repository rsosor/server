#include "game/state/game_state_data.h"

namespace rsosor {
namespace game {
namespace state {

    int GameStateData::register_player(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);

        int id = next_player_id_++;
        auto player = std::make_shared<player::Player>(id, name);
        players_[id] = player;

        return id;
    }

    void GameStateData::remove_player(int id) {
        
    }

    std::vector<card::Card> GameStateData::get_player_hand(int player_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (hands_.count(player_id))
            return hands_.at(player_id);
        return {};
    }

}   // state
}   // game
}   // rsosor