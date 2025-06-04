#include "game_room/game_room.h"
#include <iostream>

GameRoom::GameRoom(const std::vector<Player*>& players) : players(players), current_turn(0) {}

void GameRoom::start() {
    std::cout << "Game started with " << players.size() << " players.\n";
    // 初始化手牌、回合等
}

bool GameRoom::is_valid_play(Player* p, const std::vector<std::string>& cards) {
    // TODO: 呼叫 card engine 判斷牌是否合法
    return true;
}

void GameRoom::handle_play_card(Player* p, const std::vector<std::string>& cards) {
    if (!is_valid_play(p, cards)) {
        std::cout << "Invalid play!\n";
        return;
    }
    last_played_cards = cards;
    std::cout << "Player played cards.\n";
    // 更新遊戲狀態、換下一位玩家回合
}

Player* GameRoom::get_player_by_id(int id) {
    for (Player* p : players) {
        if (p->id == id) return p;
    }
    return nullptr;
}

