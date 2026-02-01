using UnityEngine;
using UnityEngine.UI; // InputField, Button용

public class LoginUI : MonoBehaviour
{
    [Header("UI References")]
    public InputField roomNameInput;
    public Button loginButton;    // 입장 버튼
    void Start()
    {
        // 버튼 클릭 시 실행될 함수 연결
        //joinButton.onClick.AddListener(OnClickJoinRoom);
        loginButton.onClick.AddListener(OnClickLogin);
        Debug.Log("[LoginUI] Start");
    }

    void OnClickLogin()
    {
        Debug.Log("[LoginUI] Login Button Clicked");
        Managers.Login(roomNameInput.text);
    }
}