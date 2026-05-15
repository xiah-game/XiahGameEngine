#include "stdafx.h"
#include "SkyBox.h"
#include "XiahPak.h"


#define		rnd()						(((FLOAT)rand() ) / RAND_MAX)
#define		STARS_TWINKLE_FREQUENCY		4000

// SKY BOX TEXTURE INDEX (SKYBOX.XPK)
long g_SkyIndex [8][4] = 
{
	50001262,50001263,50001264,50001265,	// 기암 (1)
	50001266,50001267,50001268,50001269,	// 화산 (2)
	50001270,50001271,50001272,50001273,	// 설산 (3)
	50001274,50001275,50001276,50001277,	// 사막 (4)
	50001278,50001279,50001280,50001281,	// 늪 (5)
    50001475,50001476,50001477,50001478,	// 초원(6)
	50001554,50001555,50001556,50001557,	// 고산
	50001262,50001263,50001264,50001265		// 문파전, 기암으로 대치 (10)
};

static int pre_skymapID = -1;

namespace XiahGameEngine
{
	//XIAHGE_API CSkyBox	g_SkyBox;

	XIAHGE_API CSkyBox::CSkyBox()
	{
		m_VB = NULL;
		m_bFogEnable = FALSE;
	}
	
	XIAHGE_API CSkyBox::~CSkyBox()
	{
		Release();
	}

