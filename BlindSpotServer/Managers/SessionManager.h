#pragma once
#include <set>
#include <mutex>
#include <memory>
#include <google/protobuf/message.h>
#include "../Core/Interfaces/ISessionManager.h"
#include "../Network/Session.h"

class Session;

class SessionManager : public ISessionManager {
	std::mutex sessions_mutex_;
	std::set<std::shared_ptr<Session>> sessions_;

public:

	void Add(std::shared_ptr<Session> session);
	void Remove(std::shared_ptr<Session> session);
	void Broadcast(uint16_t id, google::protobuf::Message& msg);

};