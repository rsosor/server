#pragma once

#include "game/card/card.h"
#include "game/card/deck.h"
#include "game/player/player.h"
#include "grpc/bigtwo_client.h"

#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>
#include <array>
#include <memory>

namespace gen = rsosor::generated;

/**
 * 問題類型	                建議
 * 成員是引用 Player&	    必須在初始化列表中立即給值，不可晚設
 * 若要延後初始化	         改成 Player*，在建構子體內指向對的物件
 * 所有 gRPC stub	        僅需設定自己（player_->setGrpcClient(...)）
 */
class GameClient {
public:

    explicit GameClient(std::shared_ptr<grpc::Channel> channel);

    void play();
    void single_play();
    
private:
    void subscribeGameState(const std::string& room_id);
    void dealCards(const std::vector<int>& player_ids);
    void assignHandsToPlayers(const gen::DealResponse& response);
    Card fromProtoCard(const gen::Card& proto_card);
    Suit stringToSuit(const std::string& suit_str);
    // void showHand();

    // std::vector<Card> parseInput(const std::string& input);

    // parseInput need
    // int parseRank(const std::string& s);

    // bool ownsCards(const std::vector<Card>& selected);

    // void removeCards(const std::vector<Card>& selected);

    // std::vector<Card> hand;
    
    BigTwoClient grpc_client_;      // 必須先初始化
    std::array<Player, 4> players_; // 然後建 Player
    Player& player_;                // 指向其中一個
    int self_idx_;                  // 自訂玩家號為 0
    Deck deck_;
};
