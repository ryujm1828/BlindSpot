#pragma once
#include "../Protocol/packet.pb.h"
#include <memory>
#include <functional>

class Session;

using PacketFunc = std::function<void(std::shared_ptr<Session>, uint8_t*, uint16_t)>;

class ServerPacketHandler {
public:
	
	static void Init();

	// Main packet handling function
    static void HandlePacket(std::shared_ptr<Session> session, uint16_t id, uint8_t* payload, uint16_t size);

	static void Handle_LOGIN_REQUEST(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt);
	static void Handle_JOIN_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt);

private:
	// Array to hold packet handler functions
	static PacketFunc packet_handlers_[UINT16_MAX]; // Maximum of 65536 packet IDs	    	
};