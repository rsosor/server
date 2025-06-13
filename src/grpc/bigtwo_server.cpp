#include "grpc/bigtwo_server.h"

#include <iostream>
#include <memory>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using bigtwo::BigTwoService;
using bigtwo::Card;
using bigtwo::PlayRequest;
using bigtwo::PlayResponse;
using bigtwo::PassRequest;
using bigtwo::PassResponse;
using bigtwo::JoinRequest;
using bigtwo::JoinResponse;
using bigtwo::GameStateRequest;
using bigtwo::GameStateResponse;
using bigtwo::ChatRequest;
using bigtwo::ChatResponse;

Status GameServiceImpl::JoinGame(ServerContext* context, const JoinRequest* request, JoinResponse* response) {
    std::cout << "Player joined: " << request->player_name() << std::endl;

    response->set_player_id(1); // 模擬一個固定玩家 ID
    response->set_message("Welcome, " + request->player_name());

    // 模擬發牌
    Card* card1 = response->add_hand();
    card1->set_suit("spades");
    card1->set_rank(14); // A

    Card* card2 = response->add_hand();
    card2->set_suit("hearts");
    card2->set_rank(13); // K

    return Status::OK;
}

Status GameServiceImpl::PlayCards(ServerContext* context, const PlayRequest* request, PlayResponse* response) {
    std::cout << "Player " << request->player_id() << " played " << request->cards_size() << " card(s)" << std::endl;

    for (const auto& card : request->cards()) {
        std::cout << "  - " << card.suit() << " " << card.rank() << std::endl;
    }

    response->set_success(true);
    response->set_message("Play accepted");

    return Status::OK;
}

Status GameServiceImpl::Pass(ServerContext* context, const PassRequest* request, PassResponse* response) {
    std::cout << "Player " << request->player_id() << " passed." << std::endl;

    response->set_success(true);
    response->set_message("You passed");

    return Status::OK;
}

Status GameServiceImpl::GetGameState(ServerContext* context, const GameStateRequest* request, GameStateResponse* response) {
    std::cout << "Sending game state to player " << request->player_id() << std::endl;

    // 模擬手牌
    Card* card1 = response->add_hand();
    card1->set_suit("diamonds");
    card1->set_rank(10);

    Card* card2 = response->add_hand();
    card2->set_suit("clubs");
    card2->set_rank(11);

    // 模擬上一次出牌
    Card* last = response->add_last_played_cards();
    last->set_suit("hearts");
    last->set_rank(15); // 2

    response->set_current_turn_player_id(2);
    response->set_phase("Playing");

    return Status::OK;
}

Status GameServiceImpl::Chat(ServerContext* context, const ChatRequest* request, ChatResponse* response) {
    std::cout << "Player " << request->player_id() << " says: " << request->message() << std::endl;

    response->set_success(true);
    return Status::OK;
}

void RunGrpcServer() {
  std::string server_address("0.0.0.0:50051");
  GameServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}
