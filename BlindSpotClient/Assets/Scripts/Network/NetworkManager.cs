using System;
using System.Collections.Generic;
using System.Net.Sockets;
using UnityEngine;
using Google.Protobuf;
using Blindspot; // 프로토콜 네임스페이스

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager Instance;

    private TcpClient client;
    private NetworkStream stream;

    [Header("Server Settings")]
    public string ip = "127.0.0.1";
    public int port = 7777;

    // 수신 버퍼
    private byte[] recvBuffer = new byte[1024];
    // 패킷 조립용 리스트
    private List<byte> assembleBuffer = new List<byte>();

    void Awake()
    {
        if (Instance == null) Instance = this;
        else Destroy(gameObject);
        DontDestroyOnLoad(gameObject);
    }

    void Start()
    {
        ConnectToServer();
    }

    void ConnectToServer()
    {
        try
        {
            client = new TcpClient();
            client.Connect(ip, port);
            stream = client.GetStream();

            Debug.Log($"[Client] Connected to Server {ip}:{port}");

            stream.BeginRead(recvBuffer, 0, recvBuffer.Length, new AsyncCallback(OnReceiveData), null);
            
            SendLoginPacket();
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Connection Failed: {e.Message}");
        }
    }

    private void OnReceiveData(IAsyncResult ar)
    {
        try
        {
            int bytesRead = stream.EndRead(ar);
            if (bytesRead <= 0)
            {
                Debug.Log("[Client] Disconnected from server.");
                CloseConnection();
                return;
            }

            //Add received data to assemble buffer
            byte[] temp = new byte[bytesRead];
            Array.Copy(recvBuffer, 0, temp, 0, bytesRead);
            assembleBuffer.AddRange(temp);

            // Process complete packets
            while (assembleBuffer.Count >= 4) 
            {
                ushort packetSize = BitConverter.ToUInt16(assembleBuffer.ToArray(), 0);

                if (assembleBuffer.Count < packetSize) break;

                ushort packetID = BitConverter.ToUInt16(assembleBuffer.ToArray(), 2);

                byte[] payload = new byte[packetSize - 4];
                Array.Copy(assembleBuffer.ToArray(), 4, payload, 0, payload.Length);

                HandlePacket((PacketID)packetID, payload);

                assembleBuffer.RemoveRange(0, packetSize);
            }

            stream.BeginRead(recvBuffer, 0, recvBuffer.Length, new AsyncCallback(OnReceiveData), null);
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Receive Error: {e.Message}");
            CloseConnection();
        }
    }

    private void HandlePacket(PacketID id, byte[] payload)
    {
        switch (id)
        {
            case PacketID.IdLoginResponse:
                {
                    LoginResponse pkt = LoginResponse.Parser.ParseFrom(payload);
                    Debug.Log($"[Server] Login Result: {pkt.Success}, Msg: {pkt.Message}");
                    Debug.Log($"Session ID: {pkt.SessionKey}, PlayerId: {pkt.PlayerId}");
                }
                break;

         
            case PacketID.IdJoinRoomResponse:
                {
                    JoinRoomResponse pkt = JoinRoomResponse.Parser.ParseFrom(payload);
                    if (pkt.Result == JoinRoomResult.JoinSuccess)
                    {
                        Debug.Log($"[Server] 방 입장 성공! Room ID: {pkt.RoomId}");
                    }
                    else
                    {
                        Debug.LogError($"[Server] 방 입장 실패 코드: {pkt.Result}");
                    }
                }
                break;
            case PacketID.IdMakeRoomResponse:
                {
                    MakeRoomResponse pkt = MakeRoomResponse.Parser.ParseFrom(payload);
                    if (pkt.Result == MakeRoomResult.MakeSuccess)
                    {
                        Debug.Log($"[Server] 방 생성 성공! Room ID: {pkt.RoomId}");
                    }
                    else
                    {
                        Debug.LogError($"[Server] 방 생성 실패");
                    }
                }
                break;
            default:
                Debug.LogWarning($"[Client] Unknown Packet ID: {id}");
                break;
        }
    }

    public void Send(PacketID id, IMessage packet)
    {
        if (client == null || !client.Connected) return;

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

            stream.Write(sendBuffer, 0, sendBuffer.Length);
            Debug.Log($"[Client] Sent Packet ID: {id}");
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Send Error: {e.Message}");
        }
    }

    public void SendJoinRoomPacket(int roomId)
    {
        JoinRoomRequest req = new JoinRoomRequest();
        req.RoomId = roomId;

        Send(PacketID.IdJoinRoomRequest, req);
        Debug.Log($"[Client] Joining Room Request: {roomId}");
    }

    void SendLoginPacket()
    {
        LoginRequest loginPacket = new LoginRequest();
        loginPacket.Name = "UnityPlayer";   // Enter your desired player name
        Send(PacketID.IdLoginRequest, loginPacket);
    }

    public void SendMakeRoomPacket(string roomName,int maxPlayers, string password)
    {
        MakeRoomRequest req = new MakeRoomRequest();
        req.RoomName = roomName;
        req.MaxPlayers = maxPlayers;
        req.Password = password;
        Send(PacketID.IdMakeRoomRequest, req);
        Debug.Log($"[Client] Making Room Request: {roomName}, MaxPlayers: {maxPlayers}");
    }

    void CloseConnection()
    {
        stream?.Close();
        client?.Close();
        client = null;
    }

    void OnApplicationQuit()
    {
        CloseConnection();
    }
}