#include "conn/connection_layer.h"

using namespace rsosor::game::player;

void ConnectionLayer::handle_message(int player_id, const std::string& msg) {
    auto json_msg = json::parse(msg);

    if (json_msg["type"] == "play_card") {
        // cards 是一個 string 陣列，轉成 vector<string>
        std::vector<std::string> cards = json_msg["cards"].get<std::vector<std::string>>();
        std::shared_ptr<Player> p = game_room->get_player_by_id(player_id);
        game_room->handle_play_card(p, cards);
    }
}
