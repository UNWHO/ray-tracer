#include "ui.h"

void UI::Init(HWND hWnd, const Graphic& graphic) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	io_ = &ImGui::GetIO();



	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(graphic.GetDevice(), graphic.GetDeviceContext());
}

void UI::Update(State& state) {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	ImGui::Begin("Options");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io_->Framerate, io_->Framerate);
	
	if (state.sphere != nullptr) {
		ImGui::BeginChild("Sphere");
		ImGui::ColorEdit4("Color", state.sphere->GetColorData());

		Phong* phong = state.sphere->GetPhongParameterData();
		ImGui::SliderFloat("Ambient", &phong->ambient, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse", &phong->diffuse, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular", &phong->specular, 0.0f, 1.0f);
		ImGui::SliderInt("Specular N", &phong->specular_n, 0, 1024);
		ImGui::EndChild();
	}
	
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
