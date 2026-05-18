#pragma once

namespace XiahGameEngine
{
	#define MAX_VOLUME_TBL	11
	extern XIAHGE_API int g_VolTbl[MAX_VOLUME_TBL];

	extern XIAHGE_API BOOL InitializeSound();
	extern XIAHGE_API BOOL UninitializeSound();
	extern XIAHGE_API DWORD GetSoundVolume(int vol);

	// FMOD 엔진 INIT
	extern XIAHGE_API BOOL	Init_FMOD_Sound();
	// FMOD 엔진 UNINIT
	extern XIAHGE_API BOOL	UnInit_FMOD_Sound();
};
