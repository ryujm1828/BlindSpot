using System;
using System.Collections.Generic;
using System.Net.Sockets;
using UnityEngine;
using Google.Protobuf;
using Blindspot;
using NUnit.Framework;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager Instance;

    private TcpClient client;
    private NetworkStream stream;

    [Header("Server Settings")]
    public string ip = "127.0.0.1"; // localhost
    public int port = 7777;

    void Awake()
    {
        if (Instance == null) Instance = this;
        else Destroy(gameObject);

        // Settings to persist across scenes
        DontDestroyOnLoad(gameObject);
    }

    void Start()
    {
        ConnectToServer();
    }
    
    private byte[] recvBuffer = new byte[1024];

    void ConnectToServer()
    {
        try
        {
            client = new TcpClient();
            client.Connect(ip, port); // 서버 접속 시도
            stream = client.GetStream();

            Debug.Log($"[Client] Connected to Server {ip}:{port}");

            stream.BeginRead(recvBuffer, 0, recvBuffer.Length, new AsyncCallback(pnReceiveData), null);

            // Send a test login packet upon connection
            SendLoginPacket();
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Connection Failed: {e.Message}");
        }
    }

    private byte[] receiveRawBuffer = new byte[4096];
    private List<byte> assembleBuffer = new List<byte>();

    // Callback for receiving data (asynchronous)
    private void pnReceiveData(IAsyncResult ar)
    {
        try
        {
            int bytesRead = stream.EndRead(ar);
            if (bytesRead <= 0)
            {
                Debug.Log("[Client] Disconnected from server.");
                return;
            }

            // Process received data
            byte[] temp = new byte[bytesRead];
            Array.Copy(recvBuffer, 0, temp, 0, bytesRead);
            assembleBuffer.AddRange(temp);

            while (assembleBuffer.Count >= 4) // Minimum header size
            {
                // Read total packet size
                // Check the endian if it is not working as expected
                ushort packetSize = BitConverter.ToUInt16(assembleBuffer.ToArray(), 0);

                if (assembleBuffer.Count < packetSize) break; // Wait for more data

                ushort packetID = BitConverter.ToUInt16(assembleBuffer.ToArray(), 2);
                byte[] payload = new byte[packetSize - 4];
                Array.Copy(assembleBuffer.ToArray(), 4, payload, 0, payload.Length);

                // Handle the packet based on Packet ID
                HandlePacket((PacketID)packetID, payload);
                // Remove processed packet from buffer
                assembleBuffer.RemoveRange(0, packetSize);
            }
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Receive Error: {e.Message}");
        }

        void HandlePacket(PacketID id, byte[] payload)
        {
            switch (id)
            {
                case PacketID.IdLoginResponse:
                    LoginResponse loginResponse = LoginResponse.Parser.ParseFrom(payload);
                    Debug.Log($"[Client] Received Login Response: Success={loginResponse.Success}, Message={loginResponse.Message}");
                    break;
                // Handle other packet types here
                default:
                    Debug.LogWarning($"[Client] Unknown Packet ID: {id}");
                    break;
            }
        }
    }

    // Function to send packets to the server
    public void Send(PacketID id, IMessage packet)
    {
        if (client == null || !client.Connected) return;

        // 1. Serialize the packet to a byte array
        byte[] payload = packet.ToByteArray();
        ushort payloadSize = (ushort)payload.Length;
        ushort headerSize = 4; // Size(2) + ID(2)

        ushort totalSize = (ushort)(headerSize + payloadSize);

        // 2. Create send buffer
        byte[] sendBuffer = new byte[totalSize];

        // 3. Create header [4 bytes]
        Array.Copy(BitConverter.GetBytes(totalSize), 0, sendBuffer, 0, 2);
        Array.Copy(BitConverter.GetBytes((ushort)id), 0, sendBuffer, 2, 2);

        // 4. Copy payload
        Array.Copy(payload, 0, sendBuffer, 4, payloadSize);

        // 5. Send the packet
        try
        {
            stream.Write(sendBuffer, 0, sendBuffer.Length);
            Debug.Log($"[Client] Sent Packet ID: {id}, Size: {totalSize}");
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Send Error: {e.Message}");
        }
    }

    // Test function to send a login packet
    void SendLoginPacket()
    {
        LoginRequest loginPacket = new LoginRequest();
        loginPacket.Id = 1001;
        loginPacket.Name = "UnityPlayer";

        Send(PacketID.IdLoginRequest , loginPacket);
    }

    void OnApplicationQuit()
    {
        stream?.Close();
        client?.Close();
    }
}