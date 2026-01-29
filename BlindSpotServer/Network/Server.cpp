#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Session.h"
#include "ServerPacketHandler.h"
#include "../Managers/SessionManager.h"
#include "../Managers/RoomManager.h"
#include "../Managers/PlayerManager.h"
#include "../Managers/AuthManager.h"
#include "../Services/AuthService.h"
#include "../Services/RoomService.h"
#include "Server.h"

Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    sessionMgr_ = std::make_shared<SessionManager>();
    roomMgr_ = std::make_shared<RoomManager>();
    authMgr_ = std::make_shared<AuthManager>();
    playerMgr_ = std::make_shared<PlayerManager>();
    authService_ = std::make_shared<AuthService>(authMgr_, playerMgr_, sessionMgr_);
    roomService_ = std::make_shared<RoomService>(roomMgr_);
    packetHandler_ = std::make_shared<ServerPacketHandler>(authService_, roomService_);

    packetHandler_->Init();

    std::cout << "Server initialized on port " << port << std::endl;

    DoAccept();
}

void Server::DoAccept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "New Client Connected!" << std::endl;

                auto session = std::make_shared<Session>(
                    std::move(socket),
                    sessionMgr_,
                    packetHandler_
                );

                session->Start();
            }
            DoAccept(); // Wait for the next connection
        });
}