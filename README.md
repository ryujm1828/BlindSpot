# Blindspot: Online 3:3 top-down tactical shooter game
## About
- Blindspot is a dedicated game server and client project designed for a 3:3 top-down tactical shooter.
- This project is still a work in progress.
## Features
### 1.Asynchronous Networking
- Built with Boost.Asio for high-performance, non-blocking I/O operations.
- Handles multiple concurrent connections using a scalable Session-based architecture.

### 2. Robust Packet Serialization
- Utilizes Google Protocol Buffers (Protobuf) for efficient and structured data exchange between server and client.
- Automated packet handling system with a custom PacketHandler.

### 3. Security-Conscious Design
- Session-Key Authentication: Implements a token-based session management system to prevent Player ID spoofing.
- Server-Side Authority: All critical game logic (movement, room management) is validated on the server to prevent client-side manipulation.

### 4. Thread-Safe Game Logic
- Managed concurrency using modern C++ features: std::atomic, std::mutex, and std::shared_ptr.


## Tech Stack
### Server (C++)
- Language: C++20

- Networking: Boost.Asio

- Serialization: Protobuf

- Build System: vcpkg

### Client (Unity)
- Engine: Unity 

- Language: C#

- Networking: TCP Socket communication

## System Architecture
### The server follows a modular architecture to separate networking, logic, and data management:

- Network Layer: Manages raw TCP sockets and packet framing.

- Session Layer: Handles individual connection states and authentication tokens.

- Logic Layer (Room/Player): Manages game rooms, matchmaking, and player metadata.

- Packet Handler: Dispatches incoming packets to their respective logic functions.

## Getting Started
### Prerequisites
- Server: Visual Studio 2026, Boost, Protobuf

- Client: Unity Hub, Unity 6000.3.3f1

## Installation
### Clone the repository:
```Bash
git clone https://github.com/ryujm1828/BlindSpot.git
```
### Build the Server: 

### Server Setup (C++)
#### 1.Clone the repository:
```
git clone https://github.com/YourUsername/Habitus.git
```
#### 2.Install Dependencies via vcpkg: Open your terminal/PowerShell and run:
```
vcpkg install boost-asio:x64-windows protobuf:x64-windows
```
#### 3.Integrate vcpkg with Visual Studio:
```
vcpkg integrate install
```
#### 4.Open the Solution:

- Navigate to the Blindspot_Backend folder.

- Double-click the Habitus.sln file to open it in Visual Studio.

#### 5.Build & Run:

- Set the solution configuration to Debug or Release and the platform to x64.

- Press F5 or click Start to run the server.

### Run the Client:

- Open the Blindspot_Frontend folder in Unity.

- Ensure the server address is set to 127.0.0.1.

## License
Distributed under the MIT License. See LICENSE for more information.
