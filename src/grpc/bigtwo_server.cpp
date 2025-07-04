#include "grpc/bigtwo_server.h"
#include "game/player/player.h"
#include "conn/connection.h"
#include "game/adapter/card_converter.h"

#include <iostream>
#include <memory>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using namespace rsosor::generated;
using namespace rsosor::game::player;
using namespace rsosor::game::adapter;

namespace rsosor {
namespace grpc_api {

    RoomRegistry global_room_registry;

    std::vector<int> fy_shuffle() {
        std::vector<int> deck(52);
        for (int i = 0; i < 52; ++i)
            deck[i] = i;
        for (int i = 0; i < 52; ++i) {
            int rand_to_swap = rand() % (52 - i);
            std::swap(deck[51 - i], deck[rand_to_swap]);
        }
        return deck;
    }

    // C
    // int* fy_shuffle() {
    //     int *deck = static_cast<int*>(malloc(52*sizeof(int)));
    //     for (int i = 0; i < 52; i++) {
    //         deck[i] = i;        
    //     }
    //     for (int i = 0; i < 52; i++) {
    //         int rand_to_swap = rand()%(52-i);
    //         int tmp = deck[51-i];
    //         deck[51-i] = deck[rand_to_swap];
    //         deck[rand_to_swap] = tmp;
    //     } 
    //     return deck;
    // }

    /**
     * grpc::Server 自己內部就是多執行緒的
     * gRPC C++ 內部會自動使用一個 thread pool 來處理 RPC 請求。
     * 
     * grpc::ServerBuilder::BuildAndStart() 啟動 server 後
     * 它會自動啟動多個 worker threads 去處理 HandleRpcs()
     */
    void RunGrpcServer(const std::string& grpc_addr) {
        BigTwoServiceImpl game_service;
        BigTwoTableServiceImpl table_service;
        // auto table_service = std::make_shared<BigTwoTableServiceImpl>();
        // BigTwoServiceImpl game_service(table_service);
        BigTwoManagementServiceImpl management_service;

        ServerBuilder builder;
        builder.AddListeningPort(grpc_addr, grpc::InsecureServerCredentials());
        builder.RegisterService(&game_service);
        builder.RegisterService(&table_service);
        builder.RegisterService(&management_service);

        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "[Server] RPC Server is listening on：" << grpc_addr << '\n';

        server->Wait();
    }

    Status BigTwoServiceImpl::Login(ServerContext* context, 
            const gen::LoginRequest* request, 
            gen::LoginResponse* response) {
        static std::atomic<int> next_player_id{1};
        int player_id = next_player_id++;
        response->set_success(true);
        response->set_player_id(player_id);
        response->set_message("Game Started.");
        return Status::OK;
    }

    Status BigTwoServiceImpl::JoinGame(ServerContext* context, const JoinRequest* request, JoinResponse* response) {
        int player_id = request->player_id();
        std::string player_name = request->player_name();
        std::string room_id = request->room_id();

        std::cout << "[Debug] Added player " << player_id << " to room " << room_id << '\n';

        // 加入房間的玩家列表（要同步保證）
        // {
        //     std::lock_guard<std::mutex> lock(mutex_);  // 假設 mutex 是成員變數
        //     room_to_players_[room_id].push_back(player_id);
        // }
        {
            std::lock_guard<std::mutex> lock(global_room_registry.mutex);
            global_room_registry.room_to_players[room_id].push_back(player_id);
        }

        bool auth = false;
        // 驗證可否加入房間
        auth = true;

        if (auth) {
            game_data.register_player(player_id, player_name);

            std::cout << "[Server] Player joined: " << player_name << " (ID: " << player_id << ")\n";

            response->set_success(1);
            response->set_player_id(player_id);
            response->set_player_name(player_name);
            response->set_message("Welcome, " + player_name);
        } else {
            response->set_success(0);
        }

        return Status::OK;
    }

    Status BigTwoServiceImpl::PlayCards(ServerContext* context, const PlayRequest* request, PlayResponse* response) {
        std::cout << "[Server] Player " << request->player_id() << " played " << request->cards_size() << " card(s)" << '\n';

        for (const auto& card : request->cards()) {
            std::cout << "  - " << card.suit() << " " << card.rank() << '\n';
        }

        response->set_success(true);
        response->set_message("Play accepted");

        return Status::OK;
    }

    Status BigTwoServiceImpl::Pass(ServerContext* context, const PassRequest* request, PassResponse* response) {
        std::cout << "[Server] Player " << request->player_id() << " passed." << '\n';

        response->set_success(true);
        response->set_message("You passed");

        return Status::OK;
    }

