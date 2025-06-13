#include "grpc/bigtwo_client.h"

#include <iostream>
#include <memory>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

BigTwoClient::BigTwoClient(std::shared_ptr<Channel> channel)
    : stub_(bigtwo::BigTwoService::NewStub(channel)) {};

// 呼叫 JoinGame RPC
int BigTwoClient::JoinGame(const std::string& player_name) {
    bigtwo::JoinRequest request;
    request.set_player_name(player_name);

    bigtwo::JoinResponse response;
    ClientContext context;

    Status status = stub_->JoinGame(&context, request, &response);

    if (status.ok()) {
        std::cout << "JoinGame success! Player ID: " << response.player_id() << '\n';
        std::cout << "You have " << response.hand_size() << " cards in hand." << '\n';
        for (const auto& card : response.hand()) {
            std::cout << "Card: " << card.suit() << " " << card.rank() << '\n';
        }
        return response.player_id();
    } else {
        std::cerr << "JoinGame RPC failed: " << status.error_message() << '\n';
        return -1;
    }
}

// 呼叫 PlayCards RPC
void BigTwoClient::PlayCards(int player_id, const std::vector<bigtwo::Card>& cards) {
    bigtwo::PlayRequest request;
    request.set_player_id(player_id);
    for (const auto& card : cards) {
        *request.add_cards() = card;
    }

    bigtwo::PlayResponse response;
    ClientContext context;

    Status status = stub_->PlayCards(&context, request, &response);
    if (status.ok()) {
        std::cout << "PlayCards response: " << response.message() << '\n';
    } else {
        std::cerr << "PlayCards RPC failed: " << status.error_message() << '\n';
    }
}

// 呼叫 Pass RPC
void BigTwoClient::Pass(int player_id) {
    bigtwo::PassRequest request;
    request.set_player_id(player_id);

    bigtwo::PassResponse response;
    ClientContext context;

    Status status = stub_->Pass(&context, request, &response);
    if (status.ok()) {
        std::cout << "Pass response: " << response.message() << '\n';
    } else {
        std::cerr << "Pass RPC failed: " << status.error_message() << '\n';
    }
}

// 呼叫 GetGameState RPC
void BigTwoClient::GetGameState(int player_id) {
    bigtwo::GameStateRequest request;
    request.set_player_id(player_id);

    bigtwo::GameStateResponse response;
    ClientContext context;

    Status status = stub_->GetGameState(&context, request, &response);
    if (status.ok()) {
        std::cout << "Game phase: " << response.phase() << '\n';
        std::cout << "Current turn player ID: " << response.current_turn_player_id() << '\n';
        std::cout << "Your hand cards:" << '\n';
        for (const auto& card : response.hand()) {
            std::cout << "  " << card.suit() << " " << card.rank() << '\n';
        }
        std::cout << "Last played cards:" << '\n';
        for (const auto& card : response.last_played_cards()) {
            std::cout << "  " << card.suit() << " " << card.rank() << '\n';
        }
    } else {
        std::cerr << "GetGameState RPC failed: " << status.error_message() << '\n';
    }
}

// 呼叫 Chat RPC
void BigTwoClient::Chat(int player_id, const std::string& message) {
    bigtwo::ChatRequest request;
    request.set_player_id(player_id);
    request.set_message(message);

    bigtwo::ChatResponse response;
    ClientContext context;

    Status status = stub_->Chat(&context, request, &response);
    if (status.ok()) {
        std::cout << "Chat message sent successfully." << '\n';
    } else {
        std::cerr << "Chat RPC failed: " << status.error_message() << '\n';
    }
}
