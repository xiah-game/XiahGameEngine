#include "stdafx.h"
#include "D3DDevice.h"
#include "SwordTrace.h"

#define	SWORDTRACE_CREATEDELAY	10

namespace XiahGameEngine
{

	//-----------------------------------------------------------
	XIAHGE_API CSwordTrace::CSwordTrace()
	{
		m_VB = NULL;
		m_IB = NULL;
		m_nCurrentIndex = -1;
		m_nRenderVertexIndex = 0;
		m_bStart = false;
		m_bEnd = true;
		m_bIsVisible = FALSE;
	}

	XIAHGE_API CSwordTrace::~CSwordTrace()
	{
		Release();
	}

	XIAHGE_API void CSwordTrace::Release()
	{
		if( m_VB ) m_VB->Release();
		if( m_IB ) m_IB->Release();
		m_VB = NULL;
		m_IB = NULL;
	}

	XIAHGE_API BOOL CSwordTrace::Init()
	{
		// vertex and Index buffer 
        if( FAILED( g_pDirect3DDevice->CreateVertexBuffer( SWORDTRACE_MAX*2*sizeof(VT_LVertex),
											0, D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_VB, NULL ) ) )
		return FALSE;

        if( FAILED( g_pDirect3DDevice->CreateIndexBuffer( SWORDTRACE_MAX*2*3*sizeof(WORD),
											0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IB, NULL ) ) )
		return FALSE;

		m_bIsVisible = FALSE;

		return TRUE;
	}

	XIAHGE_API bool CSwordTrace::IsStart()
	{
		return m_bStart;
	}

	XIAHGE_API bool CSwordTrace::IsEnd()
	{
		return m_bEnd;
	}

	XIAHGE_API void CSwordTrace::Start(Vector3 vStart, Vector3 vEnd, int nR, int nG, int nB)
	{
		m_bStart = true;
		m_bEnd = false;
		m_dwElapsedTime = 0;
		m_dwPrevSpawnTime = 0;
		m_nCurrentIndex = -1;
		m_nRenderVertexIndex = 0;

		m_vStartPosArray[ ++m_nCurrentIndex ] = vStart;
		m_vEndPosArray[ m_nCurrentIndex ] = vEnd;

		m_nR = nR;
		m_nG = nG;
		m_nB = nB;
		m_nAlphaArray[ m_nCurrentIndex ] = 128;
	}

	XIAHGE_API void CSwordTrace::End()
	{
		m_bEnd = true;
	}

	XIAHGE_API void CSwordTrace::SetVisible(BOOL bVisible)
	{
		m_bIsVisible = bVisible;
	}

