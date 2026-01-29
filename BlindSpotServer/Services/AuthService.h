#pragma once
#include <google/protobuf/message.h>
#include "../Protocol/packet.pb.h"

class Session;
class AuthManager;
class SessionManager;
class PlayerManager;
class IPlayerManager;
class IAuthManager;
class ISessionManager;


class AuthService {
public:

	AuthService(std::shared_ptr<IAuthManager> authMgr,
		std::shared_ptr<IPlayerManager> playerMgr,
		std::shared_ptr<ISessionManager> sessionMgr);
	void Login(std::shared_ptr<Session> session, blindspot::LoginRequest& pkt);

private:
	std::shared_ptr<IAuthManager> authMgr_;
	std::shared_ptr<ISessionManager> sessionMgr_;
	std::shared_ptr<IPlayerManager> playerMgr_;
};