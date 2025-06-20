#pragma once

#include "game/player/player.h"

#include <iostream>
#include <unordered_map>
#include <memory>

namespace player = rsosor::game::player;

class ConnectionManager {
    std::unordered_map<int, std::shared_ptr<player::Player>> players;

public:
    bool add_player(int id, const std::string& name) {
        if (players.count(id) > 0) {
            std::cout << "Player " << id << " already connected.\n";
            return false;
        }
        players[id] = std::make_shared<player::Player>(id, name);
        std::cout << "Player " << name << " (ID " << id << ") connected.\n";
        return true;
    }

    bool remove_player(int id) {
        if (players.count(id) == 0) {
            std::cout << "Player " << id << " not found.\n";
            return false;
        }
        std::cout << "Player " << players[id]->name << " (ID " << id << ") disconnected.\n";
        players.erase(id);
        return true;
    }

    std::shared_ptr<player::Player> get_player(int id) {
        if (players.count(id) == 0) return nullptr;
        return players[id];
    }
};
