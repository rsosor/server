#include <memory>
#include <iostream>

#include "conn/connection_layer.h"
#include "game_room/game_room.h"
#include "player/player.h"

int main() {
    // 建立4個玩家
    Player p1(1, "Alice");
    Player p2(2, "Bob");
    Player p3(3, "Carol");
    Player p4(4, "Dave");
    std::vector<Player*> players = {&p1, &p2, &p3, &p4};

    // 建立遊戲房間
    auto game_room = std::make_shared<GameRoom>(players);
    game_room->start();

    // 建立連線層
    ConnectionLayer conn(game_room);

    // 模擬收到玩家1出牌的訊息
    std::string msg = R"({"type":"play_card","cards":["3D","3S"]})";
    conn.handle_message(1, msg);

    return 0;
}
