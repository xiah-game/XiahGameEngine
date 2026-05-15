// Thunder.cpp: implementation of the CThunder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thunder.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


namespace XiahGameEngine
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
//CThunder::CThunder()
//{
///*
//	m_pThunderSumVB = NULL;
//	m_pThunderSumIB = NULL;
//	m_pThunderVB = NULL;
//	m_pThunderTexture = NULL;
//	m_pThunderVertex = NULL;
//	m_pd3dDevice = NULL;
//	m_TexturePath = _T("");
//	m_bPlay = false;
//*/
//}
//
//CThunder::~CThunder()
//{
////	Release();
//}
/*
void CThunder::Release()
{
	if( m_pThunderSumVB ) m_pThunderSumVB->Release();
	m_pThunderSumVB = NULL;

	if( m_pThunderSumIB ) m_pThunderSumIB->Release();
	m_pThunderSumIB = NULL;

	if( m_pThunderVB ) m_pThunderVB->Release();
	m_pThunderVB = NULL;

	if( m_pThunderTexture ) m_pThunderTexture->Release();
	m_pThunderTexture = NULL;

	if( m_pThunderVertex ) delete []m_pThunderVertex;
	m_pThunderVertex = NULL;
}

void CThunder::Init(int x, int y, int z, int Ratio, int BodyDivision, int SubBody, int SubBodyDivision, int DrawGap, int Width)
{
	CEffectEditorView *pView = (CEffectEditorView *)(((CFrameWnd *)AfxGetMainWnd())->GetActiveView());
	m_pd3dDevice = pView->m_D3D.GetDevice();

	// vertex buffer
	m_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
									  0, D3DFVF_CUSTOMVERTEX,
									  D3DPOOL_DEFAULT, &m_pThunderVB );

	// optimize thunder vertex buffer
	m_pd3dDevice->CreateVertexBuffer( THUNDER_VERTEX_MAX*4*sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pThunderSumVB );

	// optimize thunder index buffer
	m_pd3dDevice->CreateIndexBuffer( THUNDER_VERTEX_MAX*4*3*sizeof(CUSTOMVERTEX),
									0, D3DFMT_INDEX16,
									D3DPOOL_DEFAULT, &m_pThunderSumIB );

	Setting(x, y, z, Ratio, BodyDivision, SubBody, SubBodyDivision, DrawGap, Width);
}

void CThunder::Setting(int x, int y, int z, int Ratio, int BodyDivision, int SubBody, int SubBodyDivision, int DrawGap, int Width)
{
	m_nPosX = x;
	m_nPosY = y;
	m_nPosZ = z;
	m_nBlokenRatio = Ratio;
	m_fBlokenRatio = m_nBlokenRatio * 0.2f;
	m_nBodyDivision = BodyDivision;
	m_nSubBodyNumber = SubBody;
	m_nSubBodyDivision = SubBodyDivision;
	m_dwDrawTime = 0;
	m_dwDrawGap = DrawGap;
	m_nWidth = Width;

	if( m_pThunderVertex ) delete []m_pThunderVertex;
	m_pThunderVertex = NULL;

	m_pThunderVertex = new THUNDERVERTEX[m_nBodyDivision + m_nSubBodyNumber * m_nSubBodyDivision];
}

void CThunder::CreateThunder()
{
	float fX, fY, fZ;
	int nThunderIndex = -1;
	D3DXVECTOR3 vStartPos = D3DXVECTOR3( m_nPosX, m_nPosY, m_nPosZ );
	D3DXVECTOR3 vPrevPos = vStartPos;

	// Up position is Start and Down position is End
	for(int i=0; i<m_nBodyDivision; i++)	// Make Body
	{
		fX = m_fBlokenRatio * rnd();
		if( rand() % 2 ) fX = -fX;
		fY = -((float)m_nPosY / (float)m_nBodyDivision);
		fZ = m_fBlokenRatio * rnd();
		if( rand() % 2 ) fZ = -fZ;

		m_pThunderVertex[++nThunderIndex].vStart= vPrevPos;
		m_pThunderVertex[nThunderIndex].vEnd	= D3DXVECTOR3( m_pThunderVertex[nThunderIndex].vStart.x+fX, m_pThunderVertex[nThunderIndex].vStart.y+fY, m_pThunderVertex[nThunderIndex].vStart.z+fZ );

		vPrevPos = m_pThunderVertex[nThunderIndex].vEnd;
	}// for(i)

	int nSubBodyStartIndex = m_nBodyDivision / (m_nSubBodyNumber+1);
	vPrevPos = m_pThunderVertex[nSubBodyStartIndex].vEnd;
	for(i=0; i<m_nSubBodyNumber; i++)		// Make SubBody
	{
		for(int j=0; j<m_nSubBodyDivision; j++)
		{
			fX = m_fBlokenRatio * rnd();
			if( rand() % 2 ) fX = -fX;
			fY = -((float)m_nPosY / (float)m_nBodyDivision);
			fZ = m_fBlokenRatio * rnd();
			if( rand() % 2 ) fZ = -fZ;

			m_pThunderVertex[++nThunderIndex].vStart= vPrevPos;
			m_pThunderVertex[nThunderIndex].vEnd	= D3DXVECTOR3( m_pThunderVertex[nThunderIndex].vStart.x+fX, m_pThunderVertex[nThunderIndex].vStart.y+fY, m_pThunderVertex[nThunderIndex].vStart.z+fZ );

			vPrevPos = m_pThunderVertex[nThunderIndex].vEnd;
		}// for(j)

		vPrevPos = m_pThunderVertex[++nSubBodyStartIndex].vEnd;
	}// for(i)
}

void CThunder::Render()
{
	// This code is for 4 viewport. so another program should use ORIGINAL CODE!
	if( m_dwDrawTime == 0 )
		m_dwDrawTime = GetTickCount();

	DWORD dwCurGapTime = GetTickCount() - m_dwDrawTime;
	static int nRotation = 0;

	bool bLightEnable = true;
	if( dwCurGapTime > m_dwDrawGap )
	{
		if( nRotation == 0 ) CreateThunder();

		nRotation++;
		if( nRotation == 4 )
		{
			m_dwDrawTime = 0;
			nRotation = 0;
		}

		bLightEnable = false;
	}
	DrawThunder(bLightEnable);

/*	////////	 THIS CODE IS ORIGINAL!		///////////
	if( m_dwDrawTime == 0 )
		m_dwDrawTime = GetTickCount();

	DWORD dwCurGapTime = GetTickCount() - m_dwDrawTime;
	bool bLightEnable = true;
	if( dwCurGapTime > m_dwDrawGap )
	{
		CreateThunder();
		m_dwDrawTime = 0;
		bLightEnable = false;
	}
	DrawThunder(bLightEnable);
*/
/*
}

void CThunder::DrawThunder(bool bLightEnalbe)
{
	CEffectEditorView *pView = (CEffectEditorView *)(((CFrameWnd *)AfxGetMainWnd())->GetActiveView());

	D3DLIGHT8 light;
	if( !bLightEnalbe )
	{
		m_pd3dDevice->GetLight( 0, &light );
		light.Specular.r = light.Specular.g = light.Specular.b = 0.4f;
		m_pd3dDevice->SetLight( 0, &light );

		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		D3DXVECTOR3 vViewPos, vNewUp, vStart, vEnd;
		D3DXVECTOR3 vNormal = D3DXVECTOR3( 0, 0, 1 );

		// get thunder sum vertex buffer
		CUSTOMVERTEX* pDestVertices;
		m_pThunderSumVB->Lock( 0, 0, (BYTE**)&pDestVertices, 0 );
		// get thunder sum index buffer
		SHORT* pDestIndex;
		m_pThunderSumIB->Lock( 0, 0, (BYTE**)&pDestIndex, 0 );

		// Make Thunder Sum vertex and imdex buffer
		int nVertexIndex = 0;
		for(int i=0; i<m_nBodyDivision + m_nSubBodyNumber * m_nSubBodyDivision; i++)
		{
			vStart	= m_pThunderVertex[i].vStart;
			vEnd	= m_pThunderVertex[i].vEnd;

			Vector view_pos = pView->m_Viewport[ pView->m_nCurViewportIndex ].m_Camera.GetFrom();
			vViewPos.x = view_pos.x;
			vViewPos.y = view_pos.y;
			vViewPos.z = view_pos.z;

			D3DXVec3Cross( &vNewUp, &(vStart-vEnd), &(vEnd-vViewPos) );
			D3DXVec3Normalize( &vNewUp, &vNewUp );

			// current vertex buffer
			CUSTOMVERTEX* pVertices;
			m_pThunderVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

			float fY = m_nWidth * 0.02f;
			pVertices[0].position = vStart + (-vNewUp * fY );
			pVertices[1].position = vStart + ( vNewUp * fY );
			pVertices[2].position = vEnd + (-vNewUp * fY );
			pVertices[3].position = vEnd + ( vNewUp * fY );

			pVertices[0].normal = pVertices[1].normal = pVertices[2].normal = pVertices[3].normal = vNormal;

			pVertices[0].tu			= 0;
			pVertices[0].tv			= 1;
			pVertices[1].tu			= 0;
			pVertices[1].tv			= 0;
			pVertices[2].tu			= 1;
			pVertices[2].tv			= 1;
			pVertices[3].tu			= 1;
			pVertices[3].tv			= 0;

			// vertex buffer
			memcpy( &pDestVertices[nVertexIndex*4], pVertices, sizeof(CUSTOMVERTEX)*4 );

			// index buffer
			SHORT* pInd = &pDestIndex[nVertexIndex*6];
			pInd[0] = nVertexIndex*4 +0;
			pInd[1] = nVertexIndex*4 +1;
			pInd[2] = nVertexIndex*4 +2;
			pInd[3] = nVertexIndex*4 +1;
			pInd[4] = nVertexIndex*4 +3;
			pInd[5] = nVertexIndex*4 +2;

			m_pThunderVB->Unlock();
			nVertexIndex++;
		}// for(i)

		m_pThunderSumVB->Unlock();
		m_pThunderSumIB->Unlock();

		// draw sum vertex and index
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		m_pd3dDevice->SetTexture( 0, m_pThunderTexture );
		m_pd3dDevice->SetStreamSource( 0, m_pThunderSumVB, sizeof(CUSTOMVERTEX) );
		m_pd3dDevice->SetIndices( m_pThunderSumIB, 0 );
		m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

		if( nVertexIndex != 0 )
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, nVertexIndex*4, 0, nVertexIndex*2 );

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

//		This is NOT OPTIMIZED Code!
//		for(int i=0; i<m_nBodyDivision + m_nSubBodyNumber * m_nSubBodyDivision; i++)
//			DrawSubThunder( m_pThunderVertex[i].vStart, m_pThunderVertex[i].vEnd );

	}// if()
	else
	{
		m_pd3dDevice->GetLight( 0, &light );
		light.Specular.r = light.Specular.g = light.Specular.b = 0.0f;
		m_pd3dDevice->SetLight( 0, &light );
	}

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void CThunder::DrawSubThunder(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd)
{
	return;

	CEffectEditorView *pView = (CEffectEditorView *)(((CFrameWnd *)AfxGetMainWnd())->GetActiveView());

	D3DXVECTOR3 vViewPos, vNewUp;
	D3DXVECTOR3 vNormal = D3DXVECTOR3( 0, 0, 1 );

	Vector view_pos = pView->m_Viewport[ pView->m_nCurViewportIndex ].m_Camera.GetFrom();
	vViewPos.x = view_pos.x;
	vViewPos.y = view_pos.y;
	vViewPos.z = view_pos.z;

	D3DXVec3Cross( &vNewUp, &(vStart-vEnd), &(vEnd-vViewPos) );
	D3DXVec3Normalize( &vNewUp, &vNewUp );

	// Make mesh
	CUSTOMVERTEX* pVertices;
	if( FAILED( m_pThunderVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		AfxMessageBox(_T("m_pThunderVB Vertex Buffer Lock Failed"));
		exit(1);
	}

	float fY = m_nWidth * 0.1f;
	pVertices[0].position = vStart + (-vNewUp * fY );
	pVertices[1].position = vStart + ( vNewUp * fY );
	pVertices[2].position = vEnd + (-vNewUp * fY );
	pVertices[3].position = vEnd + ( vNewUp * fY );

	pVertices[0].normal = pVertices[1].normal = pVertices[2].normal = pVertices[3].normal = vNormal;

	pVertices[0].tu			= 0;
	pVertices[0].tv			= 1;
	pVertices[1].tu			= 0;
	pVertices[1].tv			= 0;
	pVertices[2].tu			= 1;
	pVertices[2].tv			= 1;
	pVertices[3].tu			= 1;
	pVertices[3].tv			= 0;

	m_pThunderVB->Unlock();
	// Draw Thunder
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	m_pd3dDevice->SetTexture( 0, m_pThunderTexture );
	m_pd3dDevice->SetStreamSource( 0, m_pThunderVB, sizeof(CUSTOMVERTEX) );
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

void CThunder::Save(FILE *fp)
{
	fwrite( &m_nPosX,			4, 1, fp );
	fwrite( &m_nPosY,			4, 1, fp );
	fwrite( &m_nPosZ,			4, 1, fp );
	fwrite( &m_nBlokenRatio,	4, 1, fp );
	fwrite( &m_nBodyDivision,	4, 1, fp );
	fwrite( &m_nSubBodyNumber,	4, 1, fp );
	fwrite( &m_nSubBodyDivision,4, 1, fp );
	fwrite( &m_dwDrawGap,		4, 1, fp );
	fwrite( &m_nWidth,			4, 1, fp );

	::Save(m_TexturePath, fp);
}

void CThunder::Load(FILE *fp)
{
	int x, y, z, ratio, body, subbody, subbodydivision, width;
	DWORD drawgap;

	fread( &x,				4, 1, fp );
	fread( &y,				4, 1, fp );
	fread( &z,				4, 1, fp );
	fread( &ratio,			4, 1, fp );
	fread( &body,			4, 1, fp );
	fread( &subbody,		4, 1, fp );
	fread( &subbodydivision,4, 1, fp );
	fread( &drawgap,		4, 1, fp );
	fread( &width,			4, 1, fp );

	::Load(m_TexturePath, fp);

	Init(x, y, z, ratio, body, subbody, subbodydivision, drawgap, width);

	if( m_TexturePath != _T("") )
		SetTexturePath( m_TexturePath );
}

void CThunder::GetSettingData(int &x, int &y, int &z, int &Ratio, int &BodyDivision, int &SubBody, int &SubBodyDivision, int &DrawGap, int &Width)
{
	x = m_nPosX;
	y = m_nPosY;
	z = m_nPosZ;
	Ratio = m_nBlokenRatio;
	BodyDivision = m_nBodyDivision;
	SubBody = m_nSubBodyNumber;
	SubBodyDivision = m_nSubBodyDivision;
	DrawGap = m_dwDrawGap;
	Width = m_nWidth;
}

void CThunder::SetTexturePath(CString strTexture)
{
	m_TexturePath = strTexture;

	if( m_pThunderTexture ) 
	{
		m_pThunderTexture->Release();
		m_pThunderTexture = NULL;
	}

	if( m_TexturePath != _T("") )
	D3DXCreateTextureFromFileEx( m_pd3dDevice, m_TexturePath, 
								D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
								D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
								D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, 
								&m_pThunderTexture );
}

void CThunder::SetOnlyData(int x, int y, int z, int Ratio, int BodyDivision, int SubBody, int SubBodyDivision, int DrawGap, int Width, CString strTexPath)
{
	m_nPosX = x;
	m_nPosY = y;
	m_nPosZ = z;
	m_nBlokenRatio = Ratio;
	m_fBlokenRatio = m_nBlokenRatio * 0.2f;
	m_nBodyDivision = BodyDivision;
	m_nSubBodyNumber = SubBody;
	m_nSubBodyDivision = SubBodyDivision;
	m_dwDrawTime = 0;
	m_dwDrawGap = DrawGap;
	m_nWidth = Width;

	m_TexturePath = strTexPath;
}
*/


}; // namespace