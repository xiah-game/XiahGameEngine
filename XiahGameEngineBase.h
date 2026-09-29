#pragma once

#ifdef XIAHGAMEENGINE_EXPORTS
#	define XIAHGE_API	__declspec(dllexport)
#else
#	define XIAHGE_API	__declspec(dllimport)
#endif

#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#if _MSC_VER >= 1900
#include <unordered_map>
#include <unordered_set>
namespace std {
    template<class _Kty, class _Ty, class _Hasher = hash<_Kty>, class _Keyeq = equal_to<_Kty>, class _Alloc = allocator<pair<const _Kty, _Ty>>>
    using hash_map = unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>;

    template<class _Kty, class _Hasher = hash<_Kty>, class _Keyeq = equal_to<_Kty>, class _Alloc = allocator<_Kty>>
    using hash_set = unordered_set<_Kty, _Hasher, _Keyeq, _Alloc>;
}
#else
#include <hash_map>
#include <hash_set>
#endif

#include "XiahDebug.h"

#include "ColorValueType.h"
#include "BasicValueType.h"
#include "Math\MathLib.h"

#include "FrameTimer.h"
#include "Debug_EngineInfo.h"

#include "Trigger.h"

// ??? ?? ???????. ????? ????.
//#define	TL_ACCEL	1


namespace XiahGameEngine
{
	//---------------------------------------------------------------------------------------

	// XIAH GAME INFOMATION
	struct sXiahGameEngine_CreateInfo
	{
		HINSTANCE m_hInstance;
		HWND	  m_hWnd;

		BOOL			m_bFullscreen;	
		BOOL			m_bRGB16;
		unsigned short	m_nWidth;
		unsigned short	m_nHeight;
		unsigned short	m_nTextureDetail;	// 0 1 2 3 4

		DWORD			m_dwGamepad;
		D3DFORMAT		m_nFormat;

		// RenderInfo
		unsigned long	m_nRenderedVertex;
		unsigned long	m_nRenderedFace;

		char strInstallDir[_MAX_PATH];

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// ??????? ??? a???? ??????? ???
		// ??????
		BOOL	m_bAllowWhisper;	// ??? ???
		BOOL	m_bAllowRelation;	// ?????u ??? 
		BOOL	m_bAllowTrade;		// ?????u ???
		BOOL	m_bHideChat;		// ??? ?????
		BOOL	m_bShowNickname;	// ????? ???????
		BOOL	m_bShowNPCname;		// ???? ???????
		BOOL	m_bItemDropChoice;		// ?????? ???????  //HO_0816_07 ?????? ????? ???
		BYTE	m_bSafeMode;

		// ??? ??? (??u?????)
		// ???? ??????. ??? ????? ??¡À? ??????
		DWORD	m_dwBuyLimit;				// ??????????
		BYTE	m_bRarityLimit;				// ??????? +
		BYTE	m_bStxTypeLimit;			// ??????? ??
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// ???? ???
		float			m_fViewDistance;	// ???©£??
		float			m_fPolygonDetail;	// ?????? ??????

		// ???? ???
		unsigned long	m_dwBGMVolume;		// BGM????
		unsigned long	m_dwFXVolume;		// SOUND EFFECT ????
		int				m_version;			// ???????? ????		

		bool			m_bGameEnd;			//HT_TEST : ???? ????
	};
	
	extern XIAHGE_API sXiahGameEngine_CreateInfo g_EngineInfo;	
	
	//---------------------------------------------------------------------------------------
	extern XIAHGE_API BOOL InitializeXiahGameEngine(sXiahGameEngine_CreateInfo *pInfo);
	extern XIAHGE_API BOOL UninitializeXiahGameEngine();
	extern XIAHGE_API BOOL ChangeXiahGameEngine(unsigned short w,unsigned short h);
	extern XIAHGE_API BOOL ChangeXiahGameOption(sXiahGameEngine_CreateInfo *pInfo);

	extern XIAHGE_API float GetPolyDetailXiahGameEngine();
	extern XIAHGE_API BOOL SetPolyDetailXiahGameEngine(float detail);	// Polygon Detail ????
	extern XIAHGE_API BOOL UpdateXiahGameEngine();	// MainThread???? ?? ?????? ?????????
	
	// ???? ???? ??¡Æ? ????
	extern XIAHGE_API BOOL ProcessWindowMessage_XiahGameEngine(UINT uMsg,WPARAM wParam,LPARAM lParam);
};
