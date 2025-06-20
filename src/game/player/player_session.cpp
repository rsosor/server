#include "game/player/player_session.h"
#include "dispatcher/command_dispatcher.h"

#include <iostream>

using namespace rsosor::game::game_room;
using namespace rsosor::dispatcher;

namespace rsosor {
namespace game {
namespace player {

    PlayerSession::PlayerSession(boost::asio::ip::tcp::socket socket)
        : socket_(std::move(socket)), state_(PlayerState::WaitingLogin) {}

    PlayerState PlayerSession::get_state() const {
        return state_;
    }

    std::shared_ptr<Player> PlayerSession::get_player() const {
        return player_;
    }

    int PlayerSession::get_player_id() const {
        return player_->id;
    }

    void PlayerSession::set_player(std::shared_ptr<Player> p) {
        player_ = p;
    }

    // void PlayerSession::set_player_id() {
    //     player_id_ = 1;
    // }

    GameRoom* PlayerSession::get_game_room() const {
        return game_room_;
    }

    void PlayerSession::change_state(PlayerState new_state) {
        state_ = new_state;
    }

    void PlayerSession::handle_login(const std::string& username, const std::string& password) {
        // 簡化驗證
        if (password == "letmein") {
            name_ = username;
            change_state(PlayerState::InLobby);
            std::cout << "* Player " << name_ << " 登入成功，進入大廳\n";
        } else {
            std::cout << "* Player " << username << " 登入失敗\n";
            change_state(PlayerState::Disconnected);
            socket_.close();
        }
    }

    void PlayerSession::handle_command(const std::string& cmd) {
        CommandDispatcher::get().dispatch(*this, cmd);
    }

    const std::string& PlayerSession::name() const {
        return name_;
    }

    boost::asio::ip::tcp::socket& PlayerSession::socket() {
        return socket_;
    }

}   // player
}   // game
}   // rsosor