#pragma once
#define _HAS_STD_BYTE 0

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#include <windows.h>


#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include <fstream>
#include <functional>
#include <thread>
#include <mutex>
#include <ctime>
#include <atomic>
#include <condition_variable>
#include <sstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <cstdint>
using namespace std;


// STL
#include <vector>
#include <map>
#include <list>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>

// DIRECTX 11 x64
#include <D3D11.h>
#include <DXGI.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;
using namespace Microsoft::WRL;


// util header
#include "util.h"


// DirectX Tex
#include <DirectXTex/DirectXTex.h>
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else // _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif // _DEBUG

// DirectXTK
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/SimpleMath.h>
#ifdef _DEBUG
#pragma comment(lib, "DirectXTK\\x64\\debug\\DirectXTK.lib")
#else // _DEBUG
#pragma comment(lib, "DirectXTK\\x64\\release\\DirectXTK.lib")
#endif // _DEBUG

// FMOD
#include <FMOD/fmod.h>
#ifdef _DEBUG
#pragma comment(lib, "FMOD\\debug\\fmodL_vc.lib")
#else // _DEBUG
#pragma comment(lib, "FMOD\\release\\fmod_vc.lib")
#endif // _DEBUG

// Rapid-JSON
#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>
#include <rapidjson\stringbuffer.h>
using namespace rapidjson;


// ImGuI
#define IMGUI_ACTIVATE 1

#ifdef IMGUI_ACTIVATE
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#else // IMGUI_ACTIVATE
#define IMGUI_DISABLE
#endif // IMGUI_ACTIVATE


#define RELEASE(p) if (nullptr != p) { p->Release(); p = nullptr; }