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

class AuthManager {
	static std::mutex token_mutex_;
	static std::map<std::string, int32_t> sessionKeyToPlayerId_;
	static std::mutex name_mutex_;
	static std::map<int32_t, std::string> playerIdToName_;
public:
	static AuthManager& Instance();
	static int32_t GetPlayerIdBySessionKey(const std::string& token);
	static std::string GenerateSessionKey();
	static void RemoveSession(const std::string& token);
	static void RegisterSession(const std::string& token, int32_t playerId);
	
};