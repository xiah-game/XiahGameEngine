#pragma once

#define	PARTICLE_MAX				250
#define RAINSURFACE_MAX				170
#define	rnd()			(((FLOAT)rand() ) / RAND_MAX)

namespace XiahGameEngine
{
	enum eRAINSNOW
	{
		eNone, eRain, eSnow
	};

	struct sRAINSNOW
	{
		Vector3 vPos;
		Vector3 vDir;

		// 눈이 돌게 한다. 각각 다른 시간으로.
		DWORD	dwPeriod;
		DWORD	dwSnowTextureChangeTime;
		BYTE	nSnowTextureStep;
	};

	struct sRAINSURFACE
	{
		DWORD dwElapsedTime;
		Vector3 vPos;
		BYTE nTextureStep;	// 텍스쳐가 3단계이므로 0, 1, 2
	};

	typedef std::list<sRAINSNOW*>	 RAINSNOWLIST;
	typedef std::list<sRAINSURFACE*> RAINSURFACELIST;

	//---------------------------------------------------
	class CRainSnow
	{
		BYTE		m_nType;
		int			m_nTotalCount;
		DWORD		m_dwElapsedTime;
		DWORD		m_dwDelaySpawnTime;
		DWORD		m_dwPreviousSpawnTime;
		DWORD		m_dwEndTime;

		int			m_nRainSurfaceTotalCount;
		DWORD		m_dwDelaySpawnTime2;
		DWORD		m_dwPreviousSpawnTime2;
		DWORD		m_dwEndTime2;

		bool		m_bStart;
		bool		m_bEnd;
		bool		m_bEndCalled;

		int			m_nRainSnowCount;		// 눈, 비
		int			m_nRainSurfaceCount;	// 비가 올때 바닥이 빗방울이 튕기는 거.

		LPDIRECT3DVERTEXBUFFER9		m_VB;	// 눈이나 비
		LPDIRECT3DINDEXBUFFER9		m_IB;
		LPDIRECT3DVERTEXBUFFER9		m_VB2;	// 비가 올때 바닥에 비가 튕기는 거 표현
		LPDIRECT3DINDEXBUFFER9		m_IB2;
		LPDIRECT3DVERTEXBUFFER9		m_VB_s;	// 눈
		LPDIRECT3DINDEXBUFFER9		m_IB_s;

		LPDIRECT3DTEXTURE9			m_pRainTexture;
		LPDIRECT3DTEXTURE9			m_pSnowTexture;
		LPDIRECT3DTEXTURE9			m_pRainSurfaceTexture;	// 비가 올때 바닥에 비가 튕기는 텍스쳐

		sRAINSNOW					m_PrePool[ PARTICLE_MAX*2 ];
		sRAINSURFACE				m_PreRainSurfacePool[ RAINSURFACE_MAX ];

		RAINSNOWLIST				m_RainSnowPool;
		RAINSNOWLIST				m_List;

		RAINSURFACELIST				m_RainSurfacePool;
		RAINSURFACELIST				m_RainSurfaceList;

	public:
		XIAHGE_API CRainSnow();
		XIAHGE_API virtual ~CRainSnow();

		XIAHGE_API BOOL Init();
		XIAHGE_API void Release();
		XIAHGE_API void Start(BYTE nType);
		XIAHGE_API void Restart();
		XIAHGE_API void End();
		XIAHGE_API void AllStop();

		void CreateParticle(int nCount);
		int CalculateSpawnParticle(DWORD dwTime);

		void CreateRainSurface(int nCount);
		int CalculateRainSurface(DWORD dwTime);

		XIAHGE_API void Update(DWORD dwTime);
		XIAHGE_API void Render();

		XIAHGE_API int GetType();
		XIAHGE_API int GetStatus();

	};

	extern XIAHGE_API CRainSnow g_RainSnow;

};

