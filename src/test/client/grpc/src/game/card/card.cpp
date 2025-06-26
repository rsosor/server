#include "game/card/card.h"

#include <map>

const char* suitNames[] = { "C", "D", "H", "S" };
const char* rankNames[] = {
    "2", "3", "4", "5", "6", "7", "8",
    "9", "10", "J", "Q", "K", "A"
};

    /**
     * struct Card
     */
    std::string Card::toString() const {
        static const std::map<int, std::string> rankMap = {
            {11, "J"}, {12, "Q"}, {13, "K"}, {14, "A"}, {15, "2"}
        };
        std::string rankStr = rank <= 10 ? std::to_string(rank) : rankMap.at(rank);
        // std::array<std::string, 4> suits = {"C", "D", "H", "S"};
        // std::string suitChar = suits[static_cast<int>(suit)];
        char suitChar = "CDHS"[static_cast<int>(suit)];
        // char suitChar = "♠♥♦♣"[static_cast<int>(suit)];
        return rankStr + suitChar;
    }

    bool Card::operator==(const Card& other) const {
        return rank == other.rank && suit == other.suit;
    }

    bool Card::operator<(const Card& other) const {
        // 自訂排序方式：先比 rank，再比 suit
        if (rank != other.rank) return rank < other.rank;
        return static_cast<int>(suit) < static_cast<int>(other.suit);
    }
