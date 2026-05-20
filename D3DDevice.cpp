#include "stdafx.h"
#include "D3DDevice.h"
#include "io.h"
#include "XiahGameEngineBase.h"

//HT_CHEAT : 윈도우창모드 
#include <windowsx.h>	

namespace XiahGameEngine
{
	IDirect3D9*							g_pDirect3D			= NULL;
	XIAHGE_API IDirect3DDevice9*		g_pDirect3DDevice	= NULL;
	D3DPRESENT_PARAMETERS				g_D3DPresent;
	D3DCAPS9							g_Direct3DCaps;

	XIAHGE_API CCamera*					g_pCurrentCamera;

	XIAHGE_API Direct3D9Device			g_Device;

	//HT_CHEAT : WINDOWSIZE
	XIAHGE_API RECT WindowRect;

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL Initialize3DDevice(HWND hWnd,BOOL bFullscreen,unsigned short nWidth,unsigned short nHeight,BOOL bRGB16)
	{
		g_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION);

		if( g_pDirect3D == NULL)
		{
			MessageBox( GetForegroundWindow(), _T("DirectX 버전이 틀림니다. DirectX 9.0버전을 설치해 주시기 바랍니다."), _T("3D엔진 초기화 실패"), MB_OK);
			return FALSE;
		}

		//HT_CHEAT : WINDOWSIZE
		if( !StartDevice_Default( hWnd, nWidth, nHeight ))
		{
			DBG_LogFile( _T("Default모드Device생성 실패"));
            g_pDirect3D->Release();
            return FALSE;
		}

		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// D3D Spy를 막자@_@
		typedef VOID (*DISABLED3DSPY)();
		HMODULE hModule;
		DISABLED3DSPY DisableD3DSpy;
		hModule = LoadLibrary( TEXT("d3d9.dll") );
		if( hModule != NULL )
		{
			DisableD3DSpy = (DISABLED3DSPY) GetProcAddress( hModule, "DisableD3DSpy" );
			if( DisableD3DSpy != NULL )
			{
				// D3DSpy is monitoring this program.
				DisableD3DSpy();

				return false;
			}
			else
			{
				// D3DSpy is not monitoring this program.
			}
		}
		//

		// 3DAnalyze 검사
		HMODULE hModule2, hModule3, hModule4, hModule5;
		hModule2 = hModule3 = hModule4 = hModule5 = NULL;

		hModule2 = LoadLibrary(TEXT("d3dg.dll"));
		hModule3 = LoadLibrary(TEXT("d3df.dll"));
		hModule4 = LoadLibrary(TEXT("hook_3DA.dll"));
		hModule5 = LoadLibrary(TEXT("ForceDLL.dll"));

		if(hModule2 != NULL || hModule3 != NULL || hModule4 != NULL || hModule5 != NULL)
		{
			MessageBox(GetForegroundWindow(), _T("3DAnalyze 실행중!!!\n불법 프로그램입니다.\n자신의 컴퓨터에서 d3dg.dll d3df.dll hook_3DA.dll ForceDLL.dll를 검색하여 삭제 하십시오."), _T("@_@ =_="), MB_OK);
			return false;
		}

		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL Uninitialize3DDevice()
	{
		ReleaseAllFont();

		if( g_pDirect3DDevice != NULL)
		{
			g_pDirect3DDevice->Release();
			g_pDirect3DDevice = NULL;
		}

		if( g_pDirect3D != NULL)
		{
			g_pDirect3D->Release();
			g_pDirect3D = NULL;
		}

		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	//HT_CHEAT : 윈도우창 모드 디폴트
	int StartDevice_Default( HWND hWnd, int xsize, int ysize )
	{
		GetClientRect( hWnd, &WindowRect );
		if( g_EngineInfo.m_bFullscreen )
		{
            if( !StartDevice_FullScreen( hWnd, xsize, ysize, g_EngineInfo.m_nFormat ))
				return FALSE;
		}
        else
		{
            if (!StartDevice_Windowed( hWnd, xsize, ysize ))
                return FALSE;
		}

		return TRUE;
	}

	// HT_CHEAT : 윈도우창 모드시 영역 체크 
	XIAHGE_API void SetWindowRect( RECT *rect )
	{
		WindowRect = *rect;
	}

	// HT_CHEAT : 윈도우창 모드시 영역 체크 
	XIAHGE_API RECT* GetWindowRect()
	{
		return &WindowRect;
	}
//---------------------------------------------------------------------------------------
	// HT_CHEAT : 윈도우창모드
	// Full Screen 일때
	int StartDevice_FullScreen( HWND hWnd, int xsize, int ysize, D3DFORMAT format )
	{
		if( !g_pDirect3D)
			return FALSE;

		D3DDISPLAYMODE d3ddm;
		HRESULT hr = g_pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm);

		if( FAILED( hr))
			return FALSE;

		hr = g_pDirect3D->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, FALSE); 

