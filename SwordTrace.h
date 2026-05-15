#pragma once

#define	SWORDTRACE_MAX		500		// Max is 500

namespace XiahGameEngine
{
	//----------------------------------------------------
	class CSwordTrace
	{
		LPDIRECT3DVERTEXBUFFER9		m_VB;
		LPDIRECT3DINDEXBUFFER9		m_IB;

		DWORD		m_dwElapsedTime;
		DWORD		m_dwPrevSpawnTime;

		Vector3		m_vStartPosArray[ SWORDTRACE_MAX ];
		Vector3		m_vEndPosArray[ SWORDTRACE_MAX ];

		signed int	m_nAlphaArray[ SWORDTRACE_MAX ];
		int			m_nR, m_nG, m_nB;

		int			m_nCurrentIndex;	// index of all array
		bool		m_bStart;
		bool		m_bEnd;
		int			m_nRenderVertexIndex;

		BOOL		m_bIsVisible;

	public:
		XIAHGE_API CSwordTrace();
		XIAHGE_API ~CSwordTrace();

		XIAHGE_API BOOL Init();
		XIAHGE_API void Release();

		XIAHGE_API void Start(Vector3 vStart, Vector3 vEnd, int nR, int nG, int nB);
		XIAHGE_API void End();

		XIAHGE_API void Update(DWORD dwTime,Vector3 vStart, Vector3 vEnd);
		XIAHGE_API void Render();

		XIAHGE_API bool IsStart();
		XIAHGE_API bool IsEnd();
		XIAHGE_API void SetVisible(BOOL bVisible);
	};

};