	XIAHGE_API BOOL CSkyBox::Create()
	{
		// 4개의 벽은 2등분되어 있다. 지평면의 색을 넣기 위해
		float fSkyBase	= -0.3f;	// 가장 밑 부분
		float fSkyMid	= 0.02f;	// 지평선 부분.

		// tex calculte of mid
		float fPlusSkyBase = fSkyBase > 0 ? fSkyBase : -fSkyBase;
		float fMidPos = 0.5f - fSkyMid;
		float fMidTex = fMidPos / ( 0.5f + fPlusSkyBase );

		g_pDirect3DDevice->CreateVertexBuffer( 36*sizeof(VT_LVertex), 0,D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_VB, NULL );
		VT_LVertex* pVertex;
		m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

		// 벽 1 Right up
		pVertex[ 0].pos = Vector3( 0.5f, 0.5f, -0.5f);		
		pVertex[ 1].pos = Vector3( 0.5f, fSkyMid, -0.5f);	
		pVertex[ 2].pos = Vector3( 0.5f, 0.5f, 0.5f);		
		pVertex[ 3].pos = Vector3( 0.5f, fSkyMid, 0.5f);	
		pVertex[ 0].tex = Vector2(1, 0);
		pVertex[ 1].tex = Vector2(1, fMidTex);
		pVertex[ 2].tex = Vector2(0, 0);
		pVertex[ 3].tex = Vector2(0, fMidTex);

		// 벽 1 Right down
		pVertex[ 4].pos = Vector3( 0.5f, fSkyMid, -0.5f);		
		pVertex[ 5].pos = Vector3( 0.5f, fSkyBase, -0.5f);	
		pVertex[ 6].pos = Vector3( 0.5f, fSkyMid, 0.5f);		
		pVertex[ 7].pos = Vector3( 0.5f, fSkyBase, 0.5f);	
		pVertex[ 4].tex = Vector2(1, fMidTex);
		pVertex[ 5].tex = Vector2(1, 1);
		pVertex[ 6].tex = Vector2(0, fMidTex);
		pVertex[ 7].tex = Vector2(0, 1);

		// 벽 2 Back up
		pVertex[ 8].pos = Vector3( -0.5f, fSkyMid, 0.5f);	
		pVertex[ 9].pos = Vector3( -0.5f, 0.5f, 0.5f);		
		pVertex[10].pos = Vector3( 0.5f,  fSkyMid, 0.5f);	
		pVertex[11].pos = Vector3( 0.5f,  0.5f, 0.5f);		
		pVertex[ 8].tex = Vector2(0, fMidTex);
		pVertex[ 9].tex = Vector2(0, 0);
		pVertex[10].tex = Vector2(1, fMidTex);
		pVertex[11].tex = Vector2(1, 0);

		// 벽 2 Back down
		pVertex[12].pos = Vector3( -0.5f, fSkyBase, 0.5f);	
		pVertex[13].pos = Vector3( -0.5f, fSkyMid, 0.5f);		
		pVertex[14].pos = Vector3( 0.5f,  fSkyBase, 0.5f);	
		pVertex[15].pos = Vector3( 0.5f,  fSkyMid, 0.5f);		
		pVertex[12].tex = Vector2(0,1);
		pVertex[13].tex = Vector2(0, fMidTex);
		pVertex[14].tex = Vector2(1, 1);
		pVertex[15].tex = Vector2(1, fMidTex);

		// 벽 3 Left up
		pVertex[16].pos = Vector3( -0.5f, fSkyMid, -0.5f);	
		pVertex[17].pos = Vector3( -0.5f, 0.5f, -0.5f);		
		pVertex[18].pos = Vector3( -0.5f, fSkyMid, 0.5f);	
		pVertex[19].pos = Vector3( -0.5f, 0.5f, 0.5f);		
		pVertex[16].tex = Vector2(0, fMidTex);
		pVertex[17].tex = Vector2(0, 0);
		pVertex[18].tex = Vector2(1, fMidTex);
		pVertex[19].tex = Vector2(1, 0);

		// 벽 3 Left down
		pVertex[20].pos = Vector3( -0.5f, fSkyBase, -0.5f);	
		pVertex[21].pos = Vector3( -0.5f, fSkyMid, -0.5f);		
		pVertex[22].pos = Vector3( -0.5f, fSkyBase, 0.5f);	
		pVertex[23].pos = Vector3( -0.5f, fSkyMid, 0.5f);		
		pVertex[20].tex = Vector2(0, 1);
		pVertex[21].tex = Vector2(0, fMidTex);
		pVertex[22].tex = Vector2(1, 1);
		pVertex[23].tex = Vector2(1, fMidTex);

		// 벽 4 Front up
		pVertex[24].pos = Vector3( -0.5f, 0.5f, -0.5f);		
		pVertex[25].pos = Vector3( -0.5f, fSkyMid, -0.5f);	
		pVertex[26].pos = Vector3( 0.5f, 0.5f, -0.5f);		
		pVertex[27].pos = Vector3( 0.5f, fSkyMid, -0.5f);	
		pVertex[24].tex = Vector2(1, 0);
		pVertex[25].tex = Vector2(1, fMidTex);
		pVertex[26].tex = Vector2(0, 0);
		pVertex[27].tex = Vector2(0, fMidTex);


		// 벽 4 Front down
		pVertex[28].pos = Vector3( -0.5f, fSkyMid, -0.5f);		
		pVertex[29].pos = Vector3( -0.5f, fSkyBase, -0.5f);	
		pVertex[30].pos = Vector3( 0.5f, fSkyMid, -0.5f);		
		pVertex[31].pos = Vector3( 0.5f, fSkyBase, -0.5f);	
		pVertex[28].tex = Vector2(1, fMidTex);
		pVertex[29].tex = Vector2(1, 1);
		pVertex[30].tex = Vector2(0, fMidTex);
		pVertex[31].tex = Vector2(0, 1);

		// 뚜껑
		pVertex[32].pos = Vector3( -0.5f, 0.5f, 0.5f);		
		pVertex[33].pos = Vector3( -0.5f, 0.5f, -0.5f);		
		pVertex[34].pos = Vector3( 0.5f, 0.5f, 0.5f);		
		pVertex[35].pos = Vector3( 0.5f, 0.5f, -0.5f);		
		pVertex[32].tex = Vector2(0, 1);
		pVertex[33].tex = Vector2(0, 0);
		pVertex[34].tex = Vector2(1, 1);
		pVertex[35].tex = Vector2(1, 0);

		m_Color = 0xFF9F9F9F;
		for(int i = 0; i < 36; i++)
			pVertex[i].diffuse = m_Color;
		m_VB->Unlock();

		m_matProjection.SetProjectionMatrix( 3.141592f / 3, 1, 0.1f, 500.0f);

		return TRUE;
	}
	
	XIAHGE_API BOOL CSkyBox::Release()
	{
		if( m_VB ) 
			m_VB->Release();

		m_VB = NULL;
		return TRUE;
	}

	// MAP이 바뀌었으므로 SKYMAP도 바꾼다
	XIAHGE_API BOOL CSkyBox::ChangeSkyMap(int MapID)
	{
		// 전에 SKYMAP을 읽었었더라면 Release 한다
		if(pre_skymapID != MapID && pre_skymapID != -1)
		{
			for(int i=0;i<4;i++)
			{
				XiahPak::ReleaseRes(g_SkyIndex[pre_skymapID-1][i]);
			}			
		}

		//  새로 SKYMAP을 읽는다.
		for(int i=0;i<4;i++)
		{
			m_pTexture[i] = XiahPak::GetTexture( g_SkyIndex[MapID-1][i], TRUE);
		}
		pre_skymapID = MapID;
		return TRUE;
	}


	//XIAHGE_API BOOL CSkyBox::SetColor(D3DCOLOR color)
	//{
/*
		for(int i = 0; i < 36; i++)
			m_Vertex[ i].diffuse = m_Color;
*/

	//return TRUE;
	//}

