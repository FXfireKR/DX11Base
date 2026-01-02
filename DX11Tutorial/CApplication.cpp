#include "pch.h"
#include "CApplication.h"

Application::Application()
{
}

Application::~Application()
{
}

bool Application::Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_)
{
	if (FAILED(DXCOM->Initialize(hWnd_, iScreenWidth_, iScreenHeight_, false, 1000.0f, 0.3f))) 
		return false;

	shaderManager.Init();

	// 1 2
	// 0 3
	// VertexData
	{
		vertices.resize(4);
		vertices[0].position = XMFLOAT3(-0.5f, -0.5f, 0.f);
		vertices[1].position = XMFLOAT3(-0.5f, 0.5f, 0.f);
		vertices[2].position = XMFLOAT3(0.5f, 0.5f, 0.f);
		vertices[3].position = XMFLOAT3(0.5f, -0.5f, 0.f);

		vertices[0].uv = XMFLOAT2(0.f, 1.f);
		vertices[1].uv = XMFLOAT2(0.f, 0.f);
		vertices[2].uv = XMFLOAT2(1.f, 0.f);
		vertices[3].uv = XMFLOAT2(1.f, 1.f);
	}

	// Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = (UINT32)(sizeof(Vertex) * vertices.size());
		
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data();

		DXCOM->GetDevice()->CreateBuffer(&desc, &data, pVertexBuffer.GetAddressOf());		
	}

	// Index Data
	{
		indices.resize(6);
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;
	}
	
	// Index Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = (UINT32)(sizeof(UINT) * indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices.data();

		DXCOM->GetDevice()->CreateBuffer(&desc, &data, pIndexBuffer.GetAddressOf());
	}

	otakuTexture.LoadFromFile(L"image.jpg");

	return true;
}

void Application::Release()
{

}

void Application::Tick()
{
	//DXCOM->ImGuiTick();
	// ImGui
	{
		//ImGui::Text("ASDSADASDA");
	}

	Render();
}

void Application::Render()
{
	DXCOM->BeginRender();
	{
		{
			shaderManager.Render(otakuTexture.GetShaderResourceView().GetAddressOf(), pVertexBuffer.GetAddressOf(), pIndexBuffer.Get(), sizeof(Vertex), 0, vertices.size());
		}

		//DXCOM->ImGuiRender();
	}
	DXCOM->EndRender();
}

LRESULT Application::WndProc(HWND hWnd_, UINT uMessage_, WPARAM wParam_, LPARAM lParam_)
{
	return DefWindowProc(hWnd_, uMessage_, wParam_, lParam_);

	/*switch (uMessage_)
	{
		default: return DefWindowProc(hWnd_, uMessage_, wParam_, lParam_);
	}*/
}
