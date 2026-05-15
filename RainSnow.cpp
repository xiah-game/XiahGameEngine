#include "stdafx.h"
#include "D3DDevice.h"
#include "RainSnow.h"
#include "MapData.h"
#include "XiahPak.h"


#define RAINSURFACE_LIFETIME		600

namespace XiahGameEngine
{
    XIAHGE_API CRainSnow g_RainSnow;

	//-------------------------------------------------------
	XIAHGE_API CRainSnow::CRainSnow()
	{
		m_nType = eNone;
		m_VB = NULL;
		m_IB = NULL;
		m_VB2 = NULL;
		m_IB2 = NULL;
		m_VB_s = NULL;
		m_IB_s = NULL;
		m_pRainTexture = NULL;
		m_pSnowTexture = NULL;
		m_pRainSurfaceTexture = NULL;

		m_nTotalCount = 0;
		m_nRainSurfaceTotalCount = 0;

		m_dwElapsedTime = 0;
		m_dwPreviousSpawnTime = 0;
		m_dwPreviousSpawnTime2 = 0;
		m_dwEndTime = 0;
		m_dwEndTime2 = 0;

		m_bStart = false;
		m_bEnd = false;
		m_bEndCalled = false;
	}

	XIAHGE_API CRainSnow::~CRainSnow()
	{
		Release();
	}

