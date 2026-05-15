#pragma once

#define		GRASS_MAX		400

namespace XiahGameEngine
{
	//---------------------------------------------------------
	class CGrass
	{
		LPDIRECT3DVERTEXBUFFER9	m_pVB;
		LPDIRECT3DINDEXBUFFER9	m_pIB;
		LPDIRECT3DTEXTURE9		m_pGrassTexture;

	public:
		XIAHGE_API CGrass();
		XIAHGE_API ~CGrass();

		XIAHGE_API BOOL Init();
		XIAHGE_API BOOL AddGrass(int nIndex, Vector3 vStart, Vector3 vEnd, float fHeight);
		XIAHGE_API BOOL Update();
		XIAHGE_API BOOL Render();
		XIAHGE_API BOOL ClearAllGrass();
		XIAHGE_API BOOL Release();

		XIAHGE_API BOOL VBLock();
		XIAHGE_API BOOL VBUnlock();

		// variables
		int		m_nCount;
		Vector3		m_vCenterPos[ GRASS_MAX ];
		float		m_fHeight[ GRASS_MAX ];
		float		m_fHalfWidth[ GRASS_MAX ];
		bool	m_bGrassAdded;

		VT_LVertex* m_pVertex;

	};

//	XIAHGE_API extern CGrass g_Grass;

};
