#pragma once

#include "game/player/player_session.h"

#include <string>

namespace player = rsosor::game::player;

namespace rsosor {
namespace dispatcher {

    class CommandDispatcher {
        public:
        static CommandDispatcher& get() {
            static CommandDispatcher instance;
            return instance;
        }
        void dispatch(player::PlayerSession& session, const std::string& cmd);
    };

}   // dispatcher
}   // rsosor