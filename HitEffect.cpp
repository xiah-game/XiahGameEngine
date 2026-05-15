#include "stdafx.h"
#include "HitEffect.h"
#include "XiahPak.h"

#define		HITEFFECT_LIFETIME	1000

namespace XiahGameEngine
{

	//-----------------------------------------------
	XIAHGE_API CHitEffect g_HitEffect;

	XIAHGE_API CHitEffect::CHitEffect()
	{
		m_pVB = NULL;
		m_pIB = NULL;
		m_pHitVB = NULL;
		m_pHitIB = NULL;
		m_pTexture = NULL;
		m_pHitTexture = NULL;
		m_nCount = 0;
		m_nHitCount = 0;
	}

	XIAHGE_API CHitEffect::~CHitEffect()
	{
		Release();
	}

	XIAHGE_API BOOL CHitEffect::Release()
	{
		if( m_pVB ) m_pVB->Release();
		if( m_pIB ) m_pIB->Release();
		if( m_pHitVB ) m_pHitVB->Release();
		if( m_pHitIB ) m_pHitIB->Release();
		m_pVB = NULL;
		m_pIB = NULL;
		m_pHitVB = NULL;
		m_pHitIB = NULL;

/*
		if( m_pTexture ) m_pTexture->Release();
		m_pTexture = NULL;
		if( m_pHitTexture ) m_pHitTexture->Release();
		m_pHitTexture = NULL;
*/

		return TRUE;
	}

	XIAHGE_API BOOL CHitEffect::Init()
	{
		// vertex and Index buffer 
/*
        g_pDirect3DDevice->CreateVertexBuffer( HITEFFECTVERTEX_MAX*4*sizeof(VT_LVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
											D3DFVF_LVERTEX, D3DPOOL_DEFAULT, &m_pVB, NULL );

        g_pDirect3DDevice->CreateIndexBuffer( HITEFFECTVERTEX_MAX*4*3*sizeof(WORD), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
											D3DPOOL_DEFAULT, &m_pIB, NULL );

        g_pDirect3DDevice->CreateVertexBuffer( HITEFFECTVERTEX_MAX*4*sizeof(VT_LVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
											D3DFVF_LVERTEX, D3DPOOL_DEFAULT, &m_pHitVB, NULL );

        g_pDirect3DDevice->CreateIndexBuffer( HITEFFECTVERTEX_MAX*4*3*sizeof(WORD), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
											D3DPOOL_DEFAULT, &m_pHitIB, NULL );
*/
		g_pDirect3DDevice->CreateVertexBuffer( HITEFFECTVERTEX_MAX*4*sizeof(VT_LVertex), D3DUSAGE_WRITEONLY, 
											D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pVB, NULL );

		g_pDirect3DDevice->CreateIndexBuffer( HITEFFECTVERTEX_MAX*4*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
											D3DPOOL_MANAGED, &m_pIB, NULL );

		g_pDirect3DDevice->CreateVertexBuffer( HITEFFECTVERTEX_MAX*4*sizeof(VT_LVertex), D3DUSAGE_WRITEONLY, 
											D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pHitVB, NULL );

		g_pDirect3DDevice->CreateIndexBuffer( HITEFFECTVERTEX_MAX*4*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
											D3DPOOL_MANAGED, &m_pHitIB, NULL );

		// texture
//		D3DXCreateTextureFromFile( g_pDirect3DDevice, "hitall.tga",		&m_pTexture );
//		D3DXCreateTextureFromFile( g_pDirect3DDevice, "hit.tga",		&m_pHitTexture );

		m_pTexture		= XiahPak::GetTexture( 50000499 );
		m_pHitTexture	= XiahPak::GetTexture( 50000500 );

		// data
		for(int i=0; i<HITEFFECT_MAX; i++)
		{
			m_Pool.push_back( &m_PrePool[i] );
			m_HitPool.push_back( &m_HitPrePool[i] );
		}

		m_Table[0] = 10;
		m_Table[1] = 100;
		m_Table[2] = 1000;
		m_Table[3] = 10000;
		m_Table[4] = 100000;
		m_Table[5] = 1000000;
		m_Table[6] = 10000000;
		m_Table[7] = 100000000;
		m_Table[8] = 1000000000;
		return TRUE;
	}

