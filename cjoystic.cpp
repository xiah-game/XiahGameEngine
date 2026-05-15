#include "stdafx.h"
#include <math.h>
#include "cjoystic.h"

//#define	_TEST
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "dxguid.lib")
#pragma comment (lib,"dinput8.lib")

namespace XiahGameEngine
{
	cJOYSTICK	*temp_cj = NULL;
	cJOYSTICK	*g_cj = NULL;	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	XIAHGE_API BOOL	  Init_Gamepad(HWND hwnd)
	{
		HRESULT hr;
		g_cj = new cJOYSTICK(hwnd);
		if(g_cj == NULL) return FALSE;

		hr = g_cj->Init_Joystick();
		if(hr == E_FAIL) return FALSE;

		hr = g_cj->Init_Rumble();

		return TRUE;
	}

	XIAHGE_API BOOL	  UnInit_Gamepad()
	{
		if(g_cj) delete g_cj;
		g_cj = NULL;

		return TRUE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,  VOID* pContext )
	{
		return temp_cj->EnumObjectsCallback(pdidoi,pContext);
	}

	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,VOID* pContext )
	{
		return temp_cj->EnumJoysticksCallback(pdidInstance,pContext);
	}


	static BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext )
	{
		return temp_cj->EnumFFDevicesCallback(pInst,pContext);
	}
	static BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
	{
		return temp_cj->EnumAxesCallback(pdidoi,pContext);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cJOYSTICK :: cJOYSTICK(HWND shwnd)
	{
		g_pDI = NULL;
		g_pJoystick = NULL;

		// rumble	
		g_pEffect = NULL;
		g_dwNumForceFeedbackAxis = 0;
		rumble_kind = 0;
		g_Duration = 0;
		
		hwnd = shwnd;

		m_joystic_enable = FALSE;
		m_rumble_enable = FALSE;

		temp_cj = this;	// for callback
	}

	cJOYSTICK :: ~cJOYSTICK()
	{
		Free_Joystick();
		Free_Rumble();
	}

	HRESULT cJOYSTICK :: Init_Joystick()
	{
		HRESULT hr;

		if(FAILED(hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,IID_IDirectInput8, (VOID**)&g_pDI, NULL)))
			return hr;

		if(FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,XiahGameEngine::EnumJoysticksCallback,NULL, DIEDFL_ATTACHEDONLY)))
			return hr;

		if(NULL == g_pJoystick)
		{
			if( g_pDI)
			{
				g_pDI->Release();
				g_pDI = NULL;
			}
			return E_FAIL;
		}

		if(FAILED(hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick2)))
			return hr;

		if(FAILED(hr = g_pJoystick->SetCooperativeLevel( hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
			return hr;

		if(FAILED(hr = g_pJoystick->EnumObjects(XiahGameEngine::EnumObjectsCallback, (VOID*)hwnd, DIDFT_ALL)))
			return hr;

/*
		if(FAILED(hr = g_pJoystick->SetEventNotification(DI_Event[0]))
			return hr;
*/
		m_joystic_enable = TRUE;
		//DBG_Put("JOYSTICK Detect");
		return S_OK;
	}

	HRESULT cJOYSTICK :: Init_Rumble()
	{
		HRESULT hr;
		DWORD           rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };
		LONG            rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf              = { 0 };

		g_Duration = 300 ;		// 얼마만큼의 시간동안 지속?

		DIPROPDWORD dipdw;
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = DIPROPAUTOCENTER_OFF;

		if( FAILED( hr = g_pJoystick ->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph ) ) )
			return hr;

		// forcefeed back의 축갯수 검사
		if ( FAILED( hr = g_pJoystick ->EnumObjects(XiahGameEngine::EnumAxesCallback, (VOID*)&g_dwNumForceFeedbackAxis, DIDFT_AXIS)))
			return hr;

		//  forcefeed back의 축이 몇개??
		if( g_dwNumForceFeedbackAxis > 2 )
			g_dwNumForceFeedbackAxis = 2;

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                  = sizeof(DIEFFECT);
		eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		//eff.dwDuration              = INFINITE;	// 무한진동
		eff.dwDuration              = g_Duration;
		eff.dwSamplePeriod          = 0;
		eff.dwGain                  = DI_FFNOMINALMAX;
		eff.dwTriggerButton         = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes                   = g_dwNumForceFeedbackAxis;
		eff.rgdwAxes                = rgdwAxes;
		eff.rglDirection            = rglDirection;
		eff.lpEnvelope              = 0;
		eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams   = &cf;
		eff.dwStartDelay            = 0;

		// Effect를 생성한다
		if( FAILED( hr = g_pJoystick ->CreateEffect( GUID_ConstantForce, &eff, &g_pEffect, NULL ) ) )
		{
			return hr;
		}

		if( NULL == g_pEffect )
			return E_FAIL;

		Change_Para(1000,0);

		m_rumble_enable = TRUE;

		//DBG_Put("Rumble Detect");
		return S_OK;	
	}

	void cJOYSTICK :: Free_Joystick()
	{
		if( g_pJoystick ) 
			g_pJoystick->Unacquire();

		if( g_pJoystick)
		{
			g_pJoystick->Release();
			g_pJoystick = NULL;
		}

		if( g_pDI)
		{
			g_pDI->Release();
			g_pDI = NULL;
		}
	}

	void cJOYSTICK :: Free_Rumble()
	{
		if( g_pJoystick ) 
			g_pJoystick->Unacquire();
			    
		// Release Effect
		if( g_pEffect )
		{
    		g_pEffect->Release();
    		g_pEffect = NULL;
		}
	}

	HRESULT cJOYSTICK :: UpdateInputState()
	{
		HRESULT     hr;

		if( NULL == g_pJoystick ) 
			return E_FAIL;

		hr = g_pJoystick->Poll();

		if(FAILED(hr))
		{
			hr = g_pJoystick->Acquire();
			while( hr == DIERR_INPUTLOST ) 
				hr = g_pJoystick->Acquire();
			return S_OK; 
		}

		if(FAILED(hr = g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &g_js)))
			return hr;

		return S_OK;
	}


	// 진동의 종류를 바꾼다.	
	HRESULT cJOYSTICK :: Change_Para(int duration, int kind)
	{
		int g_nXForce,g_nYForce;
		LONG            rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf              = { 0 };

		if(m_rumble_enable == FALSE) return E_FAIL;

		g_Duration = duration;
		if(kind > 3) return E_FAIL;
		/*
		if(kind == rumble_kind) 
			return S_OK;
		else
*/
			rumble_kind = kind;

		// Force feed back의 XY
		g_nXForce = rumble_tbl[kind][0];
		g_nYForce = rumble_tbl[kind][1];

		if( g_dwNumForceFeedbackAxis == 1 )
		{
			cf.lMagnitude = g_nXForce;
			rglDirection[0] = 0;
		}
		else
		{
			rglDirection[0] = g_nXForce;
			rglDirection[1] = g_nYForce;
			cf.lMagnitude = (DWORD)sqrt( (double)g_nXForce * (double)g_nXForce + (double)g_nYForce * (double)g_nYForce );
		}

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                  = sizeof(DIEFFECT);
		eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.cAxes                 = g_dwNumForceFeedbackAxis;
		eff.rglDirection          = rglDirection;
		eff.lpEnvelope            = 0;
		eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;
		eff.dwStartDelay            = 0;
		eff.dwDuration              = g_Duration;

		// 효과를 바꾸어 준다.
		return g_pEffect->SetParameters( &eff, DIEP_DIRECTION |	DIEP_TYPESPECIFICPARAMS);	
	}

	HRESULT cJOYSTICK :: Test_Joystick()
	{

		HRESULT     hr;
		if(Change_Para(3000,0) == E_FAIL) return E_FAIL;

		if( NULL == g_pJoystick ) 
			return E_FAIL;

		hr = g_pJoystick->Poll();

		if(FAILED(hr))
		{
	MARK:
			hr = g_pJoystick->Acquire();
			if(hr == E_ACCESSDENIED)
			{
				Sleep(10);
				goto MARK;
			}
		}

	/*	if(FAILED(hr))
		{
			hr = g_pJoystick->Acquire();
			while( hr == DIERR_INPUTLOST ) 
				hr = g_pJoystick->Acquire();

			return S_OK; 
		}
	*/
	//	if(FAILED(hr = g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &g_js)))
	//		return hr;

		return Rumble_Start();
	}

	// 진동을 중단한다.
	HRESULT cJOYSTICK :: Rumble_Stop()
	{
		HRESULT hr;
		if(m_rumble_enable == FALSE) return E_FAIL;
		hr = g_pEffect->Stop();
		return hr;
	}

	// 진동을 시작한다.
	HRESULT cJOYSTICK :: Rumble_Start()
	{
		HRESULT hr;
		if(m_rumble_enable == FALSE) return E_FAIL;
		hr = g_pEffect->Start(1,DIES_SOLO);	
		return hr;
	}	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL cJOYSTICK :: EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,  VOID* pContext )
	{
		HWND hDlg = (HWND)pContext;

		static int nSliderCount = 0;
		static int nPOVCount = 0;
		if( pdidoi->dwType & DIDFT_AXIS)
		{
			DIPROPRANGE diprg;
			diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
			diprg.diph.dwHow        = DIPH_BYID; 
			diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
			diprg.lMin              = -1000; 
			diprg.lMax              = +1000; 

			if( FAILED(g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
				return DIENUM_STOP;

		}

		return DIENUM_CONTINUE;
	}

	BOOL cJOYSTICK :: EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,VOID* pContext )
	{
		HRESULT hr;

		hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );

		if( FAILED(hr) ) 
			return DIENUM_CONTINUE;

		return DIENUM_STOP;
	}


	BOOL cJOYSTICK :: EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
	{
		DWORD* pdwNumForceFeedbackAxis = (DWORD*) pContext;

		if( (pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
			(*pdwNumForceFeedbackAxis)++;

		return DIENUM_CONTINUE;
	}

};