using System;
using System.Collections.Generic;
using System.Text;
using Unity.VisualScripting;
using UnityEditor.EditorTools;
using UnityEngine;

public class Managers : MonoBehaviour
{
    static Managers _instance;
    public static Managers Instance { get { Init(); return _instance; } }

    // 외부(UI 등)에서 편하게 쓰라고 열어두는 프로퍼티
    public static RoomService Room => Instance._roomService;
    public static NetworkManager Network => Instance._networkManager;

    // 실제 인스턴스들
    private RoomManager _roomManager;
    private NetworkManager _networkManager;
    private AuthService _authService;
    private RoomService _roomService;
    private PacketHandler _packetHandler;

    void Awake()
    {
        Init();
    }

    static void Init()
    {
        if (_instance == null)
        {
            GameObject go = GameObject.Find("@Managers");
            if (go == null) go = new GameObject { name = "@Managers" };
            _instance = go.GetOrAddComponent<Managers>();
            DontDestroyOnLoad(go);

            if (_instance._networkManager != null) return;

            _instance._roomManager = new RoomManager();
            _instance._networkManager = new NetworkManager();
            _instance._authService = new AuthService(_instance._networkManager);
            _instance._roomService = new RoomService(_instance._roomManager, _instance._networkManager);

            _instance._packetHandler = new PacketHandler(_instance._authService, _instance._roomService);

            _instance._networkManager.Init(_instance._packetHandler);
        }
    }
    public static void MakeRoom(string roomName, int maxPlayers, string password)
    {
        Instance._roomService.SendMakeRoomPacket(roomName, maxPlayers, password);
    }
    public static void JoinRoom(int roomId)
    {
        Instance._roomService.SendJoinRoomPacket(roomId);
    }
    public static void Login(string username)
    {
        Instance._authService.SendLoginPacket(username);
    }

}