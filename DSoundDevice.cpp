#include "stdafx.h"
#include "DSoundDevice.h"
#include "XiahBGM.h"
#include "SoundPak.h"

#pragma comment(lib, "dxguid.lib")

namespace XiahGameEngine
{
	// GROBAL VOLUME TABLE
	int g_VolTbl[MAX_VOLUME_TBL] = { 0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 255 };

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL InitializeSound()
	{

		// FMOD 사운드 라이브러리 INIT
		Init_FMOD_Sound();

		// Sound FX
		XiahFX::Clear_FX();

		return TRUE;
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL UninitializeSound()
	{
		XiahSoundPak::UninitializeSoundPak();

		// FMOD 사운드 라이브러리 UNINIT
		UnInit_FMOD_Sound();

		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	//
	//		FMOD SOUND LIBRARY
	//
	//---------------------------------------------------------------------------------------

	#define FSOUND_BUFFERSIZE   200       /* millisecond value for FMOD buffersize. */

	// FMOD 엔진 INIT
	XIAHGE_API BOOL	Init_FMOD_Sound()
	{
		// FMOD DLL 버전 CHECK
		float ver = FSOUND_GetVersion();

		if (ver < FMOD_VERSION)
		{
			TCHAR temp[20];
			_stprintf(temp,_T("%f"),ver);
			MessageBox(GetForegroundWindow(),_T("사운드 라이브러리 버전 틀림"),temp,MB_OK);
			return FALSE;
		}

		// 버퍼사이즈
		FSOUND_SetBufferSize(FSOUND_BUFFERSIZE);

		// DIRECT SOUND 사용
		FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
		// MIXER 조정
		FSOUND_SetDriver(0);
		FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT);

		// WINDOW HANDLE 등록
		FSOUND_SetHWND(g_EngineInfo.m_hWnd);

		// 출력조정
		if (!FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS)) return FALSE;

		// MASTER VOLUME
		FSOUND_SetVolume(FSOUND_ALL,255);
		return TRUE;
	}

	// FMOD 엔진 UNINIT
	XIAHGE_API BOOL	UnInit_FMOD_Sound()
	{
		// 종료시 FMOD_MANAGER를 사용하지 않는 관계로 사용하던 SOUND RESOURCE들은
		// 프로그래머가 알아서 해제해주어야 한다. 안해주면 RESOURCE LEAK!

		// FMOD LIB 종료
		FSOUND_Close();
		return TRUE;
	}

	//---------------------------------------------------------------------------------------

	// linear volume table
	const DWORD	VolTbl[256] = {
	-10000,-7994,-6994,-6409,-5994,-5672,-5409,-5186,-4994,-4824,-4672,-4534,-4409,-4293,-4186,-4087,
	-3994,-3906,-3824,-3746,-3672,-3602,-3534,-3470,-3409,-3350,-3293,-3239,-3186,-3136,-3087,-3040,
	-2994,-2949,-2906,-2865,-2824,-2784,-2746,-2708,-2672,-2636,-2602,-2568,-2534,-2502,-2470,-2439,
	-2409,-2379,-2350,-2321,-2293,-2266,-2239,-2212,-2186,-2161,-2136,-2111,-2087,-2063,-2040,-2017,
	-1994,-1971,-1949,-1928,-1906,-1885,-1865,-1844,-1824,-1804,-1784,-1765,-1746,-1727,-1708,-1690,
	-1672,-1654,-1636,-1619,-1602,-1584,-1568,-1551,-1534,-1518,-1502,-1486,-1470,-1455,-1439,-1424,
	-1409,-1394,-1379,-1364,-1350,-1336,-1321,-1307,-1293,-1280,-1266,-1252,-1239,-1226,-1212,-1199,
	-1186,-1174,-1161,-1148,-1136,-1123,-1111,-1099,-1087,-1075,-1063,-1051,-1040,-1028,-1017,-1005,
	-994,-983,-971,-960,-949,-939,-928,-917,-906,-896,-885,-875,-865,-854,-844,-834,
	-824,-814,-804,-794,-784,-775,-765,-755,-746,-736,-727,-718,-708,-699,-690,-681,
	-672,-663,-654,-645,-636,-628,-619,-610,-602,-593,-584,-576,-568,-559,-551,-543,
	-534,-526,-518,-510,-502,-494,-486,-478,-470,-462,-455,-447,-439,-432,-424,-416,
	-409,-401,-394,-387,-379,-372,-364,-357,-350,-343,-336,-329,-321,-314,-307,-300,
	-293,-286,-280,-273,-266,-259,-252,-246,-239,-232,-226,-219,-212,-206,-199,-193,
	-186,-180,-174,-167,-161,-155,-148,-142,-136,-130,-123,-117,-111,-105,-99,-93,
	-87,-81,-75,-69,-63,-57,-51,-45,-40,-34,-28,-22,-17,-11,-5,0 };

	XIAHGE_API DWORD GetSoundVolume(int vol)
	{
		// db을 VOL으로 변환
		return VolTbl[vol];
	}
};
