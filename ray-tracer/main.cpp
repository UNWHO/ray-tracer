#include <tchar.h>

#include "window.h"
#include "graphic.h"
#include "ui.h"
#include "scene.h"

#include <iostream>


int main(int, char**)
{
	const int WIDTH = 1280;
	const int HEIGHT = 720;

	Window& window = Window::getInstance();
	window.Init(WIDTH, HEIGHT);

	// Initialize Direct3D
	Graphic& graphic = Graphic::getInstance();
	if (graphic.Init(window.GetHandle(), window.GetWidth(), window.GetHeight()) == false) {
#ifdef _DEBUG
		graphic.Debug();
#endif
		graphic.Destory();
		window.Destroy();

		return 1;
	}

	UI& ui = UI::getInstance();
	ui.Init(window.GetHandle(), graphic);

	// state
	State state;
	state.sphere = nullptr;

	// setup scene
	Scene scene;
	scene.SetCameraPosition(Math::Vector3(0.0f, 0.0f, 0.0f));
	scene.SetCameraDirection(Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector3(0.0f, 1.0f, 0.0f));
	scene.SetCameraResolution(WIDTH, HEIGHT);
	scene.SetCameraFoV(90.0f);

	scene.SetLightPosition(Math::Vector3(0.0f, -500.0f, WIDTH * 0.7f));
	scene.SetLightColor(Math::Color(1.0f, 1.0f, 1.0f, 1.0f));


	Sphere sphere(Math::Vector3(0.0f, 0.0f, WIDTH), Math::Color(1.0f, 0.0f, 0.0f, 1.0f), 100.0f);
	sphere.SetPhongParameter(0.2f, 0.4f, 0.4f, 10);
	scene.AddSphere(sphere);
	scene.AddSphere(Sphere(Math::Vector3(50.0f, 0.0f, WIDTH + 50), Math::Color(0.0f, 1.0f, 0.0f, 1.0f), 100.0f));


	// setup listneres
	window.AddKeydownListener([](int keyCode) {std::cout << keyCode << std::endl; });
	window.AddMouseLeftClickListener([&state, &scene](const int x, const int y) {
		state.sphere = scene.PeekSphere(x, y);
	});


	while (!window.isDone())
	{
		window.HandleMessage();

		ui.Update(state);

		// Rendering
		graphic.Render(scene.Render());
		ui.Render();

		// Present
		graphic.Present();
	}

	// Cleanup
	ui.Destory();
	graphic.Destory();
	window.Destroy();


	return 0;
}