	XIAHGE_API BOOL CSkyBox::SetColor(D3DCOLOR colorUp, D3DCOLOR colorMid, D3DCOLOR colorDown)
	{
		VT_LVertex* pVertex;
		m_VB->Lock( 0, 0, (void**)&pVertex, 0 );

		// 벽 1 Right up
		pVertex[ 0].diffuse = colorUp;
		pVertex[ 1].diffuse = colorMid;
		pVertex[ 2].diffuse = colorUp;
		pVertex[ 3].diffuse = colorMid;

		// 벽 1 Right down
		pVertex[ 4].diffuse = colorMid;
		pVertex[ 5].diffuse = colorDown;
		pVertex[ 6].diffuse = colorMid;
		pVertex[ 7].diffuse = colorDown;

		// 벽 2 Back up
		pVertex[ 8].diffuse = colorMid;
		pVertex[ 9].diffuse = colorUp;
		pVertex[10].diffuse = colorMid;
		pVertex[11].diffuse = colorUp;

		// 벽 2 Back down
		pVertex[12].diffuse = colorDown;
		pVertex[13].diffuse = colorMid;
		pVertex[14].diffuse = colorDown;
		pVertex[15].diffuse = colorMid;

		// 벽 3 Left up
		pVertex[16].diffuse = colorMid;
		pVertex[17].diffuse = colorUp;
		pVertex[18].diffuse = colorMid;
		pVertex[19].diffuse = colorUp;

		// 벽 3 Left down
		pVertex[20].diffuse = colorDown;
		pVertex[21].diffuse = colorMid;
		pVertex[22].diffuse = colorDown;
		pVertex[23].diffuse = colorMid;

		// 벽 4 Front up
		pVertex[24].diffuse = colorUp;
		pVertex[25].diffuse = colorMid;
		pVertex[26].diffuse = colorUp;
		pVertex[27].diffuse = colorMid;

		// 벽 4 Front down
		pVertex[28].diffuse = colorMid;
		pVertex[29].diffuse = colorDown;
		pVertex[30].diffuse = colorMid;
		pVertex[31].diffuse = colorDown;

		// 뚜껑
		pVertex[32].diffuse = colorUp;
		pVertex[33].diffuse = colorUp;
		pVertex[34].diffuse = colorUp;
		pVertex[35].diffuse = colorUp;

		m_VB->Unlock();
		return TRUE;
	}

	XIAHGE_API void CSkyBox::SetFogEnable(BOOL bFogEnable)
	{
		m_bFogEnable = bFogEnable;
	}

