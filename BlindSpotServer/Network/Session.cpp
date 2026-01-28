// [BlindSpotServer/Network/Session.cpp]

#include "../Network/Session.h"
#include "../Core/Interfaces/ISessionManager.h"
#include "../Models/Player.h"
#include "../Models/GameRoom.h"
#include "../Network/ServerPacketHandler.h" // 헤더 포함 필수
#include <iostream>

using boost::asio::ip::tcp;

// [수정] 생성자: packetHandler를 받아서 멤버 변수 초기화
Session::Session(tcp::socket&& socket,
    std::weak_ptr<ISessionManager> sessionMgr,
    std::shared_ptr<ServerPacketHandler> packetHandler)
    : socket_(std::move(socket)),
    sessionMgr_(sessionMgr),
    packetHandler_(packetHandler) // 멤버 초기화
{
}

Session::~Session() {}

void Session::Start() {
    if (auto mgr = sessionMgr_.lock()) {
        mgr->Add(shared_from_this());
    }
    DoRead();
}

void Session::Close() {
    socket_.close();
    /* 보통 Close에서는 Remove를 직접 부르지 않고,
       비동기 에러 처리(DoRead/Send)에서 Remove가 호출되도록 하는 게 안전합니다.
       필요하다면 여기서 호출해도 됩니다.
    */
}

void Session::SetPlayer(std::shared_ptr<Player> player) {
    player_ = player;
}

std::shared_ptr<Player> Session::GetPlayer() {
    return player_;
}

std::string Session::GetPlayerName() {
    if (player_) return player_->name;
    return "Unknown";
}

void Session::SetPlayerName(const std::string& name) {
    if (player_) player_->SetName(name);
}

int32_t Session::GetPlayerId() {
    if (player_) return player_->id;
    return -1;
}

std::shared_ptr<GameRoom> Session::GetRoom() {
    if (player_) return player_->room.lock();
    return nullptr;
}

void Session::SetRoom(std::weak_ptr<GameRoom> gameRoom) {
    if (player_) player_->room = gameRoom;
}

void Session::Send(uint16_t id, google::protobuf::Message& msg) {
    std::string payload;
    msg.SerializeToString(&payload);

    uint16_t header_size = sizeof(PacketHeader);
    uint16_t payload_size = static_cast<uint16_t>(payload.size());
    uint16_t total_size = header_size + payload_size;

    auto send_buffer = std::make_shared<std::vector<uint8_t>>(total_size);
    PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer->data());
    header->length = total_size;
    header->id = id;

    std::memcpy(send_buffer->data() + header_size, payload.data(), payload_size);

    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(*send_buffer),
        [this, self, send_buffer](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                std::cout << "Send failed: " << ec.message() << std::endl;
                // [수정] Instance() 대신 멤버 변수 사용
                if (auto mgr = sessionMgr_.lock()) {
                    mgr->Remove(shared_from_this());
                }
            }
        });
}

void Session::DoRead() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                recv_buffer_.insert(recv_buffer_.end(), data_, data_ + length);
                while (recv_buffer_.size() >= sizeof(PacketHeader)) {
                    PacketHeader* header = reinterpret_cast<PacketHeader*>(recv_buffer_.data());
                    if (recv_buffer_.size() < header->length) break;

                    uint16_t packet_id = header->id;
                    uint8_t* payload = recv_buffer_.data() + sizeof(PacketHeader);
                    uint16_t payload_size = header->length - sizeof(PacketHeader);

                    HandlePacket(packet_id, payload, payload_size);

                    recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + header->length);
                }
                DoRead();
            }
            else {
                // [수정] 연결 끊김 처리
                if (auto mgr = sessionMgr_.lock()) {
                    mgr->Remove(shared_from_this());
                }
            }
        });
}

void Session::HandlePacket(uint16_t id, uint8_t* payload, uint16_t payload_size) {
    // [수정] static 호출 제거 -> 주입받은 객체 사용
    if (packetHandler_) {
        packetHandler_->HandlePacket(shared_from_this(), id, payload, payload_size);
    }
}
