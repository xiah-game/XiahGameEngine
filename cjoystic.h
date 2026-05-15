#pragma once
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION		0x0800
#endif

#include <dinput.h>

namespace XiahGameEngine
{

		#define BIG_STRONG		0
		#define BIG_WEAK		1
		#define SMALL_STRONG	2
		#define SMALL_WEAK		3

		const int rumble_tbl[4][2] = {9900,9900,-2200,200,-100,9600,100,2000};

		// Joystick 관련 CLASS include rumble
		class cJOYSTICK
		{
		protected:
				HWND	hwnd;
				DWORD	rumble_kind;
				LPDIRECTINPUT8 g_pDI;
				LPDIRECTINPUTDEVICE8 g_pJoystick;
				
				// rumble		
				LPDIRECTINPUTEFFECT	g_pEffect;		
				DWORD  g_dwNumForceFeedbackAxis;
				DWORD  g_Duration;

				BOOL	m_joystic_enable;	// Joy stick enable
				BOOL	m_rumble_enable;	// rumble enable

		public :
				DIJOYSTATE2 g_js;	// joystick state

				HRESULT Init_Joystick();
				void Free_Joystick();

				HRESULT UpdateInputState();
				
				// rumble
				HRESULT Init_Rumble();		// 초기화
				void Free_Rumble();

				// 외부 호출 가능
				XIAHGE_API HRESULT Change_Para(int duration , int kind);	// 진동의 종류를 바꾼다.	
				XIAHGE_API HRESULT Rumble_Stop();		// 진동을 중단한다.
				XIAHGE_API HRESULT Rumble_Start();		// 진동을 시작한다.
				XIAHGE_API BOOL	GetJoyState() { return m_joystic_enable; };
				XIAHGE_API BOOL	GetRumState() { return m_rumble_enable; };

				cJOYSTICK(HWND hwnd);
				~cJOYSTICK();	

				// joy stick callback
				BOOL EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,  VOID* pContext );
				BOOL EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,VOID* pContext );
				// rumble callback
				BOOL EnumFFDevicesCallback( const DIDEVICEINSTANCE* pInst, VOID* pContext );
				BOOL EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );


				HRESULT Test_Joystick();	// Rumble의 테스트
		};

		extern XIAHGE_API	cJOYSTICK	*g_cj;
		extern XIAHGE_API	BOOL	  Init_Gamepad(HWND hwnd);
		extern XIAHGE_API	BOOL	  UnInit_Gamepad();
}

