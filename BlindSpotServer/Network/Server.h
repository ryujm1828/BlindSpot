#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>

class SessionManager;
class RoomManager;
class PlayerManager;
class AuthManager;
class AuthService;
class RoomService;
class ServerPacketHandler;
class Session;

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);

private:
    void DoAccept();

    tcp::acceptor acceptor_;

    std::shared_ptr<SessionManager> sessionMgr_;
    std::shared_ptr<RoomManager> roomMgr_;
    std::shared_ptr<AuthManager> authMgr_;
    std::shared_ptr<PlayerManager> playerMgr_;

    std::shared_ptr<AuthService> authService_;
    std::shared_ptr<RoomService> roomService_;
    std::shared_ptr<ServerPacketHandler> packetHandler_;
};