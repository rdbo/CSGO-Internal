#include "pch.h"
#include "hack.h"

void InitImGui(LPDIRECT3DDEVICE9 pDevice);

void DrawMenu()
{
	ImGui::Begin("CS:GO Multihack by rdbo");
	ImGui::BeginTabBar("navbar");

	if (ImGui::BeginTabItem("Misc"))
	{
		ImGui::Checkbox("Bunnyhop", &bhop);
		ImGui::Checkbox("Radar Hack", &radar_hack);
		ImGui::Checkbox("Glow Hack", &glow_hack);
		ImGui::Checkbox("ESP Snaplines", &esp_snapline);
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Glow"))
	{
		ImGui::SliderFloat("Team R", &GlowTeamColor[0], 0, 1, "%.1f");
		ImGui::SliderFloat("Team G", &GlowTeamColor[1], 0, 1, "%.1f");
		ImGui::SliderFloat("Team B", &GlowTeamColor[2], 0, 1, "%.1f");
		ImGui::SliderFloat("Team A", &GlowTeamColor[3], 0, 1, "%.1f");
		ImGui::Separator();
		ImGui::SliderFloat("Enemy R", &GlowEnemyColor[0], 0, 1, "%.1f");
		ImGui::SliderFloat("Enemy G", &GlowEnemyColor[1], 0, 1, "%.1f");
		ImGui::SliderFloat("Enemy B", &GlowEnemyColor[2], 0, 1, "%.1f");
		ImGui::SliderFloat("Enemy A", &GlowEnemyColor[3], 0, 1, "%.1f");
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("ESP Snaplines"))
	{
		ImGui::SliderInt("Team R", &esp_snapline_color_team[0], 0, 255);
		ImGui::SliderInt("Team G", &esp_snapline_color_team[1], 0, 255);
		ImGui::SliderInt("Team B", &esp_snapline_color_team[2], 0, 255);
		ImGui::SliderInt("Team A", &esp_snapline_color_team[3], 0, 255);
		ImGui::Separator();
		ImGui::SliderInt("Enemy R", &esp_snapline_color_enemy[0], 0, 255);
		ImGui::SliderInt("Enemy G", &esp_snapline_color_enemy[1], 0, 255);
		ImGui::SliderInt("Enemy B", &esp_snapline_color_enemy[2], 0, 255);
		ImGui::SliderInt("Enemy A", &esp_snapline_color_enemy[3], 0, 255);
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
	ImGui::End();
}

long __stdcall Game::D3D::hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	GetClientRect(window, &wrect);
	if (!bInitImgui)
	{
		InitImGui(pDevice);
		bInitImgui = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (bShowMenu)
	{
		DrawMenu();
	}

	if (esp_snapline && LocalPlayerAddr)
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (EntityList[i] != NULL)
			{
				ESP_SnapLine(EntityList[i], wrect, pDevice);
			}
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(Game::window);
	ImGui_ImplDX9_Init(pDevice);
}

bool Game::D3D::GetDevice(void** pTable, size_t Size)
{
	if (!pTable)
		return false;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;

	IDirect3DDevice9* pDummyDevice = NULL;
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();

	HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDeviceCreated != S_OK)
	{
		d3dpp.Windowed = !d3dpp.Windowed;

		dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDeviceCreated != S_OK)
		{
			pD3D->Release();
			return false;
		}
	}

	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

	pDummyDevice->Release();
	pD3D->Release();
	return true;
}