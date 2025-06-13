#pragma once

#include "player/player.h"

#include <unordered_map>
#include <memory>

#include <boost/asio.hpp>

extern int next_client_id;
extern std::unordered_map<int, Player> player_registry;
std::string get_time_string(std::time_t);
void handle_client(std::shared_ptr<boost::asio::ip::tcp::socket>);
