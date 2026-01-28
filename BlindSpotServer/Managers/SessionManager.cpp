#include "../Network/Session.h"
#include "SessionManager.h"


void SessionManager::Add(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_.insert(session);
    std::cout << "Player Joined. Current Players: " << sessions_.size() << std::endl;
}

void SessionManager::Remove(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    if (sessions_.erase(session)) {
        std::cout << "Player Left. Current Players: " << sessions_.size() << std::endl;
    }
}

void SessionManager::Broadcast(uint16_t id, google::protobuf::Message& msg) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

	//Implement broadcasting to all sessions
}