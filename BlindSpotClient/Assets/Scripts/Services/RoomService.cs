using Blindspot;
using Google.Protobuf;
using UnityEngine;

public class RoomService
{   
    private RoomManager _roomManager;
    private NetworkManager _networkManager;
    public RoomService(RoomManager roomManager, NetworkManager networkManager)
    {
        _roomManager = roomManager;
        _networkManager = networkManager;
    }

    public void HandleJoinRoomResponse(S_JoinRoom pkt)
    {
        if (pkt.Result == JoinRoomResult.JoinSuccess)
        {
            Debug.Log($"[Server] 방 입장 성공! Room ID: {pkt.RoomId}");
        }
        else
        {
            Debug.LogError($"[Server] 방 입장 실패 코드: {pkt.Result}");
        }
    }

    public void HandleMakeRoomResponse(S_MakeRoom pkt)
    {
        if (pkt.Result == MakeRoomResult.MakeSuccess)
        {
            Debug.Log($"[Server] 방 생성 성공! Room ID: {pkt.RoomId}");
        }
        else
        {
            Debug.LogError($"[Server] 방 생성 실패");
        }
    }
    public void SendJoinRoomPacket(int roomId)
    {
        C_JoinRoom req = new C_JoinRoom();
        req.RoomId = roomId;

        Managers.Network.Send(PacketID.IdCJoinRoom, req);
        Debug.Log($"[Client] Joining Room Request: {roomId}");
    }

    public void SendMakeRoomPacket(string roomName, int maxPlayers, string password)
    {
        C_MakeRoom req = new C_MakeRoom();
        req.RoomName = roomName;
        req.MaxPlayers = maxPlayers;
        req.Password = password;
        Managers.Network.Send(PacketID.IdCMakeRoom, req);
        Debug.Log($"[Client] Making Room Request: {roomName}, MaxPlayers: {maxPlayers}");
    }
}




