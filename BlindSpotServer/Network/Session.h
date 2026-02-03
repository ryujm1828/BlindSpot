#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <google/protobuf/message.h>
#include "../Core/Interfaces/ISession.h"

class ServerPacketHandler;
class Player;
class GameRoom;
class ISessionManager;

struct PacketHeader {
    uint16_t length;
    uint16_t id;
};

class Session : public std::enable_shared_from_this<Session>, public ISession {
public:
    Session(boost::asio::ip::tcp::socket&& socket,
        std::weak_ptr<ISessionManager> sessionMgr,
        std::shared_ptr<ServerPacketHandler> packetHandler);
    ~Session();


    void Start();
    void Close() override;
    void Send(uint16_t id, google::protobuf::Message& msg) override;

    void SetPlayer(std::shared_ptr<Player> player) override;
    std::shared_ptr<Player> GetPlayer() override;
    std::string GetPlayerName() override;
    void SetPlayerName(const std::string& name) override;
    int32_t GetPlayerId() override;

    std::shared_ptr<GameRoom> GetRoom() override;
    void SetRoom(std::weak_ptr<GameRoom> gameRoom) override;

    std::string GetSessionKey() override { return sessionKey_; }
    void SetSessionKey(const std::string& key) override { sessionKey_ = key; }

private:
    void DoRead();
    void HandlePacket(uint16_t id, uint8_t* payload, uint16_t payload_size);

private:
    boost::asio::ip::tcp::socket socket_;
    std::vector<uint8_t> recv_buffer_;
    enum { max_length = 1024 };
    char data_[max_length];

    std::weak_ptr<ISessionManager> sessionMgr_;

    std::shared_ptr<Player> player_;
    std::string sessionKey_;
    std::weak_ptr<ServerPacketHandler> packetHandler_;
};
