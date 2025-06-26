/**
 * 代表單張紙牌的資料結構
 * 
 * 應包含：
 * 所有玩家的手牌
 * 出牌與牌堆
 * 判斷出牌是否合法、比大小等邏輯
 */
#pragma once

#include <string>

namespace rsosor {
namespace game {
namespace card {

    enum class Suit {
        Clubs = 0,
        Diamonds,
        Hearts,
        Spades
    };

    struct Card {
        Suit suit;
        int rank;   // 3~15

        bool operator==(const Card&) const = default;

        Card() = default;
        Card(Suit s, int r);

        std::string toString() const;
    };

}   // card
}   // game
}   // rsosor