    // 根據玩家 ID 查詢他自己的手牌並回傳
    Status BigTwoServiceImpl::GetGameState(ServerContext* context, const GameStateRequest* request, GameStateResponse* response) {
        int player_id = request->player_id();
        std::lock_guard<std::mutex> lock(global_room_registry.mutex);

        // 取得該玩家手牌，回傳給自己
        if (global_room_registry.player_hands.find(player_id) != global_room_registry.player_hands.end()) {
            for (const auto& card : global_room_registry.player_hands[player_id]) {
                *response->add_hand() = card;
            }
        }

        // 填寫遊戲狀態資訊，例如目前回合玩家、遊戲階段等
        response->set_current_turn_player_id(current_turn_player_id_);
        response->set_phase(current_phase_);

        // 填寫其他玩家手牌數量 (示範用)
        for (const auto& [pid, hand] : global_room_registry.player_hands) {
            (*response->mutable_hand_sizes())[pid] = hand.size();
        }

        // 回傳最近出的牌 (示範)
        for (const auto& card : last_played_cards_) {
            *response->add_last_played_cards() = card;
        }

        std::cout << "[Server] Sending game state to player " << request->player_id() << '\n';

        return Status::OK;
    }

    Status BigTwoServiceImpl::Chat(ServerContext* context, const ChatRequest* request, ChatResponse* response) {
        std::cout << "[Server] Player " << request->player_id() << " says: " << request->message() << '\n';

        response->set_success(true);
        return Status::OK;
    }

    Status BigTwoTableServiceImpl::Deal(ServerContext* context,
                        const gen::DealRequest* request,
                        gen::DealResponse* response) {
        // c version
        // int* shuffle_deck = fy_shuffle();
        // std::cout << "[Server] "
        // for (int i = 0; i < 52; ++i)
        //     std::cout << shuffle_deck[i] << '\n';

        // 確認玩家數
        int player_id = request->player_id();
        std::string room_id = request->room_id();
        std::cout << "player_id: " << player_id  << '\n';
        std::cout << "room_id: " << room_id  << '\n';
        // 確認房間及玩家
        if (global_room_registry.room_to_players.find(room_id) == global_room_registry.room_to_players.end()) {
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Room not found");
        }

        auto& players = global_room_registry.room_to_players[room_id];
        if (std::find(players.begin(), players.end(), player_id) == players.end()) {
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Player not in room");
        }

        std::cout << "[Debug] Room " << room_id << " has " << players.size() << " players\n";

        if (players.size() < 4) {
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "Not enough players to deal");
        }

        // 用 server 端的房間資訊取得所有玩家id
        // std::vector<int> player_ids = getAllPlayersInRoom(room_id);

        // int players_count = player_ids.size();
        // if (players_count == 0) {
        //     return Status(grpc::StatusCode::INVALID_ARGUMENT, "No players to deal");
        // }

        // 洗牌，發牌
        // modern c++ version
        std::vector<int> shuffle_deck = fy_shuffle();
        // 計算每位玩家分幾張牌
        int cards_per_player = 52 / players.size();

        // 對每位玩家分牌
        for (int i = 0; i < players.size(); ++i) {
            int pid = players[i];
            global_room_registry.player_hands[pid].clear();
            for (int j = 0; j < cards_per_player; ++j) {
                int card_index = shuffle_deck[i * cards_per_player + j];
                gen::Card card_proto;
                // 把點數卡牌索引轉成你的 Card protobug 格式
                toProtoCard(fromIndex(card_index), &card_proto);
                global_room_registry.player_hands[pid].push_back(card_proto);
            }
        }
        
        // 回傳呼叫玩家手牌
        auto& hand = global_room_registry.player_hands[player_id];
        for (const auto& card : hand) {
            *response->add_cards() = card;
        }
        
        // free(shuffle_deck); // c version
        return Status::OK;
    }

    Status BigTwoTableServiceImpl::ValidatePlay(ServerContext* context,
                            const gen::PlayValidationRequest* request,
                            gen::PlayValidationResponse* response) {
        std::string room_id = request->room_id();
        int player_id = request->player_id();
        std::cout << "[Server] Player: " << request->player_id() << '\n';
        if (request->cards_size() > 0) {
            response->set_is_valid(true);
            response->set_message("Valid.");
            for (auto& c : request->cards()) {
                std::cout << c.rank() << c.suit() << " ";
            }
            std::cout << '\n';

            gen::GameState state;
            state.set_current_turn_player_id((player_id + 1) % 4);

            // 記錄這回合出的牌
            for (const auto& card : request->cards()) {
                auto* played_card = state.add_last_played_cards();
                played_card->set_rank(card.rank());
                played_card->set_suit(card.suit());
            }

            // 你可以加上其他房間狀態資訊，例如剩下幾位玩家、手牌數等等

            // ---- 呼叫推播函式 ----
            BroadcastGameState(room_id, state);

            std::cout << "[Server] 推播新狀態給房間: " << room_id << '\n';
        } else {
            response->set_is_valid(false);
            response->set_message("Invalid.");
            std::cout << "[Server] 不合法的出牌 " << ++cnt << " 次" << '\n';
            std::cout  << '\n';
        }
        return Status::OK;
    }

