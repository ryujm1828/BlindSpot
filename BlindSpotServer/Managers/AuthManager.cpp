#include "AuthManager.h"
#include <string>
#include <mutex>
#include <map>
#include <atomic>
#include <random>
#include <iostream>
#include <cstdint>

int32_t AuthManager::GetPlayerIdByToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(token_mutex_);
    auto it = tokenToPlayerId_.find(token);
    if (it != tokenToPlayerId_.end()) {
        return it->second;
    }
    return -1; // Not found
}
std::string AuthManager::GenerateToken() {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    std::string key = "";
    for (int i = 0; i < 16; i++)
        key += alphanum[dis(gen)];
    return key;
}
void AuthManager::RemoveToken(const std::string& token) {
    std::scoped_lock lock(token_mutex_, name_mutex_);
    auto it = tokenToPlayerId_.find(token);
    if (it != tokenToPlayerId_.end()) {
        int32_t playerId = it->second;
        tokenToPlayerId_.erase(it);
        //플레이어 삭제
        //playerIdToName_.erase(playerId);
        std::cout << "Session and Player data removed for Id: " << playerId << std::endl;
    }

}

void AuthManager::RegisterToken(const std::string& token, int32_t playerId) {
    std::lock_guard<std::mutex> lock(token_mutex_);
    tokenToPlayerId_[token] = playerId;
}