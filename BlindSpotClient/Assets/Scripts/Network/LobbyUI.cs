using UnityEngine;
using UnityEngine.UI; // InputField, Button용

public class LobbyUI : MonoBehaviour
{
    [Header("UI References")]
    public InputField roomInput; // 방 번호 입력칸
    public Button joinButton;    // 입장 버튼

    void Start()
    {
        // 버튼 클릭 시 실행될 함수 연결
        joinButton.onClick.AddListener(OnClickJoinRoom);
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
}