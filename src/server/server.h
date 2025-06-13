#pragma once

#include "conn/connection.h"

#include <boost/asio.hpp>

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_context& io_context_;
};
