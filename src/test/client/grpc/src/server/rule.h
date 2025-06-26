#pragma once

#include "game/card/card.h"

#include <vector>

enum class HandType {
    Invalid,
    Single,
    Pair,
    Triple,
    Straight,
    // ...其他牌型
};

HandType determineHandType(const std::vector<Card>& cards);
