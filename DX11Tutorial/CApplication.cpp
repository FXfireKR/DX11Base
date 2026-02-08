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
#ifdef IMGUI_ACTIVATE
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // IMGUI_ACTIVATE

	m_gameWorld.Tick();


	m_gameWorld.BuildRenderFrame();

	m_renderWorld.BeginFrame();

	m_renderWorld.DrawFrame();

#ifdef IMGUI_ACTIVATE
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI_ACTIVATE

	m_renderWorld.EndFrame();
}

LRESULT Application::WndProc(HWND hWnd_, UINT uMessage_, WPARAM wParam_, LPARAM lParam_)
{
	//return DefWindowProc(hWnd_, uMessage_, wParam_, lParam_);

	switch (uMessage_)
	{
		case WM_KEYUP: {
			if (wParam_ == VK_ESCAPE) 
				DestroyWindow(hWnd_);
		} break;

		default: return DefWindowProc(hWnd_, uMessage_, wParam_, lParam_);
	}
}
