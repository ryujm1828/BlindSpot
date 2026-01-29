#pragma once
#include <string>
#include <cstdint>

class IAuthManager {
public:
	virtual ~IAuthManager() = default;

	virtual int32_t GetPlayerIdByToken(const std::string& token) = 0;
	virtual std::string GenerateToken() = 0;
	virtual void RemoveToken(const std::string& token) = 0;
	virtual void RegisterToken(const std::string& token, int32_t playerId) = 0;
};