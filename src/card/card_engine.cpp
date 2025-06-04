#include "card/card_engine.h"

bool is_single(const std::vector<std::string>& cards) {
    return cards.size() == 1;
}

bool is_pair(const std::vector<std::string>& cards) {
    return cards.size() == 2;
}

bool beats(const std::vector<std::string>& new_play, const std::vector<std::string>& old_play) {
    // TODO: 根據大老二規則實作比較邏輯
    return true;
}
