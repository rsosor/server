#include "game/game_client.h"
#include "server/fake_server.h"

#include <iostream>
#include <sstream>
#include <set>
#include <array>

    void GameClient::play() {
        deck.shuffle();

        std::vector<Card> allCards = deck.deal();
        for (size_t i = 0; i < allCards.size(); ++i) {
            players[i % 4].receiveCard(allCards[i]);
        }

        for (int i = 0; i < 4; ++i) {
            std::cout << "Player " << (i + 1) << ":\n";
            players[i].sortHand();
            players[i].showHand(std::cout);
            std::cout << "\n";
        }

        std::cout << "開始遊戲，模擬輪流出牌\n";
        // room.size(): vector<GameRoom>
        bool gg = false;
        while (!gg) {
            for (int i = 0; i < 4; ++i) {
                std::cout << "玩家 " << (i + 1) << " 回合：\n";
                players[i].playTurn(i + 1);

                if (players[i].is_hand_empty()) {
                    std::cout << "🎉 玩家 " << (i + 1) << " 出完牌，遊戲結束！" << '\n';
                    gg = true;
                    break;
                }
            }
        }
        std::cout << "感謝遊玩！" << '\n';
    }

    /**
     * class GameClient
     */
    // GameClient::GameClient() {
    //     // 初始化手牌（可換成洗牌發牌邏輯）
    //     hand = {
    //         {3, Suit::Spades}, {3, Suit::Hearts},
    //         {4, Suit::Diamonds}, {7, Suit::Clubs},
    //         {14, Suit::Hearts}, {15, Suit::Spades} // A, 2
    //     };
    // }

    // void GameClient::play() {
    //     while (true) {
    //         showHand();

    //         std::cout << "請輸入你要出的牌 (例如: 3 3 A): ";
    //         std::string input;
    //         std::getline(std::cin, input);

    //         auto selected = parseInput(input);

    //         if (selected.empty()) {
    //             std::cout << "[Client] 錯誤：你沒有出任何牌。\n";
    //             continue;
    //         }

    //         if (!ownsCards(selected)) {
    //             std::cout << "[Client] 錯誤：你沒有這些牌。\n";
    //             continue;
    //         }

    //         // 模擬送出給 Server
    //         bool ok = FakeServer::validatePlay(selected);

    //         if (ok) {
    //             std::cout << "[Server] 合法出牌！你出了：";
    //             for (const auto& c : selected) std::cout << c.toString() << " ";
    //             std::cout << "\n";

    //             removeCards(selected); // 從手牌移除
    //         } else {
    //             std::cout << "[Server] 非法出牌，請重新輸入。\n";
    //         }
    //     }   
    // }

    // void GameClient::showHand() {
    //     std::cout << "\n你目前的手牌：";
    //     for (const auto& c : hand) std::cout << c.toString() << " ";
    //     std::cout << "\n";
    // }

    // std::vector<Card> GameClient::parseInput(const std::string& input) {
    //     std::vector<Card> result;
    //     std::istringstream iss(input);
    //     std::string token;
    //     while (iss >> token) {
    //         int rank = parseRank(token);
    //         if (rank == -1) continue;

    //         for (const auto& c : hand) {
    //             if (c.rank == rank && std::find(result.begin(), result.end(), c) == result.end()) {
    //                 result.push_back(c);
    //                 break;
    //             }
    //         }
    //     }
    //     return result;
    // }

    // int GameClient::parseRank(const std::string& s) {
    //     if (s == "J") return 11;
    //     if (s == "Q") return 12;
    //     if (s == "K") return 13;
    //     if (s == "A") return 14;
    //     if (s == "2") return 15;
    //     try {
    //         int r = std::stoi(s);
    //         return (r >= 3 && r <= 10) ? r : -1;
    //     } catch (...) {
    //         return -1;
    //     }
    // }

    // bool GameClient::ownsCards(const std::vector<Card>& selected) {
    //     std::multiset<Card> handSet(hand.begin(), hand.end());
    //     for (const auto& c : selected) {
    //         auto it = handSet.find(c);
    //         if (it == handSet.end()) return false;
    //         handSet.erase(it);
    //     }
    //     return true;
    // }

    // void GameClient::removeCards(const std::vector<Card>& selected) {
    //     for (const auto& c : selected) {
    //         auto it = std::find(hand.begin(), hand.end(), c);
    //         if (it != hand.end()) hand.erase(it);
    //     }
    // }