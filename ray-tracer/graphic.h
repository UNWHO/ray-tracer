#pragma once
#include <d3d11.h>

class Graphic
{
private:
	Graphic() : device_(nullptr), device_context_(nullptr), swap_chain_(nullptr), render_target_view_(nullptr) {};
	Graphic(const Graphic& ref): device_(nullptr), device_context_(nullptr), swap_chain_(nullptr), render_target_view_(nullptr) {};
	Graphic& operator=(const Graphic& ref) {};
	~Graphic() {};
	
	void CreateRenderTarget();
	void CleanupRenderTarget();

	ID3D11Device* device_;
	ID3D11DeviceContext* device_context_;
	IDXGISwapChain* swap_chain_;
	ID3D11RenderTargetView* render_target_view_;

public: 
	static Graphic* getInstance() {
		static Graphic d3d11;
		return &d3d11;
	}

	bool Init(HWND hWnd);
	void Destory();

	void Resize(UINT width, UINT height);
	void Render(const float clear_color[4]);
	void Present();

	ID3D11Device* GetDevice() { return device_; };
	ID3D11DeviceContext* GetDeviceContext() { return device_context_; };
};

