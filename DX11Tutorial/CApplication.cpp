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
	if (FAILED(DXCOM.Initialize(hWnd_, iScreenWidth_, iScreenHeight_, false, 1000.0f, 0.3f))) 
		return false;


	GAMETIME.Init();

	return true;
}

void Application::Release()
{
}

void Application::Tick()
{
	// game world update

	// render world update

	DXCOM.ImGuiTick();

#ifdef IMGUI_ACTIVATE
	// ImGui
	{
		ImGui::Text("%u", GAMETIME.GetFps());
	}
#endif // IMGUI_ACTIVATE

	GAMETIME.Tick();

	DXCOM.BeginRender();
	{
		//{
		//	shaderManager.Render(otakuTexture.GetShaderResourceView().GetAddressOf(), pVertexBuffer.GetAddressOf(), pDynamicConstBuffer.GetAddressOf()
		//		, pIndexBuffer.Get(), sizeof(Vertex), 0, vertices.size());
		//}
		DXCOM.ImGuiRender();
	}
	DXCOM.EndRender();

}

//void Application::Render()
//{
//	DXCOM->BeginRender();
//	{
//		//{
//		//	shaderManager.Render(otakuTexture.GetShaderResourceView().GetAddressOf(), pVertexBuffer.GetAddressOf(), pDynamicConstBuffer.GetAddressOf()
//		//		, pIndexBuffer.Get(), sizeof(Vertex), 0, vertices.size());
//		//}
//
//		//DXCOM->ImGuiRender();
//	}
//	DXCOM->EndRender();
//}

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
