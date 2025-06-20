#pragma once

#include "game/card/card.h"
#include "generated/bigtwo.pb.h"

namespace card = rsosor::game::card;
namespace gen = rsosor::generated;

namespace rsosor {
namespace game {
namespace adapter {

    card::Card fromIndex(int index);
    void toProtoCard(const card::Card& from, gen::Card* to);
    gen::Card toProtoCard(const card::Card& from);
    card::Card toCard(const gen::Card& grpc_card);

}   // adapter
}   // game
}   // rsosor