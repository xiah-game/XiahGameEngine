#pragma once

#include "VertexType.h"
#include "RenderObject.h"
#include "Camera.h"

namespace XiahGameEngine
{
	class XIAHGE_API Direct3D9Device
	{
	public:
		Direct3D9Device();
		~Direct3D9Device();

		inline void SetD3DDevice(IDirect3DDevice9* pDevice);		

		void Clear();

		void SetFVF(DWORD dwFVF);
		void SetStreamSource(IDirect3DVertexBuffer9* pStreamData, UINT nStride);
		void SetTexture(DWORD dwStage, IDirect3DTexture9* pTex);
		void SetIndices(IDirect3DIndexBuffer9* pIndexData);

	protected:

	private:
		IDirect3DDevice9* m_pD3DDevice;

		DWORD m_dwLastFVF;
		DWORD m_dwLastTexStage;
		IDirect3DTexture9* m_pLastTex;
		IDirect3DVertexBuffer9* m_pLastVertexBuffer;
		IDirect3DIndexBuffer9* m_pLastIndexData;

	};

	inline void Direct3D9Device::SetD3DDevice(IDirect3DDevice9* pDevice)
	{
		m_pD3DDevice = pDevice;
	}

	XIAHGE_API extern Direct3D9Device		g_Device;

	//---------------------------------------------------------------------------------------
	extern IDirect3D9*						g_pDirect3D;
	XIAHGE_API extern IDirect3DDevice9*		g_pDirect3DDevice;
	XIAHGE_API extern D3DPRESENT_PARAMETERS	g_D3DPresent;

	//---------------------------------------------------------------------------------------
	extern XIAHGE_API BOOL Initialize3DDevice(HWND hWnd,BOOL bFullscreen,unsigned short nWidth,unsigned short nHeight,BOOL bRGB16 = TRUE);
	extern XIAHGE_API BOOL Uninitialize3DDevice();

	extern XIAHGE_API BOOL ClearScene(D3DCOLOR color);
	extern XIAHGE_API BOOL BeginScene();
	extern XIAHGE_API BOOL EndScene();
	extern XIAHGE_API BOOL PresentScene();
	
	extern int StartDevice_Windowed(HWND hWnd,int xsize,int ysize);
	extern int StartDevice_FullScreen(HWND hWnd,int xsize,int ysize,D3DFORMAT format);
	extern void ReleaseAllFont();
	extern BOOL CreateFontSystem();
	extern void ReleaseFontSystem();

	extern XIAHGE_API BOOL DrawBound(BBoxAABB3 &box,D3DCOLOR color = D3DCOLOR_XRGB( 255, 0, 0),BOOL bWire = TRUE,IDirect3DTexture9* pTexture = NULL);
	extern XIAHGE_API BOOL DrawBound(BBoxOBB3 &box,D3DCOLOR color = D3DCOLOR_XRGB( 255, 0, 0),BOOL bWire = TRUE,IDirect3DTexture9* pTexture = NULL);

	extern sSize TextureRescaleByQuality(sSize size);

	//HT_CHEAT : 윈도우창 모드
	XIAHGE_API extern RECT WindowRect;
	extern XIAHGE_API void  SetWindowRect( RECT *rect );
	extern XIAHGE_API RECT* GetWindowRect();

	 //HT_CHEAT : 윈도우창 모드 디폴트
	extern int StartDevice_Default( HWND hWnd, int xsize, int ysize );

	//---------------------------------------------------------------------------------------
	extern HDC			   g_NullDC;
	
	struct sFont
	{
		HFONT	   m_hFont;
		TEXTMETRIC m_TextMetric;
	
		sString	   m_strFontName;
		int		   m_nFontHeight;
	};

	XIAHGE_API sFont* GetFont(sString strFontName,int FontHeight);

	XIAHGE_API extern CCamera*	g_pCurrentCamera;
};
