#include "ServerPacketHandler.h"
#include "../Network/Session.h"
#include "../Services/AuthService.h"
#include "../Services/RoomService.h"

// [삭제 필수!] 이 줄이 "비정적 멤버 참조 오류"의 주범입니다. 지우세요.
// PacketFunc ServerPacketHandler::packet_handlers_[UINT16_MAX] = { nullptr }; 

// [추가] 생성자에서 배열 초기화 & 서비스 주입
ServerPacketHandler::ServerPacketHandler(std::shared_ptr<AuthService> authService, std::shared_ptr<RoomService> roomService)
    : _authService(authService), _roomService(roomService)
{
    // 멤버 변수(배열) 초기화는 생성자에서!
    for (int i = 0; i < UINT16_MAX; i++)
        packet_handlers_[i] = nullptr;
}

void ServerPacketHandler::Init() {
    // [수정] 람다에서 멤버 함수를 부르려면 [this] 캡처가 필수!
    packet_handlers_[blindspot::PacketID::ID_LOGIN_REQUEST] = [this](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
        blindspot::LoginRequest pkt;
        if (pkt.ParseFromArray(payload, size)) {
            Handle_LOGIN_REQUEST(session, pkt); // this 덕분에 호출 가능
        }
        };

    packet_handlers_[blindspot::PacketID::ID_JOIN_ROOM_REQUEST] = [this](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
        blindspot::JoinRoomRequest pkt;
        if (pkt.ParseFromArray(payload, size)) {
            Handle_JOIN_ROOM_REQUEST(session, pkt);
        }
        };

    packet_handlers_[blindspot::PacketID::ID_MAKE_ROOM_REQUEST] = [this](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
        blindspot::MakeRoomRequest pkt;
        if (pkt.ParseFromArray(payload, size)) {
            Handle_MAKE_ROOM_REQUEST(session, pkt);
        }
        };
}

void ServerPacketHandler::HandlePacket(std::shared_ptr<Session> session, uint16_t id, uint8_t* payload, uint16_t size) {
    if (id >= UINT16_MAX) {
        std::cout << "Invalid Packet ID: " << id << std::endl;
        return;
    }

    if (packet_handlers_[id] != nullptr) {
        packet_handlers_[id](session, payload, size);
    }
    else {
        std::cout << "No handler for Packet ID: " << id << std::endl;
    }
}

void ServerPacketHandler::Handle_LOGIN_REQUEST(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt) {
    std::cout << "Login Request Received. Name: " << pkt.name() << std::endl;
    // [수정] 주입받은 서비스 객체 사용
    _authService->Login(session, pkt);
}

void ServerPacketHandler::Handle_JOIN_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt) {
    // RoomService 구현 여부에 따라 수정 필요
    _roomService->JoinRoom(session, pkt); 
}

void ServerPacketHandler::Handle_MAKE_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt) {
    _roomService->MakeRoom(session, pkt);
}