#pragma once

#include <string>

enum class Suit { Clubs, Diamonds, Hearts, Spades };
// extern const char* suitNames[];
// extern const char* rankNames[];

struct Card {
    int rank; // 3-15，J=11, Q=12, K=13, A=14, 2=15
    Suit suit;

    std::string toString() const;

    bool operator==(const Card& other) const;
    bool operator<(const Card& other) const;
};
