#pragma once
#include <google/protobuf/message.h>
#include "../Network/Session.h"
#include "../Managers/AuthManager.h"
#include "../Managers/SessionManager.h"
#include "../Core/Interfaces/IAuthManager.h"
#include "../Core/Interfaces/ISessionManager.h"
#include "../Core/Interfaces/IPlayerManager.h"
#include "../Protocol/packet.pb.h"

class AuthService {
public:

	AuthService(std::shared_ptr<IAuthManager> authMgr,
		std::shared_ptr<IPlayerManager> playerMgr,
		std::shared_ptr<ISessionManager> sessionMgr)
		: authMgr_(authMgr), playerMgr_(playerMgr), sessionMgr_(sessionMgr) {
	}
	void Login(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt);

private:
	std::shared_ptr<IAuthManager> authMgr_;
	std::shared_ptr<ISessionManager> sessionMgr_;
	std::shared_ptr<IPlayerManager> playerMgr_;
};