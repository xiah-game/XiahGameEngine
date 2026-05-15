#pragma once

#define		HITEFFECT_MAX			200		// 화면상에 나오는 HitEffect 객체 최대 개수
#define		HITEFFECTVERTEX_MAX		300		// max vertex is 300 * 4 = 1200
#define		DAMAGE_MAX				9		//HT_0907 : 공격력 맥스값 변경( 65000을 넘다니 ㅡㅡ; )

namespace XiahGameEngine
{

#pragma warning( disable: 4305 )
#pragma warning( disable: 4309 )


	//--------------------------------------------------------
	struct sTexData
	{
		Vector2		tex[4];
	};

	struct sHitEffectData
	{
		DWORD		dwElapsedTime;
		int			nType;
		Vector3		vPos;
		int			nCount;		// 그려지는 텍스쳐의 개수.
		sTexData	tex[16];
		float		fSize;
		int			nAlpha;
	};

	typedef std::list<sHitEffectData*> HITEFFECTLIST;

	//------------------------------------------------------
	class CHitEffect
	{
		LPDIRECT3DVERTEXBUFFER9		m_pVB;	// number and miss
		LPDIRECT3DINDEXBUFFER9		m_pIB;
		LPDIRECT3DVERTEXBUFFER9		m_pHitVB;
		LPDIRECT3DINDEXBUFFER9		m_pHitIB;
		LPDIRECT3DTEXTURE9			m_pTexture;
		LPDIRECT3DTEXTURE9			m_pHitTexture;

		int		m_nCount;
		int		m_nHitCount;
		int		m_Table[DAMAGE_MAX]; //HT_0907 : 공격력 맥스값 변경( 65000을 넘다니 ㅡㅡ; )
		int		m_n4VertexIndex;
		int		m_n4VertexIndexHit;

		// number and miss
		HITEFFECTLIST		m_List;
		HITEFFECTLIST		m_Pool;
		sHitEffectData		m_PrePool[ HITEFFECT_MAX ];

		// hit
		HITEFFECTLIST		m_HitList;
		HITEFFECTLIST		m_HitPool;
		sHitEffectData		m_HitPrePool[ HITEFFECT_MAX ];

	public:
		XIAHGE_API CHitEffect();
		XIAHGE_API ~CHitEffect();

		XIAHGE_API BOOL Init();
		XIAHGE_API BOOL Release();

		XIAHGE_API BOOL AddHitEffect(int nType, DWORD wDamage, Vector3 vPos);
		XIAHGE_API BOOL Update();
		XIAHGE_API BOOL Render();

		BOOL SeparateNumber(DWORD wNumber,int &nA, int *list);

	};

	extern XIAHGE_API CHitEffect g_HitEffect;

};