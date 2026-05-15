#pragma once

#define	LINEPARTICLE_MAX	30

namespace XiahGameEngine
{
	//
	class CLineParticle
	{
		LPDIRECT3DVERTEXBUFFER9		m_VB;
		LPDIRECT3DINDEXBUFFER9		m_IB;

		DWORD				m_dwElapsedTime;
		DWORD				m_dwPrevSpawnTime;
		unsigned int		m_nPosIndex;
		bool				m_bEnd;
		bool				m_bFinished;

		Vector3				m_vPosList[ LINEPARTICLE_MAX ];
		int					m_nDiffuseArray[ LINEPARTICLE_MAX ];

		int					m_nLineCount;

	public:
		XIAHGE_API CLineParticle();
		XIAHGE_API ~CLineParticle();

		XIAHGE_API BOOL Init();
		void Release();

		XIAHGE_API void Start(Vector3 vPos);
		XIAHGE_API void End();

		XIAHGE_API void Update(DWORD dwTime, Vector3 vPos);
		XIAHGE_API void Render();

		XIAHGE_API bool IsFinished();
	};

};