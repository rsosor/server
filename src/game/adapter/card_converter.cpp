#include "game/adapter/card_converter.h"

namespace card = rsosor::game::card;
namespace gen = rsosor::generated;

namespace rsosor {
namespace game {
namespace adapter {

    card::Card fromIndex(int index) {
        if (index < 0 || index >= 52) {
            throw std::out_of_range("Invalid card index");
        }
        card::Suit suit = static_cast<card::Suit>(index / 13);
        int rank = (index % 13) + 3;
        return {suit, rank};
    }

    void toProtoCard(const card::Card& from, gen::Card* to) {
        // enum to string
        switch (from.suit) {
            case card::Suit::Spades:   to->set_suit("spades"); break;
            case card::Suit::Hearts:   to->set_suit("hearts"); break;
            case card::Suit::Diamonds: to->set_suit("diamonds"); break;
            case card::Suit::Clubs:    to->set_suit("clubs"); break;
            default:                   to->set_suit("unknown"); break;
        }

        to->set_rank(from.rank);
    }

    gen::Card toProtoCard(const card::Card& from) {
        gen::Card c;
        toProtoCard(from, &c);
        return c;
    }


    card::Card toCard(const gen::Card& grpc_card) {
        card::Suit suit = card::Suit::Spades;   // default

        const std::string& s = grpc_card.suit();
        if (s == "spades")          suit = card::Suit::Spades;
        else if (s == "hearts")     suit = card::Suit::Hearts;
        else if (s == "diamonds")   suit = card::Suit::Diamonds;
        else if (s == "clubs")      suit = card::Suit::Clubs;
        else throw std::invalid_argument("Unknown suit: " + s);

        int rank = grpc_card.rank();

        return card::Card(suit, rank);
    }

}   // adapter
}   // game
}   // rsosor