#pragma once
#include "CApplication.h"

class System
{
public:
	System();
	~System();

	bool Initialize();
	void Relase();
	void Run();
	LRESULT CALLBACK WndHandler(HWND hWnd_, UINT uMessage_, WPARAM wParam_, LPARAM lParam_);

private:
	void _InitializeWindow(int& iScreenWidth_, int& iScreenHeight_);
	void _Tick();

private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	LPCWSTR m_lpcWstrApkName;

	Application* m_pApplication;
};

static LRESULT CALLBACK WndProc(HWND hWnd_, UINT uMessage_, WPARAM wParam_, LPARAM lParam_);
static System* g_pSystem = 0;