#pragma once
#include <boost/asio.hpp>
#include "Session.h"
using boost::asio::ip::tcp;
class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "New Client Connected!" << std::endl;
                    std::make_shared<Session>(std::move(socket))->Start();
                }
				DoAccept(); // Wait for the next connection
            });
    }

    tcp::acceptor acceptor_;
};