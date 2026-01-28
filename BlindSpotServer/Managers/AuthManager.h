#pragma once
#include <iostream>
#include <set>
#include <mutex>
#include <memory>
#include <atomic>
#include <random>
#include <map>
#include <string>
#include <cstdint>
#include "../Core/Interfaces/IAuthManager.h"

class AuthManager : public IAuthManager{
	
public:
	AuthManager() = default;
	virtual ~AuthManager() = default;

	
	int32_t GetPlayerIdByToken(const std::string& token) override;
	std::string GenerateToken() override;
	void RemoveToken(const std::string& token) override;
	void RegisterToken(const std::string& token, int32_t playerId) override;
private:
	AuthManager(const AuthManager&) = delete;
	AuthManager& operator=(const AuthManager&) = delete;

	std::mutex token_mutex_;
	std::map<std::string, int32_t> tokenToPlayerId_;
	std::mutex name_mutex_;
	std::map<int32_t, std::string> playerIdToName_;
};