#pragma once
#include "../Protocol/packet.pb.h"
#include <memory>
#include <functional>
#include <iostream>

class Session;
class AuthService;
class RoomService;

using PacketFunc = std::function<void(std::shared_ptr<Session>, uint8_t*, uint16_t)>;

class ServerPacketHandler {
public:
    ServerPacketHandler(std::shared_ptr<AuthService> authService, std::shared_ptr<RoomService> roomService);

    void Init();
    void HandlePacket(std::shared_ptr<Session> session, uint16_t id, uint8_t* payload, uint16_t size);

private:
    void Handle_LOGIN_REQUEST(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt);
    void Handle_JOIN_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt);
    void Handle_MAKE_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt);

private:
    std::shared_ptr<AuthService> authService_;
    std::shared_ptr<RoomService> roomService_;

    PacketFunc packet_handlers_[UINT16_MAX];
};