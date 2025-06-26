#include "game/card/deck.h"

// #include <algorithm>
#include <random>

// 建立一副撲克牌（52 張）
Deck::Deck() {
    for (int r = 3; r < 16; ++r) {
        for (int s = 0; s < 4; ++s) {
            cards.push_back(Card{ r, static_cast<Suit>(s) });
        }
    }
}

void Deck::shuffle() {
    // 洗牌用亂數引擎
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::shuffle(cards.begin(), cards.end(), rng);
}

std::vector<Card> Deck::deal() {
    // 直接把目前整副牌回傳，並清空牌堆
    std::vector<Card> dealtCards = cards;
    cards.clear();
    return dealtCards;
}