	XIAHGE_API BOOL CHitEffect::AddHitEffect(int nType, DWORD wDamage, Vector3 vPos)//HT_0907 : 공격력 맥스값 변경( 65000을 넘다니 ㅡㅡ; )
	{
		if( nType != 4 )
		{
			if( m_nCount >= HITEFFECT_MAX )
				return FALSE;
		}
		else
		{
			if( m_nHitCount >= HITEFFECT_MAX )
				return FALSE;
		}


		// 메모리 풀에서 데이터를 하나 가져옴.
		sHitEffectData* pData;
		if( nType != 4 )
		{
			pData = m_Pool.front();
			m_Pool.pop_front();
		}
		else	// Hit
		{
			pData = m_HitPool.front();
			m_HitPool.pop_front();
		}

		pData->dwElapsedTime = 0;
		pData->nType = nType;
		pData->vPos = vPos;
		pData->fSize = 1.0f;
		pData->nAlpha = 255;

		if( nType != 4 )
		{
            m_List.push_back( pData );
			m_nCount++;
		}
		else
		{
			m_nHitCount++;
			m_HitList.push_back( pData );
		}

		//
		float fStartV;
		float ftux = 0.2f, ftuy = 1.0f / 6.0f;
		if( nType == 0 || nType == 1 )	// number, 0 is Red and 1 is Blue
		{
			int nFigure;
			int NumberList[16];
			SeparateNumber( wDamage, nFigure, NumberList );//HT_0907 : 공격력 맥스값 변경( 65000을 넘다니 ㅡㅡ; )

			pData->nCount = nFigure;

			// 각 숫자에 해당하는 텍스쳐 좌표를 연결한다.
			if( pData->nType == 0 ) fStartV = 0;
			else fStartV = 0.5f;

			for(int i=0; i<nFigure; i++)
			{
				// 텍스쳐는 5개씩 되어 있다.
				int nX = NumberList[i] % 5;
				int nY = NumberList[i] / 5;

				pData->tex[i].tex[0] = Vector2( nX*ftux,		fStartV+(nY*ftuy) +ftuy );
				pData->tex[i].tex[1] = Vector2( nX*ftux,		fStartV+(nY*ftuy)		);
				pData->tex[i].tex[2] = Vector2( nX*ftux +ftux,	fStartV+(nY*ftuy) +ftuy );
				pData->tex[i].tex[3] = Vector2( nX*ftux +ftux,	fStartV+(nY*ftuy)		);
			}// for
		}
		else 
		if( nType == 2 || nType == 3 )	// MISS, 2 is Red, 3 is Blue
		{
			pData->nCount = 1;

			if( pData->nType == 2 ) fStartV = 0;
			else fStartV = 0.5f;

			pData->tex[0].tex[0] = Vector2( 0,			fStartV+(ftuy*2.0f) +ftuy	);
			pData->tex[0].tex[1] = Vector2( 0,			fStartV+(ftuy*2.0f)			);
			pData->tex[0].tex[2] = Vector2( ftux*4.0f,	fStartV+(ftuy*2.0f) +ftuy	);
			pData->tex[0].tex[3] = Vector2( ftux*4.0f,	fStartV+(ftuy*2.0f)			);
		}
		else
		if( nType == 4 )	// HIT
		{
			pData->nCount = 1;

			pData->tex[0].tex[0] = Vector2( 0, 1 );
			pData->tex[0].tex[1] = Vector2( 0, 0 );
			pData->tex[0].tex[2] = Vector2( 1, 1 );
			pData->tex[0].tex[3] = Vector2( 1, 0 );
		}

		return TRUE;
	}

