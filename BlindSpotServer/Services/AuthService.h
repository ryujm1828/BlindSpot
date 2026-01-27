#pragma once
#include "../Network/Session.h"
#include "../Managers/AuthManager.h"
#include "../Managers/SessionManager.h"

class AuthService {
public:
	static void Login(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt) {
		std::string token = pkt.session_key();
		int32_t playerId = AuthManager::Instance().GetPlayerIdBySessionKey(token);
		//token validation logic here
		if (playerId > 0) {
			auto newPlayer = std::make_shared<Player>();
			newPlayer->name = PlayerManager::Instance().GetPlayerNameById(playerId);
			newPlayer->id = playerId;
			session->SetPlayer(newPlayer);
			session->SetSessionKey(token);
		}
		else {
			token = AuthManager::Instance().GenerateSessionKey();
			auto newPlayer = std::make_shared<Player>();
			newPlayer->name = pkt.name();
			newPlayer->id = PlayerManager::Instance().GeneratePlayerId();
			session->SetPlayer(newPlayer);
			session->SetSessionKey(token);
			AuthManager::Instance().RegisterSession(token, session->GetPlayerId());
		}

		blindspot::LoginResponse res;
		res.set_success(true);
		res.set_player_id(session->GetPlayerId());
		res.set_session_key(session->GetSessionKey());
		res.set_message("Welcome " + session->GetPlayerName() + "!");
		session->Send(blindspot::PacketID::ID_LOGIN_RESPONSE, res);
	}
};