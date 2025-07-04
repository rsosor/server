#include "game/game_client.h"
#include "server/fake_server.h"

#include <iostream>
#include <sstream>
#include <set>
#include <array>
#include <thread>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

    GameClient::GameClient(std::shared_ptr<grpc::Channel> channel) : 
            grpc_client_(channel), player_(players_[self_idx_]), 
            self_idx_(0) {
        std::cout << "BigTwoClient initialized" << '\n';
        // if (!channel->WaitForConnected(
        //         gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(5, GPR_TIMESPAN)))) {
        //     std::cerr << "[Error] 連線失敗\n";
        //     exit(1);
        // }

        // 傳遞 raw pointer 將 stub 指派給每位 Player（這裡你可能需要改 Player 結構）
        for (auto& player : players_) {
            player.setGrpcClient(&grpc_client_); // 需要你實作這個方法
        }
        // 每人一個 stub
        // for (auto& player : players_) {
        //     player.setStub(gen::BigTwoTableService::NewStub(channel)); // 需要你實作這個方法
        // }
    }

    // 連線版
    void GameClient::play() {
        std::string player_name = "RsosoR";
        std::string psw = "12345";
        auto login_res = grpc_client_.Login(player_name, psw);
        int player_id = login_res.player_id();
        // todo 房間由伺服器創建
        std::string room_id = "room123";
        auto join_res = grpc_client_.JoinGame(player_id, player_name, room_id);
        if (player_id != -1) {
            player_.setId(player_id);
            players_[self_idx_].setId(player_id);
        }

        // int bob_id = grpc_client_.JoinGame("Bob");
        // int charlie_id = grpc_client_.JoinGame("Charlie");
        // int dave_id = grpc_client_.JoinGame("Dave");

        // deck_.shuffle();

        // grpc_client_.Deal(id);
        gen::DealRequest req;
        req.set_player_id(player_id);
        req.set_room_id(room_id);
        gen::DealResponse res;
        grpc::ClientContext ctx;
        grpc::Status status = grpc_client_.table_stub_->Deal(&ctx, req, &res);
        if (!status.ok()) {
            std::cerr << "[Client] 發牌失敗：" << status.error_message() << '\n';
            return;
        }
        assignHandsToPlayers(res);

        // std::vector<int> ids = {alice_id, bob_id, charlie_id, dave_id};
        // dealCards(ids);

        player_.sortHand();
        player_.showHand(std::cout);
        // players_[self_idx_].sortHand();
        // players_[self_idx_].showHand(std::cout);
        std::cout << '\n';

        // 啟動接收推播執行緒
        std::thread subscriber_thread(&GameClient::subscribeGameState, this, room_id);

        std::cout << "開始遊戲，模擬輪流出牌\n";
        // room.size(): vector<GameRoom>
        bool gg = false;
        // 連線版
        while (!gg) {
            std::cout << "self_idx_: " << self_idx_ << '\n';
            player_.showHand(std::cout);
            auto played = player_.playTurn(player_id, room_id);
            if (player_.is_hand_empty()) {
                std::cout << "🎉 你出完牌了，遊戲結束！" << '\n';
                break;
            }

            // 等 server 廣播下一回合狀態
        }

        // 單機版
        // while (!gg) {
        //     for (int i = 0; i < 4; ++i) {
        //         std::cout << "玩家 " << (i + 1) << " 回合：\n";
        //         players_[i].playTurn(i + 1, room_id);

        //         if (players_[i].is_hand_empty()) {
        //             std::cout << "🎉 玩家 " << (i + 1) << " 出完牌，遊戲結束！" << '\n';
        //             gg = true;
        //             break;
        //         }
        //     }
        // }
        std::cout << "感謝遊玩！" << '\n';
    }
    
    /**
     * 做法	                                    是否符合官方建議	備註
     * unique_ptr 給每個 Player 各建一個 stub	 ✅	              比較浪費資源
     * unique_ptr + 傳 raw pointer 給 Player	✅	             建議做法
     * shared_ptr stub 給所有 Player 共享	    ❌（但可接受）	   設計簡單，實務可行
     */
    // 單機版
    void GameClient::single_play() {
        deck_.shuffle();

        std::vector<Card> allCards = deck_.deal();
        for (size_t i = 0; i < allCards.size(); ++i) {
            players_[i % 4].receiveCard(allCards[i]);
        }

        for (int i = 0; i < 4; ++i) {
            std::cout << "Player " << (i + 1) << ":\n";
            players_[i].sortHand();
            players_[i].showHand(std::cout);
            std::cout << "\n";
        }

        std::string room_id = "room123";

        std::cout << "開始遊戲，模擬輪流出牌\n";
        // room.size(): vector<GameRoom>
        bool gg = false;
        while (!gg) {
            for (int i = 0; i < 4; ++i) {
                std::cout << "玩家 " << (i + 1) << " 回合：\n";
                players_[i].playTurn(i + 1, room_id);

                if (players_[i].is_hand_empty()) {
                    std::cout << "🎉 玩家 " << (i + 1) << " 出完牌，遊戲結束！" << '\n';
                    gg = true;
                    break;
                }
            }
        }
        std::cout << "感謝遊玩！" << '\n';
    }

    void GameClient::subscribeGameState(const std::string& room_id) {
        grpc::ClientContext ctx;
        gen::GameRoomId req;
        req.set_room_id(room_id);

        gen::GameState state;

        std::unique_ptr<grpc::ClientReader<gen::GameState>> reader = grpc_client_.table_stub_->SubscribeGameState(&ctx, req);
        while (reader->Read(&state)) {
            std::cout << "[Client] 收到伺服器推播：下一位玩家 " << state.current_turn_player_id() << "\n";
            for (const auto& c : state.last_played_cards()) {
                std::cout << " - " << c.rank() << " " << c.suit() << '\n';
            }
        }

        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            std::cerr << "[Client] 訂閱失敗：" << status.error_message() << std::endl;
        }
    }

    void GameClient::dealCards(const std::vector<int>& player_ids) {
        gen::DealRequest req;
        for (int id : player_ids) {
            req.set_player_id(id);
        }

        gen::DealResponse res;
        grpc::ClientContext ctx;

        grpc::Status status = grpc_client_.table_stub_->Deal(&ctx, req, &res);
        if (!status.ok()) {
            std::cerr << "[Client] 發牌失敗：" << status.error_message() << '\n';
            return;
        }

        assignHandsToPlayers(res);  // ✅ 就在這裡呼叫
    }

    void GameClient::assignHandsToPlayers(const gen::DealResponse& response) {
        const auto& dealt = response.cards();
        std::cout << "dealt.size(): " << dealt.size() << '\n';

        std::vector<Card> cards;
        for (const auto& proto_card : dealt) {
            Card c = fromProtoCard(proto_card);
            cards.push_back(c);
            std::cout << c.toString() << " ";
        }
        std::cout << '\n';

        std::cout << "[Debug] player_.receiveHand start" << '\n';
        player_.receiveHand(cards);  // ✅ 這裡就塞進自己手上
        // std::cout << "[Debug] players_[self_idx_].receiveHand start" << '\n';
        // players_[self_idx_].receiveHand(cards);

        // const auto& dealt = response.cards();
        // std::cout << "dealt.size(): " << dealt.size() << '\n';
        // for (const auto& [k, v] : dealt) {
        //     std::cout << "[Debug] Player ID: " << k << " hand size: " << v.cards_size() << "\n";
        // }
        // auto it = dealt.find(self_idx_);

        // if (it == dealt.end()) {
        //     std::cerr << "[Error] 找不到自己 player " << self_idx_ << " 的手牌\n";
        //     return;
        // }

        // const gen::Hand& hand = it->second;
        // std::vector<Card> cards;
        // for (const auto& proto_card : hand.cards()) {
        //     cards.push_back(fromProtoCard(proto_card));
        //     std::cout << fromProtoCard(proto_card).toString() << " ";
        // }
        // std::cout << '\n';
        // players_[self_idx_].receiveHand(cards);
    }
    
    Card GameClient::fromProtoCard(const gen::Card& proto_card) {
        Card card;
        card.rank = proto_card.rank();
        card.suit = stringToSuit(proto_card.suit()); // 例如 "spades" -> Suit::Spades
        return card;
    }

    Suit GameClient::stringToSuit(const std::string& suit_str) {
        if (suit_str == "spades")   return Suit::Spades;
        if (suit_str == "hearts")   return Suit::Hearts;
        if (suit_str == "diamonds") return Suit::Diamonds;
        if (suit_str == "clubs")    return Suit::Clubs;
        return Suit::Spades;
    }




    /**
     * class GameClient
     * 
     * old version for test
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
    //             std::cout << "[Fake Server] 合法出牌！你出了：";
    //             for (const auto& c : selected) std::cout << c.toString() << " ";
    //             std::cout << "\n";

    //             removeCards(selected); // 從手牌移除
    //         } else {
    //             std::cout << "[Fake Server] 非法出牌，請重新輸入。\n";
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