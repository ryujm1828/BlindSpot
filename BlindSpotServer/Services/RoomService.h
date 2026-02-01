#pragma once

#include "../Protocol/packet.pb.h"

class Session;
class IRoomManager;
class RoomManager;

class RoomService {
public:
	RoomService(std::shared_ptr<IRoomManager> roomMgr);

	void JoinRoom(std::shared_ptr<Session> session, blindspot::C_JoinRoom& pkt);
	void MakeRoom(std::shared_ptr<Session> session, blindspot::C_MakeRoom& pkt);
private:
	std::shared_ptr<IRoomManager> roomMgr_;
};