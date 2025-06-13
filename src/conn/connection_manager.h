#pragma once

#include "player/player.h"

#include <iostream>
#include <unordered_map>
#include <memory>

class ConnectionManager {
    std::unordered_map<int, std::shared_ptr<Player>> players;

public:
    bool add_player(int id, const std::string& name) {
        if (players.count(id) > 0) {
            std::cout << "Player " << id << " already connected.\n";
            return false;
        }
        players[id] = std::make_shared<Player>(id, name);
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

    std::shared_ptr<Player> get_player(int id) {
        if (players.count(id) == 0) return nullptr;
        return players[id];
    }
};
