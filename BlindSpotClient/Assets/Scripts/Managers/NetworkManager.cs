using System;
using System.Buffers;
using System.IO;
using Blindspot;
using Google.Protobuf;
using UnityEditor.PackageManager;
using UnityEngine;

public class NetworkManager
{
    private ServerSession session_;

    const string ServerIP = "127.0.0.1";
    const int ServerPort = 7777;
    PacketHandler packetHandler_;
    public NetworkManager()
    {
        session_ = new ServerSession();
    }

    public void Init(PacketHandler packetHandler)
    {
        packetHandler_ = packetHandler;
        session_.Connect(ServerIP, ServerPort);
    }
    
    public void Update()
    {
        while (session_.packetQueueTryDequeue(out PacketMessage packet))
        {
            try
            {
                packetHandler_.HandlePacket(packet);
            }
            catch (System.Exception e)
            {
                Debug.LogError($"[Network] Packet Error: {e.Message}");
            }
            finally
            {
                // 다 쓴 버퍼 반납 (ArrayPool)
                if (packet.Payload != null)
                    ArrayPool<byte>.Shared.Return(packet.Payload);
            }
        }
    }
    public void Send(PacketID id,IMessage packet)
    {
        session_.Send(id, packet);
    }
}