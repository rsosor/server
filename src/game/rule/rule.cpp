#include "game/rule/rule.h"
#include "game/card/card.h"

#include <vector>

// using game = rsosor::game;
using namespace rsosor::game::card;

namespace rsosor {
namespace game {
namespace rule {

    enum class ComboType {
        Invalid,
        Single,
        Pair,
        Triple,
        Straight,
        FullHouse,
        Bomb,
        // ...
    };

    struct Combo {
        ComboType type;
        std::vector<Card> cards;
    };

    // 判斷某一組牌是哪一種牌型
    ComboType detect_combo(const std::vector<Card>& cards);

    // 比較兩組牌型的大小
    bool is_combo_stronger(const Combo& a, const Combo& b);

}   // rule
}   // game
}   // rsosor