#pragma once

#include "game/player/player.h"
#include "game/game_room/game_room.h"

#include <string>

#include <boost/asio.hpp>

namespace rsosor {
namespace game {
namespace player {

    enum class PlayerState {
        WaitingLogin,
        InLobby,
        InGame,
        WaitingMove,
        Disconnected
    };

    /**
     * 當一個類別是用 shared_ptr 管理的時候，你無法直接在內部寫 shared_ptr<PlayerSession>(this)，那樣會導致兩個獨立的 shared_ptr 指向同一個物件，最終會 double delete。
     * 為了讓物件本身能夠產出 shared_ptr<self>

    * 所以要寫成這樣：
    * 
    * auto self = shared_from_this();  // 這樣才安全
    * 這就是 enable_shared_from_this 的作用。
    * 
    * 🔹 常見應用場景：
    * 在 Boost.Asio 的非同步回呼裡（async read/write），你會看到這種寫法：
    * 
    * auto self = shared_from_this();
    * boost::asio::async_read(socket_, buffer_,
    *     [self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
    *         self->do_read();  // 保證物件在回呼裡還活著
    *     });
    * 這可以保證 this 不會在 callback 還沒跑完時被提前釋放。
    */
    class PlayerSession : public std::enable_shared_from_this<PlayerSession> {
    public:
        PlayerSession(boost::asio::ip::tcp::socket socket);

        PlayerState get_state() const;
        std::shared_ptr<Player> get_player() const;
        int get_player_id() const;
        void set_player(std::shared_ptr<Player> p);
        // void set_player_id();
        rsosor::game::game_room::GameRoom* get_game_room() const;
        void set_game_room(rsosor::game::game_room::GameRoom* room);
        void change_state(PlayerState new_state);
        void handle_login(const std::string& username, const std::string& password);
        void handle_command(const std::string& cmd);
        const std::string& name() const;
        boost::asio::ip::tcp::socket& socket();
    private:
        // int player_id_;
        std::shared_ptr<Player> player_ = nullptr;
        PlayerState state_;
        rsosor::game::game_room::GameRoom* game_room_ = nullptr;
        // 可能還有 game_room 之類資訊
        std::string name_;
        boost::asio::ip::tcp::socket socket_;
    };

}   // player
}   // game
}   // rsosor