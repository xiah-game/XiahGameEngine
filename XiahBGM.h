#pragma once
#include <dshow.h>

// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"

namespace XiahGameEngine
{
	#define MAX_FX	32	// 32 Channel

	// Charactor들의 Sound FX를 관리한다.
	namespace XiahFX
	{
		typedef struct 
		{
			Vector3	m_vec;
			long	m_pan;
			// FMOD
			FSOUND_SAMPLE*	m_XiahFX;
		}_sFX;
		
		XIAHGE_API extern DWORD		m_fx_number;
		XIAHGE_API extern _sFX		m_fx[MAX_FX];

		// FMOD
		XIAHGE_API BOOL Del_FX(FSOUND_SAMPLE* fx);

		// FMOD
		XIAHGE_API BOOL Add_FX(Vector3 vec,FSOUND_SAMPLE* fx,long pan);

		XIAHGE_API BOOL Play_FX (int s_num,long vol, long pan);
		XIAHGE_API void Clear_FX();		
	};
};