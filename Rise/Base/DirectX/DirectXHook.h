#pragma once

// Include Fonts
#include "Comfortaa.h" // Comfortaa's font.
#include "ComfortaaBold.h" // Comfortaa's font.
#include "Comic.h" // MojangLes's font.
#include "Icons.h" // Icon's font.
#include "MojangLes.h" // MojangLes's font.
#include "ProductSansBold.h" // ProductSans's font.
#include "ProductSans.h" // ProductSans's font.

void* oPresent;
void* oResize;

typedef HRESULT(__thiscall* drawIndexed)(struct ID3D11DeviceContext*, unsigned int, unsigned int, int);
drawIndexed oDrawIndexed;

DWORD frameHostPID;
bool imguiInit = false;

bool hookedDrawIndexed = false;

//SimpleHook<HRESULT, ID3D11DeviceContext*, UINT, UINT, INT> drawIndexedHook;

ID3D11Device* d3d11Device = nullptr;
ID3D12Device* d3d12Device = nullptr;


int TargetHeadWidth = 0;
int TargetHeadHeight = 0;

int CombatWidth = 0;
int CombatHeight = 0;

int MovementWidth = 0;
int MovementHeight = 0;

int PlayerWidth = 0;
int PlayerHeight = 0;

int VisualWidth = 0;
int VisualHeight = 0;

int MiscWidth = 0;
int MiscHeight = 0;

int UserProfileWidth = 0;
int UserProfileHeight = 0;

// Include LoadTextureFromFile
#include "LoadTexture.h"

// Simple helper function to load an image into a DX11 texture with common settings
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	d3d11Device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	d3d11Device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

void callRender() { // onImGui CallBack for modules
	ImGui::GetIO().FontGlobalScale = 0.5f;

	// ImGui Render stuff here
	ImGuiRenderEvent event{}; // ImGuiRenderEvent
	event.cancelled = nullptr;
	CallBackEvent(&event); // Call ImGui event for modules to be writen on this hook.

	if (Global::RenderNotifications) {
		Notifications::renderNotifications();
	}

	Authentication::RenderUI();
}

void loadFonts() { // load ImGui font.
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ComfortaaTTF, sizeof(ComfortaaTTF), 48); // 0
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ComfortaaBoldTTF, sizeof(ComfortaaBoldTTF), 48); // 1
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(ProductSansCompressedData, ProductSansCompressedSize, 48.f); // 
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ProductSansBoldTTF, sizeof(ProductSansBoldTTF), 48); // 3
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(Mojangles, sizeof(Mojangles), 48.f); // 4
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(ComicTTF, sizeof(ComicTTF), 48.f); // 5
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(IconTTF, sizeof(IconTTF), 48.f); // 6

	ImGui::GetStyle().WindowRounding = 13.f; // Set the default Windows Rounding to 8
	std::string HeadImageFilePath = FileUtils::getResourcePath() + "\\TargetHead.png";
	std::string CombatImageFilePath = FileUtils::getResourcePath() + "\\Combat.png";
	std::string MovementImageFilePath = FileUtils::getResourcePath() + "\\Movement.png";
	std::string PlayerImageFilePath = FileUtils::getResourcePath() + "\\Player.png";
	std::string VisualImageFilePath = FileUtils::getResourcePath() + "\\Visual.png";
	std::string MiscImageFilePath = FileUtils::getResourcePath() + "\\Misc.png";
	LoadTextureFromFile(HeadImageFilePath.c_str(), &Global::RenderInfo::HeadTexture, &TargetHeadWidth, &TargetHeadHeight);
	LoadTextureFromFile(CombatImageFilePath.c_str(), &Global::RenderInfo::CombatTexture, &CombatWidth, &CombatHeight);
	LoadTextureFromFile(MovementImageFilePath.c_str(), &Global::RenderInfo::MovementTexture, &MovementWidth, &MovementHeight);
	LoadTextureFromFile(PlayerImageFilePath.c_str(), &Global::RenderInfo::PlayerTexture, &PlayerWidth, &PlayerHeight);
	LoadTextureFromFile(VisualImageFilePath.c_str(), &Global::RenderInfo::VisualTexture, &VisualWidth, &VisualHeight);
	LoadTextureFromFile(MiscImageFilePath.c_str(), &Global::RenderInfo::MiscTexture, &MiscWidth, &MiscHeight);
	//LoadTextureFromFile(UserProfileImageFilePath.c_str(), &Global::RenderInfo::UserProfileTexture, &UserProfileWidth, &UserProfileHeight);
}

