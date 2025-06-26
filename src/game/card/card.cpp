#include "game/card/card.h"

#include <map>

namespace rsosor {
namespace game {
namespace card {

    Card::Card(Suit s, int r) : suit(s), rank(r) {}

    std::string Card::toString() const {
        static const std::map<int, std::string> rankMap = {
            {11, "J"}, {12, "Q"}, {13, "K"}, {14, "A"}, {15, "2"}
        };
        std::string rankStr = rank <= 10 ? std::to_string(rank) : rankMap.at(rank);
        char suitChar = "♠♥♦♣"[static_cast<int>(suit)];
        return rankStr + suitChar;
    }

    // bool Card::operator==(const Card& other) const {
    //     return rank == other.rank && suit == other.suit;
    // }

}   // card
}   // game
}   // rsosor