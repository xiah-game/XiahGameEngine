#include "stdafx.h"
#include "FrameTimer.h"

namespace XiahGameEngine
{
	XIAHGE_API float g_fFrameScale = 0;
	XIAHGE_API DWORD g_dwLastFrameTime = 0;
	XIAHGE_API DWORD g_dwLastFrameDeltaTime = 0;
	XIAHGE_API DWORD g_dwCurTime = 0;
	XIAHGE_API DWORD g_dwTargetFrameTime = 0;
	XIAHGE_API DWORD g_dwTargetFPS = 0;
	XIAHGE_API DWORD g_dwSkipCount = 0;	// ¾ÆÁ÷ ¾Ê¾¸
	
	XIAHGE_API float g_fFrameCount = 0;
	XIAHGE_API DWORD g_dwCurFPS = 0;
	XIAHGE_API DWORD g_dwCurFPSTime = 0;

	XIAHGE_API BOOL StartFrameTimer(int TargetFPS)
	{
		g_dwCurTime = timeGetTime();
		g_dwLastFrameTime = g_dwCurTime;
		g_dwTargetFPS = TargetFPS;
		g_dwTargetFrameTime = 1000 / g_dwTargetFPS;
		g_fFrameCount = 0;
		g_dwCurFPS = 0;
		
		return TRUE;
	}

	XIAHGE_API BOOL UpdateFrameTimer()
	{
		g_dwCurTime = timeGetTime();
		g_dwLastFrameDeltaTime = g_dwCurTime - g_dwLastFrameTime;
		g_dwLastFrameTime = g_dwCurTime;

		g_fFrameScale = (float)g_dwLastFrameDeltaTime / (float)g_dwTargetFrameTime;

		g_fFrameCount += 1.0f;

		if( g_dwCurTime - g_dwCurFPSTime > 1000)
		{
			g_dwCurFPSTime = g_dwCurTime;
			g_dwCurFPS = (int)g_fFrameCount;
			g_fFrameCount -= (int)g_fFrameCount;
		}
		return TRUE;
	}

	XIAHGE_API DWORD	GetFrameTimer()
	{
		return g_dwCurFPS;
	}
};

