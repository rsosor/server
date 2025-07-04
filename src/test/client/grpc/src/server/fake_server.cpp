#include "server/fake_server.h"
#include "server/rule.h"

std::vector<Card> FakeServer::last_cards;
int FakeServer::last_player_id = -1;
int FakeServer::pass_count = 0;
int FakeServer::total_players = 4; // 可提供 setter 動態設定
HandType FakeServer::last_hand_type = HandType::Invalid;

bool FakeServer::validatePlay(const std::vector<Card>& curr_cards, int curr_player_id) {
    HandType curr_type = determineHandType(curr_cards);

    // 新一輪開始，last_cards 空，第一位玩家必須出牌
    if (last_cards.empty()) {
        if (curr_cards.empty()) {
            std::cout << "[Fake Server] 新一輪開始，第一位玩家不能 PASS，請出牌！" << '\n';
            return false;
        }
    
        if (curr_type == HandType::Invalid) {
            std::cout << "[Fake Server] 無效的牌型" << '\n';
            return false;
        }

        // 第一手出牌成功
        std::cout << "[Fake Server] 新一輪開始，玩家 " << curr_player_id << " 出了：";
        for (const auto& c : curr_cards) std::cout << c.toString() << " ";
        std::cout << "\n";
        last_cards = curr_cards;
        last_hand_type = curr_type;
        last_player_id = curr_player_id;
        pass_count = 0;
        return true;
    }

    // 非新一輪，玩家 PASS 的情況
    if (curr_cards.empty()) {
        pass_count++;
        std::cout << "[Fake Server] 玩家 " << curr_player_id << "PASS" << '\n';

        if (pass_count >= total_players - 1) {
            std::cout << "[Fake Server] 本輪結束，下一位玩家可自由出牌。" << '\n';
            last_cards.clear();
            last_hand_type = HandType::Invalid;
            pass_count = 0;
        }

        return true;
    }

    // 有牌出，檢查牌型是否有效
    if (curr_type == HandType::Invalid) {
        std::cout << "[Fake Server] 無效的牌型" << '\n';
        return false;
    }

    // 要牌型一致才能比較強度
    if (curr_type != last_hand_type) {
        std::cout << "[Fake Server] 出牌型態不同，請出相同型態。" << '\n';
        return false;
    }

    // 比較強弱
    if (is_stronger(curr_cards, last_cards)) {
        // 出牌
        std::cout << "[Fake Server] 玩家 " << curr_player_id << " 出牌成功，壓過上一手。" << '\n';
        last_cards = curr_cards;
        last_hand_type = curr_type;
        last_player_id = curr_player_id;
        pass_count = 0;
        return true;
    } else {
        std::cout << "[Fake Server] 出的牌無法壓過上一手。" << '\n';
        return false;
    }

    // if (curr_cards.size() == 1) return true;
    // if (curr_cards.size() == 2 && curr_cards[0].rank == curr_cards[1].rank) return true;
    // return false;
}

static bool compare_card(const Card& a, const Card& b) {
    if (a.rank != b.rank) return a.rank > b.rank;
    return static_cast<int>(a.suit) > static_cast<int>(b.suit);
}

bool FakeServer::is_stronger(const std::vector<Card>& curr_cards, const std::vector<Card>& prev_cards) {
    // TODO：根據遊戲邏輯比對牌型與大小，這裡先假設同樣張數時比最大張牌的 rank
    if (curr_cards.size() != prev_cards.size()) return false;

    auto maxRank = [](const std::vector<Card>& cards) {
        return std::max_element(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
            return a.rank < b.rank || (a.rank == b.rank && static_cast<int>(a.suit) < static_cast<int>(b.suit));
        });
    };

    const Card& currentMax = *maxRank(curr_cards);
    const Card& previousMax = *maxRank(prev_cards);

    // return *maxRank(curr_cards) > *maxRank(prev_cards); // 假設有 operator>
    return compare_card(currentMax, previousMax);
}