		if( FAILED( hr))
		{
			DBG_LogFile( _T("Device Checking 실패"));
			return FALSE;
		}
	
		DWORD dwStyle = GetWindowStyle( hWnd );
        dwStyle &= WS_POPUP | WS_SYSMENU;
        SetWindowLong( hWnd, GWL_STYLE, dwStyle );
		ShowWindow( hWnd, SW_SHOW );
		
		D3DCAPS9 d3d_caps;

		g_pDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3d_caps);
		
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof( d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = /*D3DSWAPEFFECT_DISCARD;*/ D3DSWAPEFFECT_FLIP;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferWidth = xsize;
		d3dpp.BackBufferHeight = ysize;
		d3dpp.BackBufferFormat = d3ddm.Format;
		g_EngineInfo.m_nFormat  = d3ddm.Format;
	//	d3dpp.BackBufferFormat = format;
		
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	//	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		// T&L은 자동으로 해준다.. 그게 속편할것 같음.
		if( d3d_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		{
			// 캐릭터 애니메이션이 soft ware vertex processing이 되어야 하기에.
			hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
				D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);

			if( FAILED( hr))
			{
				hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);
			}
		}
		else
		{
			hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);
		}

		if( FAILED( hr))
		{
			MessageBox(0,"FullScreen Direct3D Device실제 생성 실패","Xiah",MB_OK);
			DBG_LogFile( _T("FullScreen Direct3D Device실제 생성 실패"));
		
			return FALSE;
		}

		ZeroMemory( &g_D3DPresent, sizeof( g_D3DPresent ) );
		// reset the device
		hr = g_pDirect3DDevice->Reset( &d3dpp );
		if( FAILED( hr ))
		{
			MessageBox( GetForegroundWindow(), "Reset() - Failed", "Error", MB_OK );
			PostQuitMessage( 0 );
		}
		memcpy( &g_D3DPresent, &d3dpp, sizeof( D3DPRESENT_PARAMETERS));
		
		g_pDirect3DDevice->GetDeviceCaps( &g_Direct3DCaps);

		g_Device.SetD3DDevice(g_pDirect3DDevice);

		return TRUE;
	}
	// Full Screen 일때...

	//---------------------------------------------------------------------------------------
	// HT_CHEAT : 윈도우창모드
	// 윈도우 창모드일때
	int StartDevice_Windowed( HWND hWnd, int xsize, int ysize )
	{
		if( !g_pDirect3D)
			return FALSE;

		D3DDISPLAYMODE d3ddm;
		HRESULT hr = g_pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm);

		if( FAILED( hr))
			return FALSE;

        hr = g_pDirect3D->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, FALSE); 

		if( FAILED( hr))
			return FALSE;

		D3DCAPS9 d3d_caps;

		g_pDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3d_caps);

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof( d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP; //D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferWidth = xsize;
		d3dpp.BackBufferHeight = ysize;
		d3dpp.BackBufferFormat = d3ddm.Format;
		g_EngineInfo.m_nFormat  = d3ddm.Format;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		// T&L은 자동으로 해준다.. 그게 속편할것 같음.
		if( d3d_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		{
			hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
				D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);

			if( FAILED( hr))
			{
				hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);
			}
		}
		else
		{
			hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);
		}

		if( FAILED( hr))
			return FALSE;

		ZeroMemory( &g_D3DPresent, sizeof( g_D3DPresent ) );
		// reset the device
		hr = g_pDirect3DDevice->Reset( &d3dpp );
		if( FAILED( hr ))
		{
			MessageBox( GetForegroundWindow(), "Reset() - Failed", "Error", MB_OK );
			PostQuitMessage( 0 );
		}
		memcpy( &g_D3DPresent, &d3dpp, sizeof( D3DPRESENT_PARAMETERS));

		DWORD dwStyle = GetWindowStyle( hWnd );
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
        SetWindowLong( hWnd, GWL_STYLE, dwStyle );

        RECT rcWork;
        RECT rc;
        /*
			set the window position and size
	    */
        SetRect( &rc, 0, 0,/*posX, posY,*/ xsize, ysize );
        AdjustWindowRectEx(&rc,
							GetWindowStyle( hWnd ),
                            GetMenu( hWnd ) != NULL,
                            GetWindowExStyle( hWnd ) );

		/*
			Check to see if RECT is off top or left side of screen (from Adjust call),
			if so, move it back to 0,0
		*/
		OffsetRect(&rc, (rc.left < 0) ? -rc.left : 0, (rc.top < 0) ? -rc.top : 0);

		/*
			make sure our window does not hang outside of the work area
			this will make people who have the tray on the top or left
			happy.
		*/
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

		OffsetRect(&rc,
			(rc.left < rcWork.left) ? rcWork.left - rc.left : 0,
			(rc.top < rcWork.top) ? rcWork.top - rc.top : 0);

		SetWindowPos( hWnd, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER );

		if ( xsize == 0 && ysize == 0 )
		{
			ShowWindow( hWnd, SW_HIDE );
		}
		
		g_pDirect3DDevice->GetDeviceCaps( &g_Direct3DCaps);

		g_Device.SetD3DDevice(g_pDirect3DDevice);

		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL ClearScene(D3DCOLOR color)
	{
		return !FAILED( g_pDirect3DDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1, 0));
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL BeginScene()
	{
		g_EngineInfo.m_nRenderedVertex	= 0;
		g_EngineInfo.m_nRenderedFace	= 0;
		
		return !FAILED( g_pDirect3DDevice->BeginScene());
	}
	
	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL EndScene()
	{
		DebugEngineInfo::DrawDebugEngineInfo();

		if( g_pCurrentCamera)
			g_pCurrentCamera->m_bUpdate = FALSE;

		return !FAILED( g_pDirect3DDevice->EndScene());
	}

	XIAHGE_API BOOL PresentScene()
	{
		HRESULT hr = g_pDirect3DDevice->Present( 0, 0, g_EngineInfo.m_hWnd, 0);

		return !FAILED(hr);
	}

	//---------------------------------------------------------------------------------------
	//int StartDevice_Windowed(HWND hWnd,int xsize,int ysize)
	//{
	//	if( !g_pDirect3D)
	//		return FALSE;

	//	D3DDISPLAYMODE d3ddm;
	//	HRESULT hr = g_pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm);

	//	if( FAILED( hr))
	//		return FALSE;

	//	hr = g_pDirect3D->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, d3ddm.Format, FALSE); 

	//	if( FAILED( hr))
	//		return FALSE;

	//	D3DCAPS9 d3d_caps;

	//	g_pDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3d_caps);

	//	D3DPRESENT_PARAMETERS d3dpp;
	//	ZeroMemory( &d3dpp, sizeof( d3dpp));
	//	d3dpp.Windowed = TRUE;
	//	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP; //D3DSWAPEFFECT_DISCARD;
	//	d3dpp.BackBufferWidth = xsize;
	//	d3dpp.BackBufferHeight = ysize;
	//	d3dpp.BackBufferFormat = d3ddm.Format;
	//	g_EngineInfo.m_nFormat  = d3ddm.Format;
	//	d3dpp.EnableAutoDepthStencil = TRUE;
	//	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	//	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	//	// T&L은 자동으로 해준다.. 그게 속편할것 같음.
	//	if( d3d_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	//	{
	//		hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
	//			D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);

	//		if( FAILED( hr))
	//		{
	//			hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
	//				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);
	//		}
	//	}
	//	else
	//	{
	//		hr = g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
	//			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDirect3DDevice);
	//	}

	//	if( FAILED( hr))
	//		return FALSE;

	//	memcpy( &g_D3DPresent, &d3dpp, sizeof( D3DPRESENT_PARAMETERS));

	//	g_pDirect3DDevice->GetDeviceCaps( &g_Direct3DCaps);

	//	g_Device.SetD3DDevice(g_pDirect3DDevice);

	//	return TRUE;
	//}

	/*************************************************************************************************************
	..............................................................................................................
	......................SSSS...EEEEEE..PPPPP.....AA....RRRRR.....AA....TTTTTT...OOOO...RRRRR....................
	.....................SS..SS..EE......PP..PP...AAAA...RR..RR...AAAA.....TT....OO..OO..RR..RR...................
	.....................SS......EE......PP..PP..AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
	......................SSSS...EEEEEE..PPPPP...AAAAAA..RRRR....AAAAAA....TT....OO..OO..RRRR.....................
	.........................SS..EE......PP......AA..AA..RR.RR...AA..AA....TT....OO..OO..RR.RR....................
	.....................SS..SS..EE......PP......AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
	......................SSSS...EEEEEE..PP......AA..AA..RR..RR..AA..AA....TT.....OOOO...RR..RR...................
	..............................................................................................................
	*************************************************************************************************************/

	#define MAX_FONT_COUNT	50

	sFont	g_FontList[MAX_FONT_COUNT];
	int		g_nFontCount;
	HDC		g_NullDC;
	
	
	//---------------------------------------------------------------------------------------
	BOOL CreateFontSystem()
	{
		g_NullDC = CreateCompatibleDC( NULL);
		
		if( g_NullDC == NULL)
			return FALSE;

		g_nFontCount = 0;
		
		return TRUE;
	}
	
	//---------------------------------------------------------------------------------------
	void ReleaseFontSystem()
	{
		ReleaseAllFont();

		DeleteDC( g_NullDC);
	}


	//---------------------------------------------------------------------------------------
	sFont* CreateFont(sString strFontName,int FontHeight)
	{
		if( g_nFontCount >= MAX_FONT_COUNT )
		{
			DBG_LogFile( _T("CreateFont: FONT POOL FULL! g_nFontCount=%d, MAX=%d. Reusing font[0]."), g_nFontCount, MAX_FONT_COUNT );
			return &g_FontList[0]; // fallback: reuse first font
		}

		sFont* pFont = &g_FontList[ g_nFontCount];

		pFont->m_hFont = ::CreateFont( FontHeight, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 
									OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
									DEFAULT_PITCH, (LPCTSTR)strFontName);

		if( pFont->m_hFont == NULL)
			return NULL;

		pFont->m_strFontName = strFontName;
		pFont->m_nFontHeight = FontHeight;

		SelectObject( g_NullDC, pFont->m_hFont);
		GetTextMetrics( g_NullDC, &pFont->m_TextMetric);
		
		g_nFontCount ++;
		return pFont;
	}
	
	XIAHGE_API sFont* GetFont(sString strFontName,int FontHeight)
	{
		for(int i = 0; i < g_nFontCount; i++)
		{
			if( g_FontList[ i].m_strFontName == strFontName &&
				g_FontList[ i].m_nFontHeight == FontHeight)
				return &g_FontList[ i];
		}

		return CreateFont( strFontName, FontHeight);
	}

	//---------------------------------------------------------------------------------------
	void ReleaseAllFont()
	{
		for(int i = 0; i < g_nFontCount; i++)
		{
			::DeleteObject( g_FontList[ i].m_hFont);
		}

		g_nFontCount = 0;
	}

	//---------------------------------------------------------------------------------------
	VT_LVertex g_DrawBoundVertex[ 8];
	unsigned short g_DrawBoundIndex[ 24] =
	{
		// 아래 뚜껑
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		// 윗 뚜껑
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		// 모서리
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	unsigned short g_DrawBoundFaceIndex[ 36] =
	{
		// 아래뚜껑
		0, 1, 3, 1, 2, 3,
		4, 5, 7, 5, 6, 7,
		0, 4, 1, 4, 1, 5,
		7, 3, 2, 6, 2, 7,
		0, 4, 3, 4, 7, 3,
		5, 1, 2, 5, 2, 6
	};

	//---------------------------------------------------------------------------------------
	XIAHGE_API BOOL DrawBound(BBoxAABB3 &box,D3DCOLOR color,BOOL bWire,IDirect3DTexture9* pTexture)
	{
		Matrix4x4 iTM;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&iTM);
		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
		
		if( (color >> 24) != 255)
		{
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		g_pDirect3DDevice->SetTexture( 0, pTexture);

		g_DrawBoundVertex[ 0].pos = Vector3( box.m_vMin.x, box.m_vMin.y, box.m_vMin.z);
		g_DrawBoundVertex[ 1].pos = Vector3( box.m_vMin.x, box.m_vMin.y, box.m_vMax.z);
		g_DrawBoundVertex[ 2].pos = Vector3( box.m_vMax.x, box.m_vMin.y, box.m_vMax.z);
		g_DrawBoundVertex[ 3].pos = Vector3( box.m_vMax.x, box.m_vMin.y, box.m_vMin.z);

		g_DrawBoundVertex[ 4].pos = Vector3( box.m_vMin.x, box.m_vMax.y, box.m_vMin.z);
		g_DrawBoundVertex[ 5].pos = Vector3( box.m_vMin.x, box.m_vMax.y, box.m_vMax.z);
		g_DrawBoundVertex[ 6].pos = Vector3( box.m_vMax.x, box.m_vMax.y, box.m_vMax.z);
		g_DrawBoundVertex[ 7].pos = Vector3( box.m_vMax.x, box.m_vMax.y, box.m_vMin.z);
		
		g_DrawBoundVertex[ 0].diffuse = color;
		g_DrawBoundVertex[ 1].diffuse = color;
		g_DrawBoundVertex[ 2].diffuse = color;
		g_DrawBoundVertex[ 3].diffuse = color;
		g_DrawBoundVertex[ 4].diffuse = color;
		g_DrawBoundVertex[ 5].diffuse = color;
		g_DrawBoundVertex[ 6].diffuse = color;
		g_DrawBoundVertex[ 7].diffuse = color;

		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX);
		
		if( bWire)
			g_pDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, g_DrawBoundIndex, D3DFMT_INDEX16, g_DrawBoundVertex, sizeof( VT_LVertex));
		else
			g_pDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 12, g_DrawBoundFaceIndex, D3DFMT_INDEX16, g_DrawBoundVertex, sizeof( VT_LVertex));

		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);

		return TRUE;
	}

	//---------------------------------------------------------------------------------------
	extern XIAHGE_API BOOL DrawBound_OBB(BBoxOBB3 &box,D3DCOLOR color,BOOL bWire,IDirect3DTexture9* pTexture);
	
	XIAHGE_API BOOL DrawBound(BBoxOBB3 &box,D3DCOLOR color,BOOL bWire,IDirect3DTexture9* pTexture)
	{
		DrawBound_OBB(box, color, bWire, pTexture);

		float unit_size = 0.2f;
		BBoxAABB3 unit_box;
		unit_box.m_vMin = Vector3( -unit_size, -unit_size, -unit_size);
		unit_box.m_vMax = -unit_box.m_vMin;

		for(int i = 0; i < 6; ++i)
		{
			Matrix4x4 tm;
			tm.t = box.m_vCenter[ i];
			BBoxOBB3 center_box( unit_box, tm);

			DrawBound_OBB( center_box, D3DCOLOR_XRGB( 255, 255, 0), bWire, pTexture);
		}

		return TRUE;
	}

	XIAHGE_API BOOL DrawBound_OBB(BBoxOBB3 &box,D3DCOLOR color,BOOL bWire,IDirect3DTexture9* pTexture)
	{
		Matrix4x4 iTM;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&iTM);
		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

		if( (color >> 24) != 255)
		{
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		g_pDirect3DDevice->SetTexture( 0, pTexture);

		for(int i = 0; i < 8; ++i)
		{
			g_DrawBoundVertex[ i].pos = box.m_Point[ i];
			g_DrawBoundVertex[ i].diffuse = color;
		}

		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX);

		if( bWire)
			g_pDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, g_DrawBoundIndex, D3DFMT_INDEX16, g_DrawBoundVertex, sizeof( VT_LVertex));
		else
			g_pDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 12, g_DrawBoundFaceIndex, D3DFMT_INDEX16, g_DrawBoundVertex, sizeof( VT_LVertex));

		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);

		return TRUE;
	}


	//---------------------------------------------------------------------------------------
	sSize TextureRescaleByQuality(sSize size)
	{
		#define TEXTURE_MIN_SIZE	16

		UINT nWidth	 = size.cx;
		UINT nHeight = size.cy;

		for(int i = 0; i < (4 - g_EngineInfo.m_nTextureDetail); ++i)
		{
			if( nWidth > TEXTURE_MIN_SIZE)
			{
				nWidth /= 2;
				if( nWidth < TEXTURE_MIN_SIZE)
					nWidth = TEXTURE_MIN_SIZE;
			
			}

			if( nHeight > TEXTURE_MIN_SIZE)
			{
				nHeight /= 2;

				if( nHeight < TEXTURE_MIN_SIZE)
					nHeight = TEXTURE_MIN_SIZE;
			}
		}

		if( nWidth > 256)
			nWidth = 256;
		if( nHeight > 256)
			nHeight = 256;

		return sSize( nWidth, nHeight);
	}

	// RS [11/28/2005] 
	Direct3D9Device::Direct3D9Device() : m_dwLastFVF(0)
	{

	}

	Direct3D9Device::~Direct3D9Device()
	{
	}

	void Direct3D9Device::Clear()
	{
		m_dwLastFVF			= 0;
		m_dwLastTexStage	= 99999;
		m_pLastTex			= NULL;
		m_pLastVertexBuffer = NULL;
		m_pLastIndexData	= NULL;
	}

	void Direct3D9Device::SetFVF(DWORD dwFVF)
	{
		if(m_dwLastFVF == dwFVF)
		{
			return;
		}

		m_pD3DDevice->SetFVF(dwFVF);

		m_dwLastFVF = dwFVF;
	}

	void Direct3D9Device::SetTexture(DWORD dwStage, IDirect3DTexture9* pTex)
	{
		if((m_dwLastTexStage == dwStage) && (m_pLastTex == pTex))
		{
			return;
		}

		m_pD3DDevice->SetTexture(dwStage, pTex);

		m_dwLastTexStage = dwStage;
		m_pLastTex = pTex;
	}

	void Direct3D9Device::SetStreamSource(IDirect3DVertexBuffer9* pStreamData, UINT nStride)
	{
		if(m_pLastVertexBuffer == pStreamData)
		{
			return;
		}

		m_pD3DDevice->SetStreamSource(0, pStreamData, 0, nStride);

		m_pLastVertexBuffer = pStreamData;
	}

	void Direct3D9Device::SetIndices(IDirect3DIndexBuffer9* pIndexData)
	{
		if(m_pLastIndexData == pIndexData)
		{
			return;
		}

		m_pD3DDevice->SetIndices(pIndexData);

		m_pLastIndexData = pIndexData;
	}
};
