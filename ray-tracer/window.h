#pragma once
#include <Windows.h>
#include <imgui.h>

#include <vector>
#include <functional>

class Window
{
private:
	Window() {};
	Window(const Window&) {};
	Window& operator=(const Window&) {};
	~Window() {};

	WNDCLASSEXW wc_;
	HWND hwnd_;

	int width_;
	int height_;
	bool done_;

	std::vector<std::function<void(int, int)>> mouse_left_click_listeners_;
	std::vector<std::function<void(int keyCode)>> keydown_listeners_;

	void OnMouseLeftClick(int x, int y) {
		for (std::function<void(int, int)> listener : mouse_left_click_listeners_) {
			listener(x, y);
		}
	};

	void OnKeyDown(int keyCode) {
		for (std::function<void(int)> listener : keydown_listeners_) {
			listener(keyCode);
		};
	}

	friend LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	static Window& getInstance() {
		static Window window;
		return window;
	}

	void Init(const int width, const int height);
	void Destroy();
	bool HandleMessage();

	void AddMouseLeftClickListener(std::function<void(int x, int y)> listener) {
		mouse_left_click_listeners_.push_back(listener);
	}
	void AddKeydownListener(std::function<void(int keyCode)> listener) {
		keydown_listeners_.push_back(listener);
	}

	HWND GetHandle() const { return hwnd_; };
	bool isDone() const { return done_; };
	int GetWidth() const { return width_; };
	int GetHeight() const { return height_; };
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);