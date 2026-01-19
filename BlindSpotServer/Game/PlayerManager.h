#pragma once
#pragma once
#include <iostream>
#include <set>
#include <mutex>
#include <memory>

class Session;

class PlayerManager {
public:
    void Add(std::shared_ptr<Session> session) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.insert(session);
        std::cout << "Player Joined. Current Players: " << sessions_.size() << std::endl;
    }

    void Remove(std::shared_ptr<Session> session) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (sessions_.erase(session)) {
            std::cout << "Player Left. Current Players: " << sessions_.size() << std::endl;
        }
    }

private:
    std::mutex mutex_;
    std::set<std::shared_ptr<Session>> sessions_;
};

// 어디서든 접근 가능하도록 전역 변수(혹은 싱글톤)로 선언
extern PlayerManager GPlayerManager;