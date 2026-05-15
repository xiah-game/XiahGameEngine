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
#include <hash_map>
#include <hash_set>

#include "XiahDebug.h"

#include "ColorValueType.h"
#include "BasicValueType.h"
#include "Math\MathLib.h"

#include "FrameTimer.h"
#include "Debug_EngineInfo.h"

#include "Trigger.h"

// 이건 좀 복잡하다. 나중에 하자.
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
		// 게임내의 옵션 창에서 설정하는 옵션
		// 게임옵션
		BOOL	m_bAllowWhisper;	// 귓말 거부
		BOOL	m_bAllowRelation;	// 관계신청 거부 
		BOOL	m_bAllowTrade;		// 거래신청 거부
		BOOL	m_bHideChat;		// 대화 숨기기
		BOOL	m_bShowNickname;	// 케렉터 별호보기
		BOOL	m_bShowNPCname;		// 몬스터 이름보기
		BOOL	m_bItemDropChoice;		// 아이템 드랍여부  //HO_0816_07 아이템 드랍시 확인
		BYTE	m_bSafeMode;

		// 거래 옵션 (임시보관용)
		// 나도 귀찮다. 그냥 여기다 임시로 넣어야지
		DWORD	m_dwBuyLimit;				// 구매제한금액
		BYTE	m_bRarityLimit;				// 판매제한 +
		BYTE	m_bStxTypeLimit;			// 판매제한 성
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// 영상 옵션
		float			m_fViewDistance;	// 가시거리
		float			m_fPolygonDetail;	// 폴리곤 디테일

		// 음향 옵션
		unsigned long	m_dwBGMVolume;		// BGM볼륨
		unsigned long	m_dwFXVolume;		// SOUND EFFECT 볼륨
		int				m_version;			// 실행파일 버전		

		bool			m_bGameEnd;			//HT_TEST : 게임 종료
	};
	
	extern XIAHGE_API sXiahGameEngine_CreateInfo g_EngineInfo;	
	
	//---------------------------------------------------------------------------------------
	extern XIAHGE_API BOOL InitializeXiahGameEngine(sXiahGameEngine_CreateInfo *pInfo);
	extern XIAHGE_API BOOL UninitializeXiahGameEngine();
	extern XIAHGE_API BOOL ChangeXiahGameEngine(unsigned short w,unsigned short h);
	extern XIAHGE_API BOOL ChangeXiahGameOption(sXiahGameEngine_CreateInfo *pInfo);

	extern XIAHGE_API float GetPolyDetailXiahGameEngine();
	extern XIAHGE_API BOOL SetPolyDetailXiahGameEngine(float detail);	// Polygon Detail 조정
	extern XIAHGE_API BOOL UpdateXiahGameEngine();	// MainThread에서 매 프레임 호출해줘야됨
	
	// 아직 별로 하는것 없음
	extern XIAHGE_API BOOL ProcessWindowMessage_XiahGameEngine(UINT uMsg,WPARAM wParam,LPARAM lParam);
};