#include "ServerPacketHandler.h"
#include "../Network/Session.h"
#include "./GameRoom.h"

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
	
	
	std::string token = pkt.session_key();
	int32_t playerId = PlayerManager::Instance().GetPlayerIdBySessionKey(token);
	//token validation logic here
	if (playerId > 0) {
		session->playerId = playerId;
		session->playerName = PlayerManager::Instance().GetPlayerNameById(playerId);
		session->sessionKey = token;
	}
	else {
		token = PlayerManager::Instance().GenerateSessionKey();
		session->playerId = PlayerManager::Instance().GeneratePlayerId();
		// name validation/filtering logic here
		session->playerName = pkt.name();
		session->sessionKey = token;
		PlayerManager::Instance().RegisterSession(token, session->playerId);
	}

	

	blindspot::LoginResponse res;
	res.set_success(true);
	res.set_player_id(session->playerId);
	res.set_session_key(session->sessionKey);
	res.set_message("Welcome " + session->playerName + "!");
	session->Send(blindspot::PacketID::ID_LOGIN_RESPONSE, res);
}

void ServerPacketHandler::Handle_JOIN_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt) {
	if (session->room.lock()) {
		// Already in a room
		blindspot::JoinRoomResponse res;
		res.set_result(blindspot::JOIN_ALREADY_IN_ROOM);
		session->Send(blindspot::PacketID::ID_JOIN_ROOM_RESPONSE, res);
		return;
	}
	int32_t room_Id = pkt.room_id();
	auto room = RoomManager::Instance().GetRoomById(room_Id);
	blindspot::JoinRoomResponse res;
	if (room == nullptr){
		std::cout << "There is no room with ID: " << room_Id << std::endl;
		res.set_result(blindspot::JOIN_ROOM_NOT_FOUND);
		session->Send(blindspot::PacketID::ID_JOIN_ROOM_RESPONSE, res);
		std::cout << "[Room] Room not Found with ID: " << room_Id << std::endl;
		return;
	}
	
	room->Enter(session);
	session->room = room;

	
	res.set_result(blindspot::JOIN_SUCCESS);
	res.set_room_id(room_Id);

	session->Send(blindspot::PacketID::ID_JOIN_ROOM_RESPONSE, res);

	std::cout << "[Room] Player joined room ID: " << room_Id << std::endl;
}

void ServerPacketHandler::Handle_MAKE_ROOM_REQUEST(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt) {
	if (session->room.lock()) {
		// Already in a room
		blindspot::MakeRoomResponse res;
		res.set_result(blindspot::MAKE_ALREADY_IN_ROOM);
		session->Send(blindspot::PacketID::ID_MAKE_ROOM_RESPONSE, res);
		return;
	}
	std::string title = pkt.room_name();
	int32_t maxPlayers = pkt.max_players();
	std::string password = pkt.password();

	//room_name filter
	//max_players limit check
	//password length check
	

	int32_t newRoomId = RoomManager::Instance().CreateRoom(title, maxPlayers, password,session->playerId);
	
	auto room = RoomManager::Instance().GetRoomById(newRoomId);

	blindspot::MakeRoomResponse res;
	if (room == nullptr) {
		res.set_result(blindspot::MAKE_SERVER_FULL); // 적절한 에러 코드
		session->Send(blindspot::PacketID::ID_MAKE_ROOM_RESPONSE, res);
		return;
	}

	room->Enter(session);
	session->room = room;	//weak_ptr assign

	res.set_result(blindspot::MAKE_SUCCESS);
	res.set_room_id(newRoomId);
	
	session->Send(blindspot::PacketID::ID_MAKE_ROOM_RESPONSE, res);
}