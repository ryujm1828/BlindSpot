#include "AuthService.h"
#include "../Models/Player.h"

void AuthService::Login(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt) {
	std::string token = pkt.session_key();
	int32_t playerId = authMgr_->GetPlayerIdByToken(token);
	//token validation logic here
	if (playerId > 0) {
		auto newPlayer = std::make_shared<Player>();
		newPlayer->name = playerMgr_->GetPlayerNameById(playerId);
		newPlayer->id = playerId;
		session->SetPlayer(newPlayer);
		session->SetSessionKey(token);
	}
	else {
		token = authMgr_->GenerateToken();
		auto newPlayer = std::make_shared<Player>();
		newPlayer->name = pkt.name();
		newPlayer->id = playerMgr_->GeneratePlayerId();
		session->SetPlayer(newPlayer);
		session->SetSessionKey(token);
		authMgr_->RegisterToken(token, session->GetPlayerId());
	}

	blindspot::LoginResponse res;
	res.set_success(true);
	res.set_player_id(session->GetPlayerId());
	res.set_session_key(session->GetSessionKey());
	res.set_message("Welcome " + session->GetPlayerName() + "!");
	session->Send(blindspot::PacketID::ID_LOGIN_RESPONSE, res);
}