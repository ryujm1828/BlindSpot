#pragma once
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <google/protobuf/message.h>
#include <atomic>

class Session;

class GameRoom : public std::enable_shared_from_this<GameRoom> {
public:
	void Init(int room_id, const std::string& title,int32_t maxPlayers,const std::string& password,int32_t ownerId) {
		this->room_id_ = room_id;
		this->title_ = title;
		this->maxPlayers_ = maxPlayers;
		this->password_ = password;
		this->ownerId_ = ownerId;
	}
	void Enter(std::shared_ptr<Session> session);
	void Leave(std::shared_ptr<Session> session);
	void Broadcast(uint16_t id, google::protobuf::Message& msg);

private:
	int room_id_;
	std::string title_;
	int32_t maxPlayers_;
	int32_t ownerId_;
	std::string password_;
	std::mutex mutex_;
	std::vector<std::shared_ptr<Session>> sessions_; // Active sessions in the game room
};

class RoomManager {
public:
	static RoomManager& Instance() {
		static RoomManager instance;
		return instance;
	}

	int32_t CreateRoom(const std::string& title, int32_t maxPlayers, const std::string& password, int32_t ownerId);
	std::shared_ptr<GameRoom> GetRoomById(int room_id);

private:
	std::mutex mutex_;
	std::map<uint32_t, std::shared_ptr<GameRoom>> rooms_;
	std::atomic<int32_t> roomIdGenerator = 1;
	void Add(uint32_t room_id, std::shared_ptr<GameRoom> room);
};