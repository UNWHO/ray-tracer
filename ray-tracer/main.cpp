#include <tchar.h>

#include "window.h"
#include "graphic.h"
#include "ui.h"


int main(int, char**)
{
	const int WIDTH = 1280;
	const int HEIGHT = 720;

	Window& window = Window::getInstance();
	window.Init(WIDTH, HEIGHT);

	// Initialize Direct3D
	Graphic& graphic = Graphic::getInstance();
	if (graphic.Init(window.GetHandle(), window.GetWidth(), window.GetHeight()) == false) {
		graphic.Destory();
		window.Destroy();

		return 1;
	}

	UI& ui = UI::getInstance();
	ui.Init(window.GetHandle(), graphic);


	// Our state
	float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };


	while (!window.isDone())
	{
		window.HandleMessage();


		ui.Update();

		// Rendering
		graphic.Render(clear_color);
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
