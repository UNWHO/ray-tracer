#pragma once
#include <d3d11.h>

class Graphic
{
private:
	Graphic() : device_(nullptr), device_context_(nullptr), swap_chain_(nullptr), render_target_view_(nullptr) {};
	Graphic(const Graphic&) : device_(nullptr), device_context_(nullptr), swap_chain_(nullptr), render_target_view_(nullptr) {};
	Graphic& operator=(const Graphic&) {};
	~Graphic() {};

	bool InitBuffers();
	bool InitShaders();

	void CreateRenderTarget();
	void CleanupRenderTarget();

	ID3D11Device* device_;
	ID3D11DeviceContext* device_context_;
	IDXGISwapChain* swap_chain_;
	ID3D11RenderTargetView* render_target_view_;

	ID3D11Buffer* vertex_buffer_;
	UINT vertex_buffer_stride_;
	UINT vertex_buffer_offset_;

	ID3D11Buffer* index_buffer_;
	UINT index_count_;

	ID3D11InputLayout* input_layout_;

	ID3D11VertexShader* vertex_shader_;
	ID3D11PixelShader* pixel_shader_;

	D3D11_VIEWPORT viewport_;


public:
	static Graphic& getInstance() {
		static Graphic d3d11;
		return d3d11;
	}

	bool Init(HWND hWnd, int widht, int height);
	void Destory();

	void Resize(UINT width, UINT height);
	void Render(const float clear_color[4]);
	void Present();

	ID3D11Device* GetDevice() const { return device_; };
	ID3D11DeviceContext* GetDeviceContext() const { return device_context_; };
};

