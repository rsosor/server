#pragma once

#include "game/card/card.h"

#include <vector>

class Deck {
public:
    Deck();                  // 建立 52 張牌
    void shuffle();          // 洗牌
    std::vector<Card> deal(); // 發牌
private:
    std::vector<Card> cards;
};