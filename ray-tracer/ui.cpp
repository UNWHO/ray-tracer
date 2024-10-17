#include "ui.h"

void UI::Init(HWND hWnd, const Graphic& graphic) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	io_ = &ImGui::GetIO();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(graphic.GetDevice(), graphic.GetDeviceContext());
}

void UI::Update() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Options");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io_->Framerate, io_->Framerate);
	ImGui::End();
	ImGui::Render();
}

void UI::Render() {
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::Destory() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
