#pragma once
#include <Windows.h>
#include <imgui.h>

class Window
{
private:
	Window() {};
	Window(const Window&) {};
	Window& operator=(const Window&) {};
	~Window() {};

	WNDCLASSEXW wc_;
	HWND hwnd_;

	bool done_;

	friend LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	static Window& getInstance() {
		static Window window;
		return window;
	}

	void Init();
	void Destroy();
	bool HandleMessage();

	HWND GetHandle() const { return hwnd_; };
	bool isDone() const { return done_; };
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);