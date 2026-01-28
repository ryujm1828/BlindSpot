#include "RoomService.h"
#include "../Protocol/packet.pb.h"

void RoomService::JoinRoom(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt) {
	if (session->GetRoom()) {
		// Already in a room
		blindspot::JoinRoomResponse res;
		res.set_result(blindspot::JOIN_ALREADY_IN_ROOM);
		session->Send(blindspot::PacketID::ID_JOIN_ROOM_RESPONSE, res);
		return;
	}
	int32_t room_Id = pkt.room_id();
	auto room = roomMgr_->GetRoomById(room_Id);
	blindspot::JoinRoomResponse res;
	if (room == nullptr) {
		std::cout << "There is no room with ID: " << room_Id << std::endl;
		res.set_result(blindspot::JOIN_ROOM_NOT_FOUND);
		session->Send(blindspot::PacketID::ID_JOIN_ROOM_RESPONSE, res);
		std::cout << "[Room] Room not Found with ID: " << room_Id << std::endl;
		return;
	}

	room->Enter(session);
	session->SetRoom(room);


	res.set_result(blindspot::JOIN_SUCCESS);
	res.set_room_id(room_Id);

	session->Send(blindspot::PacketID::ID_JOIN_ROOM_RESPONSE, res);

	std::cout << "[Room] Player joined room ID: " << room_Id << std::endl;
}

void RoomService::MakeRoom(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt) {
	if (session->GetRoom()) {
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


	int32_t newRoomId = roomMgr_->CreateRoom(title, maxPlayers, password, session->GetPlayerId());

	auto room = roomMgr_->GetRoomById(newRoomId);

	blindspot::MakeRoomResponse res;
	if (room == nullptr) {
		res.set_result(blindspot::MAKE_SERVER_FULL); // 적절한 에러 코드
		session->Send(blindspot::PacketID::ID_MAKE_ROOM_RESPONSE, res);
		return;
	}

	room->Enter(session);
	session->SetRoom(room);	//weak_ptr assign
	res.set_result(blindspot::MAKE_SUCCESS);
	res.set_room_id(newRoomId);

	session->Send(blindspot::PacketID::ID_MAKE_ROOM_RESPONSE, res);
}