#pragma once

#define FOGDOME_GRID_WIDTH		16
#define FOGDOME_GRID_HEIGHT		16
#define FOGDOME_GRID_SIZE		160
#define FOGDOME_ALTITUDE		50
#define FOGDOME_RADIUS			3000
#define MAX_NUM_CLOUDS			50
#define NUM_LOW_CLOUDS			50
#define NUM_HIGH_CLOUDS			50
#define LOW_CLOUD_TILE			2
#define HIGH_CLOUD_TILE			4
#define LOW_CLOUD_SIZE			300
#define HIGH_CLOUD_SIZE			100

namespace XiahGameEngine
{
	class CSKY
	{
	public:
		std::string					m_strFileName;
	
		LPDIRECT3DTEXTURE9			m_pHighTexture;
		LPDIRECT3DTEXTURE9			m_pLowTexture;

		e3d_dif_tex1_vertex*		m_pSkyLowVertices;
		e3d_dif_tex1_vertex*		m_pSkyHighVertices;
		WORD*						m_pSkyIndies;
		e3d_dif_vertex*				m_pFogDomeVertices;
		WORD*						m_pFogDomeIndies;
		DWORD						m_dwNumVertices;
		DWORD						m_dwNumPrimitives;
		DWORD						m_dwNumFogDomeVertices;
		DWORD						m_dwNumFogDomePrimitives;

		DWORD						m_dwSkyColor;

		float						m_fLowWindSpeed;
		float						m_fHighWindSpeed;
		float						m_fLowCloudPos;
		float						m_fHighCloudPos;

		int                         m_nNumClouds;
		SCloud                      m_LowClouds[NUM_LOW_CLOUDS];
		SCloud                      m_HighClouds[NUM_HIGH_CLOUDS];
		SKY_VERTEX                  m_LowCloudVB[LOW_CLOUD_TILE * LOW_CLOUD_TILE * 4];
		SKY_VERTEX                  m_HighCloudVB[HIGH_CLOUD_TILE * HIGH_CLOUD_TILE * 4];

		D3DXMATRIXA16				m_matWorld;

		D3DCOLOR                    m_LowCloudColor;
		D3DCOLOR                    m_HighCloudColor;

		PLANET_VERTEX               m_SunVB[4];
		PLANET_VERTEX               m_SunHaloVB[4];
		PLANET_VERTEX               m_MoonVB[4];
		PLANET_VERTEX               m_MoonHaloVB[4];

		LPDIRECT3DTEXTURE9			m_pSunTexture;
		LPDIRECT3DTEXTURE9			m_pSunHaloTexture;
		LPDIRECT3DTEXTURE9			m_pMoonTexture;
		LPDIRECT3DTEXTURE9			m_pMoonHaloTexture;

		D3DXVECTOR3					m_vSunPos;
		D3DXVECTOR3					m_vSunHaloScale;
		D3DXVECTOR3					m_vMoonPos;

		D3DCOLOR                    m_SunColor;
		D3DCOLOR                    m_SunHaloColor;
		D3DCOLOR                    m_MoonColor;
		D3DCOLOR                    m_MoonHaloColor;

		
	public:
		XIAHGE_API			CSKY();
		XIAHGE_API virtual ~CSKY();

		XIAHGE_API void		DestroyData();

		XIAHGE_API bool		Load(/* const char* name */);

		XIAHGE_API HRESULT	Init();

		XIAHGE_API HRESULT	FrameMove( D3DXVECTOR3& v3ViewPos, float fElapsedTime );
		XIAHGE_API HRESULT	Render();
		//HRESULT RenderReflection();

		XIAHGE_API void		SetColor(DWORD dwColor);
	};

	extern XIAHGE_API CSKY	g_Sky;
}

