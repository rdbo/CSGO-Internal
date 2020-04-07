#pragma once
#include "pch.h"
#include "game.h"

#define ABS(x) ((x < 0) ? (-x) : (x))
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define CSGO_MODULE L"csgo.exe"
#define CLIENT_MODULE L"client_panorama.dll"
#define ENGINE_MODULE L"engine.dll"

#define KEY_MENU VK_INSERT
#define KEY_BHOP VK_SPACE

#define D3D_ENDSCENE_INDEX 42
#define HOOK_ENDSCENE_LENGTH 7
#define MAX_THICKNESS 10
#define FONT_SIZE 18
#define IMGUI_WINDOW_W 280
#define IMGUI_WINDOW_H 180

#define STATE_INGAME 6
#define MAX_PLAYERS 32
#define LOOP_DIST 0x10
#define GLOW_CONSTANT 0x38
#define RCS_CONSTANT 2
#define BONE_HEAD 8

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Classes / Structures

struct iVec2
{
	int x, y;
};

struct flVec3
{
	float x, y, z;
};

struct flVec4
{
	float x, y, z, w;
};

struct GlowStruct
{
	union
	{
		DEFINE_MEMBER_N(float, r, 0x4);
		DEFINE_MEMBER_N(float, g, 0x8);
		DEFINE_MEMBER_N(float, b, 0xC);
		DEFINE_MEMBER_N(float, a, 0x10);
		DEFINE_MEMBER_N(bool, occluded, 0x24);
		DEFINE_MEMBER_N(bool, unoccluded, 0x25);
	};
};

struct Window : RECT
{
	int GetWidth() { return this->right - this->left; }
	int GetHeight() { return this->bottom - this->top; };
};

typedef struct { float Matrix[4][4]; } ViewMatrix;

using namespace hazedumper;
class Player
{
public:
	union
	{
		DEFINE_MEMBER_N(bool, Dormant, signatures::m_bDormant);
		DEFINE_MEMBER_N(DWORD, Team, netvars::m_iTeamNum);
		DEFINE_MEMBER_N(DWORD, Health, netvars::m_iHealth);
		DEFINE_MEMBER_N(DWORD, Flags, netvars::m_fFlags);
		DEFINE_MEMBER_N(flVec3, Velocity, netvars::m_vecVelocity);
		DEFINE_MEMBER_N(flVec3, Position, netvars::m_vecOrigin);
		DEFINE_MEMBER_N(bool, Spotted, netvars::m_bSpotted);
		DEFINE_MEMBER_N(DWORD, BoneMatrix, netvars::m_dwBoneMatrix);
		DEFINE_MEMBER_N(flVec3, AimPunchAngle, netvars::m_aimPunchAngle);
		DEFINE_MEMBER_N(DWORD, ShotsFired, netvars::m_iShotsFired);
		DEFINE_MEMBER_N(DWORD, GlowIndex, netvars::m_iGlowIndex);
		DEFINE_MEMBER_N(DWORD, Armor, netvars::m_ArmorValue);
	};
};

//Variables

extern bool bhop;
extern bool radar_hack;
extern bool glow_hack;
extern bool rcs;
extern bool esp_snapline;
extern int esp_snapline_thickness;
extern int esp_snapline_color_enemy[4];
extern int esp_snapline_color_team[4];
extern bool esp_box;
extern int esp_box_thickness;
extern int esp_box_color_enemy[4];
extern int esp_box_color_team[4];
extern float GlowEnemyColor[4];
extern float GlowTeamColor[4];

//EndScene Dependent Functions
void ESP_SnapLine(Player* ent, iVec2 EnemyPos2D, Window wnd, LPDIRECT3DDEVICE9 pDevice);
flVec3 GetBonePos(Player* ent, int bone);
void ESP_Box(Player* ent, iVec2 EnemyPos2D, iVec2 EnemyHeadPos2D, LPDIRECT3DDEVICE9 pDevice);

//Game

namespace Game
{
	namespace D3D
	{
		extern ID3DXLine* dxLine;
		extern EndScene oEndScene;
		extern ImFont* roboto;
		extern void* vtable[119];
		bool WorldToScreen(flVec3 pos, iVec2& screen, float viewMatrix[4][4], int windowWidth, int windowHeight);
		long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice);
		bool GetDevice(void** pTable, size_t Size);
		void DrawLine(iVec2 src, iVec2 dst, int thickness, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice);
		void DrawRect(int x, int y, int w, int h, int thickness, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice);
	}
	extern HWND window;
	extern Window wrect;
	extern mem_t csgo;
	extern mem_t client;
	extern mem_t engine;
	extern WNDPROC oWndProc;
	extern mem_t LocalPlayerAddr;
	extern Player* LocalPlayer;
	extern mem_t EntityListAddr;
	extern ViewMatrix vMatrix;
	extern mem_t GlowObjMg;
	extern mem_t ClientState;
	extern int* ClientState_State;
	extern DWORD* ForceJump;
	extern Player* EntityList[MAX_PLAYERS];
	extern bool run;
	extern bool bInitHooks;
	extern bool bInitImgui;
	extern bool bShowMenu;
	DWORD WINAPI HackInit(LPVOID lpReserved);
	void HackShutdown();
	LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
	HWND GetProcessWindow();
}