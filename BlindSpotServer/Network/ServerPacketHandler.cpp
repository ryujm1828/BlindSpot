#include "ServerPacketHandler.h"
#include "../Network/Session.h"
#include "../Models/GameRoom.h"
#include "../Services/AuthService.h"
#include "../Services/RoomService.h"

PacketFunc ServerPacketHandler::packet_handlers_[UINT16_MAX] = { nullptr };

void ServerPacketHandler::Init() {
	
	// Initialize all packet handlers to nullptr
	for(int i = 0; i< UINT16_MAX; i++)
		packet_handlers_[i] = nullptr;

	// Map packet IDs to their handler functions
	packet_handlers_[blindspot::PacketID::ID_LOGIN_REQUEST] = [](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
		blindspot::LoginRequest pkt;
		if (pkt.ParseFromArray(payload, size)) {
			Handle_LOGIN_REQUEST(session, pkt);
		}
	};
	packet_handlers_[blindspot::PacketID::ID_JOIN_ROOM_REQUEST] = [](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
		blindspot::JoinRoomRequest pkt;
		if (pkt.ParseFromArray(payload, size)) {
			Handle_JOIN_ROOM_REQUEST(session, pkt);
		}
	};
	packet_handlers_[blindspot::PacketID::ID_MAKE_ROOM_REQUEST] = [](std::shared_ptr<Session> session, uint8_t* payload, uint16_t size) {
		blindspot::MakeRoomRequest pkt;
		if (pkt.ParseFromArray(payload, size)) {
			Handle_MAKE_ROOM_REQUEST(session, pkt);
		}
	};
}

// Dispatch packet to appropriate handler based on packet ID
void ServerPacketHandler::HandlePacket(std::shared_ptr<Session> session, uint16_t id, uint8_t* payload, uint16_t size) {
	if (id >= UINT16_MAX) {
		std::cout << "Invalid Packet ID: " << id << std::endl;
		return;
	}

	if (packet_handlers_[id] != nullptr) {
		packet_handlers_[id](session, payload, size);
	} else {
		std::cout << "No handler for Packet ID: " << id << std::endl;
	}
}

void ServerPacketHandler::Handle_LOGIN_REQUEST(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt) {
	std::cout << "Login Request Received. Name: " << pkt.name() << std::endl;
	AuthService::Login(session, pkt);
}

void ServerPacketHandler::Handle_JOIN_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt) {
	RoomService::JoinRoom(session, pkt);
}

void ServerPacketHandler::Handle_MAKE_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt) {
	RoomService::MakeRoom(session, pkt);
}