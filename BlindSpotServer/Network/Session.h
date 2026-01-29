#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <google/protobuf/message.h>

class ServerPacketHandler;
class Player;
class GameRoom;
class ISessionManager;

struct PacketHeader {
    uint16_t length;
    uint16_t id;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket&& socket,
        std::weak_ptr<ISessionManager> sessionMgr,
        std::shared_ptr<ServerPacketHandler> packetHandler);
    ~Session();


    void Start();
    void Close();
    void Send(uint16_t id, google::protobuf::Message& msg);

    void SetPlayer(std::shared_ptr<Player> player);
    std::shared_ptr<Player> GetPlayer(); 
    std::string GetPlayerName();
    void SetPlayerName(const std::string& name);
    int32_t GetPlayerId();

    std::shared_ptr<GameRoom> GetRoom();
    void SetRoom(std::weak_ptr<GameRoom> gameRoom);

    std::string GetSessionKey() const { return sessionKey_; }
    void SetSessionKey(const std::string& key) { sessionKey_ = key; }

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
