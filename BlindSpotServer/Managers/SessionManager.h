#pragma once
#include <set>
#include <mutex>
#include <memory>
#include <google/protobuf/message.h>

class Session;

class SessionManager {
	std::mutex sessions_mutex_;
	std::set<std::shared_ptr<Session>> sessions_;

public:
	static SessionManager& Instance();
	void Add(std::shared_ptr<Session> session);
	void Remove(std::shared_ptr<Session> session);
	void Broadcast(uint16_t id, google::protobuf::Message& msg);

};