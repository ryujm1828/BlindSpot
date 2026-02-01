#pragma once
#include <iostream>
#include <set>
#include <mutex>
#include <memory>
#include <atomic>
#include <random>
#include <map>
#include "../Core/Interfaces/IPlayerManager.h"

class Session;

class PlayerManager : public IPlayerManager{
    static std::atomic<int32_t> playerIdGenerator_;
    static std::mutex name_mutex_;
    static std::map<int32_t, std::string> playerIdToName_;
public:
    int32_t GeneratePlayerId();
    void RegisterPlayerName(int32_t playerId, const std::string& name);
    void EditPlayerName(int32_t playerId, const std::string& newName);
    std::string GetPlayerNameById(int32_t playerId);
    
};