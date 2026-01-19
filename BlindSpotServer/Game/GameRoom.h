#pragma once
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <google/protobuf/message.h>

class Session;

class GameRoom : public std::enable_shared_from_this<GameRoom> {
public:
	void Enter(std::shared_ptr<Session> session);
	void Leave(std::shared_ptr<Session> session);

	void Broadcast(uint16_t id, google::protobuf::Message& msg);

private:
	std::mutex mutex_;
	std::vector<std::shared_ptr<Session>> sessions_; // Active sessions in the game room
};

class RoomManager {
public:
	static RoomManager& Instance() {
		static RoomManager instance;
		return instance;
	}

	void Add(uint32_t room_id, std::shared_ptr<GameRoom> room);
	std::shared_ptr<GameRoom> GetOrCreateRoom(int room_id);
	std::shared_ptr<GameRoom> GetRoom(int room_id);

private:
	std::mutex mutex_;
	std::map<uint32_t, std::shared_ptr<GameRoom>> rooms_;
};