    Status BigTwoTableServiceImpl::HandleTimeout(ServerContext* context,
                            const gen::TimeoutRequest* request,
                            gen::TimeoutResponse* response) {
        response->set_success(true);
        response->set_message("Timeout processed");
        return Status::OK;
    }

    Status BigTwoTableServiceImpl::Score(ServerContext* context,
                    const gen::ScoreRequest* request,
                    gen::ScoreResponse* response) {
        for (auto p : request->player_ids()) {
            (*response->mutable_scores())[p] = 100;
        }
        return Status::OK;
    }

    // 違反 grpc 限制：ServerWriter 只能在呼叫此函式的同一執行緒中使用
    Status BigTwoTableServiceImpl::SubscribeGameState(ServerContext* context,
                          const gen::GameRoomId* request,
                          grpc::ServerWriter<gen::GameState>* writer) {
        std::cout << "[Server] SubscribeGameState called for room: " << request->room_id() << '\n';
        std::string room_id = request->room_id();
        auto sub = std::make_shared<Subscriber>();

        {
            std::lock_guard<std::mutex> lock(global_room_registry.room_mutex);
            global_room_registry.game_rooms[room_id].subscribers.push_back(sub);
        }
        // {
        //     std::lock_guard<std::mutex> lock(global_room_registry.mutex);
        //     global_room_registry.room_to_players[room_id].push_back(id);
        // }

        while (!context->IsCancelled() && sub->active) {
            std::unique_lock<std::mutex> lock(sub->mtx);
            sub->cv.wait(lock, [&]{ return !sub->queue.empty() || !sub->active; });

            while (!sub->queue.empty()) {
                const auto& state = sub->queue.front();
                if (!writer->Write(state)) {
                    // client disconnected
                    sub->active = false;
                    break;
                }
                sub->queue.pop();
            }
        }

        // 移除訂閱者
        {
            std::lock_guard<std::mutex> lock(global_room_registry.room_mutex);
            auto& subs = global_room_registry.game_rooms[room_id].subscribers;
            subs.erase(std::remove(subs.begin(), subs.end(), sub), subs.end());
        }

        return grpc::Status::OK;
    }

    void BigTwoTableServiceImpl::BroadcastGameState(const std::string& room_id, const gen::GameState& state) {
        std::lock_guard<std::mutex> lock(global_room_registry.room_mutex);
        auto it = global_room_registry.game_rooms.find(room_id);
        if (it == global_room_registry.game_rooms.end()) return;

        for (auto& sub : it->second.subscribers) {
            {
                std::lock_guard<std::mutex> lk(sub->mtx);
                sub->queue.push(state);
            }
            sub->cv.notify_one();
        }
    }

    std::vector<int> BigTwoTableServiceImpl::getAllPlayersInRoom(const std::string& room_id) {
        std::lock_guard<std::mutex> lock(global_room_registry.mutex);
        auto it = global_room_registry.room_to_players.find(room_id);
        if (it != global_room_registry.room_to_players.end()) {
            return it->second;
        }
        return {}; // 沒找到房間就回空 vector
    }

    Status BigTwoManagementServiceImpl::GetRanking(ServerContext* context,
                            const gen::RankingRequest* request,
                            gen::RankingResponse* response) {
        for (int i = 0; i < request->top(); ++i) {
            gen::PlayerRanking* r = response->add_ranking();
            r->set_player_id(i);
            r->set_player_name("Play " + std::to_string(i));
            r->set_wins(10);
        }
        return Status::OK;
    }

    Status BigTwoManagementServiceImpl::GetPlayerWinRate(ServerContext* context,
                                    const gen::PlayerRateRequest* request,
                                    gen::PlayerRateResponse* response) {
        response->set_total_games(100);
        response->set_wins(60);
        response->set_win_ratio(60.0);
        return Status::OK;
    }

    Status BigTwoManagementServiceImpl::GetActivityLog(ServerContext* context,
                                const gen::ActivityLogRequest* request,
                                gen::ActivityLogResponse* response) {
        for (int i = 0; i < request->limit(); ++i) {
            gen::ActivityEntry* entry = response->add_entries();
            entry->set_game_id(i);
            entry->set_winner(1);
            entry->set_timestamp("2025-06-14 18:00:00.");
        }
        return Status::OK;
    }

}   // grpc_api
}   // rsosor