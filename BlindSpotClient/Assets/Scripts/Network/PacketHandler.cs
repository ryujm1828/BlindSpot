using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Blindspot;
using Google.Protobuf;
using UnityEngine;

public class PacketHandler
{
    private const int MaxPacketID = 100; // Assuming max 100 packet types
    private Action<byte[], int>[] packetHandlers_ = new Action<byte[], int>[MaxPacketID];
    private AuthService authService_;
    private RoomService roomService_;

    public PacketHandler(AuthService authService,RoomService roomService)
    {
        authService_ = authService;
        roomService_ = roomService;
        Init();
    }

    public void Init()
    {
        for (int i = 0; i < MaxPacketID; i++)
            packetHandlers_[i] = null;
        RegisterHandlers();
        
    }
    private void RegisterHandlers()
    {
        packetHandlers_[(int)PacketID.IdSLogin] = HandleLoginResponse;
        packetHandlers_[(int)PacketID.IdSJoinRoom] = HandleJoinRoomResponse;
        packetHandlers_[(int)PacketID.IdSMakeRoom] = HandleMakeRoomResponse;
    }

    public void HandlePacket(PacketMessage packet)
    {
        int id = (int)packet.Id;
        
        if (id >= 0 && id < packetHandlers_.Length && packetHandlers_[id] != null)
        {
            packetHandlers_[id](packet.Payload, packet.Size);
        }
        else
        {
            Debug.LogWarning($"[Client] No handler for Packet ID: {packet.Id}");
        }
    }
    private void HandleLoginResponse(byte[] payload, int size)
    {
        S_Login pkt = ParsePacket<S_Login>(payload, size);
        authService_.HandleLoginResponse(pkt);
    }

    private void HandleJoinRoomResponse(byte[] payload, int size)
    {
        S_JoinRoom pkt = ParsePacket<S_JoinRoom>(payload, size);
        roomService_.HandleJoinRoomResponse(pkt);
    }

    private void HandleMakeRoomResponse(byte[] payload, int size)
    {
        S_MakeRoom pkt = ParsePacket<S_MakeRoom>(payload, size);
        roomService_.HandleMakeRoomResponse(pkt);
    }

    private static T ParsePacket<T>(byte[] payload, int size) where T : IMessage<T>, new()
    {
        T pkt = new T();
        pkt.MergeFrom(payload, 0, size);
        return pkt;
    }
}