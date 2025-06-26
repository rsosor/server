#include "server/rule.h"

HandType determineHandType(const std::vector<Card>& cards) {
    if (cards.size() == 1) return HandType::Single;
    if (cards.size() == 2 && cards[0].rank == cards[1].rank) return HandType::Pair;
    if (cards.size() == 3 && cards[0].rank == cards[1].rank && cards[1].rank == cards[2].rank)
        return HandType::Triple;
    // 可擴充：順子、葫蘆、炸彈等
    return HandType::Invalid;
}
