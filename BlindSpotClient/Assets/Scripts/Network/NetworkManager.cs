using System;
using System.Buffers;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Drawing;
using System.Net.Sockets;
using Blindspot;
using Google.Protobuf;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager Instance;

    private TcpClient client;
    private NetworkStream stream;

    [Header("Server Settings")]
    public string ip = "127.0.0.1";
    public int port = 7777;

    private const int Buffersize = 65535;   //max size of UDP packet
    private const int MaxPacketID = 100;
    private byte[] _recvBuffer = new byte[Buffersize];
    private Action<byte[],int>[] _packetHandlers = new Action<byte[],int> [MaxPacketID]; // Assuming max 100 packet types

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
    private ConcurrentQueue<PacketMessage> _packetQueue = new ConcurrentQueue<PacketMessage>();

    void Awake()
    {
        if (Instance == null) Instance = this;
        else Destroy(gameObject);
        DontDestroyOnLoad(gameObject);
        RegisterHandlers();
    }

    void Start()
    {
        ConnectToServer();
    }

    void Update()
    {
        while (_packetQueue.TryDequeue(out PacketMessage packet))
        {
            try
            {
                ProcessPacket(packet);
            }
            catch (Exception e)
            {
                Debug.LogError($"[Client] Packet Processing Error: {e.Message}");
            }
            finally
            {
                if(packet.Payload != null) 
                    ArrayPool<byte>.Shared.Return(packet.Payload);
            }
        }
    }

    private void RegisterHandlers()
    {
        _packetHandlers[(int)PacketID.IdLoginResponse] = HandleLoginResponse;
        _packetHandlers[(int)PacketID.IdJoinRoomResponse] = HandleJoinRoomResponse;
        _packetHandlers[(int)PacketID.IdMakeRoomResponse] = HandleMakeRoomResponse;
    }

    private void ProcessPacket(PacketMessage packet)
    {
        int id = (int)packet.Id;
        ReadOnlySpan<byte> payloadSpan = new ReadOnlySpan<byte>(packet.Payload, 0, packet.Size);

        if (id >= 0 && id < _packetHandlers.Length && _packetHandlers[id] != null)
        {
            _packetHandlers[id](packet.Payload,packet.Size);
        }
        else
        {
            Debug.LogWarning($"[Client] No handler for Packet ID: {packet.Id}");
        }
    }

    void ConnectToServer()
    {
        try
        {
            client = new TcpClient();
            client.Connect(ip, port);
            stream = client.GetStream();

            Debug.Log($"[Client] Connected to Server {ip}:{port}");

            stream.BeginRead(_recvBuffer, 0, _recvBuffer.Length, new AsyncCallback(OnReceiveData), null);
            
            SendLoginPacket();
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Connection Failed: {e.Message}");
        }
    }

    private int _currentLength = 0;

    private void OnReceiveData(IAsyncResult ar)
    {
        try
        {
            int bytesRead = stream.EndRead(ar);
            if(bytesRead <= 0)
            {
                CloseConnection();
                return;
            }

            _currentLength += bytesRead;
            
            int processOffset = 0;
            while(_currentLength - processOffset >= 4) // Check for minimum header size
            {
                ushort packetSize = BitConverter.ToUInt16(_recvBuffer, processOffset);
                if (packetSize < 4 || packetSize > Buffersize)
                {
                    Debug.LogError($"[Client] Invalid Packet Size: {packetSize}");
                    // Modify to reconnect logic
                    CloseConnection();
                    return;
                }

                if(_currentLength - processOffset >= packetSize) // Check if full packet is received
                {
                    ushort packetId = BitConverter.ToUInt16(_recvBuffer, processOffset + 2);
                    int payloadSize = packetSize - 4;
                    byte[] payload = ArrayPool<byte>.Shared.Rent(packetSize - 4);
                    Array.Copy(_recvBuffer, processOffset + 4, payload, 0, payloadSize);
                    HandlePacket((PacketID)packetId, payloadSize, payload);
                    processOffset += packetSize; // Go to next packet
                }
                else
                {
                    break; // Wait for more data
                }
            }
            if(processOffset > 0)
            {
                int remaining = _currentLength - processOffset;
                if (remaining > 0)
                {
                    Buffer.BlockCopy(_recvBuffer, processOffset, _recvBuffer, 0, remaining);
                }
                _currentLength = remaining;
            }
            stream.BeginRead(_recvBuffer, _currentLength, _recvBuffer.Length - _currentLength, new AsyncCallback(OnReceiveData), null);
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Receive Error: {e.Message}");
            CloseConnection();
        }
    }

    private void HandlePacket(PacketID id, int size, byte[] payload)
    {
        _packetQueue.Enqueue(new PacketMessage(id, size, payload));
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

    void HandleLoginResponse(byte[] payload,int size)
    {
        CodedInputStream stream = new CodedInputStream(payload, 0, size);
        LoginResponse pkt = LoginResponse.Parser.ParseFrom(stream);
        Debug.Log($"[Server] Login Result: {pkt.Success}, Msg: {pkt.Message}");
        Debug.Log($"Session ID: {pkt.SessionKey}, PlayerId: {pkt.PlayerId}");
    }

    void HandleJoinRoomResponse(byte[] payload,int size)
    {
        CodedInputStream stream = new CodedInputStream(payload, 0, size);
        JoinRoomResponse pkt = JoinRoomResponse.Parser.ParseFrom(stream);
        if (pkt.Result == JoinRoomResult.JoinSuccess)
        {
            Debug.Log($"[Server] 방 입장 성공! Room ID: {pkt.RoomId}");
        }
        else
        {
            Debug.LogError($"[Server] 방 입장 실패 코드: {pkt.Result}");
        }
    }
    
    void HandleMakeRoomResponse(byte[] payload, int size)
    {
        CodedInputStream stream = new CodedInputStream(payload, 0, size);
        MakeRoomResponse pkt = MakeRoomResponse.Parser.ParseFrom(stream);
        if (pkt.Result == MakeRoomResult.MakeSuccess)
        {
            Debug.Log($"[Server] 방 생성 성공! Room ID: {pkt.RoomId}");
        }
        else
        {
            Debug.LogError($"[Server] 방 생성 실패");
        }
    }


}