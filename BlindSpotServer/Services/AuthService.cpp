#include "AuthService.h"
#include "../Models/Player.h"
#include "../Network/Session.h"
#include "../Managers/AuthManager.h"
#include "../Managers/SessionManager.h"
#include "../Core/Interfaces/IAuthManager.h"
#include "../Core/Interfaces/ISessionManager.h"
#include "../Core/Interfaces/IPlayerManager.h"

AuthService::AuthService(std::shared_ptr<IAuthManager> authMgr,
	std::shared_ptr<IPlayerManager> playerMgr,
	std::shared_ptr<ISessionManager> sessionMgr)
	: authMgr_(authMgr), playerMgr_(playerMgr), sessionMgr_(sessionMgr) {
}

void AuthService::Login(std::shared_ptr<Session> session, blindspot::C_Login& pkt) {
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

	blindspot::S_Login res;
	res.set_success(true);
	res.set_player_id(session->GetPlayerId());
	res.set_session_key(session->GetSessionKey());
	res.set_message("Welcome " + session->GetPlayerName() + "!");
	session->Send(blindspot::PacketID::ID_S_LOGIN, res);
}

void AuthService::Logout(std::shared_ptr<Session> session) {
	
}

bool AuthService::isValidSession(std::shared_ptr<Session> session) {
	if (authMgr_->GetPlayerIdByToken(session->GetSessionKey()) > 0)
		return true;
	return false;
}

