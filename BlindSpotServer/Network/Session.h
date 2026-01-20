#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "../Game/PlayerManager.h"
#include "../Protocol/packet.pb.h"
#include "../Game/ServerPacketHandler.h"
#include "../Game/GameRoom.h"
using boost::asio::ip::tcp;

class GameRoom;

struct PacketHeader {
    uint16_t length;
    uint16_t id;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}
    std::weak_ptr<GameRoom> room;
    int32_t playerId;
	std::string playerName;
	std::string sessionKey;
    void Start() {
        PlayerManager::Instance().Add(shared_from_this());
        DoRead();
    }
    void Close() {
        socket_.close();
    }
    void Send(uint16_t id, google::protobuf::Message& msg) {
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
					PlayerManager::Instance().Remove(shared_from_this());
                }
            });
    }
private:
    std::vector<uint8_t> recv_buffer_; // Buffer to store received data
    void DoRead() {
        auto self(shared_from_this());
        // Wait for data asynchronously
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
					std::cout << "[Debug] Raw Data Length: " << length << std::endl;
                    recv_buffer_.insert(recv_buffer_.end(), data_, data_ + length);

                    while (recv_buffer_.size() >= sizeof(PacketHeader)) {
                        PacketHeader* header = reinterpret_cast<PacketHeader*>(recv_buffer_.data());

						std::cout << "[Debug] Expected Packet Length: " << header->length << std::endl;
						std::cout << "[Debug] Current Buffer Size: " << recv_buffer_.size() << std::endl;
                        if (recv_buffer_.size() < header->length) {
                            break; // Not enough data for a full packet
                        }
                        
						std::cout << "[Debug] Packet ID: " << header->id << ", Length: " << header->length << std::endl;
						// Process complete packet
						uint16_t packet_id = header->id;
						uint8_t* payload = recv_buffer_.data() + sizeof(PacketHeader);
						uint16_t payload_size = header->length - sizeof(PacketHeader);

						HandlePacket(packet_id, payload, payload_size);

						recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + header->length);
                    }

                    DoRead(); // Wait for more data
                }
                else {
                    std::cout << "Client Disconnected." << ec.message() << std::endl;
                    PlayerManager::Instance().Remove(shared_from_this());
                }
            });

    }

    void HandlePacket(uint16_t id, uint8_t* payload, uint16_t payload_size) {
		ServerPacketHandler::HandlePacket(shared_from_this(), id, payload, payload_size);
    }

    
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};