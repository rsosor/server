#include "game_room/game_room.h"

#include <iostream>

GameRoom::GameRoom(const std::vector<Player*>& players, boost::asio::io_context& io) : players(players), io_context_(io), current_turn(0) {}

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
    if (p->turn_timer) {
        p->turn_timer->cancel();
    }
}

Player* GameRoom::get_player_by_id(int id) {
    for (Player* p : players) {
        if (p->id == id) return p;
    }
    return nullptr;
}

void GameRoom::start_turn_timer(Player& player) {
    // player->turn_timer = std::make_unique<boost::asio::steady_timer>(io_context);
    // player->turn_timer->expires_after(std::chrono::seconds(30));
    // player->turn_timer->async_wait([this, player](const boost::system::error_code& ec) {
    //     if (!ec) {
    //         std::cout << "Player " << player->name << " timeout!\n";
    //         handle_timeout(player);  // 自動棄牌或跳過
    //     }
    // });
    player.turn_timer = std::make_unique<boost::asio::steady_timer>(io_context_);
    player.turn_timer->expires_after(std::chrono::seconds(30));

    player.turn_timer->async_wait([&player](const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "[TIMEOUT] 玩家 " << player.name << " 超時，系統自動跳過" << '\n';
        } else if (ec == boost::asio::error::operation_aborted) {
            std::cout << "[INFO] 玩家 " << player.name << " 準時出牌，計時器已取消" << '\n';
        }
    });
}

void GameRoom::handle_player_move(Player& player) {
    if (player.turn_timer) {
        player.turn_timer->cancel();
    }

    // 處理出牌邏輯
    std::cout << player.name << " 出牌成功！\n";
}


