#include "BotSession.h"

BotSession::BotSession(std::shared_ptr<Player> player){
	player_ = player;
	SetPlayerName("Bot");
}