	XIAHGE_API BOOL CRainSnow::Init()
	{
		// vertex and Index buffer 
/*
        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( PARTICLE_MAX*4*sizeof(VT_LVertex),
											D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_LVERTEX, D3DPOOL_DEFAULT, &m_VB, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( PARTICLE_MAX*4*3*sizeof(WORD),
											D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( RAINSURFACE_MAX*4*sizeof(VT_LVertex),
											D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_LVERTEX, D3DPOOL_DEFAULT, &m_VB2, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( RAINSURFACE_MAX*4*3*sizeof(WORD),
											D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB2, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( PARTICLE_MAX*4*sizeof(VT_LVertex),
											D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_LVERTEX, D3DPOOL_DEFAULT, &m_VB_s, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( PARTICLE_MAX*4*3*sizeof(WORD),
											D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB_s, NULL ) ) )
		return FALSE;
*/
        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( PARTICLE_MAX*4*sizeof(VT_LVertex),
											0, D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_VB, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( PARTICLE_MAX*4*3*sizeof(WORD),
											0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IB, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( RAINSURFACE_MAX*4*sizeof(VT_LVertex),
											0, D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_VB2, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( RAINSURFACE_MAX*4*3*sizeof(WORD),
											0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IB2, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( PARTICLE_MAX*4*sizeof(VT_LVertex),
											0, D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_VB_s, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( PARTICLE_MAX*4*3*sizeof(WORD),
											0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IB_s, NULL ) ) )
		return FALSE;

		// texture
		m_pRainTexture = XiahPak::GetTexture( 50000207 );
		m_pSnowTexture = XiahPak::GetTexture( 50000210 );
		m_pRainSurfaceTexture = XiahPak::GetTexture( 50000212 );

//		HRESULT hr = D3DXCreateTextureFromFile( g_pDirect3DDevice, "rain.bmp", &m_pRainTexture );
//		hr = D3DXCreateTextureFromFile( g_pDirect3DDevice, "snow.bmp", &m_pSnowTexture );
//		hr = D3DXCreateTextureFromFile( g_pDirect3DDevice, "rainb.bmp", &m_pRainSurfaceTexture );

		// prememory allocation
		for(int i=0; i<PARTICLE_MAX*2; i++)
		{
			m_RainSnowPool.push_back( &m_PrePool[i] );
		}

		for(i=0; i<RAINSURFACE_MAX; i++)
		{
			m_RainSurfacePool.push_back( &m_PreRainSurfacePool[i] );
		}

		return TRUE;
	}

	XIAHGE_API void CRainSnow::Release()
	{
		if( m_VB ) m_VB->Release();
		if( m_IB ) m_IB->Release();
		if( m_VB2 ) m_VB2->Release();
		if( m_IB2 ) m_IB2->Release();
		if( m_VB_s ) m_VB_s->Release();
		if( m_IB_s ) m_IB_s->Release();

		m_VB = NULL;
		m_IB = NULL;
		m_VB2 = NULL;
		m_IB2 = NULL;
		m_VB_s = NULL;
		m_IB_s = NULL;

/*
		if( m_pRainTexture ) m_pRainTexture->Release();
		if( m_pSnowTexture ) m_pSnowTexture->Release();
		if( m_pRainSurfaceTexture ) m_pRainSurfaceTexture->Release();

		m_pRainTexture = NULL;
		m_pSnowTexture = NULL;
		m_pRainSurfaceTexture = NULL;
*/
	}

	XIAHGE_API int CRainSnow::GetType()
	{
		return m_nType;
	}

	XIAHGE_API int CRainSnow::GetStatus()
	{
		int nStatus = 0;
		if( !m_bStart && !m_bEnd )
			nStatus = 0;

		if( m_bStart && !m_bEnd )
			nStatus = 1;

		if( !m_bStart && m_bEnd )
			nStatus = 2;

		return nStatus=0;
	}

	XIAHGE_API void CRainSnow::Start(BYTE nType)
	{
		m_dwElapsedTime = 0;
		m_nType = nType;

		m_nTotalCount = 0;
		m_dwPreviousSpawnTime = 0;
		m_dwDelaySpawnTime = 0;

		m_nRainSurfaceTotalCount = 0;
		m_dwPreviousSpawnTime2 = 0;
		m_dwDelaySpawnTime2 = 0;

		m_bStart = true;
		m_bEnd = false;
		m_bEndCalled = false;
	}

	XIAHGE_API void CRainSnow::Restart()
	{
		m_bStart = true;
		m_bEnd = false;
		m_bEndCalled = false;
	}

	XIAHGE_API void CRainSnow::End()
	{
		m_bStart = false;
		m_bEnd = true;

		switch(m_nType)
		{
		case eRain:
			m_dwEndTime = m_dwElapsedTime + 7000;
			m_dwEndTime2 = m_dwElapsedTime + 4000;
			break;
		case eSnow:
			m_dwEndTime = m_dwElapsedTime + 15000;
			break;
		};// switch
	}

	XIAHGE_API void CRainSnow::AllStop()
	{
		// 현재의 상태를 모두 초기화.
		m_nType = eNone;
		m_nTotalCount = 0;
		m_nRainSurfaceTotalCount = 0;

		m_dwElapsedTime = 0;
		m_dwPreviousSpawnTime = 0;
		m_dwPreviousSpawnTime2 = 0;
		m_dwEndTime = 0;
		m_dwEndTime2 = 0;

		m_bStart = false;
		m_bEnd = false;
		m_bEndCalled = false;

		// 현재 만들어진 리스트 초기화.
		RAINSNOWLIST::iterator it;
		for(it=m_List.begin(); it!=m_List.end(); it++)
		{
			sRAINSNOW* pElm = *it;

			m_RainSnowPool.push_back( pElm );
		}// for( m_List )
		m_List.clear();

        RAINSURFACELIST::iterator rit;
		for(rit=m_RainSurfaceList.begin(); rit!=m_RainSurfaceList.end(); rit++)
		{
			sRAINSURFACE* pElm = *rit;

			m_RainSurfacePool.push_back( pElm );
		}// for( m_RainSurfaceList )
		m_RainSurfaceList.clear();
	}

	void CRainSnow::CreateParticle(int nCount)
	{
		if( nCount == 0 )
			return;

		// 딜레이 시간에 맞춰서 생성한다.
		if( m_dwElapsedTime >= m_dwPreviousSpawnTime + m_dwDelaySpawnTime )
		{
			m_dwPreviousSpawnTime = m_dwElapsedTime;

			float fRadius;
			float fDY;
			switch(m_nType)
			{
			case eRain:	
				{
					fDY = -80; //-65;
					fRadius = 100.0f;
					// 개수 만큼.
					for(int i=0; i<nCount; i++)
					{
						if(m_RainSnowPool.size() == 0)
							return;

						sRAINSNOW* pElm = m_RainSnowPool.front();
						m_RainSnowPool.pop_front();

						float fX1 = rnd() * fRadius;
						float fZ1 = rnd() * fRadius;

						if( (rand()%2) ) fX1 = -fX1;
						if( (rand()%2) ) fZ1 = -fZ1;

						float fX = g_pCurrentCamera->m_vAt.x + fX1;
						float fY = g_pCurrentCamera->m_vAt.y + 25.0f;
						float fZ = g_pCurrentCamera->m_vAt.z + fZ1;

						pElm->vPos = Vector3( fX, fY, fZ );
						pElm->vDir = Vector3( 0, fDY, 0 );

						m_List.push_back( pElm );

						m_nTotalCount++;
					}// for
				}
				break;
			case eSnow: 
				{
					// 근거리 눈과 원거리 눈을 만든다.
					float fDDY = rnd() * 3.8f;
					fDY = -( fDDY + 1.6f );

					int nNearSnow;
					int nFarSnow;
					if( nCount == 1 )
					{
						nNearSnow = 1;
						nFarSnow = 0;
					}
					else
					{
						nNearSnow = nCount/2;
						nFarSnow = nCount - nNearSnow;
					}

					// Near
					fRadius = 80.0f;
					for(int i=0; i<nNearSnow; i++)
					{
						if(m_RainSnowPool.size() == 0)
							return;

						sRAINSNOW* pElm = m_RainSnowPool.front();
						m_RainSnowPool.pop_front();

						float fX1 = rnd() * fRadius;
						float fZ1 = rnd() * fRadius;

						if( (rand()%2) ) fX1 = -fX1;
						if( (rand()%2) ) fZ1 = -fZ1;

						float fX = g_pCurrentCamera->m_vAt.x + fX1;
						float fY = g_pCurrentCamera->m_vAt.y + 25.0f;
						float fZ = g_pCurrentCamera->m_vAt.z + fZ1;

						pElm->vPos = Vector3( fX, fY, fZ );
						pElm->vDir = Vector3( 0, fDY, 0 );

						pElm->dwPeriod = 0;
						pElm->nSnowTextureStep = rand() % 4;
						pElm->dwSnowTextureChangeTime = 30 + (rand() % 120);

						m_List.push_back( pElm );

						m_nTotalCount++;
					}// for

					// Far. 카메라가 보는쪽으로 눈이 많이 보이도록 범위를 조절한다.
					fRadius = 70.0f;
					Vector3 vViewDir = g_pCurrentCamera->m_vAt - g_pCurrentCamera->m_vFrom;
					vViewDir.y = 0;
					vViewDir.Normalize();
					Vector3 vNewPos = g_pCurrentCamera->m_vAt + (vViewDir * 80.0f);
					for(int i=0; i<nFarSnow; i++)
					{
						if(m_RainSnowPool.size() == 0)
							return;

						sRAINSNOW* pElm = m_RainSnowPool.front();
						m_RainSnowPool.pop_front();

						float fX1 = rnd() * fRadius;
						float fZ1 = rnd() * fRadius;

						if( (rand()%2) ) fX1 = -fX1;
						if( (rand()%2) ) fZ1 = -fZ1;

						float fX = vNewPos.x + fX1;
						float fY = vNewPos.y + 30.0f;
						float fZ = vNewPos.z + fZ1;

						pElm->vPos = Vector3( fX, fY, fZ );
						pElm->vDir = Vector3( 0, fDY, 0 );

						pElm->dwPeriod = 0;
						pElm->nSnowTextureStep = rand() % 4;
						pElm->dwSnowTextureChangeTime = 30 + (rand() % 120);

						m_List.push_back( pElm );

						m_nTotalCount++;
					}// for

				}
				break;
			};// switch

		}// if
	}

	int CRainSnow::CalculateSpawnParticle(DWORD dwTime)
	{
		int nCount = 0;
		m_dwElapsedTime += dwTime;

		switch(m_nType)
		{
		case eRain:
			{
				// 첨에 비가올때, 조금씩 내리다가 확 내린다.
				if( m_bStart && !m_bEnd )	// Start
				{
					if( m_dwElapsedTime < 2000 )
					{
						nCount = 3;
						m_dwDelaySpawnTime = 300;
					}
					else
					if( m_dwElapsedTime < 3500 )
					{
						nCount = 5;
						m_dwDelaySpawnTime = 200;
					}
					else
					if( m_dwElapsedTime < 5000 )
					{
						nCount = 11;
						m_dwDelaySpawnTime = 130;
					}
					else
					{
						nCount = 24;
						m_dwDelaySpawnTime = 60;
					}
				}// if
				else	// 비가 그칠때에서 서서히 그친다.
				if( !m_bStart && m_bEnd )	// End
				{
					if( m_dwElapsedTime > m_dwEndTime )
					{
						nCount = 0;
						m_bEnd = false;
					}
					else
					if( m_dwEndTime - m_dwElapsedTime < 2000  )
					{
						nCount = 3;
						m_dwDelaySpawnTime = 350;
					}
					else
					if( m_dwEndTime - m_dwElapsedTime < 4000  )
					{
						nCount = 7;
						m_dwDelaySpawnTime = 250;
					}
					else
					if( m_dwEndTime - m_dwElapsedTime < 7000  )
					{
						nCount = 15;
						m_dwDelaySpawnTime = 150;
					}
				}// if
			}
			break;
		case eSnow:
			{
				// 눈이 서서히 내린다.
				if( m_bStart && !m_bEnd )	// Start
				{
					if( m_dwElapsedTime < 3000 )
					{
						nCount = 4;
						m_dwDelaySpawnTime = 450;
					}
					else
					if( m_dwElapsedTime < 6000 )
					{
						nCount = 5;
						m_dwDelaySpawnTime = 400;
					}
					else
					if( m_dwElapsedTime < 90000 )
					{
						nCount = 7;
						m_dwDelaySpawnTime = 350;
					}
					else
					{
						nCount = 14;
						m_dwDelaySpawnTime = 300;
					}
				}// if
				else	// 눈이 서서히 멈춘다.
				if( !m_bStart && m_bEnd )	// End
				{
					if( m_dwElapsedTime > m_dwEndTime )
					{
						nCount = 0;
						m_bEnd = false;
					}
					else
					if( m_dwEndTime - m_dwElapsedTime < 4000  )
					{
						nCount = 1;
						m_dwDelaySpawnTime = 650;
					}
					else
					if( m_dwEndTime - m_dwElapsedTime < 9000  )
					{
						nCount = 3;
						m_dwDelaySpawnTime = 450;
					}
					else
					if( m_dwEndTime - m_dwElapsedTime < 15000  )
					{
						nCount = 6;
						m_dwDelaySpawnTime = 300;
					}
				}// if
			}
			break;
		};// switch

		return nCount;
	}

	void CRainSnow::CreateRainSurface(int nCount)
	{
		if( nCount == 0 ) return;

		// 딜레이 시간에 맞춰서 생성한다.
		if( m_dwElapsedTime >= m_dwPreviousSpawnTime2 + m_dwDelaySpawnTime2 )
		{
			m_dwPreviousSpawnTime2 = m_dwElapsedTime;

			float fRadius = 105.0f;

			for(int i=0; i<nCount; i++)
			{
				sRAINSURFACE* pElm = m_RainSurfacePool.front();
				m_RainSurfacePool.pop_front();

				float fX1 = rnd() * fRadius;
				float fZ1 = rnd() * fRadius;

				if( (rand()%2) ) fX1 = -fX1;
				if( (rand()%2) ) fZ1 = -fZ1;

				float fX = g_pCurrentCamera->m_vAt.x + fX1;
				float fZ = g_pCurrentCamera->m_vAt.z + fZ1;
				float fY = Map::g_MapRes.GetHeight( fX, fZ );

				pElm->dwElapsedTime = 0;
				pElm->vPos = Vector3( fX, fY+0.2f, fZ );
				pElm->nTextureStep = 0;

				m_RainSurfaceList.push_back( pElm );

				m_nRainSurfaceTotalCount++;
			}// for(nCount)
		}// if
	}

	int CRainSnow::CalculateRainSurface(DWORD dwTime)
	{
		int nCount=0;

		if( m_bStart && !m_bEnd )	// Start
		{
			// 서서히 만들어진다.
			if( m_dwElapsedTime < 4500 )
			{
				nCount = 4;
				m_dwDelaySpawnTime2 = 250;
			}
			else
			if( m_dwElapsedTime < 6500 )
			{
				nCount = 8;
				m_dwDelaySpawnTime2 = 170;
			}
			else
			{
				nCount = 19;
				m_dwDelaySpawnTime2 = 70;
			}
		}
		else
		if( !m_bStart && m_bEnd )	// End
		{
			// 서서히 없어진다.
			if( m_dwElapsedTime > m_dwEndTime2 )
			{
				nCount = 0;
			}
			if( m_dwEndTime2 - m_dwElapsedTime < 800 )
			{
				nCount = 7;
				m_dwDelaySpawnTime2 = 300;
			}
			else
			if( m_dwEndTime2 - m_dwElapsedTime < 4000 )
			{
				nCount = 13;
				m_dwDelaySpawnTime2 = 150;
			}
		}// if

		return nCount;
	}

	XIAHGE_API void CRainSnow::Update(DWORD dwTime)
	{
		if( m_nType == eNone ) return;

		int nTotalLimit = 0;
		if( m_nType == eRain ) nTotalLimit = PARTICLE_MAX;
		else if( m_nType == eSnow ) nTotalLimit = PARTICLE_MAX*2;

		// 1. Spawn Control
		if( m_bStart && !m_bEnd )	// Start
		{
			int nCount = CalculateSpawnParticle(dwTime);
			if( m_nTotalCount + nCount < nTotalLimit )
				CreateParticle( nCount );

			// 비가 올때 바닥에 비가 튕기는 거.
			if( m_nType == eRain )
			{
				int nCount2 = CalculateRainSurface(dwTime);
				if( m_nRainSurfaceTotalCount + nCount2 < RAINSURFACE_MAX )
					CreateRainSurface(nCount2);
			}
		}
		else
		if( !m_bStart && m_bEnd )	// End
		{
			int nCount = CalculateSpawnParticle(dwTime);
			if( m_nTotalCount + nCount < nTotalLimit )
				CreateParticle( nCount );

			// 비가 올때 바닥에 비가 튕기는 거.
			if( m_nType == eRain )
			{
				int nCount2 = CalculateRainSurface(dwTime);
				if( m_nRainSurfaceTotalCount + nCount2 < RAINSURFACE_MAX )
					CreateRainSurface(nCount2);
			}
		}

		// 2. Update Particles
		float fD = (float)dwTime / 1000.0f;
		RAINSNOWLIST DeleteList;
		RAINSNOWLIST::iterator it;

		switch( m_nType )
		{
		case eRain:
			{
				// Rain
				for(it=m_List.begin(); it!=m_List.end(); it++)
				{
					sRAINSNOW* pElm = *it;

					pElm->vPos += pElm->vDir * fD;

//					float fBY = Map::g_MapRes.GetHeight( pElm->vPos.x, pElm->vPos.z );
//					if( pElm->vPos.y < fBY - 1 )
					if( pElm->vPos.y < g_pCurrentCamera->m_vAt.y - 10.0f )
					{
						DeleteList.push_back( pElm );
					}
				}// for( m_List )

				// Rain Surface
				RAINSURFACELIST RainSurfaceDeleteList;
				RAINSURFACELIST::iterator rit;

				for(rit=m_RainSurfaceList.begin(); rit!=m_RainSurfaceList.end(); rit++)
				{
					sRAINSURFACE* pElm = *rit;

					pElm->dwElapsedTime += dwTime;
					if( pElm->dwElapsedTime < 150 )
						pElm->nTextureStep = 0;
					else
					if( pElm->dwElapsedTime < 300 )
						pElm->nTextureStep = 1;
					else
					if( pElm->dwElapsedTime < 450 )
						pElm->nTextureStep = 2;
					else
						pElm->nTextureStep = 3;

					if( pElm->dwElapsedTime > RAINSURFACE_LIFETIME )
					{
						RainSurfaceDeleteList.push_back( pElm );
					}
				}// for(m_RainSurfaceList)

				// delete old rain surface
				for(rit=RainSurfaceDeleteList.begin(); rit!=RainSurfaceDeleteList.end(); rit++)
				{
					sRAINSURFACE* pElm = *rit;

					m_RainSurfacePool.push_back( pElm );
					m_RainSurfaceList.remove( pElm );
					m_nRainSurfaceTotalCount--;
				}// for( RainSurfaceDeleteList )
			}
			break;
		case eSnow:
			{
				for(it=m_List.begin(); it!=m_List.end(); it++)
				{
					sRAINSNOW* pElm = *it;

					// 눈의 액션.
					float fX = rnd() * 0.40f;
					float fZ = rnd() * 0.40f;

					if( rand() % 2 ) fX = -fX;
					if( rand() % 2 ) fZ = -fZ;

					pElm->vDir.x = fX;
					pElm->vDir.z = fZ;
					pElm->vPos += pElm->vDir * fD;

					pElm->dwPeriod += dwTime;
					if( pElm->dwSnowTextureChangeTime <= pElm->dwPeriod )
					{
						pElm->dwPeriod = 0;
						pElm->nSnowTextureStep = (++pElm->nSnowTextureStep) % 4;
					}

					if( pElm->vPos.y < g_pCurrentCamera->m_vAt.y - 10.0f )
					{
						DeleteList.push_back( pElm );
					}
				}// for( m_List )

			}
			break;
		};// switch

		// delete old particle
		for(it=DeleteList.begin(); it!=DeleteList.end(); it++)
		{
			sRAINSNOW* pElm = *it;

			m_RainSnowPool.push_back( pElm );
			m_List.remove( pElm );
			m_nTotalCount--;
		}// for( DeleteList )

		//
		if( !m_bStart && !m_bEnd && m_nTotalCount == 0 )
			m_nType = eNone;

/*
		// auto end
		DWORD dwFinishTime;
		if( m_nType == eRain )
			dwFinishTime = 38000;
		else
		if( m_nType == eSnow )
			dwFinishTime = 45000;

		if( !m_bEndCalled && m_dwElapsedTime > dwFinishTime )
		{
			End();
			m_bEndCalled = true;
		}
*/
/*
		// test
		char szChar[200];
		_stprintf( szChar, "\n Rain=%d, RainSurface=%d, Elapsed=%d, DelaySpawn=%d, DelaySpawn2=%d", 
							m_nTotalCount,m_nRainSurfaceTotalCount,m_dwElapsedTime, m_dwDelaySpawnTime, m_dwDelaySpawnTime2 );
		OutputDebugString( szChar );
*/


		// 3. Vertex Buffer Make
		if( m_nTotalCount == 0 ) return;

		//
		D3DMATRIX matView;
		g_pDirect3DDevice->GetTransform( D3DTS_VIEW, &matView );
		Vector3 vView		= Vector3( matView._13, matView._23, matView._33 );
		Vector3 vViewRight	= Vector3( matView._11, matView._21, matView._31 );
		Vector3 vViewUp		= Vector3( matView._12, matView._22, matView._32 );

		m_nRainSnowCount = 0;
		float fHalfSizeX, fHalfSizeY;
		P_COLOR ParticleColor;
		switch( m_nType )
		{
		case eRain:
			{
				fHalfSizeX = 0.3f;
				fHalfSizeY = 4.5f;
				ParticleColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			}
			break;
		case eSnow:
			{
				fHalfSizeX = 0.20f;
				fHalfSizeY = 0.24f;
				ParticleColor = D3DCOLOR_ARGB( 179, 179, 179, 179 );
			}
			break;
		};// switch

		// make vertex and Index buffer
		VT_LVertex* pVertex;
		m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

		WORD* pIndices;
		m_IB->Lock( 0, 0, (void**)&pIndices, 0 );

		VT_LVertex* pVertex2 = NULL;
		WORD* pIndices2 = NULL;
		if( m_nType == eSnow )
		{
			m_VB_s->Lock( 0, 0, (void**)&pVertex2, 0 );
			m_IB_s->Lock( 0, 0, (void**)&pIndices2, 0 );
		}

		for(it=m_List.begin(); it!=m_List.end(); it++)
		{
			sRAINSNOW* pElm = *it;

			int nIndex = m_nRainSnowCount * 4;

			Vector3 vUP( 0, 1, 0 );
			if( m_nRainSnowCount < 250 )
			{
				pVertex[nIndex+0].pos = -vUP * fHalfSizeY - vViewRight * fHalfSizeX;
				pVertex[nIndex+1].pos =  vUP * fHalfSizeY - vViewRight * fHalfSizeX;
				pVertex[nIndex+2].pos = -vUP * fHalfSizeY + vViewRight * fHalfSizeX;
				pVertex[nIndex+3].pos =  vUP * fHalfSizeY + vViewRight * fHalfSizeX;

				for(int i=0; i<4; i++)
				{
					pVertex[nIndex+i].pos = pVertex[nIndex+i].pos + pElm->vPos;
					pVertex[nIndex+i].diffuse = ParticleColor;
				}// for

				if( m_nType == eRain )
				{
					pVertex[nIndex+0].tex.u = 0;
					pVertex[nIndex+0].tex.v = 1;
					pVertex[nIndex+1].tex.u = 0;
					pVertex[nIndex+1].tex.v = 0;
					pVertex[nIndex+2].tex.u = 1;
					pVertex[nIndex+2].tex.v = 1;
					pVertex[nIndex+3].tex.u = 1;
					pVertex[nIndex+3].tex.v = 0;
				}
				else
				if( m_nType == eSnow )
				{
					switch( pElm->nSnowTextureStep )
					{
					case 0:
						pVertex[nIndex+0].tex.u = 0;
						pVertex[nIndex+0].tex.v = 1;
						pVertex[nIndex+1].tex.u = 0;
						pVertex[nIndex+1].tex.v = 0;
						pVertex[nIndex+2].tex.u = 1;
						pVertex[nIndex+2].tex.v = 1;
						pVertex[nIndex+3].tex.u = 1;
						pVertex[nIndex+3].tex.v = 0;
						break;
					case 1:
						pVertex[nIndex+0].tex.u = 1;
						pVertex[nIndex+0].tex.v = 1;
						pVertex[nIndex+1].tex.u = 0;
						pVertex[nIndex+1].tex.v = 1;
						pVertex[nIndex+2].tex.u = 1;
						pVertex[nIndex+2].tex.v = 0;
						pVertex[nIndex+3].tex.u = 0;
						pVertex[nIndex+3].tex.v = 0;
						break;
					case 2:
						pVertex[nIndex+0].tex.u = 1;
						pVertex[nIndex+0].tex.v = 0;
						pVertex[nIndex+1].tex.u = 1;
						pVertex[nIndex+1].tex.v = 1;
						pVertex[nIndex+2].tex.u = 0;
						pVertex[nIndex+2].tex.v = 0;
						pVertex[nIndex+3].tex.u = 0;
						pVertex[nIndex+3].tex.v = 1;
						break;
					case 3:
						pVertex[nIndex+0].tex.u = 0;
						pVertex[nIndex+0].tex.v = 0;
						pVertex[nIndex+1].tex.u = 1;
						pVertex[nIndex+1].tex.v = 0;
						pVertex[nIndex+2].tex.u = 0;
						pVertex[nIndex+2].tex.v = 1;
						pVertex[nIndex+3].tex.u = 1;
						pVertex[nIndex+3].tex.v = 1;
						break;
					};
				}

				// index
				WORD* pIndex = &pIndices[m_nRainSnowCount*6];
				pIndex[0] = nIndex + 0;
				pIndex[1] = nIndex + 1;
				pIndex[2] = nIndex + 2;
				pIndex[3] = nIndex + 1;
				pIndex[4] = nIndex + 3;
				pIndex[5] = nIndex + 2;
			}
			else
			{
				nIndex = (m_nRainSnowCount-250) * 4;

				pVertex2[nIndex+0].pos = -vUP * fHalfSizeY - vViewRight * fHalfSizeX;
				pVertex2[nIndex+1].pos =  vUP * fHalfSizeY - vViewRight * fHalfSizeX;
				pVertex2[nIndex+2].pos = -vUP * fHalfSizeY + vViewRight * fHalfSizeX;
				pVertex2[nIndex+3].pos =  vUP * fHalfSizeY + vViewRight * fHalfSizeX;

				for(int i=0; i<4; i++)
				{
					pVertex2[nIndex+i].pos = pVertex2[nIndex+i].pos + pElm->vPos;
					pVertex2[nIndex+i].diffuse = ParticleColor;
				}// for

				if( m_nType == eRain )
				{
					pVertex2[nIndex+0].tex.u = 0;
					pVertex2[nIndex+0].tex.v = 1;
					pVertex2[nIndex+1].tex.u = 0;
					pVertex2[nIndex+1].tex.v = 0;
					pVertex2[nIndex+2].tex.u = 1;
					pVertex2[nIndex+2].tex.v = 1;
					pVertex2[nIndex+3].tex.u = 1;
					pVertex2[nIndex+3].tex.v = 0;
				}
				else
				if( m_nType == eSnow )
				{
					switch( pElm->nSnowTextureStep )
					{
					case 0:
						pVertex2[nIndex+0].tex.u = 0;
						pVertex2[nIndex+0].tex.v = 1;
						pVertex2[nIndex+1].tex.u = 0;
						pVertex2[nIndex+1].tex.v = 0;
						pVertex2[nIndex+2].tex.u = 1;
						pVertex2[nIndex+2].tex.v = 1;
						pVertex2[nIndex+3].tex.u = 1;
						pVertex2[nIndex+3].tex.v = 0;
						break;
					case 1:
						pVertex2[nIndex+0].tex.u = 1;
						pVertex2[nIndex+0].tex.v = 1;
						pVertex2[nIndex+1].tex.u = 0;
						pVertex2[nIndex+1].tex.v = 1;
						pVertex2[nIndex+2].tex.u = 1;
						pVertex2[nIndex+2].tex.v = 0;
						pVertex2[nIndex+3].tex.u = 0;
						pVertex2[nIndex+3].tex.v = 0;
						break;
					case 2:
						pVertex2[nIndex+0].tex.u = 1;
						pVertex2[nIndex+0].tex.v = 0;
						pVertex2[nIndex+1].tex.u = 1;
						pVertex2[nIndex+1].tex.v = 1;
						pVertex2[nIndex+2].tex.u = 0;
						pVertex2[nIndex+2].tex.v = 0;
						pVertex2[nIndex+3].tex.u = 0;
						pVertex2[nIndex+3].tex.v = 1;
						break;
					case 3:
						pVertex2[nIndex+0].tex.u = 0;
						pVertex2[nIndex+0].tex.v = 0;
						pVertex2[nIndex+1].tex.u = 1;
						pVertex2[nIndex+1].tex.v = 0;
						pVertex2[nIndex+2].tex.u = 0;
						pVertex2[nIndex+2].tex.v = 1;
						pVertex2[nIndex+3].tex.u = 1;
						pVertex2[nIndex+3].tex.v = 1;
						break;
					};
				}

				// index
				WORD* pIndex = &pIndices[(m_nRainSnowCount-250)*6];
				pIndex[0] = nIndex + 0;
				pIndex[1] = nIndex + 1;
				pIndex[2] = nIndex + 2;
				pIndex[3] = nIndex + 1;
				pIndex[4] = nIndex + 3;
				pIndex[5] = nIndex + 2;
			}

			m_nRainSnowCount++;
		}// for

		m_VB->Unlock();
		m_IB->Unlock();

		if( m_nType == eSnow )
		{
			m_VB_s->Unlock();
			m_IB_s->Unlock();
		}

		m_nRainSurfaceCount = 0;
		// Rain Surface
		if( m_nType == eRain && m_nRainSurfaceTotalCount )
		{
			// make vertex and Index buffer
			VT_LVertex* pVertex;
			m_VB2->Lock( 0, 0, (void**)&pVertex, 0 );

			WORD* pIndices;
			m_IB2->Lock( 0, 0, (void**)&pIndices, 0 );

			float fHalfSizeX = 0.56f;
			float fHalfSizeY = 0.32f;

			RAINSURFACELIST::iterator rit;
			for(rit=m_RainSurfaceList.begin(); rit!=m_RainSurfaceList.end(); rit++)
			{
				sRAINSURFACE* pElm = *rit;

				int nIndex2 = m_nRainSurfaceCount * 4;

				pVertex[nIndex2+0].pos = -vViewUp * fHalfSizeY - vViewRight * fHalfSizeX;
				pVertex[nIndex2+1].pos =  vViewUp * fHalfSizeY - vViewRight * fHalfSizeX;
				pVertex[nIndex2+2].pos = -vViewUp * fHalfSizeY + vViewRight * fHalfSizeX;
				pVertex[nIndex2+3].pos =  vViewUp * fHalfSizeY + vViewRight * fHalfSizeX;

				for(int i=0; i<4; i++)
				{
					pVertex[nIndex2+i].pos = pVertex[nIndex2+i].pos + pElm->vPos;
				}// for

				switch( pElm->nTextureStep )
				{
				case 0:
					pVertex[nIndex2+0].tex.u = 0;
					pVertex[nIndex2+0].tex.v = 1;
					pVertex[nIndex2+1].tex.u = 0;
					pVertex[nIndex2+1].tex.v = 0;
					pVertex[nIndex2+2].tex.u = 0.25f;
					pVertex[nIndex2+2].tex.v = 1;
					pVertex[nIndex2+3].tex.u = 0.25f;
					pVertex[nIndex2+3].tex.v = 0;

					for(i=0; i<4; i++)
                        pVertex[nIndex2+i].diffuse = D3DCOLOR_ARGB( 204, 204, 204, 204 );
					break;
				case 1:
					pVertex[nIndex2+0].tex.u = 0.25f;
					pVertex[nIndex2+0].tex.v = 1;
					pVertex[nIndex2+1].tex.u = 0.25f;
					pVertex[nIndex2+1].tex.v = 0;
					pVertex[nIndex2+2].tex.u = 0.5f;
					pVertex[nIndex2+2].tex.v = 1;
					pVertex[nIndex2+3].tex.u = 0.5f;
					pVertex[nIndex2+3].tex.v = 0;

					for(i=0; i<4; i++)
                        pVertex[nIndex2+i].diffuse = D3DCOLOR_ARGB( 166, 166, 166, 166 );
					break;
				case 2:
					pVertex[nIndex2+0].tex.u = 0.5f;
					pVertex[nIndex2+0].tex.v = 1;
					pVertex[nIndex2+1].tex.u = 0.5f;
					pVertex[nIndex2+1].tex.v = 0;
					pVertex[nIndex2+2].tex.u = 0.75f;
					pVertex[nIndex2+2].tex.v = 1;
					pVertex[nIndex2+3].tex.u = 0.75f;
					pVertex[nIndex2+3].tex.v = 0;

					for(i=0; i<4; i++)
                        pVertex[nIndex2+i].diffuse = D3DCOLOR_ARGB( 115, 115, 115, 115 );
					break;
				case 3:
					pVertex[nIndex2+0].tex.u = 0.75f;
					pVertex[nIndex2+0].tex.v = 1;
					pVertex[nIndex2+1].tex.u = 0.75f;
					pVertex[nIndex2+1].tex.v = 0;
					pVertex[nIndex2+2].tex.u = 1;
					pVertex[nIndex2+2].tex.v = 1;
					pVertex[nIndex2+3].tex.u = 1;
					pVertex[nIndex2+3].tex.v = 0;

					for(i=0; i<4; i++)
                        pVertex[nIndex2+i].diffuse = D3DCOLOR_ARGB( 64, 64, 64, 64 );
					break;
				};// switch

				// index
				WORD* pIndex = &pIndices[m_nRainSurfaceCount*6];
				pIndex[0] = nIndex2 + 0;
				pIndex[1] = nIndex2 + 1;
				pIndex[2] = nIndex2 + 2;
				pIndex[3] = nIndex2 + 1;
				pIndex[4] = nIndex2 + 3;
				pIndex[5] = nIndex2 + 2;

				m_nRainSurfaceCount++;
			}// for

			m_VB2->Unlock();
			m_IB2->Unlock();
		}// if

	}

	XIAHGE_API void CRainSnow::Render()
	{
		if( m_nType == eNone ) return;
		if( m_nTotalCount == 0 ) return;

		// render
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );//D3DBLEND_ONE
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		g_pDirect3DDevice->SetRenderState( D3DRS_BLENDOP,	D3DBLENDOP_ADD );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);

		if( m_nType == eRain )
			g_Device.SetTexture(0, m_pRainTexture);
            //g_pDirect3DDevice->SetTexture( 0, m_pRainTexture );
		else if( m_nType == eSnow )
			g_Device.SetTexture(0, m_pSnowTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pSnowTexture );

		g_Device.SetFVF(D3DFVF_LVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );
		Matrix4x4 matWorld;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&matWorld );

		//
		if( m_nRainSnowCount && m_nRainSnowCount <= 250)
		{
			g_Device.SetStreamSource( m_VB, sizeof(VT_LVertex) );
			g_Device.SetIndices( m_IB );
			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nRainSnowCount*4, 0, m_nRainSnowCount*2 );
		}
		else
		if( m_nRainSnowCount > 250)
		{
			g_Device.SetStreamSource( m_VB, sizeof(VT_LVertex) );
			g_Device.SetIndices( m_IB );
			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 1000, 0, 500 );

			g_Device.SetStreamSource( m_VB_s, sizeof(VT_LVertex) );
			g_Device.SetIndices( m_IB_s );
			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, (m_nRainSnowCount-250)*4, 0, (m_nRainSnowCount-250)*2 );
		}

		// RainSurface
		if( m_nType == eRain && m_nRainSurfaceTotalCount )
		{
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );//D3DBLEND_ONE
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			g_Device.SetTexture(0, m_pRainSurfaceTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pRainSurfaceTexture );

			g_Device.SetStreamSource( m_VB2, sizeof(VT_LVertex) );
			g_Device.SetIndices( m_IB2 );
			g_Device.SetFVF(D3DFVF_LVERTEX);
			//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );
			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nRainSurfaceCount*4, 0, m_nRainSurfaceCount*2 );
		}// if

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}

};










