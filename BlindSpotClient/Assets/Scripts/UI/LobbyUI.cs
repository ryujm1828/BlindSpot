using UnityEngine;
using UnityEngine.UI;

public class LobbyUI : MonoBehaviour
{
    [Header("UI References")]
    public InputField roomInput; // roomId
    public Button joinButton;    
    public InputField roomNameInput;
    public InputField passwordInput;
    public InputField maxPlayersInput;
    public Button makeButton;
    void Start()
    {
        // 버튼 클릭 시 실행될 함수 연결
        //joinButton.onClick.AddListener(OnClickJoinRoom);
        makeButton.onClick.AddListener(OnClickMakeRoom);
    }

    void OnClickJoinRoom()
    {
        Debug.Log("[LobbyUI] Join Room Button Clicked");
        if (int.TryParse(roomInput.text, out int roomId))
        {
            Managers.JoinRoom(roomId);
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
            Managers.MakeRoom(roomNameInput.text,maxPlayers, passwordInput.text);
        }
        else
        {
            Debug.LogWarning("최대 유저는 숫자만 입력해주세요!");
        }
        
    }
}