#pragma once
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <google/protobuf/message.h>
#include <atomic>
#include "../Models/GameRoom.h"
class RoomManager {
public:
	static RoomManager& Instance();

	int32_t CreateRoom(const std::string& title, int32_t maxPlayers, const std::string& password, int32_t ownerId);
	std::shared_ptr<GameRoom> GetRoomById(int room_id);

private:
	std::mutex mutex_;
	std::map<uint32_t, std::shared_ptr<GameRoom>> rooms_;
	std::atomic<int32_t> roomIdGenerator = 1;
	void Add(uint32_t room_id, std::shared_ptr<GameRoom> room);
};