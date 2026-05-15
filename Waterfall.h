// Waterfall2.h: interface for the CWaterfall2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATERFALL2_H__45305FE3_43F7_4E08_8347_5F9E929F3248__INCLUDED_)
#define AFX_WATERFALL2_H__45305FE3_43F7_4E08_8347_5F9E929F3248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace XiahGameEngine
{

#define		rnd()			(((FLOAT)rand() ) / RAND_MAX)
#define		PARTICLE_WATER_MAX				1000
#define		PARTICLE_WATER_BOTTOM_MAX		PARTICLE_WATER_MAX*2


// particle class
//class CParticleInfo
//{
///*
//public:
//	CParticleInfo()
//	{
//		m_vStartPos	= D3DXVECTOR3( 0, 0, 0 );
//		m_vPos		= D3DXVECTOR3( 0, 0, 0 );
//		m_vDir		= D3DXVECTOR3( 0, 0, 0 );
//
//		m_dwCreateTime = m_dwPrevTime = GetTickCount();
//	};
//
//	D3DXVECTOR3		m_vStartPos, m_vPos, m_vDir;
//	DWORD			m_dwCreateTime, m_dwPrevTime;
//	float			fPower, fTTL, fScale, fAlpha;
//	int				iMoveRightCount;
//	bool			bMoveRight;
//*/
//};

//typedef std::list<CParticleInfo *> WPARTICLELIST;
//
//
//class CWaterfall
//{
///*
//	// vertex buffer and etc pointer
//	LPDIRECT3DVERTEXBUFFER8		m_pWaterfallVB;				// squre mesh x-y
//	LPDIRECT3DTEXTURE8			m_pWaterTexture;
//	LPDIRECT3DVERTEXBUFFER8		m_pWaterBottomVB;			// squre mesh x-y
//	LPDIRECT3DVERTEXBUFFER8		m_pWaterSumVB;				// optimize vertex buffer of water
//	LPDIRECT3DINDEXBUFFER8		m_pWaterSumIB;				// optimize index buffer of water
//	LPDIRECT3DVERTEXBUFFER8		m_pWaterBottomSumVB;		// optimize vertex buffer of water bottom
//	LPDIRECT3DINDEXBUFFER8		m_pWaterBottomSumIB;		// optimize index buffer of water bottom
//	LPDIRECT3DDEVICE8			m_pd3dDevice;
//
//	// variables
//	int				m_iParticles;				// Current particles size
//	D3DXVECTOR3		m_vParticleStartPos;	// Particle Start Position
//	D3DXVECTOR3		m_vGravity;
//	WPARTICLELIST	m_ListParticles;
//	int				m_PARTICLE_WATERFALL_MAX;
//	int				m_nPosX, m_nPosY, m_nPosZ, m_nNumber;
//	int				m_nDirection, m_nPower, m_nSize, m_nWidth, m_nSizeB;
//	float			m_fWaterWidth, m_fWaterBottomWidth;
//	DWORD			m_dwCreateTime;			// Waterfall create time
//	WPARTICLELIST	m_ListWaterBottom;
//	int				m_nBottomY;				// 이것은 폭포가 떨어지는 바닥의 높이. Client에서 사용.
//
//	enum
//	{
//		DIRECTION_X,
//		DIRECTION_X_MINUS,
//		DIRECTION_Z,
//		DIRECTION_Z_MINUS
//	};
//
//public:
//	void SetOnlyData(int dir, int x, int y, int z, int power, int number, int size, int width, int sizeB, CString strTexPath );
//	void Start(bool bStart);
//	void SetTexturePath(CString strTexture);
//	void GetSettingData(int &dir, int &x, int &y, int &z, int &power, int &number, int &size, int &width, int &sizeB);
//	void Load(FILE *fp);
//	void Save(FILE *fp);
//*/
//public:
//	CWaterfall();
//	virtual ~CWaterfall();
///*
//// Attributes
//	bool			m_bPlay;
//	CString			m_TexturePath;
//
//// Operations
//	void DrawWaterBottom();
//	void UpdateWaterBottom();
//	void CreateWaterBottom(D3DXVECTOR3 vPos);
//	void Init(int dir, int x, int y, int z, int power, int number, int size, int width, int sizeB );
//	void Flow();
//	void Destroy();
//
//	void Setting(int dir, int x, int y, int z, int power, int number, int size, int width, int sizeB );
//	void CreateWaterfall();
//	void UpdateWaterfall();
//	void DrawWaterfall();
//	void RemoveAllParticles();
//	void Release();
//*/
//};

}; // namespace

#endif // !defined(AFX_WATERFALL2_H__45305FE3_43F7_4E08_8347_5F9E929F3248__INCLUDED_)
