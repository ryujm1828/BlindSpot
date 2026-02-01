#include "ServerPacketHandler.h"
#include "../Network/Session.h"
#include "../Services/AuthService.h"
#include "../Services/RoomService.h"


ServerPacketHandler::ServerPacketHandler(std::shared_ptr<AuthService> authService, std::shared_ptr<RoomService> roomService)
    : authService_(authService), roomService_(roomService)
{

    for (int i = 0; i < UINT16_MAX; i++)
        packet_handlers_[i] = nullptr;
}

void ServerPacketHandler::Init() {

    packet_handlers_[(int)blindspot::PacketID::ID_C_LOGIN] = [this](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
        blindspot::C_Login pkt;
        if (pkt.ParseFromArray(payload, size)) {
            Handle_C_LOGIN(session, pkt);
        }
        };

    packet_handlers_[(int)blindspot::PacketID::ID_C_JOIN_ROOM] = [this](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
        blindspot::C_JoinRoom pkt;
        if (pkt.ParseFromArray(payload, size)) {
            Handle_C_JOIN_ROOM(session, pkt);
        }
        };

    packet_handlers_[(int)blindspot::PacketID::ID_C_MAKE_ROOM] = [this](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
        blindspot::C_MakeRoom pkt;
        if (pkt.ParseFromArray(payload, size)) {
            Handle_C_MAKE_ROOM(session, pkt);
        }
        };
}

void ServerPacketHandler::HandlePacket(std::shared_ptr<Session> session, uint16_t id, uint8_t* payload, uint16_t size) {
    if (id >= UINT16_MAX) {
        std::cout << "Invalid Packet ID: " << id << std::endl;
        return;
    }
	//is not login packet, check session validity
    if (id != 1) {
        if (!authService_->isValidSession(session)) {
            std::cout << "Invalid User" << std::endl;
            return;
        }
    }
    if (packet_handlers_[id] != nullptr) {
        packet_handlers_[id](session, payload, size);
    }
    else {
        std::cout << "No handler for Packet ID: " << id << std::endl;
    }
}

void ServerPacketHandler::Handle_C_LOGIN(std::shared_ptr<Session> session, blindspot::C_Login& pkt) {
    std::cout << "Login Request Received. Name: " << pkt.name() << std::endl;

    authService_->Login(session, pkt);
}

void ServerPacketHandler::Handle_C_JOIN_ROOM(std::shared_ptr<Session> session, blindspot::C_JoinRoom& pkt) {

    roomService_->JoinRoom(session, pkt); 
}

void ServerPacketHandler::Handle_C_MAKE_ROOM(std::shared_ptr<Session> session, blindspot::C_MakeRoom& pkt) {
    roomService_->MakeRoom(session, pkt);
}