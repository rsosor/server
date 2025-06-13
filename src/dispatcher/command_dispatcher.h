#pragma once

#include "player/player_session.h"

#include <string>

class CommandDispatcher {
public:
    static CommandDispatcher& get() {
        static CommandDispatcher instance;
        return instance;
    }
    void dispatch(PlayerSession& session, const std::string& cmd);
};