	XIAHGE_API void CSwordTrace::Update(DWORD dwTime,Vector3 vStart, Vector3 vEnd)
	{
		if( m_nCurrentIndex == -1 ) return;

		m_dwElapsedTime += dwTime;

		// 시간이 지나면 색깔이 투명해진다.
		float fTime = (float)dwTime / 1000;
		fTime *= 800.0f;
		for(int i=0; i<=m_nCurrentIndex; i++)
		{
			m_nAlphaArray[i] -= (DWORD)fTime;
			if( m_nAlphaArray[i] < 0 )
				m_nAlphaArray[i] = 0;
		}

		//
		if( !m_bEnd )
		if( m_dwElapsedTime - m_dwPrevSpawnTime > SWORDTRACE_CREATEDELAY ) // Create
		{
			m_dwPrevSpawnTime = m_dwElapsedTime;

			if( m_nCurrentIndex < SWORDTRACE_MAX-1 )
			{
				m_vStartPosArray[ ++m_nCurrentIndex ] = vStart;
				m_vEndPosArray[ m_nCurrentIndex ] = vEnd;
				m_nAlphaArray[ m_nCurrentIndex ] = 128;
			}// if
		}// if

		if( m_nAlphaArray[m_nCurrentIndex] == 0 )
		{
			m_bStart = false;
			m_nCurrentIndex = -1;
		}

		// 
		if( m_nCurrentIndex >= 1 && m_bIsVisible )
		{
			// make vertex and Index buffer
			VT_LVertex* pVertex;
			m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

			WORD* pIndices;
			m_IB->Lock( 0, 0, (void**)&pIndices, 0 );

			int nVertexIndex;
			m_nRenderVertexIndex = 0;
			for(int i=0; i<m_nCurrentIndex; i++)
			{
				Vector3 vS0 = m_vStartPosArray[i];
				Vector3 vE0 = m_vEndPosArray[i];
				Vector3 vS1 = m_vStartPosArray[i+1];
				Vector3 vE1 = m_vEndPosArray[i+1];

				// 이룬, 캐릭터가 점프를 하네, 그럼 위치가 너무 차이가 많이 나는 것들은 제외시킨다.
				Vector3 vDist = vE0 - vE1;
				float fDist = vDist.GetLength();

//				DBG_Put("vE0(%6.2f,%6.2f,%6.2f) vE1(%6.2f,%6.2f,%6.2f) fDist=%6.2f", 
//						vE0.x, vE0.y, vE0.z, vE1.x, vE1.y, vE1.z, fDist );

				if( fDist >= 24.0f )	// 길이 차이가 많이 나면 건너뛴다. 이건 테스트에의한 값.^^
				{
//					DBG_Put("sword skipped");

					m_nRenderVertexIndex = 0;
					continue;
				}

				nVertexIndex = m_nRenderVertexIndex * 4;

				pVertex[nVertexIndex+0].pos = vS0;
				pVertex[nVertexIndex+1].pos = vE0;
				pVertex[nVertexIndex+2].pos = vS1;
				pVertex[nVertexIndex+3].pos = vE1;

				pVertex[nVertexIndex+0].diffuse = D3DCOLOR_ARGB( m_nAlphaArray[i],   m_nR, m_nG, m_nB );
				pVertex[nVertexIndex+1].diffuse = D3DCOLOR_ARGB( m_nAlphaArray[i],   m_nR, m_nG, m_nB );
				pVertex[nVertexIndex+2].diffuse = D3DCOLOR_ARGB( m_nAlphaArray[i+1], m_nR, m_nG, m_nB );
				pVertex[nVertexIndex+3].diffuse = D3DCOLOR_ARGB( m_nAlphaArray[i+1], m_nR, m_nG, m_nB );

				pVertex[nVertexIndex+0].tex.u = 0;
				pVertex[nVertexIndex+0].tex.v = 1;
				pVertex[nVertexIndex+1].tex.u = 0;
				pVertex[nVertexIndex+1].tex.v = 0;
				pVertex[nVertexIndex+2].tex.u = 1;
				pVertex[nVertexIndex+2].tex.v = 1;
				pVertex[nVertexIndex+3].tex.u = 1;
				pVertex[nVertexIndex+3].tex.v = 0;

				// index
				WORD* pIndex = &pIndices[i*6];
				pIndex[0] = nVertexIndex + 0;
				pIndex[1] = nVertexIndex + 1;
				pIndex[2] = nVertexIndex + 2;
				pIndex[3] = nVertexIndex + 1;
				pIndex[4] = nVertexIndex + 3;
				pIndex[5] = nVertexIndex + 2;

				if( m_nRenderVertexIndex == 249 ) break;

				m_nRenderVertexIndex++;
			}// for(m_nCurrentIndex)
			m_VB->Unlock();
			m_IB->Unlock();
		}// if


/*
		// test
		DBG_Put("\n m_nCurrentIndex=%d, m_nAlphaArray[0]=%d", m_nCurrentIndex, m_nAlphaArray[0]);
*/

	}

	XIAHGE_API void CSwordTrace::Render()
	{
		if( m_nCurrentIndex < 1 ) return;
		if( m_nRenderVertexIndex == 0 ) return;
		if( !m_bIsVisible ) return;

		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );//D3DBLEND_ONE
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_pDirect3DDevice->SetRenderState( D3DRS_BLENDOP,	D3DBLENDOP_ADD );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		g_Device.SetTexture(0, NULL);
		//g_pDirect3DDevice->SetTexture( 0, NULL );

		g_Device.SetStreamSource( m_VB, sizeof(VT_LVertex) );
		g_Device.SetIndices( m_IB );
		g_Device.SetFVF(D3DFVF_LVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );

		Matrix4x4 matWorld;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&matWorld );
		g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nRenderVertexIndex*4, 0, m_nRenderVertexIndex*2 );

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}
    //-----------------------------------------------------------

};
