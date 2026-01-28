#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <google/protobuf/message.h>

class Player;
class GameRoom;
class ISessionManager;

// 패킷 헤더 구조체
struct PacketHeader {
    uint16_t length;
    uint16_t id;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    // 생성자: 인터페이스(ISessionManager)로 받음
    Session(boost::asio::ip::tcp::socket&& socket, std::weak_ptr<ISessionManager> sessionMgr);
    ~Session();


    void Start();
    void Close();
    void Send(uint16_t id, google::protobuf::Message& msg);

    // Player 관련
    void SetPlayer(std::shared_ptr<Player> player);
    std::shared_ptr<Player> GetPlayer(); // player_ 직접 접근보다 Getter 권장
    std::string GetPlayerName();
    void SetPlayerName(const std::string& name);
    int32_t GetPlayerId();

    // Room 관련
    std::shared_ptr<GameRoom> GetRoom();
    void SetRoom(std::weak_ptr<GameRoom> gameRoom);

    // Session Key
    std::string GetSessionKey() const { return _sessionKey; }
    void SetSessionKey(const std::string& key) { _sessionKey = key; }

public:
    // 외부에서 접근 가능한 멤버 변수라면 public (Getter/Setter 권장하지만 기존 유지)
    // std::shared_ptr<Player> player_; // 가급적 private으로 옮기고 Getter 사용 권장

private:
    void DoRead();
    void HandlePacket(uint16_t id, uint8_t* payload, uint16_t payload_size);

private:
    boost::asio::ip::tcp::socket socket_;
    std::vector<uint8_t> recv_buffer_;
    enum { max_length = 1024 };
    char data_[max_length];

    // [중요] 의존성 주입된 매니저 (싱글톤 대신 사용)
    std::weak_ptr<ISessionManager> sessionMgr_;

    std::shared_ptr<Player> player_; // private으로 내리는 것을 추천
    std::string _sessionKey;
};