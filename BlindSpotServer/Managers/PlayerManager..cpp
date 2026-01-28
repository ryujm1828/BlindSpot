#include "PlayerManager.h"

std::atomic<int32_t> PlayerManager::playerIdGenerator_ = 1;
std::mutex PlayerManager::name_mutex_;
std::map<int32_t, std::string> PlayerManager::playerIdToName_;

int32_t PlayerManager::GeneratePlayerId() {
    return playerIdGenerator_.fetch_add(1);
}

void PlayerManager::RegisterPlayerName(int32_t playerId, const std::string& name) {
    std::lock_guard<std::mutex> lock(name_mutex_);
    playerIdToName_[playerId] = name;
}

void PlayerManager::EditPlayerName(int32_t playerId, const std::string& newName) {

}

std::string PlayerManager::GetPlayerNameById(int32_t playerId) {
    std::lock_guard<std::mutex> lock(name_mutex_);
    auto it = playerIdToName_.find(playerId);
    if (it != playerIdToName_.end()) {
        return it->second;
    }
    return ""; // Not found
}