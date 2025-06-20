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
        std::cout << "RPC Server is listening on：" << grpc_addr << '\n';

        server->Wait();
    }

    Status BigTwoServiceImpl::StartGame(ServerContext* context, 
        const gen::StartGameRequest* request, 
        gen::StartGameResponse* response) {
        response->set_success(true);
        response->set_message("Game Started.");
        return Status::OK;
    }

    Status BigTwoServiceImpl::JoinGame(ServerContext* context, const JoinRequest* request, JoinResponse* response) {
        // 使用 register_player 來註冊新玩家，會自動分配 ID 並新增至 players_
        int id = game_data.register_player(request->player_name());

        std::cout << "Player joined: " << request->player_name() << " (ID: " << id << ")\n";

        Player player(1, request->player_name());
        // response->set_player_id(next_client_id); // 模擬一個固定玩家 ID
        response->set_player_id(id);
        response->set_message("Welcome, " + request->player_name());

        // 模擬發牌
        // Card* card1 = response->add_hand();
        // card1->set_suit("spades");
        // card1->set_rank(14); // A

        // Card* card2 = response->add_hand();
        // card2->set_suit("hearts");
        // card2->set_rank(13); // K

        return Status::OK;
    }

    Status BigTwoServiceImpl::PlayCards(ServerContext* context, const PlayRequest* request, PlayResponse* response) {
        std::cout << "Player " << request->player_id() << " played " << request->cards_size() << " card(s)" << '\n';

        for (const auto& card : request->cards()) {
            std::cout << "  - " << card.suit() << " " << card.rank() << '\n';
        }

        response->set_success(true);
        response->set_message("Play accepted");

        return Status::OK;
    }

    Status BigTwoServiceImpl::Pass(ServerContext* context, const PassRequest* request, PassResponse* response) {
        std::cout << "Player " << request->player_id() << " passed." << '\n';

        response->set_success(true);
        response->set_message("You passed");

        return Status::OK;
    }

    // 根據玩家 ID 查詢他自己的手牌並回傳
    Status BigTwoServiceImpl::GetGameState(ServerContext* context, const GameStateRequest* request, GameStateResponse* response) {
        // 提供真正的玩家 ID
        int player_id = request->player_id();

        std::cout << "Sending game state to player " << request->player_id() << '\n';

        // 提供真正的手牌
        auto hand = game_data.get_player_hand(player_id);
        for (const auto& c : hand) {
            auto* proto_card = response->add_hand();
            toProtoCard(c, proto_card);
        }

        // 模擬手牌資訊
        // Card* card1 = response->add_hand();
        // card1->set_suit("diamonds");
        // card1->set_rank(10);

        // Card* card2 = response->add_hand();
        // card2->set_suit("clubs");
        // card2->set_rank(11);

        // 模擬上一次出牌資訊
        // Card* last = response->add_last_played_cards();
        // last->set_suit("hearts");
        // last->set_rank(15); // 2

        response->set_current_turn_player_id(2);
        response->set_phase("Playing");

        return Status::OK;
    }

    Status BigTwoServiceImpl::Chat(ServerContext* context, const ChatRequest* request, ChatResponse* response) {
        std::cout << "Player " << request->player_id() << " says: " << request->message() << '\n';

        response->set_success(true);
        return Status::OK;
    }

    Status BigTwoTableServiceImpl::Deal(ServerContext* context,
                        const gen::DealRequest* request,
                        gen::DealResponse* response) {
        // c version
        // int* shuffle_deck = fy_shuffle();
        // for (int i = 0; i < 52; ++i)
        //     std::cout << shuffle_deck[i] << '\n';

        // modern c++ version
        std::vector<int> shuffle_deck = fy_shuffle();
        for (auto& c_idx : shuffle_deck) {
            int suit = c_idx / 13;
            int rank = c_idx % 13 + 3;  // 0-12 + 3
            std::cout << "c_idx: " << c_idx << '\n';
            std::cout << "suit: " << suit << '\n';
            std::cout << "rank: " << rank << '\n';
        }

        int players_count = request->player_ids_size();
        if (players_count == 0) {
            // free(shuffle_deck);  // c version
            return Status(grpc::StatusCode::INVALID_ARGUMENT, "No players to deal");
        }

        int cards_per_player = 52 / players_count;
        for (int i = 0; i < players_count; ++i) {
            gen::Hand hand;
            int player_id = request->player_ids(i);
            for (int j = 0; j < cards_per_player; ++j) {
                int card_index = shuffle_deck[i * cards_per_player + j];
                toProtoCard(fromIndex(card_index), hand.add_cards());

                // gen::Card* card = hand.add_cards();
                // // 花色轉換
                // int suit_idx = card_index / 13;
                // switch (suit_idx) {
                //     case 0 : card->set_suit("spades"); break;
                //     case 1 : card->set_suit("hearts"); break;
                //     case 2 : card->set_suit("diamonds"); break;
                //     case 3 : card->set_suit("clubs"); break;
                //     default: card->set_suit("unknown"); break;
                // }
                // // 點數 1-13
                // int rank = (card_index % 13) + 1;
                // card->set_rank(rank);
            }
            (*response->mutable_dealt_hands())[player_id] = hand;
        }
        // free(shuffle_deck); // c version
        return Status::OK;
    }

    Status BigTwoTableServiceImpl::ValidatePlay(ServerContext* context,
                            const gen::PlayValidationRequest* request,
                            gen::PlayValidationResponse* response) {
        if (request->cards_size() > 0) {
            response->set_is_valid(true);
            response->set_message("Valid.");
        } else {
            response->set_is_valid(false);
            response->set_message("Invalid.");
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