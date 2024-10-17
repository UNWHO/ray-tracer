#include "graphic.h"

#include <d3dcompiler.h>

bool Graphic::Init(HWND hWnd, const int width, const int height) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC descriptor;
	ZeroMemory(&descriptor, sizeof(descriptor));
	descriptor.BufferCount = 2;
	descriptor.BufferDesc.Width = width;
	descriptor.BufferDesc.Height = height;
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

	ZeroMemory(&viewport_, sizeof(viewport_));
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.Width = width;
	viewport_.Height = height;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	device_context_->RSSetViewports(1, &viewport_);

	if (InitBuffers() == false) {
		return false;
	}
	device_context_->IASetVertexBuffers(0, 1, &vertex_buffer_, &vertex_buffer_stride_, &vertex_buffer_offset_);
	device_context_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
	device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (InitShaders() == false) {
		return false;
	}
	device_context_->VSSetShader(vertex_shader_, NULL, 0);
	device_context_->PSSetShader(pixel_shader_, NULL, 0);
	device_context_->IASetInputLayout(input_layout_);

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

	if (vertex_buffer_) {
		vertex_buffer_->Release();
		vertex_buffer_ = nullptr;
	}

	if (index_buffer_) {
		index_buffer_->Release();
		index_buffer_ = nullptr;
	}

	if (vertex_shader_) {
		vertex_shader_->Release();
		vertex_shader_ = nullptr;
	}

	if (pixel_shader_) {
		pixel_shader_->Release();
		pixel_shader_ = nullptr;
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

	//device_context_->IASetVertexBuffers(0, 1, &vertex_buffer_, &vertex_buffer_stride_, &vertex_buffer_offset_);
	//device_context_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
	//device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//device_context_->VSSetShader(vertex_shader_, NULL, 0);
	//device_context_->PSSetShader(pixel_shader_, NULL, 0);
	//device_context_->IASetInputLayout(input_layout_);

	device_context_->DrawIndexed(index_count_, 0, 0);
}

void Graphic::Present() {
	swap_chain_->Present(1, 0); // with vsync
	//swap_chain_->Present(0, 0); // without vsync
}

bool Graphic::InitBuffers() {
	const float vertices[][4] = {
		{ -1.0f, -1.0f, 0.0f, 1.0f }, // TOP LEFT 
		{ -1.0f, 1.0f, 0.0f, 1.0f }, // BOTTOM LEFT
		{ 1.0f, 1.0f, 0.0f, 1.0f }, // BOTTOM RIGHT
		{ 1.0f, -1.0f, 0.0f, 1.0f }, // TOP RIGHT
	};
	vertex_buffer_stride_ = sizeof(float) * 4;
	vertex_buffer_offset_ = 0;

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.ByteWidth = vertex_buffer_stride_ * 4;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(vertex_buffer_stride_);

	D3D11_SUBRESOURCE_DATA subresource_data;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	HRESULT result = device_->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer_);
	if (FAILED(result)) {
		return false;
	}

	const unsigned int indices[][3] = {
		{0, 1, 3},
		{1, 2, 3}
	};
	index_count_ = 6;

	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.ByteWidth = sizeof(unsigned int) * index_count_;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(unsigned int);

	subresource_data.pSysMem = indices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	result = device_->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer_);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

bool Graphic::InitShaders() {
	ID3DBlob* vs_blob;
	HRESULT result = D3DCompileFromFile(L"vertex_shader.hlsl", NULL, NULL, "main", "vs_5_0", 0, 0, &vs_blob, NULL);
	if (FAILED(result)) {
		if (vs_blob) {
			vs_blob->Release();
		}
		return false;
	}

	ID3DBlob* ps_blob;
	result = D3DCompileFromFile(L"pixel_shader.hlsl", NULL, NULL, "main", "ps_5_0", 0, 0, &ps_blob, NULL);
	if (FAILED(result)) {
		if (ps_blob) {
			ps_blob->Release();
		}
		return false;
	}

	result = device_->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &vertex_shader_);
	if (FAILED(result)) {
		return false;
	}

	result = device_->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), NULL, &pixel_shader_);
	if (FAILED(result)) {
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC input_elem_desc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	device_->CreateInputLayout(input_elem_desc, 1, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout_);
	
	return true;
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