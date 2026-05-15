// Thunder.h: interface for the CThunder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THUNDER_H__C4105059_F4A4_4876_84FC_952AA681E659__INCLUDED_)
#define AFX_THUNDER_H__C4105059_F4A4_4876_84FC_952AA681E659__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace XiahGameEngine
{

//#define		rnd()			(((FLOAT)rand() ) / RAND_MAX)
//#define		THUNDER_VERTEX_MAX		400
//
//class CThunder  
//{
///*
//	LPDIRECT3DDEVICE8			m_pd3dDevice;
//	LPDIRECT3DVERTEXBUFFER8		m_pThunderVB;
//	LPDIRECT3DTEXTURE8			m_pThunderTexture;
//	LPDIRECT3DVERTEXBUFFER8		m_pThunderSumVB;
//	LPDIRECT3DINDEXBUFFER8		m_pThunderSumIB;
//
//	THUNDERVERTEX*				m_pThunderVertex;
//	int							m_nPosX, m_nPosY, m_nPosZ;
//	int							m_nBlokenRatio;			// 번개의 줄기의 껌임 정도 
//	float						m_fBlokenRatio;	
//	int							m_nBodyDivision;		// 번개의 메인 줄기의 껌임 개수 
//	int							m_nSubBodyNumber;		// 번개의 메인 줄기에서 뻗어난 작은 줄기 개수 
//	int							m_nSubBodyDivision;		// 번개의 작은 줄기의 껌임 정도 
//	int							m_nWidth;				// Thunder width
//	DWORD						m_dwDrawTime, m_dwDrawGap;
//
//public:
//	void SetOnlyData(int x, int y, int z, int Ratio, int BodyDivision, int SubBody, int SubBodyDivision, int DrawGap, int Width, CString strTexPath);
//	void Setting(int x, int y, int z, int Ratio, int BodyDivision, int SubBody, int SubBodyDivision, int DrawGap, int Width);
//	void SetTexturePath(CString strTexture);
//	void GetSettingData(int &x, int &y, int &z, int &Ratio, int &BodyDivision, int &SubBody, int &SubBodyDivision, int &DrawGap, int &Width);
//	void Load(FILE *fp);
//	void Save(FILE *fp);
//	void DrawSubThunder(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd);
//	void DrawThunder(bool bLightEnable);
//	void Init(int x, int y, int z, int Ratio, int BodyDivision, int SubBody, int SubBodyDivision, int DrawGap, int Width);
//	void Render();
//	void Release();
//	void CreateThunder();
//*/
//public:
//	CThunder();
//	virtual ~CThunder();
///*
//	CString m_TexturePath;
//	bool	m_bPlay;
//*/
//};

};// namespace

#endif // !defined(AFX_THUNDER_H__C4105059_F4A4_4876_84FC_952AA681E659__INCLUDED_)