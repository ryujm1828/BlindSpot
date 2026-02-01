using System;
using System.Buffers;
using System.Collections.Concurrent;
using System.IO;
using System.Net.Sockets;
using System.Runtime.InteropServices.ComTypes;
using Blindspot;
using Google.Protobuf;
using UnityEditor.PackageManager;
using UnityEngine;
using UnityEngine.LightTransport;

public class ServerSession
{
    private TcpClient client_;
    private NetworkStream stream_;

    private const int BufferSize = 65535;
    private byte[] recvBuffer_ = new byte[BufferSize];
    private int currentLength_ = 0;
    private ConcurrentQueue<PacketMessage> packetQueue_ = new ConcurrentQueue<PacketMessage>();

    public bool packetQueueTryDequeue(out PacketMessage msg)
    {
        return packetQueue_.TryDequeue(out msg);
    }

    public void Connect(string ip, int port)
    {
        try
        {
            client_ = new TcpClient();
            client_.Connect(ip, port);
            stream_ = client_.GetStream();

            Debug.Log("[ServerSession] Connected to server.");

            stream_.BeginRead(recvBuffer_, currentLength_, recvBuffer_.Length - currentLength_, new AsyncCallback(OnReceiveData), null);
        }
        catch (Exception e)
        {
            Debug.LogError($"[ServerSession] Connection Failed: {e.Message}");
        }
    }

    public void Send(PacketID id, IMessage packet)
    {
        if (client_ == null || !client_.Connected) return;

        try
        {
            byte[] payload = packet.ToByteArray();
            ushort payloadSize = (ushort)payload.Length;
            ushort headerSize = 4;
            ushort totalSize = (ushort)(headerSize + payloadSize);

            byte[] sendBuffer = new byte[totalSize];

            Array.Copy(BitConverter.GetBytes(totalSize), 0, sendBuffer, 0, 2);
            Array.Copy(BitConverter.GetBytes((ushort)id), 0, sendBuffer, 2, 2);
            Array.Copy(payload, 0, sendBuffer, 4, payloadSize);

            stream_.Write(sendBuffer, 0, sendBuffer.Length);
            Debug.Log($"[Client] Sent Packet ID: {id}");
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Send Error: {e.Message}");
        }
    }

    private void OnReceiveData(IAsyncResult ar)
    {
        try
        {
            int bytesRead = stream_.EndRead(ar);
            if (bytesRead <= 0)
            {
                Disconnect();
                return;
            }

            currentLength_ += bytesRead;

            int processOffset = 0;
            while (currentLength_ - processOffset >= 4) // Check for minimum header size
            {
                ushort packetSize = BitConverter.ToUInt16(recvBuffer_, processOffset);
                if (packetSize < 4 || packetSize > BufferSize)
                {
                    Debug.LogError($"[Client] Invalid Packet Size: {packetSize}");
                    // Modify to reconnect logic
                    Disconnect();
                    return;
                }

                if (currentLength_ - processOffset >= packetSize) // Check if full packet is received
                {
                    ushort packetId = BitConverter.ToUInt16(recvBuffer_, processOffset + 2);
                    int payloadSize = packetSize - 4;
                    byte[] payload = ArrayPool<byte>.Shared.Rent(packetSize - 4);
                    Array.Copy(recvBuffer_, processOffset + 4, payload, 0, payloadSize);
                    packetQueue_.Enqueue(new PacketMessage((PacketID)packetId, payloadSize, payload));

                    processOffset += packetSize; // Go to next packet
                }
                else
                {
                    break; // Wait for more data
                }
            }
            if (processOffset > 0)
            {
                int remaining = currentLength_ - processOffset;
                if (remaining > 0)
                {
                    Buffer.BlockCopy(recvBuffer_, processOffset, recvBuffer_, 0, remaining);
                }
                currentLength_ = remaining;
            }
            stream_.BeginRead(recvBuffer_, currentLength_, recvBuffer_.Length - currentLength_, new AsyncCallback(OnReceiveData), null);
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Receive Error: {e.Message}");
            Disconnect();
        }
    }

    public void Disconnect()
    {
        stream_?.Close();
        client_?.Close();
        client_ = null;
    }
    void OnApplicationQuit()
    {
        Disconnect();
    }
}

public struct PacketMessage
{
    public PacketID Id;
    public int Size;
    public byte[] Payload;

    public PacketMessage(PacketID id, int size, byte[] payload)
    {
        Id = id;
        Payload = payload;
        Size = size;
    }
}