#pragma once
#pragma once
#include <iostream>
#include <set>
#include <mutex>
#include <memory>
#include <atomic>
#include <random>
#include <map>

class Session;

class PlayerManager {
public:
    static PlayerManager& Instance() {
        static PlayerManager instance;
        return instance;
    }

    void Add(std::shared_ptr<Session> session) {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        sessions_.insert(session);
        std::cout << "Player Joined. Current Players: " << sessions_.size() << std::endl;
    }

    void Remove(std::shared_ptr<Session> session) {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        if (sessions_.erase(session)) {
            std::cout << "Player Left. Current Players: " << sessions_.size() << std::endl;
        }
    }

    int32_t GeneratePlayerId() {
        return playerIdGenerator_.fetch_add(1);
    }

    std::string GenerateSessionKey() {
        static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static thread_local std::random_device rd;
        static thread_local std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

        std::string key = "";
        for (int i = 0; i < 16; i++)
            key += alphanum[dis(gen)];
        return key;
    }

    void RegisterSession(const std::string& token, int32_t playerId) {
		std::lock_guard<std::mutex> lock(token_mutex_);
		sessionKeyToPlayerId_[token] = playerId;
    }

    int32_t GetPlayerIdBySessionKey(const std::string& token) {
		std::lock_guard<std::mutex> lock(token_mutex_);
		auto it = sessionKeyToPlayerId_.find(token);
		if (it != sessionKeyToPlayerId_.end()) {
            return it->second;
        }
		return -1; // Not found
    }

    void RegisterPlayerName(int32_t playerId, const std::string& name) {
		std::lock_guard<std::mutex> lock(name_mutex_);
		playerIdToName_[playerId] = name;
    }

    void EditPlayerName(int32_t playerId, const std::string& newName) {
        
    }

    std::string GetPlayerNameById(int32_t playerId) {
		std::lock_guard<std::mutex> lock(name_mutex_);
		auto it = playerIdToName_.find(playerId);
		if (it != playerIdToName_.end()) {
            return it->second;
        }
		return ""; // Not found
    }
    
    void RemoveSession(const std::string& token) {
		std::scoped_lock lock(token_mutex_, name_mutex_);
		auto it = sessionKeyToPlayerId_.find(token);
        if (it != sessionKeyToPlayerId_.end()) {
            int32_t playerId = it->second;
            sessionKeyToPlayerId_.erase(it);
			playerIdToName_.erase(playerId);
			std::cout << "Session and Player data removed for Id: " << playerId << std::endl;
        }
        
    }

private:
    std::mutex sessions_mutex_;
    std::set<std::shared_ptr<Session>> sessions_;
	std::atomic<int32_t> playerIdGenerator_ = 1;
	std::mutex token_mutex_;
	std::map<std::string, int32_t> sessionKeyToPlayerId_;
	std::mutex name_mutex_;
	std::map<int32_t, std::string> playerIdToName_;
};