#include "Player.h"

bool Player::IsRotationValid(float newLookAngle) {
	if (newLookAngle < 0 || newLookAngle >= 360)
        return false;

	//ignore invalid 
    //if (now - lastPacketTime < 10) return false;

    return true;
}

void Player::SetName(const std::string& playerName) {
    std::lock_guard<std::mutex> lock(nameLock_);
    name = playerName;
}

std::string Player::GetName() {
    std::lock_guard<std::mutex> lock(nameLock_);
    return name;
}
void Player::SetRoom(std::shared_ptr<GameRoom> newRoom) {
    room = newRoom;
}
std::shared_ptr<GameRoom> Player::GetRoom() {
	std::lock_guard<std::mutex> lock(roomLock_);
    return room.lock();
}