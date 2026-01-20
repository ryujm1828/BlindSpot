using UnityEngine;
using UnityEngine.UI; // InputField, Button용

public class LobbyUI : MonoBehaviour
{
    [Header("UI References")]
    public InputField roomInput; // 방 번호 입력칸
    public Button joinButton;    // 입장 버튼
    public InputField roomNameInput;
    public InputField passwordInput;
    public InputField maxPlayersInput;
    public Button makeButton;    // 입장 버튼
    void Start()
    {
        // 버튼 클릭 시 실행될 함수 연결
        //joinButton.onClick.AddListener(OnClickJoinRoom);
        makeButton.onClick.AddListener(OnClickMakeRoom);
    }

    void OnClickJoinRoom()
    {
        Debug.Log("[LobbyUI] Join Room Button Clicked");
        // 입력된 텍스트를 숫자로 변환
        if (int.TryParse(roomInput.text, out int roomId))
        {
            // 네트워크 매니저에게 패킷 전송 요청
            NetworkManager.Instance.SendJoinRoomPacket(roomId);
        }
        else
        {
            Debug.LogWarning("방 번호는 숫자만 입력해주세요!");
        }
    }

    void OnClickMakeRoom()
    {
        Debug.Log("[LobbyUI] Make Room Button Clicked");
        // 네트워크 매니저에게 방 생성 패킷 전송 요청
        // add check input fields
        if (int.TryParse(maxPlayersInput.text, out int maxPlayers))
        {
            // 네트워크 매니저에게 패킷 전송 요청
            NetworkManager.Instance.SendMakeRoomPacket(roomNameInput.text,maxPlayers, passwordInput.text);
        }
        else
        {
            Debug.LogWarning("최대 유저는 숫자만 입력해주세요!");
        }
        
    }
}