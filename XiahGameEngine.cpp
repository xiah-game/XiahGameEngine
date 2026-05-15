#include "stdafx.h"
#include "D3DDevice.h"
#include "DSoundDevice.h"
#include "XiahInput.h"
#include "cjoystic.h"

//#include "interface/IUIManager.h"
#include "XiahPak.h"
#include "time.h"
#include "cpu\\cpu.h"

namespace XiahGameEngine
{
	XIAHGE_API sXiahGameEngine_CreateInfo g_EngineInfo;	

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL InitializeXiahGameEngine(sXiahGameEngine_CreateInfo *pInfo)
	{

/*
// 임시!!
#ifndef MASTER
		SETUP_UNHANDLED_EXCEPTION_FILTER;
#endif
*/
		InitializeMathLut();

		//HT_CHEAT : 게임 패드 삭제
		//if(Init_Gamepad(pInfo->m_hWnd) == FALSE)
		//	g_cj = NULL;
		//else
		//	XiahInput::g_Pad_Mode = pInfo->m_dwGamepad;

		//HT_TEST : 여기서 클라이언트 용량을 체크 해보자


		memcpy( &g_EngineInfo, pInfo, sizeof( sXiahGameEngine_CreateInfo));

		if( !Initialize3DDevice( pInfo->m_hWnd, pInfo->m_bFullscreen, pInfo->m_nWidth, pInfo->m_nHeight, pInfo->m_bRGB16))
		{
			MessageBox(0,"DirectX 초기화 실패","Xiah",MB_OK);
			DBG_LogFile(_T("D3D Device초기화 실패"));
			return FALSE;
		}

		if( !CreateFontSystem())
		{
			MessageBox(0,"Font System초기화 실패","Xiah",MB_OK);
			DBG_LogFile(_T("Font System초기화 실패"));
			return FALSE;
		}

		// INIT SOUND ENGINE & FX
		InitializeSound();
		
		StartFrameTimer( 30);	// 목표 FPS는 30이닷

		// 랜덤 숫자.
		srand( (unsigned)time( NULL ) );

		return TRUE;
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL UninitializeXiahGameEngine()
	{
		XiahPak::UninitializeXiahPak();
		
		UninitializeSound();

		UnInit_Gamepad();
		
		//ReleaseFontSystem();

		DebugEngineInfo::ReleaseDebugEngineInfo();

		if( !Uninitialize3DDevice())
			return FALSE;

		return TRUE;
	}

	// GAME의 옵션을 바꾼다.
	XIAHGE_API BOOL ChangeXiahGameOption(sXiahGameEngine_CreateInfo *pInfo)
	{
		memcpy( &g_EngineInfo, pInfo, sizeof( sXiahGameEngine_CreateInfo));
		return TRUE;
	}

	// 해상도 관련 파라메터를 바꾼다.
	XIAHGE_API BOOL ChangeXiahGameEngine(unsigned short w,unsigned short h)
	{
		g_EngineInfo.m_nWidth = w;
		g_EngineInfo.m_nHeight = h;
		//HT_CHEAT : 윈도우창모드
		g_D3DPresent.BackBufferWidth = w;
		g_D3DPresent.BackBufferHeight = h;
		return TRUE;
	}

	// Polygon Detail 조정
	XIAHGE_API BOOL SetPolyDetailXiahGameEngine(float detail)
	{
		g_EngineInfo.m_fPolygonDetail = detail;
		return TRUE;
	}

	XIAHGE_API float GetPolyDetailXiahGameEngine()
	{
		return g_EngineInfo.m_fPolygonDetail;
	}

	XIAHGE_API BOOL UpdateXiahGameEngine()
	{
		UpdateFrameTimer();
		XiahInput::UpdateInput();
	
		return TRUE;
	}
	
	XIAHGE_API BOOL ProcessWindowMessage_XiahGameEngine(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return TRUE;
	}
};