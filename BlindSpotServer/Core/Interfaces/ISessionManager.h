#pragma once
#include <google/protobuf/message.h>
#include <cstdint>

class Session;

class ISessionManager {
public:
	virtual ~ISessionManager() = default;

	virtual void Add(std::shared_ptr<Session> session);
	virtual void Remove(std::shared_ptr<Session> session);
	virtual void Broadcast(uint16_t id, google::protobuf::Message& msg);
};