	BOOL CHitEffect::SeparateNumber(DWORD wNumber,int &nA, int *list)//HT_0907 : 공격력 맥스값 변경( 65000을 넘다니 ㅡㅡ; )
	{
		// 100만 보다 크면 안된다. 최대 99만까지 현재 계산된다.
		// 65500(WORD)를 넘으면 안된다. 
		// Removed 65500 cap


		int nTableIndex = 0;
		nA = 0;

		// 숫자의 자릿수를 계산하기 위해. 
		while(1)
		{
			if( wNumber < m_Table[nTableIndex] ) break;

			if( nTableIndex > DAMAGE_MAX ) break; //m_Table[] 값이 0으로 나오면 안되자낭.. 

			nTableIndex++;
		}

		if( nTableIndex != 0 ) nTableIndex--;

		//  숫자 분류
		while( nTableIndex != -1 )
		{
			int biResult1 = wNumber / m_Table[ nTableIndex ];
			int biResult2 = wNumber % m_Table[ nTableIndex ];

			// 일 단위 숫자에서는 앞에 0을 없애준다.
			if( biResult1 == 0 && nA == 0 )
			{
			}
			else
			{
                list[ nA++ ] = biResult1;
			}

			wNumber = biResult2;
			nTableIndex--;
		}

		list[ nA++ ] = wNumber;

		return TRUE;
	}

