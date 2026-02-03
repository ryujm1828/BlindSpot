#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <google/protobuf/message.h>

class Player;
class GameRoom;

class ISession
{
public:
    virtual ~ISession() {}
    virtual void Send(uint16_t id, google::protobuf::Message& msg) = 0; // 순수 가상 함수
    virtual void Close() = 0;
	virtual std::string GetPlayerName() = 0;
	virtual void SetPlayer(std::shared_ptr<Player> player) = 0;
	virtual void SetPlayerName(const std::string& name) = 0;
	virtual std::shared_ptr<Player> GetPlayer() = 0;
	virtual int32_t GetPlayerId() = 0;
	virtual std::string GetSessionKey() = 0;
	virtual void SetRoom(std::weak_ptr<GameRoom> gameRoom) = 0;
	virtual void SetSessionKey(const std::string& key) = 0;
	virtual std::shared_ptr<GameRoom> GetRoom() = 0;
};