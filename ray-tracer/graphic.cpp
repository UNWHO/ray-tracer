#include "graphic.h"

#include <iostream>
#include <d3dcompiler.h>
#include <system_error>



bool Graphic::Init(HWND hWnd, const unsigned int screen_width, const unsigned int screen_height) {
	screen_width_ = screen_width;
	screen_height_ = screen_height;


	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC descriptor;
	ZeroMemory(&descriptor, sizeof(descriptor));
	descriptor.BufferCount = 2;
	descriptor.BufferDesc.Width = screen_width_;
	descriptor.BufferDesc.Height = screen_height_;
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
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


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
	viewport_.Width = static_cast<float>(screen_width_);
	viewport_.Height = static_cast<float>(screen_height_);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	device_context_->RSSetViewports(1, &viewport_);


	if (InitTexture() == false) {
		std::cout << "Failed to init textures" << std::endl;
		return false;
	}
	device_context_->PSSetShaderResources(0, 1, &texture_resource_view_);


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

	if (texture_) {
		texture_->Release();
		texture_ = nullptr;
	}

	if (texture_resource_view_) {
		texture_resource_view_->Release();
		texture_resource_view_ = nullptr;
	}
}

void Graphic::Resize(UINT width, UINT height) {
	CleanupRenderTarget();
	swap_chain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

	// TODO: resize texture 

	CreateRenderTarget();
}

void Graphic::Render(const std::vector<Math::Color>& frame_buffer) {

	device_context_->OMSetRenderTargets(1, &render_target_view_, nullptr);
	//device_context_->ClearRenderTargetView(render_target_view_, clear_color_with_alpha);

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	ZeroMemory(&mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));


	device_context_->Map(texture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, frame_buffer.data(), sizeof(Math::Color) * frame_buffer.size());
	device_context_->Unmap(texture_, 0);

	device_context_->DrawIndexed(index_count_, 0, 0);
}

void Graphic::Present() {
	swap_chain_->Present(1, 0); // with vsync
	//swap_chain_->Present(0, 0); // without vsync
}

bool Graphic::InitBuffers() {
	const float vertices[][4] = {
		// LEFT TOP 
		{
			-1.0f, -1.0f, 0.0f, 1.0f,
		},

		// LEFT BOTTOM
		{
			-1.0f, 1.0f, 0.0f, 1.0f,
		},

		// RIGHT BOTTOM 
		{
			1.0f, 1.0f, 0.0f, 1.0f,
		},

		// RIGHT TOP 
		{
			1.0f, -1.0f, 0.0f, 1.0f,
		},
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

bool Graphic::InitTexture() {
	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = screen_width_;
	texture_desc.Height = screen_height_;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DYNAMIC;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture_desc.MiscFlags = 0;


	texture_ = NULL;
	HRESULT result = device_->CreateTexture2D(&texture_desc, NULL, &texture_);
	if (FAILED(result)) {
		Debug();
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resource_view_desc;
	resource_view_desc.Format = texture_desc.Format;
	resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resource_view_desc.Texture2D.MostDetailedMip = 0;
	resource_view_desc.Texture2D.MipLevels = 1;

	result = device_->CreateShaderResourceView(texture_, &resource_view_desc, &texture_resource_view_);
	if (FAILED(result)) {
		return false;
	}

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

void Graphic::Debug() {
	ID3D11InfoQueue* debug_info_queue;
	device_->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&debug_info_queue);

	UINT64 message_count = debug_info_queue->GetNumStoredMessages();

	for (UINT64 i = 0; i < message_count; i++) {
		SIZE_T message_size = 0;
		debug_info_queue->GetMessage(i, nullptr, &message_size); //get the size of the message

		D3D11_MESSAGE* message = (D3D11_MESSAGE*)malloc(message_size); //allocate enough space
		debug_info_queue->GetMessage(i, message, &message_size); //get the actual message

		printf("Directx11: %.*s\n", static_cast<int>(message->DescriptionByteLength), message->pDescription);

		free(message);
	}

	debug_info_queue->ClearStoredMessages();
}