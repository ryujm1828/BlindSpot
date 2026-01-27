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

