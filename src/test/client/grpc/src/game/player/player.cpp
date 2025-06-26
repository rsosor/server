#include "game/player/player.h"
#include "server/fake_server.h"

#include <algorithm>
#include <sstream>
#include <set>

void Player::receiveCard(const Card& card) {
    hand.push_back(card);
}

void Player::sortHand() {
    std::sort(hand.begin(), hand.end(), [](const Card& a, const Card& b) {
        return a.rank < b.rank || (a.rank == b.rank && static_cast<int>(a.suit) < static_cast<int>(b.suit));
    });
}

void Player::showHand(std::ostream& os) const {
    for (const auto& c : hand) os << c.toString() << " ";
    os << "\n";
}

// 這邊示範簡單 playTurn，從標準輸入讀牌並驗證
std::vector<Card> Player::playTurn(int player_id) {
    while (true) {
        std::cout << "請輸入你要出的牌 (例如: 3 3): ";
        std::string input;
        std::getline(std::cin, input);

        auto selected = parseInput(input);
        for (auto& c : selected) std::cout << "selected: " << c.toString();
        std::cout << '\n';

        // if (selected.empty()) {
        //     std::cout << "[Client] 錯誤：你沒有出任何牌。\n";
        //     continue;
        // }

        if (!ownsCards(selected)) {
            std::cout << "[Client] 錯誤：你沒有這些牌。\n";
            continue;
        }

        bool ok = FakeServer::validatePlay(selected, player_id);
        if (ok) {
            removeCards(selected);
            return selected;
        } else {
            std::cout << "[Server] 非法出牌，請重新輸入。" << '\n';
        }

        // 呼叫伺服器驗證
        // if (FakeServer::validatePlay(selected, player_id)) {
        //     std::cout << "[Server] 合法出牌！你出了：";
        //     for (const auto& c : selected) std::cout << c.toString() << " ";
        //     std::cout << "\n";
        //     removeCards(selected);
        //     return selected;
        // } else {
        //     std::cout << "[Server] 非法出牌，請重新輸入。\n";
        // }
    }
}

bool Player::is_hand_empty() const {
    return hand.empty();
}

std::vector<Card> Player::parseInput(const std::string& input) const {
    std::vector<Card> result;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        Card c = parseCard(token);
        if (c.rank == -1) continue;

        for (const auto& h : hand) {
            if (h == c && std::find(result.begin(), result.end(), h) == result.end()) {
                result.push_back(h);
                break;
            }
        }
    }

    return result;
}

bool Player::ownsCards(const std::vector<Card>& selected) const {
    std::multiset<Card> handSet(hand.begin(), hand.end());
    for (const auto& c : selected) {
        auto it = handSet.find(c);
        if (it == handSet.end()) return false;
        handSet.erase(it);
    }
    return true;
}

void Player::removeCards(const std::vector<Card>& selected) {
    for (const auto& c : selected) {
        auto it = std::find(hand.begin(), hand.end(), c);
        if (it != hand.end()) hand.erase(it);
    }
}

Card Player::parseCard(const std::string& token) const {
    if (token.size() < 2) return { -1, Suit::Clubs };
    
    std::string rankPart = token.substr(0, token.size() - 1);
    char suitChar = token.back();

    int rank = parseRank(rankPart);
    if (rank == -1) return { -1, Suit::Clubs };

    Suit suit;
    switch (suitChar) {
        case 'C': suit = Suit::Clubs; break;
        case 'D': suit = Suit::Diamonds; break;
        case 'H': suit = Suit::Hearts; break;
        case 'S': suit = Suit::Spades; break;
        default: return { -1, Suit::Clubs };
    }

    return { rank, suit };
}

int Player::parseRank(const std::string& s) const {
    if (s == "J") return 11;
    if (s == "Q") return 12;
    if (s == "K") return 13;
    if (s == "A") return 14;
    if (s == "2") return 15;
    try {
        int r = std::stoi(s);
        return (r >= 3 && r <= 10) ? r : -1;
    } catch (...) {
        return -1;
    }
}
