// Waterfall2.cpp: implementation of the CWaterfall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Waterfall.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace XiahGameEngine
{

//CWaterfall::CWaterfall()
//{
///*
//	m_vParticleStartPos	= D3DXVECTOR3( 0, 0, 0 );
//	m_vGravity			= D3DXVECTOR3( 0,-1, 0 );
//
//	m_pWaterfallVB		= NULL;
//	m_pWaterTexture		= NULL;
//	m_pWaterBottomVB	= NULL;
//	m_pWaterSumVB		= NULL;
//	m_pWaterSumIB		= NULL;
//	m_pWaterBottomSumVB = NULL;
//	m_pWaterBottomSumIB = NULL;
//	m_iParticles		= 0;
//	m_nDirection		= 0;
//	m_nBottomY			= 0;
//
//	m_pd3dDevice		= NULL;
//	m_TexturePath		= _T("");
//
//	m_PARTICLE_WATERFALL_MAX = 0;
//
//	m_bPlay = false;
//
//	RemoveAllParticles();
//*/
//}
//
//CWaterfall::~CWaterfall()
//{
///*
//	RemoveAllParticles();
//	Release();
//*/
//}
/*
void CWaterfall::Init(int dir, int x, int y, int z, int power, int number, int size, int width, int sizeB )
{
	srand( time(NULL) );

	CEffectEditorView *pView = (CEffectEditorView *)(((CFrameWnd *)AfxGetMainWnd())->GetActiveView());
	m_pd3dDevice = pView->m_D3D.GetDevice();

	// make water fall vertex buffer
	m_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pWaterfallVB );
	// make water bottom vertex buffer
	m_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX2),
									0, D3DFVF_CUSTOMVERTEX2,
									D3DPOOL_DEFAULT, &m_pWaterBottomVB );

	// make water optimize vertex buffer
	m_pd3dDevice->CreateVertexBuffer( PARTICLE_WATER_MAX*4*sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pWaterSumVB );
	// make water optimize index buffer
	m_pd3dDevice->CreateIndexBuffer( PARTICLE_WATER_MAX*4*3*sizeof(CUSTOMVERTEX),
								   0, D3DFMT_INDEX16,
								   D3DPOOL_DEFAULT, &m_pWaterSumIB );

	// make water bottom optimize vertex buffer
	m_pd3dDevice->CreateVertexBuffer( PARTICLE_WATER_BOTTOM_MAX*4*sizeof(CUSTOMVERTEX2),
									0, D3DFVF_CUSTOMVERTEX2,
									D3DPOOL_DEFAULT, &m_pWaterBottomSumVB );
	// make water bottom optimize index buffer
	m_pd3dDevice->CreateIndexBuffer( PARTICLE_WATER_BOTTOM_MAX*4*3*sizeof(CUSTOMVERTEX2),
								   0, D3DFMT_INDEX16,
								   D3DPOOL_DEFAULT, &m_pWaterBottomSumIB );

	m_dwCreateTime		= GetTickCount();
	Setting(dir, x, y, z, power, number, size, width, sizeB);
}

void CWaterfall::GetSettingData(int &dir, int &x, int &y, int &z, int &power, int &number, int &size, int &width, int &sizeB)
{
	dir		= m_nDirection;
	x		= m_nPosX;
	y		= m_nPosY;
	z		= m_nPosZ;
	power	= m_nPower;
	number	= m_nNumber;
	size	= m_nSize;
	width	= m_nWidth;
	sizeB	= m_nSizeB;
}

void CWaterfall::Setting(int dir, int x, int y, int z, int power, int number, int size, int width, int sizeB )
{
	RemoveAllParticles();

	// variable set
	m_nPosX				= x;
	m_nPosY				= y;
	m_nPosZ				= z;
	m_nNumber			= number;
	m_vParticleStartPos	= D3DXVECTOR3( m_nPosX, m_nPosY, m_nPosZ );
	m_nDirection		= dir;
	m_nPower			= power;
	m_nSize				= size;
	m_nWidth			= width;
	m_nSizeB			= sizeB;
	m_PARTICLE_WATERFALL_MAX = m_nNumber * 2;
	m_fWaterWidth		= m_nSize * 0.16f;
	m_fWaterBottomWidth	= m_nSizeB * 0.1f;
	m_nBottomY			= -0.2f;

	// vertex buffer set
	CUSTOMVERTEX2* pVertices;
	m_pWaterBottomVB->Lock(0,0,(BYTE**)&pVertices,0);

	pVertices[0].position	= D3DXVECTOR3(-m_fWaterBottomWidth, 0,  0.0f );
	pVertices[0].tu			= 1;
	pVertices[0].tv			= 1;
	pVertices[1].position	= D3DXVECTOR3(-m_fWaterBottomWidth, m_fWaterBottomWidth,  0.0f );
	pVertices[1].tu			= 0;
	pVertices[1].tv			= 1;
	pVertices[2].position	= D3DXVECTOR3( m_fWaterBottomWidth, 0,  0.0f );
	pVertices[2].tu			= 1;
	pVertices[2].tv			= 0;
	pVertices[3].position	= D3DXVECTOR3( m_fWaterBottomWidth, m_fWaterBottomWidth,  0.0f );
	pVertices[3].tu			= 0;
	pVertices[3].tv			= 0;

	m_pWaterBottomVB->Unlock();
}

void CWaterfall::Release()
{
	// vertex buffer
	if( m_pWaterfallVB )		m_pWaterfallVB->Release();
	if( m_pWaterBottomVB )		m_pWaterBottomVB->Release();
	if( m_pWaterSumVB )			m_pWaterSumVB->Release();
	if( m_pWaterSumIB )			m_pWaterSumIB->Release();
	if( m_pWaterBottomSumVB )	m_pWaterBottomSumVB->Release();
	if( m_pWaterBottomSumIB )	m_pWaterBottomSumIB->Release();

	m_pWaterfallVB		= NULL;
	m_pWaterBottomVB	= NULL;
	m_pWaterSumVB		= NULL;
	m_pWaterSumIB		= NULL;
	m_pWaterBottomSumVB = NULL;
	m_pWaterBottomSumIB = NULL;

	// texture 
	if( m_pWaterTexture ) m_pWaterTexture->Release();
	m_pWaterTexture = NULL;
}

void CWaterfall::RemoveAllParticles()
{
	WPARTICLELIST::iterator wit;
	for(wit=m_ListParticles.begin(); wit!=m_ListParticles.end(); wit++)
	{
		CParticleInfo* pParticle = *wit;
		delete pParticle;
	}
	m_iParticles = 0;
	m_ListParticles.clear();

	for(wit=m_ListWaterBottom.begin(); wit!=m_ListWaterBottom.end(); wit++)
	{
		CParticleInfo* pParticle = *wit;
		delete pParticle;
	}
	m_ListWaterBottom.clear();
}

#define WATERFALL_CREATE_GAP	100

void CWaterfall::CreateWaterfall()
{
	DWORD dwCurTick = GetTickCount();

	// need constant time to create water, so that gap to make water
	if( dwCurTick - m_dwCreateTime >= WATERFALL_CREATE_GAP )
		m_dwCreateTime = dwCurTick;
	else
		return;

	D3DXVECTOR3* vStartPos = new D3DXVECTOR3[m_nWidth];
	for(int i=0; i<m_nWidth; i++)
	{
		vStartPos[i] = m_vParticleStartPos;
		switch( m_nDirection )
		{
		case DIRECTION_X:
			vStartPos[i].z += m_fWaterWidth * i;
			break;
		case DIRECTION_X_MINUS:
			vStartPos[i].z -= m_fWaterWidth * i;
			break;
		case DIRECTION_Z:
			vStartPos[i].x += m_fWaterWidth * i;
			break;
		case DIRECTION_Z_MINUS:
			vStartPos[i].x -= m_fWaterWidth * i;
			break;
		}// switch
	}// for(m_nWidth)

	for(i=0; i<m_nWidth; i++)
	{
		D3DXVECTOR3 vStart = vStartPos[i];
		float fX, fY, fZ;
		float fYGap = m_nSize*0.5f;	//6.5f;	// Height of a piece of water
		fX = rnd() * 0.15f;
		if( rand() % 2 ) fX = -fX;
		fY = rnd() * 0.15f;
		if( rand() % 2 ) fY = -fY;
		fZ = rnd() * 0.1f;
		if( rand() % 2 ) fZ = -fZ;
		
		if( m_iParticles < m_PARTICLE_WATERFALL_MAX )	// can make particle
		{
			m_iParticles += 2;
			CParticleInfo* particle1 = new CParticleInfo();
			CParticleInfo* particle2 = new CParticleInfo();
			
			particle1->m_vStartPos = D3DXVECTOR3( vStart.x+fX, vStart.y+fY, vStart.z+fZ );
			particle1->fPower = ( 0.3f + rnd()*0.1f ) * 0.2f;
			
			particle2->m_vStartPos = particle1->m_vStartPos;
			
			float fPowerUp = (m_nPower/2) * 0.1f;
			float fPower = 0.3f + fPowerUp + fPowerUp * rnd() * 0.1f;
			switch( m_nDirection )
			{
			case DIRECTION_X:
				particle1->m_vDir.x = fPower;
				particle1->m_vDir.z = fZ;
				particle2->m_vStartPos.x -= fYGap;
				break;
			case DIRECTION_X_MINUS:
				particle1->m_vDir.x =-fPower;
				particle1->m_vDir.z = fZ;
				particle2->m_vStartPos.x += fYGap;
				break;
			case DIRECTION_Z:
				particle1->m_vDir.x = fZ;
				particle1->m_vDir.z = fPower;
				particle2->m_vStartPos.z -= fYGap;
				break;
			case DIRECTION_Z_MINUS:
				particle1->m_vDir.x = fZ;
				particle1->m_vDir.z =-fPower;
				particle2->m_vStartPos.z += fYGap;
				break;
			default:
				particle1->m_vDir.x = fPower;
				particle1->m_vDir.z = 0.0f;
			}// switch
			particle1->m_vDir.y = (float)tan( D3DXToRadian(10) );
			
			particle2->fPower = particle1->fPower - 0.035f;
			particle2->m_vDir = particle1->m_vDir;

			m_ListParticles.push_back( particle1 );
			m_ListParticles.push_back( particle2 );
		}// if()
		else		// when number decreaseed by user
		{
			while( m_iParticles > m_PARTICLE_WATERFALL_MAX )
			{
				CParticleInfo* pParticle = m_ListParticles.front();
				m_ListParticles.pop_front();
				if( pParticle ) delete pParticle;

				pParticle = m_ListParticles.front();
				m_ListParticles.pop_front();
				if( pParticle ) delete pParticle;

				m_iParticles -= 2;
			}// while
		}
	}// for(m_nWidth)

	delete []vStartPos;
}

void CWaterfall::UpdateWaterfall()
{
	DWORD dwCurTick = GetTickCount();
	float fProgressTime;

	bool bIsDelete = false;
	WPARTICLELIST::iterator wit, wit2;
	for(wit=m_ListParticles.begin(); wit!=m_ListParticles.end(); wit++)
	{
		if( bIsDelete )
		{
			wit = wit2;
			bIsDelete = false;
		}

		CParticleInfo* pParticle1 = *wit;
		wit++;
		CParticleInfo* pParticle2 = *wit;
		wit2 = wit;
		wit2++;

		if( pParticle1 && pParticle2 )
		{
			fProgressTime = (float)(dwCurTick - pParticle1->m_dwPrevTime);
			fProgressTime *= 0.015f;

			pParticle1->m_vPos = pParticle1->m_vStartPos + ( pParticle1->m_vDir * fProgressTime )
				+ ( pParticle1->fPower * m_vGravity * fProgressTime * fProgressTime );

			pParticle2->m_vPos = pParticle2->m_vStartPos + ( pParticle2->m_vDir * fProgressTime )
				+ ( pParticle2->fPower * m_vGravity * fProgressTime * fProgressTime );

			if( pParticle1->m_vPos.y < m_nBottomY )	// delete water
			{
				float fX = rnd();
				if( rand() % 2 ) fX = -fX;
				float fY = rnd();
				if( rand() % 2 ) fY = -fY;
				float fZ = rnd();
				if( rand() % 2 ) fZ = -fZ;
				D3DXVECTOR3 vPos1 = pParticle1->m_vPos;
				vPos1.x += fX;
				vPos1.y += fY;
				vPos1.z += fZ;

				fX = rnd();
				if( rand() % 2 ) fX = -fX;
				fY = rnd();
				if( rand() % 2 ) fY = -fY;
				fZ = rnd();
				if( rand() % 2 ) fZ = -fZ;
				D3DXVECTOR3 vPos2 = pParticle1->m_vPos;
				vPos2.x += fX;
				vPos2.y += fY;
				vPos2.z += fZ;

				m_ListParticles.remove(pParticle1);
				m_ListParticles.remove(pParticle2);
				delete pParticle1;
				delete pParticle2;

				bIsDelete = true;

				m_iParticles -= 2;

				CreateWaterBottom(vPos1);		// create water bottom => bobble
				CreateWaterBottom(vPos2);
			}// if() 

		}// if( pParticle1 && pParticle2 )

	}// m_ListParticles
}

void CWaterfall::DrawWaterfall()
{
	CEffectEditorView *pView = (CEffectEditorView *)(((CFrameWnd *)AfxGetMainWnd())->GetActiveView());

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,			0 );
//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		  FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	// get vertex buffer of water sum VB
	CUSTOMVERTEX* pDestVertices;
	m_pWaterSumVB->Lock( 0, 0, (BYTE**)&pDestVertices, 0 );
	// get index buffer of water sum IB
	SHORT* pDestIndex;
	m_pWaterSumIB->Lock( 0, 0, (BYTE**)&pDestIndex, 0 );

	int iVertexIndex = 0;
	DWORD dwCurTick = GetTickCount();

	WPARTICLELIST::iterator wit;
	for(wit=m_ListParticles.begin(); wit!=m_ListParticles.end(); wit++)
	{
		CParticleInfo* pParticle1 = *wit;
		wit++;
		CParticleInfo* pParticle2 = *wit;

		if( pParticle1 && pParticle2 )
		{
			D3DXVECTOR3 vViewPos, vNewUp, vStart, vEnd;
			D3DXVECTOR3 vNormal = D3DXVECTOR3( 0, 0, 1 );

			Vector view_pos = pView->m_Viewport[ pView->m_nCurViewportIndex ].m_Camera.GetFrom();
			vViewPos.x = view_pos.x;
			vViewPos.y = view_pos.y;
			vViewPos.z = view_pos.z;

			vStart	= pParticle1->m_vPos;
			vEnd	= pParticle2->m_vPos;

			D3DXVECTOR3 v1 = vStart-vEnd;
			D3DXVECTOR3 v2 = vEnd-vViewPos;

			D3DXVec3Cross( &vNewUp, &v1, &v2 );
			D3DXVec3Normalize( &vNewUp, &vNewUp );

			// Make mesh
			CUSTOMVERTEX* pVertices;
			m_pWaterfallVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

			float fY = m_fWaterWidth;
			float fProgressTime = (float)(dwCurTick - pParticle1->m_dwPrevTime);
			fProgressTime *= 0.01f;
			float fRatio = (dwCurTick - pParticle1->m_dwCreateTime) / 500 ; // / pParticle1->fTTL;
			float fChangeY = fY + fY * fRatio;

			pVertices[0].position = vStart + (-vNewUp * fChangeY );
			pVertices[1].position = vStart + ( vNewUp * fChangeY );
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
			memcpy( &pDestVertices[iVertexIndex*4], pVertices, sizeof(CUSTOMVERTEX)*4 );

			// index buffer
			SHORT* pInd = &pDestIndex[iVertexIndex*6];
			pInd[0] = iVertexIndex*4 +0;
			pInd[1] = iVertexIndex*4 +1;
			pInd[2] = iVertexIndex*4 +2;
			pInd[3] = iVertexIndex*4 +1;
			pInd[4] = iVertexIndex*4 +3;
			pInd[5] = iVertexIndex*4 +2;

			m_pWaterfallVB->Unlock();
			iVertexIndex++;
		}// if( pParticle1 && pParticle2 )

	}// m_ListParticles

	m_pWaterSumVB->Unlock();
	m_pWaterSumIB->Unlock();

	// Draw Water
	m_pd3dDevice->SetTexture( 0, m_pWaterTexture );
	m_pd3dDevice->SetStreamSource( 0, m_pWaterSumVB, sizeof(CUSTOMVERTEX) );
	m_pd3dDevice->SetIndices( m_pWaterSumIB, 0 );
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

	if( iVertexIndex != 0 )
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, iVertexIndex*4, 0, iVertexIndex*2 );

	DrawWaterBottom();

//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}

void CWaterfall::Destroy()
{
	RemoveAllParticles();
	Release();
}

void CWaterfall::Start(bool bStart)
{
	if(bStart)
	{
		m_bPlay = true;
		RemoveAllParticles();
	}
	else
		m_bPlay = false;
}

void CWaterfall::Flow()
{
	CreateWaterfall();
	UpdateWaterfall();
	UpdateWaterBottom();
	DrawWaterfall();
}

void CWaterfall::CreateWaterBottom(D3DXVECTOR3 vPos)
{
	CParticleInfo* particle1 = new CParticleInfo();

	particle1->m_vPos = vPos;
//	particle1->m_vPos.y += 0.1f;
	particle1->fTTL   = 500;
	particle1->fScale = 1.0f;
	particle1->fAlpha = 1.0f;

	m_ListWaterBottom.push_back(particle1);
}

void CWaterfall::UpdateWaterBottom()
{
	DWORD dwCurTick = GetTickCount();

	WPARTICLELIST::iterator wit;
	for(wit=m_ListWaterBottom.begin(); wit!=m_ListWaterBottom.end(); )
	{
		CParticleInfo* pParticle = *wit;

		if(pParticle)
		{
			pParticle->fScale += 0.07f;
			pParticle->fAlpha -= 0.06f;

			if( dwCurTick - pParticle->m_dwCreateTime > pParticle->fTTL )
			{
				wit = m_ListWaterBottom.erase(wit);
				delete pParticle;
			}
			else { wit++; }
		}
		else { wit++; }

	}// for( m_ListWaterBottom )
}

void CWaterfall::DrawWaterBottom()
{
	CEffectEditorView *pView = (CEffectEditorView *)(((CFrameWnd *)AfxGetMainWnd())->GetActiveView());

	// get vertex buffer of water bottom sum VB
	CUSTOMVERTEX2* pDestVertices;
	m_pWaterBottomSumVB->Lock( 0, 0, (BYTE**)&pDestVertices, 0 );
	// get index buffer of water bottom sum IB
	SHORT* pDestIndex;
	m_pWaterBottomSumIB->Lock( 0, 0, (BYTE**)&pDestIndex, 0 );

	int iVertexIndex = 0;

	WPARTICLELIST::iterator wit;
	for(wit=m_ListWaterBottom.begin(); wit!=m_ListWaterBottom.end(); wit++)
	{
		CParticleInfo* pParticle = *wit;
		if(pParticle)
		{
			D3DXMATRIX matView, matBillboard, matScale;

			Matrix matV = pView->m_Viewport[ pView->m_nCurViewportIndex ].m_Camera.GetView();
			memcpy( &matView, &matV, sizeof(matV) );

			D3DXMatrixInverse( &matBillboard, NULL, &matView );

			matBillboard._41 = pParticle->m_vPos.x;
			matBillboard._42 = pParticle->m_vPos.y;
			matBillboard._43 = pParticle->m_vPos.z;

			D3DXMatrixScaling( &matScale, pParticle->fScale, pParticle->fScale, 0 );
			matBillboard = matScale * matBillboard;

			int iAlpha = pParticle->fAlpha * 255;

			CUSTOMVERTEX2* pVertices;
			m_pWaterBottomVB->Lock(0,0,(BYTE**)&pVertices,0);
			pVertices[0].diffuse = D3DCOLOR_ARGB( iAlpha, iAlpha, iAlpha, iAlpha );
			pVertices[1].diffuse = D3DCOLOR_ARGB( iAlpha, iAlpha, iAlpha, iAlpha );
			pVertices[2].diffuse = D3DCOLOR_ARGB( iAlpha, iAlpha, iAlpha, iAlpha );
			pVertices[3].diffuse = D3DCOLOR_ARGB( iAlpha, iAlpha, iAlpha, iAlpha );

			// vertex buffer
			memcpy( &pDestVertices[iVertexIndex*4], pVertices, sizeof(CUSTOMVERTEX2)*4 );
			CUSTOMVERTEX2 *pVer = &pDestVertices[iVertexIndex*4];
			for(int i=0; i<4; i++)
				D3DMath_VectorMatrixMultiply(pVer[i].position, pVertices[i].position, matBillboard );

			// index buffer
			SHORT* pInd = &pDestIndex[iVertexIndex*6];
			pInd[0] = iVertexIndex*4 +0;
			pInd[1] = iVertexIndex*4 +1;
			pInd[2] = iVertexIndex*4 +2;
			pInd[3] = iVertexIndex*4 +1;
			pInd[4] = iVertexIndex*4 +3;
			pInd[5] = iVertexIndex*4 +2;

			m_pWaterBottomVB->Unlock();
			iVertexIndex++;
		}// if

	}// for( m_ListWaterBottom )

	m_pWaterBottomSumVB->Unlock();
	m_pWaterBottomSumIB->Unlock();

	m_pd3dDevice->SetStreamSource( 0, m_pWaterBottomSumVB, sizeof(CUSTOMVERTEX2) );
	m_pd3dDevice->SetIndices( m_pWaterBottomSumIB, 0 );
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX2 );

	if( iVertexIndex != 0 )
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, iVertexIndex*4, 0, iVertexIndex*2 );
}

void CWaterfall::Save(FILE *fp)
{
	fwrite( &m_nDirection,	4, 1, fp );
	fwrite( &m_nPosX,		4, 1, fp );
	fwrite( &m_nPosY,		4, 1, fp );
	fwrite( &m_nPosZ,		4, 1, fp );
	fwrite( &m_nPower,		4, 1, fp );
	fwrite( &m_nNumber,		4, 1, fp );
	fwrite( &m_nSize,		4, 1, fp );
	fwrite( &m_nWidth,		4, 1, fp );
	fwrite( &m_nSizeB,		4, 1, fp );

	::Save(m_TexturePath, fp);
}

void CWaterfall::Load(FILE *fp)
{
	int dir, PosX, PosY, PosZ, Power, Number, Size, Width, SizeB;

	fread( &dir,	4, 1, fp );
	fread( &PosX,	4, 1, fp );
	fread( &PosY,	4, 1, fp );
	fread( &PosZ,	4, 1, fp );
	fread( &Power,	4, 1, fp );
	fread( &Number,	4, 1, fp );
	fread( &Size,	4, 1, fp );
	fread( &Width,	4, 1, fp );
	fread( &SizeB,	4, 1, fp );

	::Load(m_TexturePath, fp);

	Init(dir, PosX, PosY, PosZ, Power, Number, Size, Width, SizeB);

	if( !m_TexturePath.IsEmpty() )
		SetTexturePath( m_TexturePath );
}

void CWaterfall::SetTexturePath(CString strTexture)
{
	m_TexturePath = strTexture;

	if( m_pWaterTexture ) 
	{
		m_pWaterTexture->Release();
		m_pWaterTexture = NULL;
	}

	if( m_TexturePath != _T("") )
	D3DXCreateTextureFromFileEx( m_pd3dDevice, m_TexturePath, 
								D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
								D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
								D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, 
								&m_pWaterTexture );
}

void CWaterfall::SetOnlyData(int dir, int x, int y, int z, int power, int number, int size, int width, int sizeB, CString strTexPath )
{
	// variable set
	m_nPosX				= x;
	m_nPosY				= y;
	m_nPosZ				= z;
	m_nNumber			= number;
	m_vParticleStartPos	= D3DXVECTOR3( m_nPosX, m_nPosY, m_nPosZ );
	m_nDirection		= dir;
	m_nPower			= power;
	m_nSize				= size;
	m_nWidth			= width;
	m_nSizeB			= sizeB;
	m_PARTICLE_WATERFALL_MAX = m_nNumber * 2;
	m_fWaterWidth		= m_nSize * 0.16f;
	m_fWaterBottomWidth	= m_nSizeB * 0.1f;
	m_nBottomY			= -0.2f;

	m_TexturePath = strTexPath;
}
*/


};// namesapce
