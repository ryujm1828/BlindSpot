@echo off
setlocal

:: 1. 경로 설정 (루트 폴더 기준으로 상대 경로 설정)
set PROTOC_PATH=..\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe
set PROTO_SOURCE_DIR=..\BlindSpotServer\Protocol
set CPP_OUT_DIR=..\BlindSpotServer\Protocol
set CS_OUT_DIR=..\BlindSpotClient\Assets\Scripts\Network\Protocol

:: 2. 유니티 출력 폴더가 없으면 생성
if not exist "%CS_OUT_DIR%" (
    mkdir "%CS_OUT_DIR%"
    echo [Info] Created directory: %CS_OUT_DIR%
)

:: 3. Protoc 실행 (C++ 및 C# 동시 생성)
echo [Protobuf] Generating codes...

"%PROTOC_PATH%" --proto_path="%PROTO_SOURCE_DIR%" ^
    --cpp_out="%CPP_OUT_DIR%" ^
    --csharp_out="%CS_OUT_DIR%" ^
    "%PROTO_SOURCE_DIR%\packet.proto"

if %errorlevel% equ 0 (
    echo [Success] C++ and C# files generated successfully!
) else (
    echo [Error] Failed to generate files.
    pause
)

endlocal