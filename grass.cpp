#include "stdafx.h"
#include "grass.h"
#include "XiahPak.h"


namespace XiahGameEngine
{

    //---------------------------------------------------------
	XIAHGE_API CGrass g_Grass;

	XIAHGE_API CGrass::CGrass()
	{
		m_pVB = NULL;
		m_pIB = NULL;
		m_pGrassTexture = NULL;
		m_nCount = 0;
		m_bGrassAdded = false;
		m_pVertex = NULL;
	}

	XIAHGE_API CGrass::~CGrass()
	{
		Release();
	}

	XIAHGE_API BOOL CGrass::Release()
	{
		if( m_pVB ) m_pVB->Release();
		if( m_pIB ) m_pIB->Release();
		m_pVB = NULL;
		m_pIB = NULL;

		m_pGrassTexture = NULL;

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::ClearAllGrass()
	{
		m_nCount = 0;
		m_bGrassAdded = false;

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::Init()
	{
		// vertex buffer
        g_pDirect3DDevice->CreateVertexBuffer( GRASS_MAX*4*sizeof(VT_LVertex), 0,
												D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pVB, NULL );

        g_pDirect3DDevice->CreateIndexBuffer( GRASS_MAX*4*3*sizeof(WORD), 0,
											   D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

		// texture
		m_pGrassTexture = XiahPak::GetTexture( 50000938 );

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::VBLock()
	{
		m_pVB->Lock( 0, 0, (void**)&m_pVertex, 0 );

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::VBUnlock()
	{
		m_pVB->Unlock();
		m_pVertex = NULL;

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::AddGrass(int nIndex, Vector3 vStart, Vector3 vEnd, float fHeight)
	{
		if( m_nCount == GRASS_MAX-1 ) return FALSE;
		if( m_pVertex == NULL ) return FALSE;

		Vector3 vV = (vStart-vEnd);

		m_vCenterPos[m_nCount]	= (vStart+vEnd) / 2.0f;
		m_fHeight[m_nCount]		= fHeight;
		m_fHalfWidth[m_nCount]	= vV.GetLength() / 2.0f;

/*
		// make vertex. 이 함수를 한번 호출할때 마다 Lock을 함. 좀... ㅡ,.ㅡ^;
		VT_LVertex* pVertex;
		m_pVB->Lock( 0, 0, (void**)&pVertex, 0 );

*/
        int nVertexIndex = m_nCount * 4;

		// 텍스쳐는 8등분으로 되어 있다. nIndex : 0 - 7
		float fStep = 1.0f / 4.0f;
		int nX = nIndex % 4;
		int nY = nIndex / 4;

		m_pVertex[ nVertexIndex+0 ].tex = Vector2( nX*fStep,			nY*0.5f + 0.5f );
		m_pVertex[ nVertexIndex+1 ].tex = Vector2( nX*fStep,			nY*0.5f );
		m_pVertex[ nVertexIndex+2 ].tex = Vector2( nX*fStep + fStep,	nY*0.5f + 0.5f );
		m_pVertex[ nVertexIndex+3 ].tex = Vector2( nX*fStep + fStep,	nY*0.5f );

		for(int i=0; i<4; i++)
            m_pVertex[ nVertexIndex+i ].diffuse = D3DCOLOR_XRGB( 255, 255, 255 );

/*
		m_pVB->Unlock();
*/

		//
		m_nCount++;
		m_bGrassAdded = true;

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::Update()
	{
		if( m_nCount == 0 ) return TRUE;

		D3DMATRIX matView;
		g_pDirect3DDevice->GetTransform( D3DTS_VIEW, &matView );
		Vector3 vView		= Vector3( matView._13, matView._23, matView._33 );
		Vector3 vViewRight	= Vector3( matView._11, matView._21, matView._31 );
		Vector3 vViewUp		= Vector3( matView._12, matView._22, matView._32 );

		// 빌보드로 만들어준다.
		VT_LVertex* pVertex;
		m_pVB->Lock( 0, 0, (void**)&pVertex, 0 );

		for(int nCount=0; nCount<m_nCount; nCount++)
		{
			int nVertexIndex = nCount * 4;

			pVertex[ nVertexIndex+0 ].pos = m_vCenterPos[nCount] + (-vViewRight * m_fHalfWidth[nCount]);
			pVertex[ nVertexIndex+1 ].pos = m_vCenterPos[nCount] + (-vViewRight * m_fHalfWidth[nCount]);
			pVertex[ nVertexIndex+2 ].pos = m_vCenterPos[nCount] + ( vViewRight * m_fHalfWidth[nCount]);
			pVertex[ nVertexIndex+3 ].pos = m_vCenterPos[nCount] + ( vViewRight * m_fHalfWidth[nCount]);

			pVertex[ nVertexIndex+1 ].pos.y += m_fHeight[nCount];
			pVertex[ nVertexIndex+3 ].pos.y += m_fHeight[nCount];
		}// for

		m_pVB->Unlock();

		// index buffer
		if( m_bGrassAdded )
		{
			WORD* pIndices;
			m_pIB->Lock( 0, 0, (void**)&pIndices, 0 );
			for(int i=0; i<m_nCount; i++)
			{
				int nIndex = i * 4;

				WORD* pIndex = &pIndices[ i*6 ];
				pIndex[0] = nIndex + 0;
				pIndex[1] = nIndex + 1;
				pIndex[2] = nIndex + 2;
				pIndex[3] = nIndex + 1;
				pIndex[4] = nIndex + 3;
				pIndex[5] = nIndex + 2;
			}

			m_pIB->Unlock();

			m_bGrassAdded = false;
		}

		return TRUE;
	}

	XIAHGE_API BOOL CGrass::Render()
	{
		if( m_nCount == 0 ) return TRUE;

		Matrix4x4 matWorld;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&matWorld );

		g_Device.SetTexture(0, m_pGrassTexture);
		//g_pDirect3DDevice->SetTexture( 0, m_pGrassTexture );
		g_Device.SetFVF(D3DFVF_LVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );
		g_Device.SetStreamSource( m_pVB, sizeof(VT_LVertex) );
		g_Device.SetIndices( m_pIB );
		g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nCount*4, 0, m_nCount*2 );

		return TRUE;
	}

};
