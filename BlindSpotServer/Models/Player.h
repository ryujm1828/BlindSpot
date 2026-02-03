#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "../Core/Interfaces/ISession.h"
class GameRoom;

class Player {
public:
    int32_t id;
    std::string name;
    std::mutex nameLock_;
    std::weak_ptr<GameRoom> room;

	float posX = 0, posY = 0, posZ = 0;
	float lookAngle = 0;
	uint64_t lastPacketTime = 0;

    bool IsRotationValid(float newLookAngle);
    void SetName(const std::string& playerName);
    std::string GetName();
    void SetRoom(std::shared_ptr<GameRoom> newRoom);
    std::shared_ptr<GameRoom> GetRoom();

private:
	std::mutex roomLock_;
};