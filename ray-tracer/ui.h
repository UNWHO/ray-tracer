#pragma once
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "graphic.h"

struct State {
	struct {
		float pos[3];
		float color[4];
	} light;

	Sphere* sphere;
};

class UI
{
private:
	UI() {};
	UI(const UI& ui) {};
	UI& operator=(const UI&) {};
	~UI() {};

	ImGuiIO* io_;

public:
	static UI& getInstance() {
		static UI ui;
		return ui;
	}

	void Init(HWND hWnd, const Graphic& graphic);
	void Destory();

	void Update(State& state);
	void Render();
};

