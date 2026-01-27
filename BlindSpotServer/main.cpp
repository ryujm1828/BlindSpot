#include <iostream>
#include <boost/asio.hpp>
#include "Network/Server.h"
#include "Network/ServerPacketHandler.h"

constexpr short PORT = 7777;

int main() {
	ServerPacketHandler::Init(); // Initialize packet handlers

    try {
        boost::asio::io_context io_context;

        std::cout << "Server starting on port "<< PORT << "..." << std::endl;
        Server s(io_context, PORT);

		io_context.run(); // Start the server event loop
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}