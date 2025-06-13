#include "grpc/bigtwo_client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

int main() {
    BigTwoClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    int player_id = client.JoinGame("Bob");
    if (player_id < 0) return 1;

    // 模擬打出一張牌 (以加入時回傳的第一張牌為例)
    bigtwo::Card card;
    card.set_suit("spades");
    card.set_rank(14); // A

    client.PlayCards(player_id, {card});
    client.Pass(player_id);
    client.GetGameState(player_id);
    client.Chat(player_id, "Hello everyone!");

    return 0;
}