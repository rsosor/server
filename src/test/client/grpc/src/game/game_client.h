#pragma once

#include "game/card/card.h"
#include "game/card/deck.h"
#include "game/player/player.h"

#include <string>
#include <vector>
#include <array>

class GameClient {
public:

    // GameClient();

    void play();
    
private:
    // void showHand();

    // std::vector<Card> parseInput(const std::string& input);

    // parseInput need
    // int parseRank(const std::string& s);

    // bool ownsCards(const std::vector<Card>& selected);

    // void removeCards(const std::vector<Card>& selected);

    // std::vector<Card> hand;
    std::array<Player, 4> players;
    Deck deck;
};
