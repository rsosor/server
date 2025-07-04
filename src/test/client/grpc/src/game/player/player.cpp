#include "game/player/player.h"
#include "server/fake_server.h"

#include <algorithm>
#include <sstream>
#include <set>

Player::Player() {
    std::cout << "[Debug] Player constructed at " << this << "\n";
}

int Player::getId() {
    return player_id_;
}

void Player::setId(int player_id) {
    player_id_ = player_id;
}

int Player::getHandSize() {
    return hand_size_;
}
    
void Player::setHandSize(int hand_size) {
    hand_size_ = hand_size;
}

// 單機版四人
void Player::receiveCard(const Card& card) {
    hand_.push_back(card);
}

// 連線版本
void Player::receiveHand(const std::vector<Card>& cards) {
    hand_ = cards;
    sortHand();
    std::cout << "[Debug] Player::receiveHand: called at " << this << ", hand size = " << hand_.size() << "\n";
}

void Player::sortHand() {
    std::sort(hand_.begin(), hand_.end(), [](const Card& a, const Card& b) {
        return a.rank < b.rank || (a.rank == b.rank && static_cast<int>(a.suit) < static_cast<int>(b.suit));
    });
}

void Player::showHand(std::ostream& os) const {
    os << "[Debug] Player::showHand: Player hand size: " << hand_.size() << "\n";
    for (const auto& c : hand_) os << c.toString() << " ";
    os << "\n";
}

// 這邊示範簡單 playTurn，從標準輸入讀牌並驗證
std::vector<Card> Player::playTurn(int player_id, const std::string& room_id) {
    while (true) {
        std::cout << "[Debug] Player::playTurn: using grpc_client_: " << grpc_client_ << '\n';

        std::cout << "[Debug] Player::playTurn: called at " << this << ", hand size = " << hand_.size() << "\n";
        showHand(std::cout);
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

        // grpc 驗證
        gen::PlayValidationRequest req;
        req.set_player_id(player_id);
        req.set_room_id(room_id);
        for (const auto& c : selected) {
            std::cout << c.toString() << " ";
            auto* card = req.add_cards();
            card->set_rank(c.rank);
            card->set_suit(suitToString(c.suit));
        }
        std::cout << '\n';

        // 可選：傳上最後出牌資訊（視 proto 設計而定）
        // 也可以讓伺服器自己記住 TableState，就不用傳 last_played_cards

        if (!grpc_client_) {
            std::cerr << "[Error] game_stub_ is nullptr. 尚未建立 gRPC stub。\n";
            return {};
        }

        gen::PlayValidationResponse res;
        grpc::ClientContext ctx;
        grpc::Status status = grpc_client_->table_stub_->ValidatePlay(&ctx, req, &res);
        if (status.ok()) {
            std::cout << "Valid: " << res.is_valid()
                    << " message: " << res.message()
                    << '\n';
        } else {
            std::cout << "[Client] RPC Failed：" 
                      << status.error_code() << " - " 
                      << status.error_message() << " - " 
                      << status.error_details() << '\n';
            continue;
        }

        if (res.is_valid()) {
            std::cout << "[Server] 合法出牌。\n";
            removeCards(selected);
            return selected;
        } else {
            std::cout << "[Server] 非法出牌：" << res.message() << '\n';
        }

        // 單機驗證
        // if (FakeServer::validatePlay(selected, player_id)) {
        //     std::cout << "[Fake Server] 合法出牌！你出了：";
        //     for (const auto& c : selected) std::cout << c.toString() << " ";
        //     std::cout << "\n";
        //     removeCards(selected);
        //     return selected;
        // } else {
        //     std::cout << "[Fake Server] 非法出牌，請重新輸入。\n";
        // }
    }
}

bool Player::is_hand_empty() const {
    return hand_.empty();
}

void Player::setGrpcClient(BigTwoClient* client) {
    std::cout << "[Debug] Player::setStub called with " << client << '\n';
    grpc_client_ = client;
}

std::vector<Card> Player::parseInput(const std::string& input) const {
    std::vector<Card> result;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        Card c = parseCard(token);
        if (c.rank == -1) {
            std::cout << "[DEBUG] 無法解析的輸入：" << token << '\n';
            continue;
        }

        // 不檢查是否真的擁有，全部加起來
        result.push_back(c);

        // for (const auto& h : hand) {
        //     if (h == c && std::find(result.begin(), result.end(), h) == result.end()) {
        //         result.push_back(h);
        //         break;
        //     }
        // }
    }

    return result;
}

bool Player::ownsCards(const std::vector<Card>& selected) const {
    std::multiset<Card> handSet(hand_.begin(), hand_.end());
    for (const auto& c : selected) {
        auto it = handSet.find(c);
        if (it == handSet.end()) return false;
        handSet.erase(it);
    }
    return true;
}

void Player::removeCards(const std::vector<Card>& selected) {
    for (const auto& c : selected) {
        auto it = std::find(hand_.begin(), hand_.end(), c);
        if (it != hand_.end()) hand_.erase(it);
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

std::string Player::suitToString(Suit suit) {
    switch (suit) {
        case Suit::Clubs: return "clubs";
        case Suit::Diamonds: return "diamonds";
        case Suit::Hearts: return "hearts";
        case Suit::Spades: return "spades";
        default: return "unknown";
    }
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
