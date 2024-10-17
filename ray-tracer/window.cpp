#include "window.h"

void Window::Init(const int width,const int height) {
	width_ = width;
	height_ = height;
	done_ = false;
	
	wc_ = {
		sizeof(wc_),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(nullptr),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		L"Ray Tracer",
		nullptr
	};
	RegisterClassExW(&wc_);

	hwnd_ = CreateWindowW(wc_.lpszClassName, L"Ray Tracer", WS_OVERLAPPEDWINDOW, 100, 100, width_, height_, nullptr, nullptr, wc_.hInstance, nullptr);

	ShowWindow(hwnd_, SW_SHOWDEFAULT);
	UpdateWindow(hwnd_);
}

bool Window::HandleMessage() {
	MSG msg;
	if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) == false) {
		return false;
	}

	if (msg.message == WM_QUIT) {
		done_ = true;
		return true;
	}

	TranslateMessage(&msg);
	DispatchMessage(&msg);

	return true;
}

void Window::Destroy() {
	DestroyWindow(hwnd_);
	UnregisterClassW(wc_.lpszClassName, wc_.hInstance);
}


// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;

		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}