#pragma once

#include "game/validator/hand_types.h"
#include "game/card/card.h"

#include <vector>

namespace card = rsosor::game::card;

namespace rsosor {
namespace game {
namespace validator {

    // enum class HandType {
    //     Invalid,
    //     Single,
    //     Pair,
    //     Triple,
    //     Straight,
    //     FullHouse,
    //     Bomb,
    //     // ...
    // };

    // // 給牌型排序的 enum 整數比較基礎值（可用於 strength）
    // enum class HandRank {
    //     Low = 0,
    //     Medium = 1,
    //     High = 2,
    //     Ultra = 3
    //     // Bomb 可能最高
    // };

    // struct HandInfo {
    //     HandType type;
    //     std::vector<card::Card> cards;
    //     int strength; // 用來比較哪組牌比較大（例如最大牌的rank）
    // };

    class IHandValidator {
    public:
        virtual ~IHandValidator() = default;
        virtual HandInfo parse(const std::vector<card::Card>& cards) = 0;
        virtual bool is_stronger(const std::vector<card::Card>& prev, const std::vector<card::Card>& current) = 0;
    };

}   // validator
}   // game
}   // rsosor