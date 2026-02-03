#pragma once
#include "../Core/Interfaces/ISession.h"
#include "../Models/Player.h"

class BotSession : public ISession {
	std::shared_ptr<Player> player_;
	std::weak_ptr<GameRoom> room_;
public:
	BotSession(std::shared_ptr<Player> player);
	virtual void Send(uint16_t id, google::protobuf::Message& msg) override {
		return;
	}
	virtual void Close() override { player_ = nullptr; }

	virtual void SetPlayer(std::shared_ptr<Player> player) override { player_ = player; }
	virtual std::shared_ptr<Player> GetPlayer() override { return player_; }
	virtual int32_t GetPlayerId() override { return player_ ? player_->id : -1; }
	virtual std::string GetPlayerName() override {
		if (player_) return player_->GetName();
		return "Bot";
	}
	virtual std::string GetSessionKey() override { return "BotSession"; }
	virtual void SetRoom(std::weak_ptr<GameRoom> room) override { room_ = room; }
	virtual std::shared_ptr<GameRoom> GetRoom() override { return room_.lock(); }
	virtual void SetPlayerName(const std::string& name) override {
		if (player_) player_->SetName(name);
	}
	virtual void SetSessionKey(const std::string& key) override {
		return;
	}

};