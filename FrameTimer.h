#pragma once

namespace XiahGameEngine
{
	extern XIAHGE_API float g_fFrameScale;
	extern XIAHGE_API DWORD g_dwCurTime;
	extern XIAHGE_API DWORD g_dwCurFPS;
	//HT_CHEAT : 시간 추가
	extern XIAHGE_API DWORD g_dwLastFrameDeltaTime;

	extern XIAHGE_API BOOL StartFrameTimer(int TargetFPS);
	extern XIAHGE_API BOOL UpdateFrameTimer();
	extern XIAHGE_API DWORD	GetFrameTimer();
};
