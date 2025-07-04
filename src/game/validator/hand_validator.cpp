#include "game/validator/hand_validator.h"

namespace card = rsosor::game::card;

namespace rsosor {
namespace game {
namespace validator {

    HandInfo HandValidator::parse(const std::vector<card::Card>& cards) {
        HandInfo info;
        info.cards = cards;
        info.strength = 0;

        if (cards.empty()) {
            info.type = HandType::Invalid;
            return info;
        }


        if (cards.size() == 1) {
            info.type = HandType::Single;
            info.strength = cards[0].rank;
            return info;
        }

        if (cards.size() == 2 && cards[0].rank == cards[1].rank) {
            info.type = HandType::Pair;
            info.strength = cards[0].rank;
            return info;
        }

        if (cards.size() == 3 &&
            cards[0].rank == cards[1].rank &&
            cards[1].rank == cards[2].rank) {
            info.type = HandType::Triple;
            info.strength = cards[0].rank;
            return info;
        }

        // 可擴充 Straight, FullHouse, Bomb...
        info.type = HandType::Invalid;
        return info;
    }

    bool HandValidator::is_stronger(const std::vector<card::Card>& curr_cards, const std::vector<card::Card>& prev_cards) {
        return true;
    }

}   // validator
}   // game
}   // rsosor