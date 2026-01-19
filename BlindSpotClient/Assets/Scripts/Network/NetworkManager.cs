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

            // 데이터 수신 대기 시작
            stream.BeginRead(recvBuffer, 0, recvBuffer.Length, new AsyncCallback(OnReceiveData), null);

            // 접속하자마자 로그인 패킷 전송 (테스트용)
            SendLoginPacket();
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Connection Failed: {e.Message}");
        }
    }

    // [수정됨] 콜백 함수 이름을 더 명확하게 변경했습니다.
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

            // 받은 데이터를 조립 버퍼에 추가
            byte[] temp = new byte[bytesRead];
            Array.Copy(recvBuffer, 0, temp, 0, bytesRead);
            assembleBuffer.AddRange(temp);

            // 패킷 처리 루프
            while (assembleBuffer.Count >= 4) // 헤더 크기(4) 이상일 때만
            {
                // 1. 패킷 전체 크기 확인 (Total Size)
                ushort packetSize = BitConverter.ToUInt16(assembleBuffer.ToArray(), 0);

                // 2. 데이터가 충분히 도착했는지 확인
                if (assembleBuffer.Count < packetSize) break;

                // 3. 패킷 ID 확인
                ushort packetID = BitConverter.ToUInt16(assembleBuffer.ToArray(), 2);

                // 4. 페이로드(내용물) 추출
                byte[] payload = new byte[packetSize - 4];
                Array.Copy(assembleBuffer.ToArray(), 4, payload, 0, payload.Length);

                // 5. 패킷 처리 (함수 분리함)
                HandlePacket((PacketID)packetID, payload);

                // 6. 처리한 패킷 제거
                assembleBuffer.RemoveRange(0, packetSize);
            }

            // [중요!] 다시 수신 대기 (이게 없으면 한 번 받고 멈춤)
            stream.BeginRead(recvBuffer, 0, recvBuffer.Length, new AsyncCallback(OnReceiveData), null);
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Receive Error: {e.Message}");
            CloseConnection();
        }
    }

    // [추가됨] 패킷 처리 로직을 별도 함수로 뺐습니다.
    private void HandlePacket(PacketID id, byte[] payload)
    {
        switch (id)
        {
            case PacketID.IdLoginResponse:
                {
                    LoginResponse pkt = LoginResponse.Parser.ParseFrom(payload);
                    Debug.Log($"[Server] Login Result: {pkt.Success}, Msg: {pkt.Message}");
                }
                break;

            // --- [추가된 부분] 방 입장 응답 처리 ---
            case PacketID.IdJoinRoomResponse:
                {
                    JoinRoomResponse pkt = JoinRoomResponse.Parser.ParseFrom(payload);
                    if (pkt.Result == JoinRoomResult.Success)
                    {
                        Debug.Log($"[Server] 방 입장 성공! Room ID: {pkt.RoomId}");
                        // 여기서 씬 이동 등을 처리 (주의: 메인 스레드 처리가 필요할 수 있음)
                    }
                    else
                    {
                        Debug.LogError($"[Server] 방 입장 실패 코드: {pkt.Result}");
                    }
                }
                break;
            // --------------------------------------

            default:
                Debug.LogWarning($"[Client] Unknown Packet ID: {id}");
                break;
        }
    }

    // 공용 전송 함수
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
            // Debug.Log($"[Client] Sent Packet ID: {id}"); // 로그가 너무 많으면 주석 처리
        }
        catch (Exception e)
        {
            Debug.LogError($"[Client] Send Error: {e.Message}");
        }
    }

    // --- [추가된 부분] 방 입장 요청 보내기 함수 ---
    public void SendJoinRoomPacket(int roomId)
    {
        JoinRoomRequest req = new JoinRoomRequest();
        req.RoomId = roomId;
        // req.PlayerName은 Proto에서 뺐으므로 삭제

        Send(PacketID.IdJoinRoomRequest, req);
        Debug.Log($"[Client] Joining Room Request: {roomId}");
    }
    // ------------------------------------------

    void SendLoginPacket()
    {
        LoginRequest loginPacket = new LoginRequest();
        loginPacket.Id = 1001;
        loginPacket.Name = "UnityPlayer";
        Send(PacketID.IdLoginRequest, loginPacket);
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