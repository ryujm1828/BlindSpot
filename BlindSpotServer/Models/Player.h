#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
class GameRoom;

class Player {
public:
    int32_t id;
    std::string name;
    std::mutex _nameLock;
    std::weak_ptr<GameRoom> room;

    void SetName(const std::string& playerName) {
        std::lock_guard<std::mutex> lock(_nameLock);
        name = playerName;
    }

    std::string GetName() {
        std::lock_guard<std::mutex> lock(_nameLock);
        return name;
    }
};