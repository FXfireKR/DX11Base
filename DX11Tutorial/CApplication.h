#pragma once
#include "Texture.h"
#include "ShaderManager.h"

const bool FULL_SCREEN = false;
const int SCREENX = 1440;
const int SCREENY = 1024;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
};

class Application
{
public:
	Application();
	~Application();

	bool Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_);
	void Release();
	void Tick();
	void Render();
	LRESULT CALLBACK WndProc(HWND hWnd_, UINT uMessage_, WPARAM wParam_, LPARAM lParam_);


private:
	ShaderManager shaderManager;
	vector<Vertex> vertices;
	ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

	vector<UINT> indices;
	ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;

	XMFLOAT3 offsetPosition;
	ComPtr<ID3D11Buffer> pDynamicConstBuffer = nullptr;

	Texture otakuTexture;
};