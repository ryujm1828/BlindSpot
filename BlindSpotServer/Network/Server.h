#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>

// [필수] 필요한 헤더들 포함
#include "Session.h"
#include "ServerPacketHandler.h"

// 매니저 및 서비스 헤더 (경로는 프로젝트 구조에 맞게 확인해주세요)
#include "../Managers/SessionManager.h"
#include "../Managers/RoomManager.h"
#include "../Managers/PlayerManager.h"
#include "../Managers/AuthManager.h"

#include "../Services/AuthService.h"
#include "../Services/RoomService.h"

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        sessionMgr_ = std::make_shared<SessionManager>();
        roomMgr_ = std::make_shared<RoomManager>();
        authMgr_ = std::make_shared<AuthManager>();
        playerMgr_ = std::make_shared<PlayerManager>();
        authService_ = std::make_shared<AuthService>(authMgr_,playerMgr_,sessionMgr_);
        roomService_ = std::make_shared<RoomService>(roomMgr_); // 예: _roomService = std::make_shared<RoomService>(_roomMgr);

        // 3. 패킷 핸들러 생성 및 의존성 주입
        packetHandler_ = std::make_shared<ServerPacketHandler>(authService_, roomService_);

        // [중요] 핸들러 초기화 (람다 등록 등)
        packetHandler_->Init();

        std::cout << "Server initialized on port " << port << std::endl;

        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "New Client Connected!" << std::endl;

                    // [수정 포인트] Session 생성자에 필요한 의존성(매니저, 패킷핸들러)을 전달
                    // Session.h의 생성자와 파라미터 순서가 정확히 일치해야 합니다.
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

    tcp::acceptor acceptor_;

    std::shared_ptr<SessionManager> sessionMgr_;
    std::shared_ptr<RoomManager> roomMgr_;
    std::shared_ptr<AuthManager> authMgr_;
    std::shared_ptr<PlayerManager> playerMgr_;

    std::shared_ptr<AuthService> authService_;
    std::shared_ptr<RoomService> roomService_;
    std::shared_ptr<ServerPacketHandler> packetHandler_;
};