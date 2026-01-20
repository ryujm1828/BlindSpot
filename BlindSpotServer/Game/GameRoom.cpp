#include "GameRoom.h"
#include "../Network/Session.h"
#include <iostream>

void GameRoom::Enter(std::shared_ptr<Session> session) {
	std::lock_guard<std::mutex> lock(mutex_);
	sessions_.push_back(session);
	std::cout << "Session entered room. Current sessions: " << sessions_.size() << std::endl;
}

void GameRoom::Leave(std::shared_ptr<Session> session) {
	std::lock_guard<std::mutex> lock(mutex_);

	auto it = std::find(sessions_.begin(), sessions_.end(), session);
	if (it != sessions_.end()) {
		sessions_.erase(it);
		std::cout << "Session left room. Current sessions: " << sessions_.size() << std::endl;
	}
}

void GameRoom::Broadcast(uint16_t id, google::protobuf::Message& msg) {
	std::lock_guard<std::mutex> lock(mutex_);
	for (auto& session : sessions_) {
		session->Send(id, msg);
	}
}

void RoomManager::Add(uint32_t roomId, std::shared_ptr<GameRoom> room) {
	std::lock_guard<std::mutex> lock(mutex_);
	rooms_[roomId] = room;
}

std::shared_ptr<GameRoom> RoomManager::GetRoomById(int roomId) {
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = rooms_.find(roomId);

	if (it == rooms_.end())
		return nullptr;
	
	return it->second;
}

int32_t RoomManager::CreateRoom(const std::string& title, int32_t maxPlayers, const std::string& password,int32_t ownerId) {
	int32_t newRoomId = roomIdGenerator.fetch_add(1);
	auto newRoom = std::make_shared<GameRoom>();
	
	newRoom->Init(newRoomId, title,maxPlayers,password,ownerId);
	Add(newRoomId, newRoom);

	std::cout << "Room Created - ID: " << newRoomId << ", Title: " << title << ", Max Players: " << maxPlayers << std::endl;

	return newRoomId;
}