#pragma once

#include <string>

class Player {
public:
    int id;
    std::string name;

    Player(int id, std::string name) : id(id), name(name) {}

    std::string get_name() const { return name; }
};
