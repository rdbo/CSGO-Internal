#include "pch.h"
#include "hack.h"
#include "colors.h"

bool menu_glow_team;
bool menu_glow_enemy;

bool menu_box_team;
bool menu_box_enemy;

bool menu_snapline_team;
bool menu_snapline_enemy;

void InitImGui(LPDIRECT3DDEVICE9 pDevice);

void DrawMenu()
{
	ImGui::Begin("CS:GO Multihack by rdbo");
	ImGui::BeginTabBar("navbar");


	//Tabs

	if (ImGui::BeginTabItem("ESP"))
	{
		ImGui::Text("ESP");
		ImGui::Checkbox("Radar ESP", &radar_hack);
		ImGui::Separator();
		ImGui::Text("ESP Box");
		ImGui::Checkbox("Enable ESP Box", &esp_box);
		if (ImGui::Button("Set Team Box Color"))
		{
			menu_box_team = !menu_box_team;
		}

		if (ImGui::Button("Set Enemy Box Color"))
		{
			menu_box_enemy = !menu_box_enemy;
		}

		ImGui::Separator();

		ImGui::Text("ESP Snaplines");
		ImGui::Checkbox("Enable ESP Snaplines", &esp_snapline);

		if(ImGui::Button("Set Team Line Color"))
		{
			menu_snapline_team = !menu_snapline_team;
		}

		if (ImGui::Button("Set Enemy Line Color"))
		{
			menu_snapline_enemy = !menu_snapline_enemy;
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Glow"))
	{
		ImGui::Text("Glow");
		ImGui::Checkbox("Enable Glow", &glow_hack);
		if (ImGui::Button("Set Team Glow Color"))
		{
			menu_glow_team = !menu_glow_team;
		}

		if (ImGui::Button("Set Enemy Glow Color"))
		{
			menu_glow_enemy = !menu_glow_enemy;
		}
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Misc"))
	{
		ImGui::Checkbox("Bunnyhop", &bhop);
		ImGui::Checkbox("Recoil Control System", &rcs);
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	//Windows

	if (menu_box_team)
	{
		ImGui::Begin("Change Team Box Color");
		ImGui::SliderInt("Team Box Red", &esp_box_color_team[0], 0, 255);
		ImGui::SliderInt("Team Box Green", &esp_box_color_team[1], 0, 255);
		ImGui::SliderInt("Team Box Blue", &esp_box_color_team[2], 0, 255);
		ImGui::SliderInt("Team Box Alpha", &esp_box_color_team[3], 0, 255);
		ImGui::End();
	}

	if (menu_box_enemy)
	{
		ImGui::Begin("Change Enemy Box Color");
		ImGui::SliderInt("Enemy Box Red", &esp_box_color_enemy[0], 0, 255);
		ImGui::SliderInt("Enemy Box Green", &esp_box_color_enemy[1], 0, 255);
		ImGui::SliderInt("Enemy Box Blue", &esp_box_color_enemy[2], 0, 255);
		ImGui::SliderInt("Enemy Box Alpha", &esp_box_color_enemy[3], 0, 255);
		ImGui::End();
	}

	if (menu_snapline_team)
	{
		ImGui::Begin("Change Team Snapline Color");
		ImGui::SliderInt("Team Line Red", &esp_snapline_color_team[0], 0, 255);
		ImGui::SliderInt("Team Line Green", &esp_snapline_color_team[1], 0, 255);
		ImGui::SliderInt("Team Line Blue", &esp_snapline_color_team[2], 0, 255);
		ImGui::SliderInt("Team Line Alpha", &esp_snapline_color_team[3], 0, 255);
		ImGui::End();
	}

	if (menu_snapline_enemy)
	{
		ImGui::Begin("Change Enemy Snapline Color");
		ImGui::SliderInt("Enemy Line Red", &esp_snapline_color_enemy[0], 0, 255);
		ImGui::SliderInt("Enemy Line Green", &esp_snapline_color_enemy[1], 0, 255);
		ImGui::SliderInt("Enemy Line Blue", &esp_snapline_color_enemy[2], 0, 255);
		ImGui::SliderInt("Enemy Line Alpha", &esp_snapline_color_enemy[3], 0, 255);
		ImGui::End();
	}

	if (menu_glow_team)
	{
		ImGui::Begin("Change Team Glow Color");
		ImGui::SliderFloat("Team Glow Red", &GlowTeamColor[0], 0, 1, "%.1f");
		ImGui::SliderFloat("Team Glow Green", &GlowTeamColor[1], 0, 1, "%.1f");
		ImGui::SliderFloat("Team Glow Blue", &GlowTeamColor[2], 0, 1, "%.1f");
		ImGui::SliderFloat("Team Glow Alpha", &GlowTeamColor[3], 0, 1, "%.1f");
		ImGui::End();
	}

	if (menu_glow_enemy)
	{
		ImGui::Begin("Change Enemy Glow Color");
		ImGui::SliderFloat("Enemy Glow Red", &GlowEnemyColor[0], 0, 1, "%.1f");
		ImGui::SliderFloat("Enemy Glow Green", &GlowEnemyColor[1], 0, 1, "%.1f");
		ImGui::SliderFloat("Enemy Glow Blue", &GlowEnemyColor[2], 0, 1, "%.1f");
		ImGui::SliderFloat("Enemy Glow Alpha", &GlowEnemyColor[3], 0, 1, "%.1f");
		ImGui::End();
	}

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

	if (esp_snapline || esp_box && LocalPlayerAddr)
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (EntityList[i] != NULL && EntityList[i] != LocalPlayer)
			{

				iVec2 EnemyPos2D, EnemyHeadPos2D;
				flVec3 EnemyHeadPos3D = GetBonePos(EntityList[i], BONE_HEAD);
				if (WorldToScreen(EntityList[i]->Position, EnemyPos2D, Game::vMatrix.Matrix, wrect.GetWidth(), wrect.GetHeight()) && WorldToScreen(EnemyHeadPos3D, EnemyHeadPos2D, Game::vMatrix.Matrix, wrect.GetWidth(), wrect.GetHeight()))
				{
					ESP_SnapLine(EntityList[i], EnemyPos2D, wrect, pDevice);
					ESP_Box(EntityList[i], EnemyPos2D, EnemyHeadPos2D, pDevice);
				}
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
	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	Game::D3D::roboto = io.Fonts->AddFontFromMemoryTTF((char*)roboto_regular, sizeof(roboto_regular), FONT_SIZE, &font_cfg);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMinSize = ImVec2(IMGUI_WINDOW_W, IMGUI_WINDOW_H);
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = COLOR_BLUE_TXT;
	colors[ImGuiCol_WindowBg] = COLOR_BLACK;
	colors[ImGuiCol_TitleBg] = COLOR_DARK_GRAY;
	colors[ImGuiCol_TitleBgActive] = COLOR_BLACK;
	colors[ImGuiCol_Tab] = COLOR_DARK_GRAY;
	colors[ImGuiCol_TabHovered] = COLOR_GRAY;
	colors[ImGuiCol_TabActive] = COLOR_DARK_GRAY_2;
	colors[ImGuiCol_CheckMark] = COLOR_BLUE;
	colors[ImGuiCol_FrameBg] = COLOR_DARK_GRAY;
	colors[ImGuiCol_FrameBgHovered] = COLOR_DARK_GRAY_2;
	colors[ImGuiCol_FrameBgActive] = COLOR_GRAY;
	colors[ImGuiCol_SliderGrab] = COLOR_BLUE;
	colors[ImGuiCol_SliderGrabActive] = COLOR_LIGHT_BLUE;
	colors[ImGuiCol_Button] = COLOR_DARK_GRAY;
	colors[ImGuiCol_ButtonHovered] = COLOR_DARK_GRAY_2;
	colors[ImGuiCol_ButtonActive] = COLOR_GRAY;
	colors[ImGuiCol_Border] = COLOR_BLUE;
	colors[ImGuiCol_ResizeGrip] = COLOR_DARK_BLUE;
	colors[ImGuiCol_ResizeGripHovered] = COLOR_BLUE;
	colors[ImGuiCol_ResizeGripActive] = COLOR_LIGHT_BLUE;

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

void Game::D3D::DrawLine(iVec2 src, iVec2 dst, int thickness, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	if (!dxLine)
		D3DXCreateLine(pDevice, &dxLine);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(src.x, src.y);
	Line[1] = D3DXVECTOR2(dst.x, dst.y);
	dxLine->SetWidth(thickness);
	dxLine->Draw(Line, 2, color);
}

void Game::D3D::DrawRect(int x, int y, int w, int h, int thickness, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	iVec2 l1 = { x, y };
	iVec2 l2 = { w, y };
	iVec2 l3 = { w, h };
	iVec2 l4 = { x, h };

	DrawLine(l1, l2, thickness, color, pDevice);
	DrawLine(l2, l3, thickness, color, pDevice);
	DrawLine(l3, l4, thickness, color, pDevice);
	DrawLine(l4, l1, thickness, color, pDevice);
}

bool Game::D3D::WorldToScreen(flVec3 pos, iVec2& screen, float viewMatrix[4][4], int windowWidth, int windowHeight)
{
	flVec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0][0] + pos.y * viewMatrix[0][1] + pos.z * viewMatrix[0][2] + viewMatrix[0][3];
	clipCoords.y = pos.x * viewMatrix[1][0] + pos.y * viewMatrix[1][1] + pos.z * viewMatrix[1][2] + viewMatrix[1][3];
	clipCoords.z = pos.x * viewMatrix[2][0] + pos.y * viewMatrix[2][1] + pos.z * viewMatrix[2][2] + viewMatrix[2][3];
	clipCoords.w = pos.x * viewMatrix[3][0] + pos.y * viewMatrix[3][1] + pos.z * viewMatrix[3][2] + viewMatrix[3][3];

	if (clipCoords.w < 0.1f)
		return false;

	flVec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}