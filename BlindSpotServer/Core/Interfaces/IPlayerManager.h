#pragma once
#include <cstdint>
#include <string>

class IPlayerManager {
public:
	virtual ~IPlayerManager() = default;

    virtual int32_t GeneratePlayerId() = 0;
    virtual void RegisterPlayerName(int32_t playerId, const std::string& name) = 0;
    virtual void EditPlayerName(int32_t playerId, const std::string& newName) = 0;
    virtual std::string GetPlayerNameById(int32_t playerId) = 0;
};