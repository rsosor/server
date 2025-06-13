#include "dispatcher/command_dispatcher.h"

#include <iostream>

#include <nlohmann/json.hpp>

void CommandDispatcher::dispatch(PlayerSession& session, const std::string& cmd) {
    auto json = nlohmann::json::parse(cmd);
    std::string type = json["type"];
    
    if (type == "login") {
        // 只在 WaitingLogin 狀態才接受
        if (session.get_state() == PlayerState::WaitingLogin) {
            std::string account = json["account"];
            std::string password = json["password"];
            session.handle_login(account, password);
        }
    } else if (type == "play_card") {
        if (session.get_state() == PlayerState::InGame) {
            std::vector<std::string> cards = json["cards"].get<std::vector<std::string>>();
            GameRoom* room = session.get_game_room();
            Player* player_ptr = session.get_player();
            if (room && player_ptr) {
                room->handle_play_card(player_ptr, cards);
            } else {
                std::cout << "Error: player pointer is nullptr." << '\n';
            }
        }
    }
    // 加上更多判斷以防止非法操作
}
