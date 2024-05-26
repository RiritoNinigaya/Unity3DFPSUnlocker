#pragma once

#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <d3d9.h>
#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imconfig.h"
#include "3rdparty/ImGui/imgui_impl_dx11.h"
#include "3rdparty/ImGui/imgui_impl_win32.h"
#include "3rdparty/UnityResolve.hpp"
#include "3rdparty/nimbussansfont.hpp"
#include "3rdparty/kiero/kiero.h"
using namespace std;
typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;
Present oPresent;
using namespace std;
// Declare internal and external function for handling Win32 window messages
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
bool init = false;
bool init_hook = false;
void AllocateConsole() {
	AllocConsole();
	SetConsoleTitleA("Unity3DFPSUnlocker Debugger by RiritoNinigaya");
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	printf("Console Is Created!!!");
}
namespace Unity3DFPSUnlocker {
	namespace UNITY_FUNCTION {
		static int GetTargetFrameRate()
		{
			static UnityResolve::Method* method;
			if (!method) method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<UnityResolve::Method>("get_targetFrameRate");
			if (method) return method->Invoke<int>();
			return 0;
		}

		static void SetTargetFrameRate(int value)
		{
			static UnityResolve::Method* method;
			if (!method) method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<UnityResolve::Method>("set_targetFrameRate");
			if (method) method->Invoke<void>(value);
		}
		static void SetVSyncCount(int value)
		{
			static UnityResolve::Method* method;
			if (!method) method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("QualitySettings")->Get<UnityResolve::Method>("set_vSyncCount");
			if (method) method->Invoke<void>(value);
		}
	}
	void AttachUnity3D() 
	{
		const char* il2cpp_gameasm = "GameAssembly.dll";
		HMODULE mod_il2cpp = GetModuleHandleA((LPCSTR)"GameAssembly.dll"); 
		if (mod_il2cpp == 0) {
			printf("\nNot Founded GameAssembly.dll");
			Sleep(4300);
			exit(122);
		}
		else {
			printf("\nFounded IL2Cpp Library or DLL!!!");
		}
		//Attaching UnityResolve
		UnityResolve::Init(mod_il2cpp, UnityResolve::Mode::Il2Cpp);
		UnityResolve::ThreadAttach(); //Attaching Thread!!! :D
	}
	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	void InitImgui() {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		io.Fonts->AddFontFromMemoryTTF(nimbussans, sizeof(nimbussans), 18.F);
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
	}
	void EndAndRender() {
		ImGui::End();
		ImGui::Render();
	}

	HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
		if (!init)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pDevice->GetImmediateContext(&pContext);
				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				window = sd.OutputWindow;
				ID3D11Texture2D* pBackBuffer{ 0 };
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView); //False Positive Warning DirectX 11 SDK(Idk Why)
				pBackBuffer->Release();
				oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
				InitImgui();
				init = true;
			}

			else
				return oPresent(pSwapChain, SyncInterval, Flags);
		}
		ImGuiWindowFlags fl = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings; //ImGuiWindowFlags_NoSavedSettings it is Preventing Saving Settings for some Cheeto :D
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Unity3D FPS Unlocker by RiritoNinigaya", (bool*)fl);
		ImGui::SetWindowSize(ImVec2(450, 600), 0); //Set Window Size :D
		ImGui::Text("Welcome to the Unity3DFPSUnlocker... \nMade by RiritoNinigaya!!! \nSo Enjoy to use this!!!");
		if (ImGui::Button("Set VSync and Target FrameRate As ZERO!!!")) 
		{
			Unity3DFPSUnlocker::UNITY_FUNCTION::SetTargetFrameRate(0);
			Unity3DFPSUnlocker::UNITY_FUNCTION::SetVSyncCount(0);
		}
		if (ImGui::BeginMenu("FrameRate Menu")) 
		{
			ImGui::TextColored(ImVec4(100.f, 0.f, 33.f, 255.f), "FrameRate Count: %s", (const char*)Unity3DFPSUnlocker::UNITY_FUNCTION::GetTargetFrameRate());
			ImGui::EndMenu();
		}
		EndAndRender();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		return oPresent(pSwapChain, SyncInterval, Flags);
	}
	DWORD WINAPI Thread() 
	{
		AllocateConsole();
		AttachUnity3D();
		do
		{
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				kiero::bind(8, (void**)&oPresent, hkPresent);
				init_hook = true;
			}
			else {
				Sleep(8000);
				exit(122);
			}
		} while (!init_hook);
		return TRUE;
	}
}
