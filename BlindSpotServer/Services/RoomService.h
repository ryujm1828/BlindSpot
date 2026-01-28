#pragma once
#include "../Network/Session.h"
#include "../Managers/RoomManager.h"
#include "../Core/Interfaces/IRoomManager.h"
#include "../Core/Interfaces/ISessionManager.h"
#include "../Protocol/packet.pb.h"

class RoomService {
public:
	RoomService(std::shared_ptr<IRoomManager> roomMgr)
		:roomMgr_(roomMgr) {
	}

	void JoinRoom(std::shared_ptr<Session> session, blindspot::JoinRoomRequest& pkt);
	void MakeRoom(std::shared_ptr<Session> session, blindspot::MakeRoomRequest& pkt);
private:
	std::shared_ptr<IRoomManager> roomMgr_;
};