	XIAHGE_API BOOL CHitEffect::Update()
	{
		if( m_nCount == 0 && m_nHitCount == 0 ) return TRUE;

		//
		// 1.Update
		//
		D3DMATRIX matView;
		g_pDirect3DDevice->GetTransform( D3DTS_VIEW, &matView );
		Vector3 vView		= Vector3( matView._13, matView._23, matView._33 );
		Vector3 vViewRight	= Vector3( matView._11, matView._21, matView._31 );
		Vector3 vViewUp		= Vector3( matView._12, matView._22, matView._32 );

		//
		DWORD dwTime = 33.0f * g_fFrameScale;
		float fTime = (float)dwTime / 1000;
		float ffY = fTime * 5.0f;
		float fSize = fTime * 2.0f;
		int   nAlpha = fTime * 500;

		HITEFFECTLIST DeleteList;
		HITEFFECTLIST::iterator hit;
		for(hit=m_List.begin(); hit!=m_List.end(); hit++)
		{
			sHitEffectData* pData = *hit;

			pData->dwElapsedTime += dwTime;
			pData->vPos.y += ffY;

			if( pData->dwElapsedTime <= (float)HITEFFECT_LIFETIME * 0.25f )
			{
				pData->fSize += fSize;
			}
			else
			if( pData->dwElapsedTime <= (float)HITEFFECT_LIFETIME * 0.5f )
			{
				pData->fSize -= fSize;
			}
			else
			if( pData->dwElapsedTime <= HITEFFECT_LIFETIME )
			{
				pData->nAlpha -= nAlpha;
				if( pData->nAlpha < 0 )
					pData->nAlpha = 0;
			}
			else
			{
				DeleteList.push_back( pData );
				continue;
			}
		}

		HITEFFECTLIST HitDeleteList;
		for(hit=m_HitList.begin(); hit!=m_HitList.end(); hit++)
		{
			sHitEffectData* pData = *hit;

			pData->dwElapsedTime += dwTime;
			pData->vPos.y += ffY;

			if( pData->dwElapsedTime <= (float)(HITEFFECT_LIFETIME-300) * 0.25f )
			{
				pData->fSize += fSize;
			}
			else
			if( pData->dwElapsedTime <= (float)(HITEFFECT_LIFETIME-300) * 0.5f )
			{
				pData->fSize -= fSize;
			}
			else
			if( pData->dwElapsedTime <= (HITEFFECT_LIFETIME-300) )
			{
				pData->nAlpha -= nAlpha;
				if( pData->nAlpha < 0 )
					pData->nAlpha = 0;
			}
			else
			{
				HitDeleteList.push_back( pData );
				continue;
			}
		}

		// 
		for(hit=DeleteList.begin(); hit!=DeleteList.end(); hit++)
		{
			sHitEffectData* pData = *hit;

			m_Pool.push_back( pData );
			m_List.remove( pData );
			m_nCount--;
		}

		for(hit=HitDeleteList.begin(); hit!=HitDeleteList.end(); hit++)
		{
			sHitEffectData* pData = *hit;

			m_HitPool.push_back( pData );
			m_HitList.remove( pData );
			m_nHitCount--;
		}

		//
		// 2. Make Vertex Data
		//
		float fX = 1.4f, fY = 2.0f;
		m_n4VertexIndex = 0;
		if( m_nCount > 0 )
		{
			VT_LVertex* pVertex;
			m_pVB->Lock( 0, 0, (void**)&pVertex, 0 );

			WORD* pIndices;
			m_pIB->Lock( 0, 0, (void**)&pIndices, 0 );

			for(hit=m_List.begin(); hit!=m_List.end(); hit++)
			{
				sHitEffectData* pData = *hit;

				switch( pData->nType )
				{
				case 0:
				case 1:
					{
						Vector3 vStartPos = pData->vPos;
						if( pData->fSize != 1.0f )
                            vStartPos += ((-vViewRight)*pData->fSize*0.5f);

						for(int i=0; i<pData->nCount; i++)
						{
							int nVertexIndex = m_n4VertexIndex * 4;

							pVertex[ nVertexIndex+0 ].pos = vStartPos;
							pVertex[ nVertexIndex+1 ].pos = vStartPos;
							pVertex[ nVertexIndex+1 ].pos.y += (fY * pData->fSize);

							vStartPos += (vViewRight * fX * pData->fSize);

							pVertex[ nVertexIndex+2 ].pos = vStartPos;
							pVertex[ nVertexIndex+3 ].pos = vStartPos;
							pVertex[ nVertexIndex+3 ].pos.y += (fY * pData->fSize);

							for(int j=0; j<4; j++)
							{
								pVertex[ nVertexIndex+j ].tex = pData->tex[i].tex[j];
								pVertex[ nVertexIndex+j ].diffuse = D3DCOLOR_ARGB( pData->nAlpha, 250, 250, 250 );
							}

							// index
							WORD* pIndex = &pIndices[m_n4VertexIndex*6];
							pIndex[0] = nVertexIndex + 0;
							pIndex[1] = nVertexIndex + 1;
							pIndex[2] = nVertexIndex + 2;
							pIndex[3] = nVertexIndex + 1;
							pIndex[4] = nVertexIndex + 3;
							pIndex[5] = nVertexIndex + 2;

							//
							m_n4VertexIndex++;
							if( m_n4VertexIndex >= HITEFFECTVERTEX_MAX )
								break;

							vStartPos += ((-vViewRight)*0.3f*pData->fSize);
						}// for
					}
					break;
				case 2:		// MISS
				case 3:
					{
						int nVertexIndex = m_n4VertexIndex * 4;

						Vector3 vStartPos = pData->vPos;
						if( pData->fSize != 1.0f )
                            vStartPos += ((-vViewRight)*pData->fSize*2.0f);

						pVertex[ nVertexIndex+0 ].pos = vStartPos;
						pVertex[ nVertexIndex+1 ].pos = vStartPos;
						pVertex[ nVertexIndex+1 ].pos.y += (fY * pData->fSize);

						vStartPos += (vViewRight * fX * 4 * pData->fSize);

						pVertex[ nVertexIndex+2 ].pos = vStartPos;
						pVertex[ nVertexIndex+3 ].pos = vStartPos;
						pVertex[ nVertexIndex+3 ].pos.y += (fY * pData->fSize);

						for(int i=0; i<4; i++)
						{
							pVertex[ nVertexIndex+i ].tex = pData->tex[0].tex[i];
							pVertex[ nVertexIndex+i ].diffuse = D3DCOLOR_ARGB( pData->nAlpha, 250, 250, 250 );
						}

						// index
						WORD* pIndex = &pIndices[m_n4VertexIndex*6];
						pIndex[0] = nVertexIndex + 0;
						pIndex[1] = nVertexIndex + 1;
						pIndex[2] = nVertexIndex + 2;
						pIndex[3] = nVertexIndex + 1;
						pIndex[4] = nVertexIndex + 3;
						pIndex[5] = nVertexIndex + 2;

						//
						m_n4VertexIndex++;
						if( m_n4VertexIndex >= HITEFFECTVERTEX_MAX )
							break;
					}
					break;
				};// switch
			}// for

			// test
			//DBG_Put("\n m_List.size()=%d, nVertexIndex=%d", m_List.size(), n4VertexIndex*4);
			// test

			m_pVB->Unlock();
			m_pIB->Unlock();
		}// if( m_nCount )

		m_n4VertexIndexHit = 0;
		if( m_nHitCount > 0 )
		{
			float fX = 2.2f, fY = 2.6f;

			VT_LVertex* pVertex;
			m_pHitVB->Lock( 0, 0, (void**)&pVertex, 0 );

			WORD* pIndices;
			m_pHitIB->Lock( 0, 0, (void**)&pIndices, 0 );

			for(hit=m_HitList.begin(); hit!=m_HitList.end(); hit++)
			{
				sHitEffectData* pData = *hit;

				int nVertexIndex = m_n4VertexIndexHit * 4;

				Vector3 vStartPos = pData->vPos;
				if( pData->fSize != 1.0f )
					vStartPos += ((-vViewRight)*pData->fSize*1.5f);

				pVertex[ nVertexIndex+0 ].pos = vStartPos;
				pVertex[ nVertexIndex+1 ].pos = vStartPos;
				pVertex[ nVertexIndex+1 ].pos.y += (fY * pData->fSize);

				vStartPos += (vViewRight * fX * 3 * pData->fSize);

				pVertex[ nVertexIndex+2 ].pos = vStartPos;
				pVertex[ nVertexIndex+3 ].pos = vStartPos;
				pVertex[ nVertexIndex+3 ].pos.y += (fY * pData->fSize);

				for(int i=0; i<4; i++)
				{
					pVertex[ nVertexIndex+i ].tex = pData->tex[0].tex[i];
					pVertex[ nVertexIndex+i ].diffuse = D3DCOLOR_ARGB( pData->nAlpha, 250, 250, 250 );
				}

				// index
				WORD* pIndex = &pIndices[m_n4VertexIndexHit*6];
				pIndex[0] = nVertexIndex + 0;
				pIndex[1] = nVertexIndex + 1;
				pIndex[2] = nVertexIndex + 2;
				pIndex[3] = nVertexIndex + 1;
				pIndex[4] = nVertexIndex + 3;
				pIndex[5] = nVertexIndex + 2;

				//
				m_n4VertexIndexHit++;
				if( m_n4VertexIndexHit >= HITEFFECTVERTEX_MAX )
					break;
			}// for

			// test
//			DBG_Put("\n m_HitList.size()=%d, nVertexIndex=%d", m_HitList.size(), n4VertexIndexHit*4);
			// test

			m_pHitVB->Unlock();
			m_pHitIB->Unlock();
		}// if(m_nHitCount)

		return TRUE;
	}

	XIAHGE_API BOOL CHitEffect::Render()
	{
		if( m_nCount == 0 && m_nHitCount == 0 ) return TRUE;

		//
		// 3. Render
		//
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );//D3DBLEND_ONE
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE);

		Matrix4x4 matWorld;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&matWorld );

		g_Device.SetFVF(D3DFVF_LVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );

		if( m_n4VertexIndex > 0 )
		{
			g_Device.SetTexture(0, m_pTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pTexture );
			g_Device.SetStreamSource( m_pVB, sizeof(VT_LVertex) );
			g_Device.SetIndices( m_pIB );
			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_n4VertexIndex*4, 0, m_n4VertexIndex*2 );
		}

		if( m_n4VertexIndexHit > 0 )
		{
			g_Device.SetTexture(0, m_pHitTexture);
			//g_pDirect3DDevice->SetTexture( 0, m_pHitTexture );
			g_Device.SetStreamSource( m_pHitVB, sizeof(VT_LVertex) );
			g_Device.SetIndices( m_pHitIB );
			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_n4VertexIndexHit*4, 0, m_n4VertexIndexHit*2 );
		}

		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		return TRUE;
	}


};