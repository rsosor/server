#pragma once

#include "game/card/card.h"

#include <vector>
#include <string>
#include <iostream>

class Player {
public:
    void receiveCard(const Card& card);

    void sortHand();

    void showHand(std::ostream& os) const;

    inline const std::vector<Card>& getHand() const { return hand; }

    // 玩家出牌回合，回傳出的牌
    std::vector<Card> playTurn(int player_id);
    
    bool is_hand_empty() const;

private:
    // playTurn need
    std::vector<Card> parseInput(const std::string& input) const;

    // playTurn need
    bool ownsCards(const std::vector<Card>& selected) const;

    // playTurn need
    void removeCards(const std::vector<Card>& selected);
    
    // parseInput need
    Card parseCard(const std::string& token) const;
    
    // parseCard need
    int parseRank(const std::string& s) const;

    std::vector<Card> hand;
};