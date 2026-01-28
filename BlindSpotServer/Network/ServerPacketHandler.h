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
    // [수정] 생성자 추가 (서비스 주입)
    ServerPacketHandler(std::shared_ptr<AuthService> authService, std::shared_ptr<RoomService> roomService);

    // [수정] static 제거
    void Init();
    void HandlePacket(std::shared_ptr<Session> session, uint16_t id, uint8_t* payload, uint16_t size);

private:
    // [수정] static 제거 (멤버 함수들)
    void Handle_LOGIN_REQUEST(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt);
    void Handle_JOIN_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt);
    void Handle_MAKE_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt);

private:
    // [수정] 서비스 멤버 변수
    std::shared_ptr<AuthService> _authService;
    std::shared_ptr<RoomService> _roomService;

    // [수정] static 제거 (이제 각 핸들러 객체마다 배열을 가짐)
    PacketFunc packet_handlers_[UINT16_MAX];
};