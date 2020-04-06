#include "pch.h"
#include "hack.h"
using namespace hazedumper;

bool bhop;
bool radar_hack;
bool glow_hack;
bool esp_snapline;
int esp_snapline_thickness = 1;
int esp_snapline_color_enemy[4] = { 255, 125, 0, 255 };
int esp_snapline_color_team[4] = { 0, 125, 255, 255 };
float GlowEnemyColor[4] = { 1.f, 0.f, 0.f, 1.f };
float GlowTeamColor[4] = { 0.f, 0.f, 1.f, 1.f };

//Variables
namespace Game
{
	namespace D3D
	{
		EndScene oEndScene;
		void* vtable[119];
	}
	HWND window;
	Window wrect;
	mem_t csgo;
	mem_t client;
	mem_t engine;
	WNDPROC oWndProc;
	mem_t LocalPlayerAddr;
	Player* LocalPlayer;
	mem_t EntityListAddr;
	ViewMatrix vMatrix;
	mem_t GlowObjMg;
	mem_t ClientState;
	int* ClientState_State;
	DWORD* ForceJump;
	Player* EntityList[MAX_PLAYERS];
	bool run;
	bool bInitHooks;
	bool bInitImgui;
	bool bShowMenu;
}

//Functions

void Bunnyhop();
void RadarHack(Player* ent);
void GlowHack(Player* ent);
void EntityListLoop();

DWORD WINAPI Game::HackInit(LPVOID lpReserved)
{
	run = true;
	//Modules
	csgo = Memory::In::GetModuleAddress(CSGO_MODULE);
	client = Memory::In::GetModuleAddress(CLIENT_MODULE);
	engine = Memory::In::GetModuleAddress(ENGINE_MODULE);

	if (csgo == 0 || client == 0 || engine == 0) return FALSE; //Check modules

	ForceJump = (DWORD*)(client + signatures::dwForceJump);

	//Hooks
	if (!D3D::GetDevice(D3D::vtable, sizeof(D3D::vtable))) return FALSE;
	D3D::oEndScene = (EndScene)Memory::In::Hook::TrampolineHook((byte_t*)D3D::vtable[D3D_ENDSCENE_INDEX], (byte_t*)D3D::hkEndScene, HOOK_ENDSCENE_LENGTH);
	oWndProc = (WNDPROC)SetWindowLongPtr(Game::window, GWL_WNDPROC, (LONG_PTR)hkWndProc);
	bInitHooks = true;

	//Main Loop

	while (run)
	{
		ClientState = *(mem_t*)(engine + signatures::dwClientState);
		ClientState_State = (int*)(ClientState + signatures::dwClientState_State);
		if (*ClientState_State == STATE_INGAME)
		{
			LocalPlayerAddr = *(mem_t*)(client + signatures::dwLocalPlayer);
			LocalPlayer = (Player*)LocalPlayerAddr;
			EntityListAddr = (mem_t)(client + signatures::dwEntityList);
			if (LocalPlayerAddr)
			{
				vMatrix = *(ViewMatrix*)(client + signatures::dwViewMatrix);
				GlowObjMg = *(mem_t*)(client + signatures::dwGlowObjectManager);
				Bunnyhop();
				EntityListLoop();
			}
		}
	}

	return TRUE;
}

void Game::HackShutdown()
{
	if (bInitHooks)
	{
		Memory::In::Hook::Restore((mem_t)D3D::vtable[D3D_ENDSCENE_INDEX]);
	}

	if (bInitImgui)
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	run = false;
}

//Cheats

void Bunnyhop()
{
	if (bhop && Game::LocalPlayer->Flags & (1 << 0) && GetAsyncKeyState(KEY_BHOP))
	{
		*Game::ForceJump = 6;
	}
}

void RadarHack(Player* ent)
{
	if (radar_hack && !ent->Dormant && !ent->Spotted)
	{
		ent->Spotted = true;
	}
}

void GlowHack(Player* ent)
{
	if (glow_hack && !ent->Dormant && ent->Health > 0 && ent->Team != 0)
	{
		GlowStruct* pGlow = (GlowStruct*)(Game::GlowObjMg + (ent->GlowIndex * GLOW_CONSTANT));

		if (ent->Team == Game::LocalPlayer->Team)
		{
			pGlow->r = GlowTeamColor[0];
			pGlow->g = GlowTeamColor[1];
			pGlow->b = GlowTeamColor[2];
			pGlow->a = GlowTeamColor[3];
		}

		if (ent->Team != Game::LocalPlayer->Team)
		{
			pGlow->r = GlowEnemyColor[0];
			pGlow->g = GlowEnemyColor[1];
			pGlow->b = GlowEnemyColor[2];
			pGlow->a = GlowEnemyColor[3];
		}

		pGlow->occluded = true;
		pGlow->unoccluded = false;
	}
}

void ESP_SnapLine(Player* ent, Window wnd, LPDIRECT3DDEVICE9 pDevice)
{
	if (esp_snapline && !ent->Dormant && ent->Health > 0 && ent->Team != NULL)
	{
		int wWidth = wnd.GetWidth();
		int wHeight = wnd.GetHeight();
		iVec2 EnemyPos2D;
		if (Game::D3D::WorldToScreen(ent->Position, EnemyPos2D, Game::vMatrix.Matrix, wWidth, wHeight))
		{
			iVec2 screenCoords = { wWidth / 2, wHeight };

			if (ent->Team == Game::LocalPlayer->Team)
			{
				Game::D3D::DrawLine(screenCoords, EnemyPos2D, esp_snapline_thickness, D3DCOLOR_RGBA(esp_snapline_color_team[0], esp_snapline_color_team[1], esp_snapline_color_team[2], esp_snapline_color_team[3]), pDevice);
			}

			if (ent->Team != Game::LocalPlayer->Team)
			{
				Game::D3D::DrawLine(screenCoords, EnemyPos2D, esp_snapline_thickness, D3DCOLOR_RGBA(esp_snapline_color_enemy[0], esp_snapline_color_enemy[1], esp_snapline_color_enemy[2], esp_snapline_color_enemy[3]), pDevice);
			}
		}
	}
}

void Game::D3D::DrawLine(iVec2 src, iVec2 dst, int thickness, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	ID3DXLine* LineL;
	D3DXCreateLine(pDevice, &LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(src.x, src.y);
	Line[1] = D3DXVECTOR2(dst.x, dst.y);
	LineL->SetWidth(thickness);
	LineL->Begin();
	LineL->Draw(Line, 2, color);
	LineL->End();
	LineL->Release();
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

void EntityListLoop()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		mem_t ent = *(mem_t*)(Game::EntityListAddr + i * LOOP_DIST);
		if (ent)
		{
			Game::EntityList[i] = (Player*)ent;
			RadarHack(Game::EntityList[i]);
			GlowHack(Game::EntityList[i]);
		}

		else
		{
			Game::EntityList[i] = NULL;
		}
	}
}

//Other Functions

flVec3 GetBonePos(Player* ent, int bone) {
	uintptr_t bonePtr = ent->BoneMatrix;
	flVec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);
	return bonePos;
}

LRESULT CALLBACK Game::hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == KEY_MENU)
		{
			bShowMenu = !bShowMenu;
		}
	}

	if (bShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK Game::EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	DWORD wndProcId;
	GetWindowThreadProcessId(handle, &wndProcId);

	if (GetCurrentProcessId() != wndProcId)
		return TRUE; // skip to next window

	window = handle;
	return FALSE; // window found abort search
}

HWND Game::GetProcessWindow()
{
	window = NULL;
	EnumWindows(EnumWindowsCallback, NULL);
	return window;
}
