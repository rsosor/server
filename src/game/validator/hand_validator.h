/**
 * 出牌合法性判定核心模組
 */
#pragma once

#include "game/validator/ihand_validator.h"

namespace rsosor {
namespace game {
namespace validator {

    class HandValidator : public IHandValidator {
    public:
        // 解析目前出的牌是否合法，並轉換為規則手牌型別
        HandInfo parse(const std::vector<card::Card>& cards);

        // 和上一手比較是否可以出
        bool is_stronger(const std::vector<card::Card>& prev, const std::vector<card::Card>& current);
    };

}   // validator
}   // game
}   // rsosor