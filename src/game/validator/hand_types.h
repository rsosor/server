// hand_types.h 或 validator_types.h
#pragma once

#include "game/card/card.h"

#include <vector>

namespace card = rsosor::game::card;

namespace rsosor {
namespace game {
namespace validator {
    
    enum class HandType {
        Invalid,
        Single,
        Pair,
        Triple,
        Straight,
        FullHouse,
        Bomb,
        // ...
    };

    enum class HandRank {
        Low = 0,
        Medium = 1,
        High = 2,
        Ultra = 3
        // 可擴充
    };

    struct HandInfo {
        HandType type;
        std::vector<card::Card> cards;
        int strength;
    };

}   // validator
}   // game
}   // rsosor