	XIAHGE_API BOOL CSkyBox::Render()
	{
		if( g_pCurrentCamera == NULL)
			return FALSE;

		Matrix4x4 mat;
		mat.SetScale( 300 ); // 190
		mat.t = g_pCurrentCamera->m_vAt;
//		mat.y -= 12.0f;

		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&mat);
		g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&m_matProjection);

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, m_bFogEnable );
		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE);

		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

		g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		//g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		//g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		//g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

		g_pDirect3DDevice->SetStreamSource( 0, m_VB, 0, sizeof(VT_LVertex) );
		g_pDirect3DDevice->SetVertexShader( NULL);
		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX);
		
		for(int t = 0; t<4 ;t++)
		{
			g_pDirect3DDevice->SetTexture( 0, m_pTexture[t]);
			for(int i=0; i < 2; i++)
			{
				g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, (i*4) + (t*8), 2);
			}
		}

		// TOP
		g_pDirect3DDevice->SetTexture( 0, NULL);
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 32, 2);

		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

		//g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE);
		g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&g_pCurrentCamera->m_matProjection);
		return TRUE;
	}


	//
	//------------------------------------------------------------------------
	//
	//XIAHGE_API CSkyStar	g_SkyStar;

	XIAHGE_API CSkyStar::CSkyStar()
	{
		m_pVB1 = NULL;
		m_pVB2 = NULL;
		m_pVB3 = NULL;
		m_pIB = NULL;
		m_pCloudVB = NULL;
		m_pCloudIB = NULL;
		m_pSunTexture = NULL;
		m_pMoon1Texture = NULL;
		m_pMoon2Texture = NULL;
		m_pStarsTexture = NULL;
		m_pCloudTexture = NULL;
		m_nType = eNONE;
		m_Color = D3DCOLOR_XRGB( 200, 200, 200 );
	}

	XIAHGE_API CSkyStar::~CSkyStar()
	{
		Release();
	}

	XIAHGE_API BOOL CSkyStar::Release()
	{
		if( m_pVB1 ) m_pVB1->Release();
		if( m_pVB2 ) m_pVB2->Release();
		if( m_pVB3 ) m_pVB3->Release();
		if( m_pIB ) m_pIB->Release();
		if( m_pCloudVB ) m_pCloudVB->Release();
		if( m_pCloudIB ) m_pCloudIB->Release();
		m_pVB1 = NULL;
		m_pVB2 = NULL;
		m_pVB3 = NULL;
		m_pIB = NULL;
		m_pCloudVB = NULL;
		m_pCloudIB = NULL;

/*
		if( m_pSunTexture ) m_pSunTexture->Release();
		if( m_pMoon1Texture ) m_pMoon1Texture->Release();
		if( m_pMoon2Texture ) m_pMoon2Texture->Release();
		if( m_pStarsTexture ) m_pStarsTexture->Release();
		if( m_pCloudTexture ) m_pCloudTexture->Release();
		m_pSunTexture = NULL;
		m_pMoon1Texture = NULL;
		m_pMoon2Texture = NULL;
		m_pStarsTexture = NULL;
		m_pCloudTexture = NULL;
*/

		return TRUE;
	}

	XIAHGE_API void CSkyStar::SetType(BYTE nType)
	{
		m_nType = nType;

		if( m_nType == eStars )
			m_dwElapsedTime = 0;
	}

	XIAHGE_API BOOL CSkyStar::Create()
	{
		// vertex buffer
		g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_LVertex), D3DUSAGE_WRITEONLY,
												D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pVB1, NULL );

		g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_LVertex), D3DUSAGE_WRITEONLY,
												D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pVB2, NULL );

		g_pDirect3DDevice->CreateVertexBuffer( STAR_MAX*4*sizeof(VT_LVertex), D3DUSAGE_WRITEONLY,
												D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pVB3, NULL );

		g_pDirect3DDevice->CreateVertexBuffer( CLOUD_MAX*4*sizeof(VT_LVertex), D3DUSAGE_WRITEONLY,
												D3DFVF_LVERTEX, D3DPOOL_MANAGED, &m_pCloudVB, NULL );

		// index buffer
		g_pDirect3DDevice->CreateIndexBuffer( STAR_MAX*4*3*sizeof(WORD), D3DUSAGE_WRITEONLY,
											   D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

        g_pDirect3DDevice->CreateIndexBuffer( CLOUD_MAX*4*3*sizeof(WORD), 0,
											   D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pCloudIB, NULL );

		// texture
		m_pCloudTexture	= XiahPak::GetTexture( 50000416 );
		m_pSunTexture	= XiahPak::GetTexture( 50000417 );
		m_pMoon1Texture	= XiahPak::GetTexture( 50000418 );
		m_pMoon2Texture	= XiahPak::GetTexture( 50000419 );
		m_pStarsTexture	= XiahPak::GetTexture( 50000420 );

/*
		D3DXCreateTextureFromFile( g_pDirect3DDevice, "sun.tga", &m_pSunTexture );
		D3DXCreateTextureFromFile( g_pDirect3DDevice, "moon1.tga", &m_pMoon1Texture );
		D3DXCreateTextureFromFile( g_pDirect3DDevice, "moon2.tga", &m_pMoon2Texture );
		D3DXCreateTextureFromFile( g_pDirect3DDevice, "stars.tga", &m_pStarsTexture );
		D3DXCreateTextureFromFile( g_pDirect3DDevice, "cloud.tga", &m_pCloudTexture );
*/

		// Sun ans Moon
		VT_LVertex* pSunVertex;
		m_pVB1->Lock( 0, 0, (void**)&pSunVertex, 0 );

		for(int i=0; i<4; i++)
            pSunVertex[i].diffuse = m_Color;

		pSunVertex[0].tex.u = 0;
		pSunVertex[0].tex.v = 1;
		pSunVertex[1].tex.u = 0;
		pSunVertex[1].tex.v = 0;
		pSunVertex[2].tex.u = 1;
		pSunVertex[2].tex.v = 1;
		pSunVertex[3].tex.u = 1;
		pSunVertex[3].tex.v = 0;

		m_pVB1->Unlock();

		// 달무리
		VT_LVertex* pMoonVertex;
		m_pVB2->Lock( 0, 0, (void**)&pMoonVertex, 0 );

		for(int i=0; i<4; i++)
            pMoonVertex[i].diffuse = m_Color;

		pMoonVertex[0].tex.u = 0;
		pMoonVertex[0].tex.v = 1;
		pMoonVertex[1].tex.u = 0;
		pMoonVertex[1].tex.v = 0;
		pMoonVertex[2].tex.u = 1;
		pMoonVertex[2].tex.v = 1;
		pMoonVertex[3].tex.u = 1;
		pMoonVertex[3].tex.v = 0;

		m_pVB2->Unlock();

		//
		CreateStarPosition();
		CreateCloudPosition();

		return TRUE;
	}

	XIAHGE_API BOOL CSkyStar::CreateStarPosition()
	{
		// Stars, 이때 위치를 지정해준다.
		VT_LVertex* pStarsVertex;
		m_pVB3->Lock( 0, 0, (void**)&pStarsVertex, 0 );
		WORD* pStarsIndices;
		m_pIB->Lock( 0, 0, (void**)&pStarsIndices, 0 );

		for(int i=0; i<STAR_MAX; i++)
		{
			int nIndex = i * 4;

			float fAngle = rnd() * 6.28f;
			Matrix4x4 matY;
			matY.SetRotationY( fAngle );

			Vector3 vNewPos( rnd(), 0, rnd() );
			vNewPos = vNewPos * matY;

			int nValue = i % 3;
			float fDistance;
			switch( nValue )
			{
			case 0:
				fDistance = rnd() * 50.0f + 30.0f;
				break;
			case 1:
				fDistance = rnd() * 75.0f + 80.0f;
				break;
			case 2:
				fDistance = rnd() * 100.0f + 155.0f;
				break;
			};

			vNewPos *= fDistance;

			//
			m_vStarsPosList[i].x = vNewPos.x;
			m_vStarsPosList[i].y = vNewPos.z;

			// 별이 깜박 거리는 시간.
			m_dwStarsTwinkleTime[i] = 0;
//			if( rand() % 2 )
				m_dwStarsTwinkleTime[i] = 200 + rand() % (STARS_TWINKLE_FREQUENCY-500);

			// 0이면 아무것도 안하고, 1이면 Diffuse 감소, 2이면 Diffuse 증가. 3이면 한번 깜박임
			m_bStarsTwinkleStep[i] = 0;
			m_nStarsTwinkleAlpha[i] = 255;

			for(int j=0; j<4; j++)
				pStarsVertex[ nIndex+j ].diffuse = m_Color;

			// 이미지가 8개가 연결된것으로 가정.
			int nTexture = rand() % 8;
			float fTextureSize = 1.0f / 8.0f;

			pStarsVertex[nIndex+0].tex.u = nTexture * fTextureSize;
			pStarsVertex[nIndex+0].tex.v = 1;
			pStarsVertex[nIndex+1].tex.u = nTexture * fTextureSize;
			pStarsVertex[nIndex+1].tex.v = 0;

			pStarsVertex[nIndex+2].tex.u = (nTexture * fTextureSize) + fTextureSize;
			pStarsVertex[nIndex+2].tex.v = 1;
			pStarsVertex[nIndex+3].tex.u = (nTexture * fTextureSize) + fTextureSize;
			pStarsVertex[nIndex+3].tex.v = 0;

			// index
			WORD* pIndex = &pStarsIndices[i*6];
			pIndex[0] = nIndex + 0;
			pIndex[1] = nIndex + 1;
			pIndex[2] = nIndex + 2;
			pIndex[3] = nIndex + 1;
			pIndex[4] = nIndex + 3;
			pIndex[5] = nIndex + 2;

		}// for

		m_pVB3->Unlock();
		m_pIB->Unlock();

		return TRUE;
	}

	XIAHGE_API BOOL CSkyStar::CreateCloudPosition()
	{
		// cloud
		VT_LVertex* pCloudVertex;
		m_pCloudVB->Lock( 0, 0, (void**)&pCloudVertex, 0 );
		WORD* pCloudIndices;
		m_pCloudIB->Lock( 0, 0, (void**)&pCloudIndices, 0 );

		for(int i=0; i<CLOUD_MAX; i++)
		{
			int nIndex = i * 4;

			float fAngle = rnd() * 6.28f;
			Matrix4x4 matY;
			matY.SetRotationY( fAngle );

			Vector3 vNewPos( rnd(), 0, rnd() );
			vNewPos = vNewPos * matY;

			int nValue = i % 3;
			int nA, nRGB;
			float fDistance;
			switch( nValue )
			{
			case 0:
				fDistance = rnd() * 50.0f + 20.0f;
				nA = 160 + rand() * 70;
				nRGB = 120 + rand() * 100;
				break;
			case 1:
				fDistance = rnd() * 110.0f + 70.0f;
				nA = 100 + rand() * 70;
				nRGB = 110 + rand() * 120;
				break;
			case 2:
				fDistance = rnd() * 175.0f + 180.0f;
				nA = 50 + rand() * 80;
				nRGB = 100 + rand() * 130;
				break;
			};

			vNewPos *= fDistance;

			//
			m_vCloudPosList[i].x = vNewPos.x;
			m_vCloudPosList[i].y = vNewPos.z;

			m_vCloudSizeList[i].x = rnd() * 12.0f + 9.5f;
			m_vCloudSizeList[i].y = rnd() * 4.0f + 2.0f;

			for(int j=0; j<4; j++)
				pCloudVertex[ nIndex+j ].diffuse = m_Color;

			// 하나의 텍스쳐를 이리, 저리 찍어봄.
			int nNum = i % 3;
			switch( nNum )
			{
			case 0:
				pCloudVertex[nIndex+0].tex.u = 0;
				pCloudVertex[nIndex+0].tex.v = 1;
				pCloudVertex[nIndex+1].tex.u = 0;
				pCloudVertex[nIndex+1].tex.v = 0;
				pCloudVertex[nIndex+2].tex.u = 1;
				pCloudVertex[nIndex+2].tex.v = 1;
				pCloudVertex[nIndex+3].tex.u = 1;
				pCloudVertex[nIndex+3].tex.v = 0;
				break;
			case 1:
				pCloudVertex[nIndex+0].tex.u = 1;
				pCloudVertex[nIndex+0].tex.v = 0;
				pCloudVertex[nIndex+1].tex.u = 1;
				pCloudVertex[nIndex+1].tex.v = 1;
				pCloudVertex[nIndex+2].tex.u = 0;
				pCloudVertex[nIndex+2].tex.v = 0;
				pCloudVertex[nIndex+3].tex.u = 0;
				pCloudVertex[nIndex+3].tex.v = 1;
				break;
			case 2:
				pCloudVertex[nIndex+0].tex.u = 0;
				pCloudVertex[nIndex+0].tex.v = 0;
				pCloudVertex[nIndex+1].tex.u = 0;
				pCloudVertex[nIndex+1].tex.v = 1;
				pCloudVertex[nIndex+2].tex.u = 1;
				pCloudVertex[nIndex+2].tex.v = 0;
				pCloudVertex[nIndex+3].tex.u = 1;
				pCloudVertex[nIndex+3].tex.v = 1;
				break;
			};// switch

			// index
			WORD* pIndex = &pCloudIndices[i*6];
			pIndex[0] = nIndex + 0;
			pIndex[1] = nIndex + 1;
			pIndex[2] = nIndex + 2;
			pIndex[3] = nIndex + 1;
			pIndex[4] = nIndex + 3;
			pIndex[5] = nIndex + 2;

		}// for

		m_pCloudVB->Unlock();
		m_pCloudIB->Unlock();

		return TRUE;
	}

	XIAHGE_API BOOL CSkyStar::SetColor(D3DCOLOR color)
	{
		m_Color = color;

		return TRUE;
	}

	XIAHGE_API BOOL CSkyStar::Update()
	{
		D3DMATRIX matView;
		g_pDirect3DDevice->GetTransform( D3DTS_VIEW, &matView );
		m_vView			= Vector3( matView._13, matView._23, matView._33 );
		m_vViewRight	= Vector3( matView._11, matView._21, matView._31 );
		m_vViewUp		= Vector3( matView._12, matView._22, matView._32 );

		switch( m_nType )
		{
		case eSun:
//			UpdateSun();
//			UpdateCloud();
			break;
		case eMoon:
//			UpdateMoon(false);
			break;
		case eStars:
			UpdateStars();
//			UpdateMoon();
		}; // switch

		return TRUE;
	}

	XIAHGE_API BOOL CSkyStar::Render()
	{
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING,		FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );

		Matrix4x4 matWorld;
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&matWorld );

		switch( m_nType )
		{
		case eSun:
//			RenderSun();
//			RenderCloud();
			break;
		case eMoon:
//			RenderMoon(false);
			break;
		case eStars:
			RenderStars();
//			RenderMoon();
		}; // switch

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );

		return TRUE;
	}

	BOOL CSkyStar::UpdateSun()
	{
		// 카메라 위치를 따라서 해를 그린다.
		float fPosX = g_pCurrentCamera->m_vAt.x + 50.0f;
		float fPosY = g_pCurrentCamera->m_vAt.y + 47.0f;
		float fPosZ = g_pCurrentCamera->m_vAt.z + 50.0f;
		Vector3 vPos( fPosX, fPosY, fPosZ );

		// Sun
		float fSunX = 30.0f, fSunY = 27.0f;
		VT_LVertex* pSunVertex;
		m_pVB1->Lock( 0, 0, (void**)&pSunVertex, 0 );

		pSunVertex[0].pos = -m_vViewUp * fSunY - m_vViewRight * fSunX;
		pSunVertex[1].pos =  m_vViewUp * fSunY - m_vViewRight * fSunX;
		pSunVertex[2].pos = -m_vViewUp * fSunY + m_vViewRight * fSunX;
		pSunVertex[3].pos =  m_vViewUp * fSunY + m_vViewRight * fSunX;

		for(int i=0; i<4; i++)
			pSunVertex[i].pos = pSunVertex[i].pos + vPos;

		m_pVB1->Unlock();

		return TRUE;
	}

	BOOL CSkyStar::RenderSun()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		g_pDirect3DDevice->SetTexture( 0, m_pSunTexture );
		g_pDirect3DDevice->SetStreamSource( 0, m_pVB1, 0, sizeof(VT_LVertex) );
		g_Device.SetIndices( NULL );
		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		return TRUE;
	}

	BOOL CSkyStar::UpdateMoon(bool bRenderEdge)
	{
		// 카메라 위치를 따라서 달를 그린다.
		float fPosX = g_pCurrentCamera->m_vAt.x - 50.0f;
		float fPosY = g_pCurrentCamera->m_vAt.y + 45.0f;
		float fPosZ = g_pCurrentCamera->m_vAt.z + 50.0f;
		Vector3 vPos( g_pCurrentCamera->m_vAt.x-50.0f, g_pCurrentCamera->m_vAt.y+45.0f, g_pCurrentCamera->m_vAt.z+50.0f );

		float fSunX = 12.0f, fSunY = 12.0f;

		Vector3 PreCalVector[4];
		PreCalVector[0] = -m_vViewUp * fSunY - m_vViewRight * fSunX;
		PreCalVector[1] =  m_vViewUp * fSunY - m_vViewRight * fSunX;
		PreCalVector[2] = -m_vViewUp * fSunY + m_vViewRight * fSunX;
		PreCalVector[3] =  m_vViewUp * fSunY + m_vViewRight * fSunX;

		// Moon1
		VT_LVertex* pMoon1Vertex;
		m_pVB1->Lock( 0, 0, (void**)&pMoon1Vertex, 0 );

		for(int i=0; i<4; i++)
		{
			pMoon1Vertex[i].pos = PreCalVector[i] + vPos;
			pMoon1Vertex[i].diffuse = D3DCOLOR_ARGB( 230, 230, 230, 230 ); // m_Color;
		}

		m_pVB1->Unlock();

		// Moon2 달무리. 밤에는 어둡게, 새벽에는 밝은 달무리를 만든다.
		VT_LVertex* pMoon2Vertex;
		m_pVB2->Lock( 0, 0, (void**)&pMoon2Vertex, 0 );

		P_COLOR EdgeColor;
		if( bRenderEdge )
			EdgeColor = D3DCOLOR_ARGB( 230, 230, 230, 230 );
		else
			EdgeColor = D3DCOLOR_ARGB( 70, 70, 70, 70 );

		for(i=0; i<4; i++)
		{
			pMoon2Vertex[i].pos = PreCalVector[i] + vPos;
			pMoon2Vertex[i].diffuse = EdgeColor;
		}

		m_pVB2->Unlock();

		return TRUE;
	}

	BOOL CSkyStar::RenderMoon(bool bRenderEdge)
	{
		// Moon1
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		g_pDirect3DDevice->SetTexture( 0, m_pMoon1Texture );
		g_pDirect3DDevice->SetStreamSource( 0, m_pVB1, 0, sizeof(VT_LVertex) );
		g_Device.SetIndices( NULL );
		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		// Moon halo
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		g_pDirect3DDevice->SetTexture( 0, m_pMoon2Texture );
		g_pDirect3DDevice->SetStreamSource( 0, m_pVB2, 0, sizeof(VT_LVertex) );
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		return TRUE;
	}

	BOOL CSkyStar::UpdateStars()
	{
		float fTime = 33.0f * g_fFrameScale;
		m_dwElapsedTime += fTime;

		int nIncDecValue = 900 * fTime / 1000;

		if( m_dwElapsedTime >= STARS_TWINKLE_FREQUENCY )
		{
			m_dwElapsedTime = 0;
			for(int i=0; i<STAR_MAX; i++)
				m_bStarsTwinkleStep[i] = 0;
		}

		float fStarsX = 0.35f, fStarsY = 0.3f;
		Vector3 PreCalVector[4];
		PreCalVector[0] = -m_vViewUp * fStarsY - m_vViewRight * fStarsX;
		PreCalVector[1] =  m_vViewUp * fStarsY - m_vViewRight * fStarsX;
		PreCalVector[2] = -m_vViewUp * fStarsY + m_vViewRight * fStarsX;
		PreCalVector[3] =  m_vViewUp * fStarsY + m_vViewRight * fStarsX;

		// Stars
		VT_LVertex* pStarsVertex;
		m_pVB3->Lock( 0, 0, (void**)&pStarsVertex, 0 );

		for(int i=0; i<STAR_MAX; i++)
		{
			int nIndex = i * 4;

			Vector3 vStarPos( g_pCurrentCamera->m_vAt.x+m_vStarsPosList[i].x, g_pCurrentCamera->m_vAt.y + 45.0f, g_pCurrentCamera->m_vAt.z+m_vStarsPosList[i].y );

			for(int j=0; j<4; j++)
				pStarsVertex[nIndex+j].pos = PreCalVector[j] + vStarPos;

			// 별이 깜박거리도록.
			if( m_dwStarsTwinkleTime[i] )
			{
				switch( m_bStarsTwinkleStep[i] )
				{
				case 0:
					if( m_dwElapsedTime >= m_dwStarsTwinkleTime[i] )
					{
						m_bStarsTwinkleStep[i] = 1;
					}
					break;
				case 1:	// decrease
					{
						int nR = GetBValue( pStarsVertex[nIndex].diffuse );
						int nG = GetGValue( pStarsVertex[nIndex].diffuse );
						int nB = GetRValue( pStarsVertex[nIndex].diffuse );

						m_nStarsTwinkleAlpha[i] -= nIncDecValue;
						if( m_nStarsTwinkleAlpha[i] < 0 )
						{
							m_nStarsTwinkleAlpha[i] = 0;
							m_bStarsTwinkleStep[i] = 2;
						}

						for(int j=0; j<4; j++)
                            pStarsVertex[nIndex+j].diffuse = D3DCOLOR_ARGB( m_nStarsTwinkleAlpha[i], nR, nG, nB );
					}
					break;
				case 2:	// increase
					{
						int nR = GetBValue( pStarsVertex[nIndex].diffuse );
						int nG = GetGValue( pStarsVertex[nIndex].diffuse );
						int nB = GetRValue( pStarsVertex[nIndex].diffuse );

						m_nStarsTwinkleAlpha[i] += nIncDecValue;
						if( m_nStarsTwinkleAlpha[i] > 255 )
						{
							m_nStarsTwinkleAlpha[i] = 255;
							m_bStarsTwinkleStep[i] = 3;
						}

						for(int j=0; j<4; j++)
                            pStarsVertex[nIndex+j].diffuse = D3DCOLOR_ARGB( m_nStarsTwinkleAlpha[i], nR, nG, nB );
					}
					break;
				};// switch

			}// if

		}// for

		m_pVB3->Unlock();

		return TRUE;
	}

	BOOL CSkyStar::RenderStars()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );//D3DBLEND_ONE
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		g_pDirect3DDevice->SetTexture( 0, m_pStarsTexture );
		g_pDirect3DDevice->SetStreamSource( 0, m_pVB3, 0, sizeof(VT_LVertex) );
		g_Device.SetIndices( m_pIB );
		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );

		g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, STAR_MAX*4, 0, STAR_MAX*2 );

		return TRUE;
	}

	BOOL CSkyStar::UpdateCloud()
	{
		VT_LVertex* pCloudVertex;
		m_pCloudVB->Lock( 0, 0, (void**)&pCloudVertex, 0 );

		for(int i=0; i<CLOUD_MAX; i++)
		{
			int nIndex = i * 4;

			pCloudVertex[nIndex+0].pos = -m_vViewUp * m_vCloudSizeList[i].y - m_vViewRight * m_vCloudSizeList[i].x;
			pCloudVertex[nIndex+1].pos =  m_vViewUp * m_vCloudSizeList[i].y - m_vViewRight * m_vCloudSizeList[i].x;
			pCloudVertex[nIndex+2].pos = -m_vViewUp * m_vCloudSizeList[i].y + m_vViewRight * m_vCloudSizeList[i].x;
			pCloudVertex[nIndex+3].pos =  m_vViewUp * m_vCloudSizeList[i].y + m_vViewRight * m_vCloudSizeList[i].x;

			Vector3 vCloudPos( g_pCurrentCamera->m_vAt.x+m_vCloudPosList[i].x, g_pCurrentCamera->m_vAt.y + 28.0f, g_pCurrentCamera->m_vAt.z+m_vCloudPosList[i].y );

			for(int j=0; j<4; j++)
			{
				pCloudVertex[nIndex+j].pos = pCloudVertex[nIndex+j].pos + vCloudPos;
			}

		}// for

		m_pCloudVB->Unlock();

		return TRUE;
	}

	BOOL CSkyStar::RenderCloud()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );//D3DBLEND_ONE
		g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		g_pDirect3DDevice->SetTexture( 0, m_pCloudTexture );
		g_pDirect3DDevice->SetStreamSource( 0, m_pCloudVB, 0, sizeof(VT_LVertex) );
		g_Device.SetIndices( m_pCloudIB );
		g_pDirect3DDevice->SetFVF( D3DFVF_LVERTEX );
		g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, CLOUD_MAX*4, 0, CLOUD_MAX*2 );

		return TRUE;
	}

};