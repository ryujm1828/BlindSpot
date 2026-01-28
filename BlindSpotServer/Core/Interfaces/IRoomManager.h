#pragma once
#include "../../Network/Session.h"
#include <cstdint>

class IRoomManager {
public:
	virtual ~IRoomManager() = default;


	virtual int32_t CreateRoom(const std::string& title, int32_t maxPlayers, const std::string& password, int32_t ownerId);
	virtual std::shared_ptr<GameRoom> GetRoomById(int room_id);
};