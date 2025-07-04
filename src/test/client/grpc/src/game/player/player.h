#pragma once

#include "game/card/card.h"
#include "grpc/bigtwo_client.h"
#include "bigtwo.grpc.pb.h"

#include <vector>
#include <string>
#include <iostream>

namespace gen = rsosor::generated;

class Player {
public:
    Player();
    
    int getId();

    void setId(int player_id);

    int getHandSize();

    void setHandSize(int hand_size);

    // 單機版四人
    void receiveCard(const Card& card);

    // 連線版本
    void receiveHand(const std::vector<Card>& cards);

    void sortHand();

    void showHand(std::ostream& os) const;

    inline const std::vector<Card>& getHand() const { return hand_; }

    // 玩家出牌回合，回傳出的牌
    std::vector<Card> playTurn(int player_id, const std::string& room_id);
    
    bool is_hand_empty() const;

    void setGrpcClient(BigTwoClient* s);

private:
    // playTurn need
    std::vector<Card> parseInput(const std::string& input) const;
    
    // playTurn need
    bool ownsCards(const std::vector<Card>& selected) const;
    
    // playTurn need
    void removeCards(const std::vector<Card>& selected);
    
    // parseInput need
    Card parseCard(const std::string& token) const;
    
    std::string suitToString(Suit suit);

    // parseCard need
    int parseRank(const std::string& s) const;

    int player_id_ = -1;
    BigTwoClient* grpc_client_;
    std::vector<Card> hand_;
    int hand_size_;
};