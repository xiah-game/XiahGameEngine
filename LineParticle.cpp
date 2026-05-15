#include "stdafx.h"
#include "D3DDevice.h"
#include "LineParticle.h"

#define		CREATE_DELAYTIME	50
#define		LIFETIME			100

namespace XiahGameEngine
{
	//--------------------------------------------------------------------
	XIAHGE_API CLineParticle::CLineParticle()
	{
		m_VB = NULL;
		m_IB = NULL;
		m_nPosIndex = -1;
		m_bFinished = false;
		m_nLineCount = 0;
	}

	XIAHGE_API CLineParticle::~CLineParticle()
	{
		Release();
	}
	
	void CLineParticle::Release()
	{
		if( m_VB ) m_VB->Release();
		if( m_IB ) m_IB->Release();
		m_VB = NULL;
		m_IB = NULL;
	}

	XIAHGE_API BOOL CLineParticle::Init()
	{
		// vertex and Index buffer 
        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( LINEPARTICLE_MAX*4*sizeof(VT_LVertex),
											0, D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_VB, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( LINEPARTICLE_MAX*4*3*sizeof(WORD),
											0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IB, NULL ) ) )
		return FALSE;

		return TRUE;
	}

	XIAHGE_API void CLineParticle::Start(Vector3 vPos)
	{
		m_bFinished = false;
		m_bEnd = false;
		m_dwElapsedTime = 0;
		m_dwPrevSpawnTime = 0;
		m_nPosIndex = -1;
		m_vPosList[ ++m_nPosIndex ] = vPos;
		m_nDiffuseArray[ m_nPosIndex ] = 255;
	}

	XIAHGE_API void CLineParticle::End()
	{
		m_bEnd = true;
	}

	XIAHGE_API bool CLineParticle::IsFinished()
	{
		return m_bFinished;
	}

	XIAHGE_API void CLineParticle::Update(DWORD dwTime, Vector3 vPos)
	{
		if( m_nPosIndex == -1) return;

		// time
		m_dwElapsedTime += dwTime;

		float fTime = (float)dwTime / 1000;
		int nDecrease = 600 * fTime;

		for(int i=0; i<=m_nPosIndex; i++)
		{
			// Åõ¸íµµ
			m_nDiffuseArray[i] -= nDecrease;
			if( m_nDiffuseArray[i] <= 0 )
				m_nDiffuseArray[i] = 0;
		}

		// create
		if( !m_bEnd )
		if( m_dwElapsedTime - m_dwPrevSpawnTime >= CREATE_DELAYTIME )
		{
			m_dwPrevSpawnTime = m_dwElapsedTime;

			if( m_nPosIndex < LINEPARTICLE_MAX-1 )
			{
                m_vPosList[ ++m_nPosIndex ] = vPos;
				m_nDiffuseArray[ m_nPosIndex ] = 255;
			}
		}// if

		if( m_nDiffuseArray[m_nPosIndex] == 0 )
			m_bFinished = true;


		//
		if( m_nPosIndex >= 1)
		{
			// make vertex and Index buffer
			VT_LVertex* pVertex;
			m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

			WORD* pIndices;
			m_IB->Lock( 0, 0, (void**)&pIndices, 0 );

			// make line particle fro two position
			m_nLineCount = 0;
			float fHalfSizeY = 0.07f;
			for(int i=0; i<m_nPosIndex; i++)
			{
				Vector3 vPos1 = m_vPosList[i+1];	// start
				Vector3 vPos2 = m_vPosList[i];		// end

				Vector3 vP1 = vPos1 - vPos2;
				Vector3 vP2 = vPos2 - g_pCurrentCamera->m_vFrom;

				Vector3 vNewUp = vP1.Cross( vP2 );
				vNewUp.Normalize();

				int nIndex = m_nLineCount * 4;

				pVertex[nIndex+0].pos = vPos1 - vNewUp * fHalfSizeY;
				pVertex[nIndex+1].pos = vPos1 + vNewUp * fHalfSizeY;
				pVertex[nIndex+2].pos = vPos2 - vNewUp * fHalfSizeY;
				pVertex[nIndex+3].pos = vPos2 + vNewUp * fHalfSizeY;

				int nValue = m_nDiffuseArray[i];
				for(int j=0; j<4; j++)
				{
					pVertex[nIndex+j].diffuse = D3DCOLOR_ARGB( nValue, nValue, nValue, nValue );
				}// for

				pVertex[nIndex+0].tex.u = 0;
				pVertex[nIndex+0].tex.v = 1;
				pVertex[nIndex+1].tex.u = 0;
				pVertex[nIndex+1].tex.v = 0;
				pVertex[nIndex+2].tex.u = 1;
				pVertex[nIndex+2].tex.v = 1;
				pVertex[nIndex+3].tex.u = 1;
				pVertex[nIndex+3].tex.v = 0;

				// index
				WORD* pIndex = &pIndices[m_nLineCount*6];
				pIndex[0] = nIndex + 0;
				pIndex[1] = nIndex + 1;
				pIndex[2] = nIndex + 2;
				pIndex[3] = nIndex + 1;
				pIndex[4] = nIndex + 3;
				pIndex[5] = nIndex + 2;

				m_nLineCount++;
			}// for

			m_VB->Unlock();
			m_IB->Unlock();
		}// if


/*
		// test
		char szChar[200];
		_stprintf( szChar, "\n m_nPosIndex=%d, m_nDiffuseArray[0]=%d", 
							m_nPosIndex, m_nDiffuseArray[0] );
		OutputDebugString( szChar );
*/

	}

	XIAHGE_API void CLineParticle::Render()
	{
		if( m_nPosIndex < 1) return;

		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );//D3DBLEND_ONE
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		g_pDirect3DDevice->SetRenderState( D3DRS_BLENDOP,	D3DBLENDOP_ADD );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);

		g_Device.SetTexture(0, NULL);
		//g_pDirect3DDevice->SetTexture( 0, NULL );

		g_Device.SetStreamSource( m_VB, sizeof(VT_LVertex) );
		g_Device.SetIndices( m_IB );
		g_Device.SetFVF(D3DFVF_LVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );

		Matrix4x4 matWorld;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&matWorld );
		g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nLineCount*4, 0, m_nLineCount*2 );

		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	}
	//--------------------------------------------------------------------

};
