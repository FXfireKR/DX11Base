#pragma once
#define _HAS_STD_BYTE 0

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <fstream>
#include <functional>

using namespace std;

// STL
#include <vector>
#include <map>
#include <list>

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
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;
using namespace Microsoft::WRL;


// DirectX Tex
#include <DirectXTex/DirectXTex.h>
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else // _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif // _DEBUG

// DirectXTK
#include <DirectXTK/WICTextureLoader.h>
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

// ImGuI
//#define IMGUI_ACTIVATE

#ifdef IMGUI_ACTIVATE
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // IMGUI_ACTIVATE


// singleton
#include "DirectX11Com.h"

#define RELEASE(p) if (nullptr != p) { p->Release(); p = nullptr; }

#define DXCOM DirectX11Com::Get()
#define g_Device DXCOM->GetDevice()
#define g_DeviceContext DXCOM->GetDeviceContext()