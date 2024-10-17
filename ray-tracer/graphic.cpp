#include "graphic.h"

bool Graphic::Init(HWND hWnd) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC descriptor;
	ZeroMemory(&descriptor, sizeof(descriptor));
	descriptor.BufferCount = 2;
	descriptor.BufferDesc.Width = 0;
	descriptor.BufferDesc.Height = 0;
	descriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descriptor.BufferDesc.RefreshRate.Numerator = 60;
	descriptor.BufferDesc.RefreshRate.Denominator = 1;
	descriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descriptor.OutputWindow = hWnd;
	descriptor.SampleDesc.Count = 1;
	descriptor.SampleDesc.Quality = 0;
	descriptor.Windowed = TRUE;
	descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT create_device_flags = 0;
	//create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL feature_level;
	const D3D_FEATURE_LEVEL feature_levels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_levels, 2, D3D11_SDK_VERSION, &descriptor, &swap_chain_, &device_, &feature_level, &device_context_);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_levels, 2, D3D11_SDK_VERSION, &descriptor, &swap_chain_, &device_, &feature_level, &device_context_);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void Graphic::Destory() {
	CleanupRenderTarget();

	if (swap_chain_) {
		swap_chain_->Release();
		swap_chain_ = nullptr;
	}

	if (device_context_) {
		device_context_->Release();
		device_context_ = nullptr;
	}

	if (device_) {
		device_->Release();
		device_ = nullptr;
	}
}

void Graphic::Resize(UINT width, UINT height) {
	CleanupRenderTarget();
	swap_chain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	CreateRenderTarget();
}

void Graphic::Render(const float clear_color[4]) {
	const float clear_color_with_alpha[4] = { clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3] };
	device_context_->OMSetRenderTargets(1, &render_target_view_, nullptr);
	device_context_->ClearRenderTargetView(render_target_view_, clear_color_with_alpha);
}

void Graphic::Present() {
	swap_chain_->Present(1, 0); // with vsync
	//swap_chain_->Present(0, 0); // without vsync
}

void Graphic::CreateRenderTarget()
{
	ID3D11Texture2D* back_buffer;
	swap_chain_->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	device_->CreateRenderTargetView(back_buffer, nullptr, &render_target_view_);
	back_buffer->Release();
}

void Graphic::CleanupRenderTarget()
{
	if (render_target_view_) {
		render_target_view_->Release();
		render_target_view_ = nullptr;
	}
}