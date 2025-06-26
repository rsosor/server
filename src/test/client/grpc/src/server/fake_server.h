#pragma once

#include "game/game_client.h"
#include "server/rule.h"

#include <vector>

struct PlayResult {
    bool success;
    std::string message;
    bool round_ended;
    int next_player_id;
};

class FakeServer {
public:
    static bool validatePlay(const std::vector<Card>& curr_cards, int curr_player_id);
    static bool is_stronger(const std::vector<Card>& curr_cards, const std::vector<Card>& prev_cards);
private:
    // bool compare_card(const Card& a, const Card& b);
    /**
     * GameState 先放這
     */
    static std::vector<Card> last_cards;
    static HandType last_hand_type;
    static int last_player_id;
    static int pass_count;
    static int total_players;
};