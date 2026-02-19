#include "pch.h"
#include "CApplication.h"
#include "CDeltaTimeManager.h"

Application::Application()
{
}

Application::~Application()
{
}

bool Application::Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_)
{
	m_renderWorld.Initialize(hWnd_, iScreenWidth_, iScreenHeight_);

	m_gameWorld.Initialize(m_renderWorld);

	m_window.Initialize(hWnd_);

#ifdef IMGUI_ACTIVATE
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	if (false == ImGui_ImplWin32_Init(hWnd_))
		return false;

	if (false == ImGui_ImplDX11_Init(m_renderWorld.GetDevice(), m_renderWorld.GetContext()))
		return false;
#endif // IMGUI_ACTIVATE

	_RegisterRawInput(hWnd_);

	//m_rawInputDispatcher.init();
	m_rawInputDispatcher.GetMouse().SetWindowTarget(m_window);

	CInputManager::Get().Initialize();
	CInputManager::Get().SetMouseDevice(m_rawInputDispatcher.GetMouse());
	CInputManager::Get().SetKeyboardDevice(m_rawInputDispatcher.GetKeyboard());
	CInputManager::Get().SetGamePadDevice(m_rawInputDispatcher.GetGamePad());

	return true;
}

void Application::Release()
{
#ifdef IMGUI_ACTIVATE
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // IMGUI_ACTIVATE
}

void Application::Tick()
{
	m_window.CalcWindowSize();

	CInputManager::Get().BeginFrame();
	{
		// Input Dispatch
		m_rawInputDispatcher.DispatchRawQueue();

		// Logic
		{
#ifdef IMGUI_ACTIVATE
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#endif // IMGUI_ACTIVATE

			m_gameWorld.Tick();

			m_gameWorld.BuildRenderFrame();
		}

		// Render
		{
			m_renderWorld.BeginFrame();

			m_renderWorld.DrawFrame();

#ifdef IMGUI_ACTIVATE
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI_ACTIVATE

			m_renderWorld.EndFrame();
		}
	}
	CInputManager::Get().EndFrame();
}

LRESULT Application::WndProc(HWND hWnd_, UINT uMessage_, WPARAM wParam_, LPARAM lParam_)
{
	//return DefWindowProc(hWnd_, uMessage_, wParam_, lParam_);

	switch (uMessage_)
	{
		case WM_MOVE:
		case WM_SIZE:
		{
			
		} break;

		case WM_INPUT :
		{
			UINT size = 0;
			GetRawInputData((HRAWINPUT)lParam_, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

			static BYTE buffer[1024];
			if (size > sizeof(buffer)) break;

			if (GetRawInputData((HRAWINPUT)lParam_, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) != size) break;

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);
			m_rawInputDispatcher.Push(*raw);
			return 0;
		} break;

		case WM_KEYUP: {
			if (wParam_ == VK_ESCAPE) 
				DestroyWindow(hWnd_);
		} break;
	}

	return DefWindowProc(hWnd_, uMessage_, wParam_, lParam_);
}

void Application::_RegisterRawInput(HWND hWnd_)
{
	const UINT SIZE_OF_DEVICE = 3;
	RAWINPUTDEVICE devices[SIZE_OF_DEVICE] = {};

	// keyboard
	devices[0].usUsagePage = 0x01;
	devices[0].usUsage = 0x06; // keyboard
	devices[0].dwFlags = 0;
	devices[0].hwndTarget = hWnd_;

	// mouse
	devices[1].usUsagePage = 0x01;
	devices[1].usUsage = 0x02; // mouse
	devices[1].dwFlags = 0;
	devices[1].hwndTarget = hWnd_;

	// game pad
	devices[2].usUsagePage = 0x01;
	devices[2].usUsage = 0x05; // game pad
	devices[2].dwFlags = 0;
	devices[2].hwndTarget = hWnd_;

	if (FALSE == RegisterRawInputDevices(devices, SIZE_OF_DEVICE, sizeof(RAWINPUTDEVICE))) {
		assert(false && "RegisterRawInputDevice failed!");
	}
}
