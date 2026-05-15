#pragma once

#define		STAR_MAX		250		// Max is 250
#define		CLOUD_MAX		100

namespace XiahGameEngine
{
	//----------------------------------------------------------
	class CSkyBox : public CRenderObject
	{
	DECLARE_RENDERTYPE( eRT_NormalSort)
	public:
		XIAHGE_API CSkyBox();
		XIAHGE_API ~CSkyBox();

		XIAHGE_API BOOL Create();
		XIAHGE_API BOOL Release();
		XIAHGE_API BOOL ChangeSkyMap(int MapID);
		
		XIAHGE_API BOOL Render();
		//XIAHGE_API BOOL SetColor(D3DCOLOR color);
		XIAHGE_API BOOL SetColor(D3DCOLOR colorUp, D3DCOLOR colorMid, D3DCOLOR colorDown);

		XIAHGE_API void SetFogEnable(BOOL bFogEnable);

	protected:
		IDirect3DTexture9*		m_pTexture[ 5];
//		VT_LVertex				m_Vertex[ 36];
		D3DCOLOR				m_Color;

		BOOL					m_bFogEnable;

		LPDIRECT3DVERTEXBUFFER9	m_VB;
//		LPDIRECT3DINDEXBUFFER9	m_pIB;

		Matrix4x4				m_matProjection;
	};

	//extern XIAHGE_API CSkyBox	g_SkyBox;


	//----------------------------------------------------------
	// 해, 달, 별을 표현한다.
	enum eSKYSTAR
	{
		eNONE,
		eSun,	// 낮에 해를 표현 
		eMoon,	// 새벽에 달을 표현
		eStars	// 밤에 달과 별을 표현
	};

	class CSkyStar
	{
		LPDIRECT3DVERTEXBUFFER9		m_pVB1;			// 해, 달을 표현할 4-vertex mesh
		LPDIRECT3DVERTEXBUFFER9		m_pVB2;			// 달무리
		LPDIRECT3DVERTEXBUFFER9		m_pVB3;			// 별을 한꺼번에 찍음. 
		LPDIRECT3DINDEXBUFFER9		m_pIB;			// 별 index
		LPDIRECT3DVERTEXBUFFER9		m_pCloudVB;		// 구름
		LPDIRECT3DINDEXBUFFER9		m_pCloudIB;

		LPDIRECT3DTEXTURE9			m_pSunTexture;
		LPDIRECT3DTEXTURE9			m_pMoon1Texture;
		LPDIRECT3DTEXTURE9			m_pMoon2Texture;
		LPDIRECT3DTEXTURE9			m_pStarsTexture;
		LPDIRECT3DTEXTURE9			m_pCloudTexture;

		D3DCOLOR			m_Color;
		DWORD		m_dwElapsedTime;

		BYTE		m_nType;

		Vector2		m_vStarsPosList[ STAR_MAX ];
		DWORD		m_dwStarsTwinkleTime[ STAR_MAX ];
		BYTE		m_bStarsTwinkleStep[ STAR_MAX ];
		int			m_nStarsTwinkleAlpha[ STAR_MAX ];
		Vector2		m_vCloudPosList[ CLOUD_MAX ];
		Vector2		m_vCloudSizeList[ CLOUD_MAX ];

		Vector3		m_vView;
		Vector3		m_vViewRight;
		Vector3		m_vViewUp;

	public:
		XIAHGE_API CSkyStar();
		XIAHGE_API ~CSkyStar();

		XIAHGE_API void SetType(BYTE nType);

		XIAHGE_API BOOL Create();
		XIAHGE_API BOOL CreateStarPosition();
		XIAHGE_API BOOL CreateCloudPosition();
		XIAHGE_API BOOL Release();

		XIAHGE_API BOOL SetColor(D3DCOLOR color);
		XIAHGE_API BOOL Update();
		XIAHGE_API BOOL Render();

		BOOL UpdateSun();
		BOOL RenderSun();
		BOOL UpdateMoon(bool bRenderEdge=true);
		BOOL RenderMoon(bool bRenderEdge=true);
		BOOL UpdateStars();
		BOOL RenderStars();
		BOOL UpdateCloud();
		BOOL RenderCloud();

	};

	//extern XIAHGE_API CSkyStar	g_SkyStar;

	//----------------------------------------------------------

};