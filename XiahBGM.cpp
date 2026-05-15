#include "stdafx.h"
#include <process.h>
#include "XiahBGM.h"
//#include "dshow.h"
#include "DSoundDevice.h"

// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"

// SUPPORT FMOD SOUND LIBRARY
#pragma comment(lib, "fmodvc.lib")
#pragma comment(lib, "strmiids.lib")

namespace XiahGameEngine
{
	// Charactor들의 SOund FX를 관리한다.
	namespace XiahFX
	{
		DWORD	m_fx_number;
		_sFX	m_fx[MAX_FX];

		// FX 등록
		BOOL Add_FX(Vector3 vec,FSOUND_SAMPLE* fx,long pan)
		{
			if(m_fx_number < MAX_FX)
			{
				m_fx[m_fx_number].m_vec = vec;
				m_fx[m_fx_number].m_pan = pan;
				m_fx[m_fx_number].m_XiahFX = fx;
				m_fx_number++;
			}
			else
			{
				DBG_Put(_T("FX FULL"));
				return FALSE;
			}
			return TRUE;
		}


		// FX 해제
		BOOL Del_FX(FSOUND_SAMPLE* fx)
		{
			for(int i = 0; i< MAX_FX; i++)
			{
				if(m_fx[i].m_XiahFX == fx)
				{
					m_fx[m_fx_number].m_vec = Vector3(0,0,0);
					m_fx[i].m_pan = 0;
					m_fx[i].m_XiahFX = NULL;
					return TRUE;
				}
			}
			return FALSE;
		}


		// FX 출력
		BOOL Play_FX (int s_num,long vol, long pan)
		{
			if(vol == 0) return FALSE;

			int m_Channel;
			if(m_fx[s_num].m_XiahFX == NULL) return FALSE;
			// 일단 PAUSE!
			m_Channel = FSOUND_PlaySoundEx(FSOUND_FREE, m_fx[s_num].m_XiahFX, NULL, TRUE);
			// 0~255
			FSOUND_SetVolume(m_Channel , vol);

			// 0 (LEFT) 255 (RIGHT)
			FSOUND_SetPan(m_Channel ,pan);
			FSOUND_SetPaused(m_Channel , FALSE);
			//m_fx[s_num].m_XiahFX->Play(0,0,0);
			return TRUE;
		}

		// CLEAR
		void Clear_FX()
		{
			m_fx_number = 0;
			for(int i = 0; i< MAX_FX; i++)
			{
				m_fx[m_fx_number].m_vec = Vector3(0,0,0);
				m_fx[i].m_pan = 0;
				m_fx[i].m_XiahFX = NULL;
			}			
			//DBG_Put("Clear");
		}
	};

	/***************************************************************************************************/
	/***************************************************************************************************/
	/***************************************************************************************************/
	/***************************************************************************************************/

};