HRESULT DrawIndexed(struct ID3D11DeviceContext* Context, unsigned int IndexCount, unsigned int StartIndexLocation, int BaseVertexLocation) {
	ID3D11Buffer* vertBuffer;
	UINT vertBufferOffset;
	UINT stride;

	ImGui::ShowDemoWindow();

	Context->IAGetVertexBuffers(0, 1, &vertBuffer, &stride, &vertBufferOffset);
	if (stride == 24) {
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ID3D11DepthStencilState* depthStencil;
		UINT stencilRef;

		// get info about current depth stencil
		Context->OMGetDepthStencilState(&depthStencil, &stencilRef);
		depthStencil->GetDesc(&depthStencilDesc);
		depthStencilDesc.DepthEnable = false; // disable depth to ignore all other geometry
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable writing to depth buffer
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; // always pass depth test
		depthStencilDesc.StencilEnable = false; // disable stencil
		depthStencilDesc.StencilReadMask = 0; // disable stencil


		// create a new depth stencil based on current drawn one but with the depth disabled as mentioned before
		d3d11Device->CreateDepthStencilState(&depthStencilDesc, &depthStencil);
		Context->OMSetDepthStencilState(depthStencil, stencilRef);

		// call original function
		oDrawIndexed(Context, IndexCount, StartIndexLocation, BaseVertexLocation);

		// release memory
		depthStencil->Release();

		


		// draw a glow effect

		return 0;
	}

	/*if (isSecondPass && daSwapChain)
	{
		isSecondPass = false;

		ImGuiHelper::Init(daSwapChain, device11.get(), context.get());
		D2D::Init(daSwapChain, device11.get());

		// Start a new imgui fram
		ImGuiHelper::NewFrame();

		// draw a rect
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(100, 100), ImVec2(1000, 600), IM_COL32(255, 0, 255, 255));

		ImGuiHelper::EndFrame();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}*/

	return oDrawIndexed(Context, IndexCount, StartIndexLocation, BaseVertexLocation);
}

HRESULT D3D12_PresentDetour(IDXGISwapChain3* pSwapChain, int syncInterval, int flags) {
	if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d11Device)))) {
		ID3D11DeviceContext* pContext = nullptr;
		d3d11Device->GetImmediateContext(&pContext);

		// DrawIndexedHook
	     uintptr_t* contextVfT = *(uintptr_t**)(pContext);
		 bool isInitialized = false;

		 if (!isInitialized) {

			 /*if (MH_CreateHook((void*)contextVfT[12], &DrawIndexedDetour, (LPVOID*)onDrawIndexed) != MH_OK)
			 {
				 return false;
			 }

			 if (MH_EnableHook((void*)contextVfT[12]) != MH_OK)
			 {
				 return false;
			 }*/

			 //drawIndexedHook.Initialize((void*)contextVfT[12], &DrawIndexedDetour, "DrawIndexed", true);
		 }

		/*static bool tryHookIndexed = false;

		if (!tryHookIndexed) {
			Utils::HookFunction((void*)contextVfT[12], &DrawIndexedDetour, &onDrawIndexed, "DrawIndexed");
			tryHookIndexed = true;
		}
		*/

		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

		IDXGISurface* dxgiBackBuffer;
		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

		ID3D11RenderTargetView* mainRenderTargetView = nullptr;
		if (pBackBuffer != nullptr) {
			d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		}

		if (!imguiInit) {
			ImGui::CreateContext();
			loadFonts();
			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = NULL;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(d3d11Device, pContext);
			imguiInit = true;
		}
		pBackBuffer->Release();
		ImFX::NewFrame(d3d11Device, dxgiBackBuffer, GetDpiForWindow(window));
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		callRender(); // Render the modules and stuff in imgui
		ImGui::EndFrame();
		ImGui::Render();

		if (mainRenderTargetView != nullptr)
			pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplDX11_RenderDrawData(drawData);
		ImFX::EndFrame();
		pContext->Flush();
		pContext->Release();
		if (mainRenderTargetView)
			mainRenderTargetView->Release();
		d3d11Device->Release();
	}
	else if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12Device)))) {
		pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		static_cast<ID3D12Device5*>(d3d12Device)->RemoveDevice();
	}

	return Utils::CallFunc<HRESULT, IDXGISwapChain3*, UINT, UINT>(oPresent, pSwapChain, syncInterval, flags);
}

HRESULT resizeBuffersCallback(IDXGISwapChain* pSwapChain, int bufferCount, int width, int height, DXGI_FORMAT newFormat, int swapChainFlags) {
	ImFX::CleanupFX();

	return Utils::CallFunc<HRESULT, IDXGISwapChain*, int, int, int, DXGI_FORMAT, int>(oResize, pSwapChain, bufferCount, width,
		height, newFormat, swapChainFlags);
}

class DirectXHook : public FuncHook {
public:
	bool Initialize() override {
		winrt::Windows::UI::Core::CoreWindow cw = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
		winrt::com_ptr<ICoreWindowInterop> interop;
		winrt::check_hresult(winrt::get_unknown(cw)->QueryInterface(interop.put()));
		winrt::check_hresult(interop->get_WindowHandle(&window));

		if (kiero::init(kiero::RenderType::D3D12) != kiero::Status::Success && kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) {
			FileUtils::debugOutput("[Rise] [DirectX] failed to initialize hook");
			return false;
		}

		uint16_t index = kiero::getRenderType() == kiero::RenderType::D3D12 ? 145 : 13;

		if (kiero::bind(index, (void**)&oResize, resizeBuffersCallback) != kiero::Status::Success) {
			FileUtils::debugOutput("[Rise] [DirectX] failed to create hook for resize buffer");
			return false;
		}

		uint16_t index2 = kiero::getRenderType() == kiero::RenderType::D3D12 ? 140 : 8;

		if (kiero::bind(index2, (void**)&oPresent, D3D12_PresentDetour) != kiero::Status::Success) {
			FileUtils::debugOutput("[Rise] [DirectX] failed to create hook for present");
			return false;
		}

		return true;
	}

	static DirectXHook& Instance() {
		static DirectXHook instance;
		return instance;
	}
};