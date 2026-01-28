#pragma once
#include "../../Network/Session.h"
#include <cstdint>

class IPlayerManager {
public:
	virtual ~IPlayerManager() = default;

    virtual int32_t GeneratePlayerId();
    virtual void RegisterPlayerName(int32_t playerId, const std::string& name);
    virtual void EditPlayerName(int32_t playerId, const std::string& newName);
    virtual std::string GetPlayerNameById(int32_t playerId);
};