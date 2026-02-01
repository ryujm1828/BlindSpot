using System;
using System.Collections.Generic;
using System.Text;
using Blindspot;
using Google.Protobuf;
using UnityEngine;

public class AuthService
{
    private NetworkManager networkManager_;
    public AuthService(NetworkManager networkManager)
    {
        this.networkManager_ = networkManager;
    }
    public void SendLoginPacket(string name)
    {
        C_Login loginPacket = new C_Login();
        loginPacket.Name = name;   // Enter your desired player name
        Managers.Network.Send(PacketID.IdCLogin, loginPacket);
    }
    public void HandleLoginResponse(S_Login pkt)
    {
        Debug.Log($"[Server] Login Result: {pkt.Success}, Msg: {pkt.Message}");
        Debug.Log($"Session ID: {pkt.SessionKey}, PlayerId: {pkt.PlayerId}");
    }

}