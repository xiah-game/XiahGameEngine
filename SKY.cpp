#include "stdafx.h"
#include "SKY.h"
#include "XiahPak.h"
#include <time.h>


namespace XiahGameEngine
{

#define SKY_GRID_WIDTH  16
#define SKY_GRID_HEIGHT 16
#define SKY_GRID_SIZE   80
#define SKY_ALTITUDE    2
#define SKY_RADIUS      10000
#define SKY_TILE        4

#define SUN_SIZE  50
#define SUN_HALO_SIZE 1

#define MOON_SIZE 400
#define MOON_HALO_SIZE 500

#define MRAND(x) (((float) rand() / RAND_MAX) * x)

const DWORD SKY_VERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

const DWORD PLANET_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

#define	SKY_FOV					D3DX_PI / 4	// Sky Projection matrix 설정
#define	SKY_ASPECT				1			// Sky Projection matrix 설정
#define	SKY_NEAR_DISTANCE		1.0f		// Sky Projection matrix 설정
#define	SKY_FAR_DISTANCE		2000.0f		// Sky Projection matrix 설정

XIAHGE_API CSKY	g_Sky;

XIAHGE_API CSKY::CSKY()
{
	m_pHighTexture				= NULL;
	m_pLowTexture				= NULL;

    m_pSunTexture               = NULL;
    m_pSunHaloTexture           = NULL;
    m_pMoonTexture              = NULL;
    m_pMoonHaloTexture          = NULL;

    m_vSunPos       = D3DXVECTOR3(0, 200, 2000);
    m_vSunHaloScale = D3DXVECTOR3(2000, 2000, 1);
    m_vMoonPos      = D3DXVECTOR3(2000, 2000, 0);

	m_pSkyLowVertices			= NULL;
	m_pSkyHighVertices			= NULL;
	m_pSkyIndies				= NULL;
	m_pFogDomeVertices			= NULL;
	m_pFogDomeIndies			= NULL;

	m_dwNumVertices				= (SKY_GRID_WIDTH + 1) * (SKY_GRID_HEIGHT + 1);
	m_dwNumPrimitives			= SKY_GRID_WIDTH * SKY_GRID_HEIGHT * 2;

	m_dwNumFogDomeVertices		= (FOGDOME_GRID_WIDTH + 1) * (FOGDOME_GRID_HEIGHT + 1);
	m_dwNumFogDomePrimitives	= FOGDOME_GRID_WIDTH * FOGDOME_GRID_HEIGHT * 2;

	m_dwSkyColor				= D3DCOLOR_RGBA(126, 165, 250, 255);

//	m_fLowWindSpeed				= 0.0002f;
//	m_fHighWindSpeed			= 0.00005f;
	m_fLowWindSpeed				= 0.006f;
	m_fHighWindSpeed			= 0.0015f;
	
	m_fLowCloudPos				= 0;
	m_fHighCloudPos				= 0;

	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld._42				= 100.0f;

    m_nNumClouds = 50;

	Init();
}

//--------------------------------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------------------------------
XIAHGE_API CSKY::~CSKY()
{
	DestroyData();
}

#define SAFE_DELETE_ARRAY_( array) \
	if( array != NULL)\
{\
	delete [] array;\
	array = NULL;\
}

XIAHGE_API void CSKY::DestroyData()
{
	SAFE_DELETE_ARRAY_( m_pSkyLowVertices );
	SAFE_DELETE_ARRAY_( m_pSkyHighVertices );
	SAFE_DELETE_ARRAY_( m_pSkyIndies );
	SAFE_DELETE_ARRAY_( m_pFogDomeVertices );
	SAFE_DELETE_ARRAY_( m_pFogDomeIndies );
}

XIAHGE_API bool CSKY::Load(/* const char* name */)
{
//	m_strFileName = name;
//
//	FILE*	pFile;
//
//	char    buff[ 256 ], cTemp[ 256 ];
//
//    pFile = fopen( name, "r" );
//    if( pFile == NULL )
//    {
//		return false;
//	}
//
////	Texture
//	fgets(buff, sizeof(buff), pFile);
//
//	fgets(buff, sizeof(buff), pFile);
//
//	fgets(buff, sizeof(buff), pFile);
//	sscanf(buff, "%s", cTemp);
//	D3DXCreateTextureFromFile( g_pDirect3DDevice, cTemp, &m_pHighTexture );
//	//m_pHighTexture = g_E3DResourceDataManager.RequestTexture(cTemp);
	
	m_pHighTexture = XiahPak::GetTexture(50002669, true);

//	fgets(buff, sizeof(buff), pFile);
//	sscanf(buff, "%s", cTemp);
//	//m_pLowTexture = g_E3DResourceDataManager.RequestTexture(cTemp);
//	D3DXCreateTextureFromFile( g_pDirect3DDevice, cTemp, &m_pLowTexture );

	m_pLowTexture = XiahPak::GetTexture(50002670, true);

//	fgets(buff, sizeof(buff), pFile);
//	sscanf(buff, "%s", cTemp);
//	//m_pSunTexture = g_E3DResourceDataManager.RequestTexture(cTemp);
//	D3DXCreateTextureFromFile( g_pDirect3DDevice, cTemp, &m_pSunTexture );

	m_pSunTexture = XiahPak::GetTexture(50002673, true);

//	fgets(buff, sizeof(buff), pFile);
//	sscanf(buff, "%s", cTemp);
//	//m_pSunHaloTexture = g_E3DResourceDataManager.RequestTexture(cTemp);
//	D3DXCreateTextureFromFile( g_pDirect3DDevice, cTemp, &m_pSunHaloTexture );

	m_pSunHaloTexture = XiahPak::GetTexture(50002674, true);

//	fgets(buff, sizeof(buff), pFile);
//	sscanf(buff, "%s", cTemp);
//	//m_pMoonTexture = g_E3DResourceDataManager.RequestTexture(cTemp);
//	D3DXCreateTextureFromFile( g_pDirect3DDevice, cTemp, &m_pMoonTexture );

	m_pMoonTexture = XiahPak::GetTexture(50002671, true);

//	fgets(buff, sizeof(buff), pFile);
//	sscanf(buff, "%s", cTemp);
//	//m_pMoonHaloTexture = g_E3DResourceDataManager.RequestTexture(cTemp);
//	D3DXCreateTextureFromFile( g_pDirect3DDevice, cTemp, &m_pMoonHaloTexture );

	m_pMoonHaloTexture = XiahPak::GetTexture(50002672, true);

//	fgets(buff, sizeof(buff), pFile);
//
//
//    fclose(pFile);

	return true;
}

//--------------------------------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------------------------------
XIAHGE_API HRESULT CSKY::Init()
{
	m_pFogDomeIndies   = new WORD[(FOGDOME_GRID_WIDTH + 1) * (FOGDOME_GRID_HEIGHT + 1) * 6];
	m_pFogDomeVertices = new e3d_dif_vertex[(FOGDOME_GRID_WIDTH + 1) * (FOGDOME_GRID_HEIGHT + 1)];
	m_pSkyIndies       = new WORD[(SKY_GRID_WIDTH + 1) * (SKY_GRID_HEIGHT + 1) * 6];
	m_pSkyLowVertices  = new e3d_dif_tex1_vertex[(SKY_GRID_WIDTH + 1) * (SKY_GRID_HEIGHT + 1)];
	m_pSkyHighVertices = new e3d_dif_tex1_vertex[(SKY_GRID_WIDTH + 1) * (SKY_GRID_HEIGHT + 1)];

	e3d_dif_tex1_vertex* pLowVertex, *pHighVertex;
	e3d_dif_vertex*      pFogDomeVertex;
	WORD*             pIndi;

	for(int j = 0; j <= SKY_GRID_HEIGHT; j++)
	{
		for(int i = 0; i <= SKY_GRID_WIDTH; i++)
		{
			pLowVertex = &(m_pSkyLowVertices[i+ j * (SKY_GRID_WIDTH+1)]);

			pLowVertex->x  = (i - SKY_GRID_WIDTH / 2) * SKY_GRID_SIZE;
			pLowVertex->z  = (j - SKY_GRID_HEIGHT / 2) * SKY_GRID_SIZE;

			float dist2 = pLowVertex->x * pLowVertex->x + pLowVertex->z * pLowVertex->z;
			pLowVertex->y = sqrt(SKY_RADIUS * SKY_RADIUS - dist2) - SKY_RADIUS + SKY_ALTITUDE;

			int alpha =  (pLowVertex->y / SKY_ALTITUDE) * 255;
			if(alpha < 0) alpha = 0;

			pLowVertex->color = D3DCOLOR_RGBA(255, 255, 255, alpha);

			pLowVertex->u = ((float) i) / SKY_GRID_WIDTH  * SKY_TILE;
			pLowVertex->v = ((float) j) / SKY_GRID_HEIGHT * SKY_TILE;

			pHighVertex = &(m_pSkyHighVertices[i+ j * (SKY_GRID_WIDTH+1)]);

			memcpy(pHighVertex, pLowVertex, sizeof(e3d_dif_tex1_vertex));
		}
	}

	for(j = 0; j < SKY_GRID_HEIGHT; j++)
	{
		for(int i = 0; i < SKY_GRID_WIDTH; i++)
		{
			pIndi = &(m_pSkyIndies[(i + j * SKY_GRID_WIDTH) * 6]);

			pIndi[0] = i + j * (SKY_GRID_WIDTH + 1);
			pIndi[1] = (i + 1)+ j * (SKY_GRID_WIDTH + 1);
			pIndi[2] = i + (j+1) * (SKY_GRID_WIDTH + 1);

			pIndi[3] = (i+1) + j * (SKY_GRID_WIDTH + 1);
			pIndi[4] = (i+1) + (j+1) * (SKY_GRID_WIDTH + 1);
			pIndi[5] = i + (j+1) * (SKY_GRID_WIDTH + 1);
		}
	}


	for(j = 0; j <= FOGDOME_GRID_HEIGHT; j++)
	{
		for(int i = 0; i <= FOGDOME_GRID_WIDTH; i++)
		{
			pFogDomeVertex = &(m_pFogDomeVertices[i+ j * (FOGDOME_GRID_WIDTH+1)]);

			pFogDomeVertex->x  = (i - FOGDOME_GRID_WIDTH / 2) * FOGDOME_GRID_SIZE;
			pFogDomeVertex->z  = (j - FOGDOME_GRID_HEIGHT / 2) * FOGDOME_GRID_SIZE;

			float dist2 = pFogDomeVertex->x * pFogDomeVertex->x + pFogDomeVertex->z * pFogDomeVertex->z;
			pFogDomeVertex->y = sqrt(FOGDOME_RADIUS * FOGDOME_RADIUS - dist2) - FOGDOME_RADIUS + FOGDOME_ALTITUDE;

			pFogDomeVertex->color = m_dwSkyColor; //D3DCOLOR_RGBA(155, 0, 0, 0); //m_dwSkyColor;// D3DCOLOR_RGBA(255, 255, 255, alpha);
		}
	}

	for(j = 0; j < FOGDOME_GRID_HEIGHT; j++)
	{
		for(int i = 0; i < FOGDOME_GRID_WIDTH; i++)
		{
			pIndi = &(m_pFogDomeIndies[(i + j * FOGDOME_GRID_WIDTH) * 6]);

			pIndi[0] = i + j * (FOGDOME_GRID_WIDTH + 1);
			pIndi[1] = (i + 1)+ j * (FOGDOME_GRID_WIDTH + 1);
			pIndi[2] = i + (j+1) * (FOGDOME_GRID_WIDTH + 1);

			pIndi[3] = (i+1) + j * (FOGDOME_GRID_WIDTH + 1);
			pIndi[4] = (i+1) + (j+1) * (FOGDOME_GRID_WIDTH + 1);
			pIndi[5] = i + (j+1) * (FOGDOME_GRID_WIDTH + 1);
		}
	}

    SKY_VERTEX *vb = m_LowCloudVB;

    for(int i = 0; i < LOW_CLOUD_TILE ; i++)
    {
        for(j = 0; j < LOW_CLOUD_TILE; j++)
        {
            vb->p = D3DXVECTOR3(-1, 0, -1) * (LOW_CLOUD_SIZE / 2);
            vb->tu = i * (1.f / LOW_CLOUD_TILE); vb->tv = j * (1.f / LOW_CLOUD_TILE);
            vb++;

            vb->p = D3DXVECTOR3( 1, 0, -1) * (LOW_CLOUD_SIZE / 2);
            vb->tu = (i+1) * (1.f / LOW_CLOUD_TILE); vb->tv = j * (1.f / LOW_CLOUD_TILE);
            vb++;

            vb->p = D3DXVECTOR3( 1, 0, 1) * (LOW_CLOUD_SIZE / 2);
            vb->tu = (i+1) * (1.f / LOW_CLOUD_TILE); vb->tv = (j+1) * (1.f / LOW_CLOUD_TILE);
            vb++;

            vb->p = D3DXVECTOR3( -1, 0, 1) * (LOW_CLOUD_SIZE / 2);
            vb->tu = i * (1.f / LOW_CLOUD_TILE); vb->tv = (j+1) * (1.f / LOW_CLOUD_TILE);
            vb++;
        }
    }


    vb = m_HighCloudVB;

    for(i = 0; i < HIGH_CLOUD_TILE ; i++)
    {
        for(j = 0; j < HIGH_CLOUD_TILE; j++)
        {
            vb->p = D3DXVECTOR3(-1, 0, -1) * (HIGH_CLOUD_SIZE / 2);
            vb->tu = i * (1.f / HIGH_CLOUD_TILE); vb->tv = j * (1.f / HIGH_CLOUD_TILE);
            vb++;

            vb->p = D3DXVECTOR3( 1, 0, -1) * (HIGH_CLOUD_SIZE / 2);
            vb->tu = (i+1) * (1.f / HIGH_CLOUD_TILE); vb->tv = j * (1.f / HIGH_CLOUD_TILE);
            vb++;

            vb->p = D3DXVECTOR3( 1, 0, 1) * (HIGH_CLOUD_SIZE / 2);
            vb->tu = (i+1) * (1.f / HIGH_CLOUD_TILE); vb->tv = (j+1) * (1.f / HIGH_CLOUD_TILE);
            vb++;

            vb->p = D3DXVECTOR3( -1, 0, 1) * (HIGH_CLOUD_SIZE / 2);
            vb->tu = i * (1.f / HIGH_CLOUD_TILE); vb->tv = (j+1) * (1.f / HIGH_CLOUD_TILE);
            vb++;
        }
    }

    srand( (unsigned)time( NULL ) );


    for(i = 0; i < NUM_LOW_CLOUDS; i++)
    {
        m_LowClouds[i].pos = D3DXVECTOR3(MRAND(2048) - 1024, 100, MRAND(2048) - 1024);
        m_LowClouds[i].kind = (int) MRAND(4);
    }

    for(i = 0; i < NUM_HIGH_CLOUDS; i++)
    {
        m_HighClouds[i].pos = D3DXVECTOR3(MRAND(2048) - 1024, 200, MRAND(2048) - 1024);
        m_HighClouds[i].kind = (int) MRAND(16);
    }

    m_LowCloudColor  = D3DCOLOR_RGBA(255, 255, 255, 255);
    m_HighCloudColor = D3DCOLOR_RGBA(255, 255, 255, 255);

    m_SunColor     = D3DCOLOR_RGBA(255, 255, 200, 13);
    m_SunHaloColor = D3DCOLOR_RGBA(255, 128, 128, 255);
    m_MoonColor    = D3DCOLOR_RGBA(255, 255, 255, 255);
    m_MoonHaloColor = D3DCOLOR_RGBA(255, 255, 255, 255);
      
    m_SunVB[0].p = D3DXVECTOR3(-1, 1, 0) * SUN_SIZE;
    m_SunVB[0].tu = 0; m_SunVB[0].tv = 1;
    m_SunVB[1].p = D3DXVECTOR3( 1, 1, 0) * SUN_SIZE;
    m_SunVB[1].tu = 0; m_SunVB[1].tv = 0;
    m_SunVB[2].p = D3DXVECTOR3( 1,-1, 0) * SUN_SIZE;
    m_SunVB[2].tu = 1; m_SunVB[2].tv = 0;
    m_SunVB[3].p = D3DXVECTOR3(-1,-1, 0) * SUN_SIZE;
    m_SunVB[3].tu = 1; m_SunVB[3].tv = 1;

    m_SunHaloVB[0].p = D3DXVECTOR3(-1, 1, 0) * SUN_HALO_SIZE;
    m_SunHaloVB[0].tu = 0; m_SunHaloVB[0].tv = 1;
    m_SunHaloVB[1].p = D3DXVECTOR3( 1, 1, 0) * SUN_HALO_SIZE;
    m_SunHaloVB[1].tu = 0; m_SunHaloVB[1].tv = 0;
    m_SunHaloVB[2].p = D3DXVECTOR3( 1,-1, 0) * SUN_HALO_SIZE;
    m_SunHaloVB[2].tu = 1; m_SunHaloVB[2].tv = 0;
    m_SunHaloVB[3].p = D3DXVECTOR3(-1,-1, 0) * SUN_HALO_SIZE;
    m_SunHaloVB[3].tu = 1; m_SunHaloVB[3].tv = 1;


    m_MoonVB[0].p = D3DXVECTOR3(-1, 1, 0) * MOON_SIZE;
    m_MoonVB[0].tu = 0; m_MoonVB[0].tv = 0;
    m_MoonVB[1].p = D3DXVECTOR3( 1, 1, 0) * MOON_SIZE;
    m_MoonVB[1].tu = 0; m_MoonVB[1].tv = 1;
    m_MoonVB[2].p = D3DXVECTOR3( 1,-1, 0) * MOON_SIZE;
    m_MoonVB[2].tu = 1; m_MoonVB[2].tv = 1;
    m_MoonVB[3].p = D3DXVECTOR3(-1,-1, 0) * MOON_SIZE;
    m_MoonVB[3].tu = 1; m_MoonVB[3].tv = 0;

    m_MoonHaloVB[0].p = D3DXVECTOR3(-1, 1, 0) * MOON_HALO_SIZE;
    m_MoonHaloVB[0].tu = 0; m_MoonHaloVB[0].tv = 0;
    m_MoonHaloVB[1].p = D3DXVECTOR3( 1, 1, 0) * MOON_HALO_SIZE;
    m_MoonHaloVB[1].tu = 0; m_MoonHaloVB[1].tv = 1;
    m_MoonHaloVB[2].p = D3DXVECTOR3( 1,-1, 0) * MOON_HALO_SIZE;
    m_MoonHaloVB[2].tu = 1; m_MoonHaloVB[2].tv = 1;
    m_MoonHaloVB[3].p = D3DXVECTOR3(-1,-1, 0) * MOON_HALO_SIZE;
    m_MoonHaloVB[3].tu = 1; m_MoonHaloVB[3].tv = 0;

	return S_OK;
}

XIAHGE_API HRESULT CSKY::FrameMove(D3DXVECTOR3& v3ViewPos, float fElapsedTime)
{
	m_matWorld._41 = v3ViewPos.x;
	m_matWorld._42 = v3ViewPos.y;
	m_matWorld._43 = v3ViewPos.z;

    //  Cloud
    for(int i = 0; i < NUM_LOW_CLOUDS; i++)
    {
        m_LowClouds[i].pos.x += (m_fLowWindSpeed * fElapsedTime) * 500.f;

        if(m_LowClouds[i].pos.x > 1024) m_LowClouds[i].pos.x = -1024;
    }

    for(i = 0; i < NUM_HIGH_CLOUDS; i++)
    {
        m_HighClouds[i].pos.x += (m_fHighWindSpeed * fElapsedTime) * 500.f;

        if(m_HighClouds[i].pos.x > 1024) m_HighClouds[i].pos.x = -1024;
    }

/*
	D3DXVec3Project((D3DXVECTOR3*)&m_vSunPos, 
					(D3DXVECTOR3*)&(m_v3LightPos), 
					g_pCamera->GetViewport(), 
					(D3DXMATRIX*)&E3DGetCamera()->GetProjMatrix(), 
					(D3DXMATRIX*)&(E3DGetCamera()->GetViewMatrix()),
					(D3DXMATRIX*)&D3DXMATRIXA16::IDENTITY);
*/

	m_vSunPos.z = 0;

/*
    int c;

    c = fabs(st) * 1024;
    if(c > 255) c =255;

    m_SunColor = D3DCOLOR_RGBA( 255, 255, c, 13);

    m_SunHaloColor = D3DCOLOR_RGBA(255, c, c, 255);

    c = -st * 255;
    if(c < 0) c = 0;

    m_MoonColor = D3DCOLOR_RGBA( c, c, c, 255);
    m_MoonHaloColor = D3DCOLOR_RGBA(c, c, c, 255);

    c = (fabs(ct) - 0.93) * 16000;
    if(c < 0) c = 0;

    m_vSunHaloScale.x = 500 + c * 2; 
    m_vSunHaloScale.y = 500 + c; 
*/

	return S_OK;
}

XIAHGE_API HRESULT CSKY::Render()
{ 
	g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    int i;
    D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matProjection;
	D3DXMATRIXA16 matBackupProj;
	
	g_pDirect3DDevice->GetTransform( D3DTS_PROJECTION, (D3DXMATRIX *) &matBackupProj );
	//float fAspect = E3DGetCamera()->GetAspect();// (float) pRenderer->GetBackBufferWidth() / (float) pRenderer->GetBackBufferHeight();
	D3DXMatrixPerspectiveFovLH( ( D3DXMATRIX* )&matProjection, SKY_FOV, SKY_ASPECT, SKY_NEAR_DISTANCE, SKY_FAR_DISTANCE );
	g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, (D3DXMATRIX* )&matProjection );

	D3DXVECTOR3 eye = ( D3DXVECTOR3 )g_pCurrentCamera->m_vFrom;
	m_matWorld._41 = eye.x;
	m_matWorld._42 = eye.y;
	m_matWorld._43 = eye.z;
	
	g_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,					false);

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    D3DXMATRIXA16 matView;
    D3DXMATRIXA16 matScale;

/*
    //--------------------------------------------------------------------------------------------------
    //  Sun
	//E3DAPI_Alpha(true);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	true );
	//E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCCOLOR );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x01 );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );
    g_pDirect3DDevice->SetFVF( PLANET_VERTEX::FVF );

    g_pDirect3DDevice->GetTransform( D3DTS_VIEW, ( D3DXMATRIX* )&matView );
	D3DXMatrixInverse( &matWorld, NULL, &matView );
	matWorld._41 += m_vSunPos.x;
	matWorld._42 += m_vSunPos.y;
	matWorld._43 += m_vSunPos.z;
    
    m_SunHaloVB[0].p = D3DXVECTOR3(-m_vSunHaloScale.x, m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;
    m_SunHaloVB[1].p = D3DXVECTOR3(-m_vSunHaloScale.x,-m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;
    m_SunHaloVB[2].p = D3DXVECTOR3( m_vSunHaloScale.x,-m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;
    m_SunHaloVB[3].p = D3DXVECTOR3( m_vSunHaloScale.x, m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;

	g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&( D3DXMATRIXA16::IDENTITY ) );

    g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_SunHaloColor );
    g_pDirect3DDevice->SetTexture( 0, m_pSunHaloTexture );
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_SunHaloVB, sizeof(PLANET_VERTEX));


    m_SunVB[0].p = D3DXVECTOR3(-SUN_SIZE, SUN_SIZE, 0) + m_vSunPos;
    m_SunVB[1].p = D3DXVECTOR3(-SUN_SIZE,-SUN_SIZE, 0) + m_vSunPos;
    m_SunVB[2].p = D3DXVECTOR3( SUN_SIZE,-SUN_SIZE, 0) + m_vSunPos;
    m_SunVB[3].p = D3DXVECTOR3( SUN_SIZE, SUN_SIZE, 0) + m_vSunPos;

    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_SunColor);
    g_pDirect3DDevice->SetTexture(0, m_pSunTexture );
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_SunVB, sizeof(PLANET_VERTEX));
*/

	//--------------------------------------------------------------------------------------------------
    //  Sun
	//E3DAPI_Alpha(true);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	true );
	//E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCCOLOR );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x01 );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );
	g_Device.SetFVF(PLANET_VERTEX::FVF);
    //g_pDirect3DDevice->SetFVF( PLANET_VERTEX::FVF );

    g_pDirect3DDevice->GetTransform( D3DTS_VIEW, ( D3DXMATRIX* )&matView );
	D3DXMatrixInverse( &matWorld, NULL, &matView );
	matWorld._41 += m_vSunPos.x;
	matWorld._42 += m_vSunPos.y;
	matWorld._43 += m_vSunPos.z;
	matWorld = m_matWorld * matWorld;

	g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&matWorld );
    
    //m_SunHaloVB[0].p = D3DXVECTOR3(-m_vSunHaloScale.x, m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;
    //m_SunHaloVB[1].p = D3DXVECTOR3(-m_vSunHaloScale.x,-m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;
    //m_SunHaloVB[2].p = D3DXVECTOR3( m_vSunHaloScale.x,-m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;
    //m_SunHaloVB[3].p = D3DXVECTOR3( m_vSunHaloScale.x, m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + m_vSunPos;

	//g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&( D3DXMATRIXA16::IDENTITY ) );

    g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_SunHaloColor );
	g_Device.SetTexture(0, m_pSunHaloTexture);
    //g_pDirect3DDevice->SetTexture( 0, m_pSunHaloTexture );
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_SunHaloVB, sizeof(PLANET_VERTEX));


    //m_SunVB[0].p = D3DXVECTOR3(-SUN_SIZE, SUN_SIZE, 0) + m_vSunPos;
    //m_SunVB[1].p = D3DXVECTOR3(-SUN_SIZE,-SUN_SIZE, 0) + m_vSunPos;
    //m_SunVB[2].p = D3DXVECTOR3( SUN_SIZE,-SUN_SIZE, 0) + m_vSunPos;
    //m_SunVB[3].p = D3DXVECTOR3( SUN_SIZE, SUN_SIZE, 0) + m_vSunPos;

    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_SunColor);
	g_Device.SetTexture(0, m_pSunTexture);
    //g_pDirect3DDevice->SetTexture(0, m_pSunTexture );
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_SunVB, sizeof(PLANET_VERTEX));
	

    //--------------------------------------------------------------------------------------------------
    //  Sky

	//E3DAPI_Alpha(true);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	true );
	//E3DAPI_AlphaBlend(D3DBLEND_ONE, D3DBLEND_INVSRCCOLOR);
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_ONE);
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCCOLOR);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x01);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL);

    g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

    g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&m_matWorld );
	/*D3DXMATRIXA16 aaa;
	D3DXMatrixIdentity( &aaa );
	aaa._11 += 1.2f;
	aaa._22 += 3.0f;
	aaa._33 += 1.2f;
	aaa._42 += 100.0f;
	g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&aaa );*/

    g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_dwSkyColor );

	g_Device.SetTexture(0, NULL);
	//g_pDirect3DDevice->SetTexture( 0, NULL );
	g_Device.SetFVF(D3DFVF_DIF_VERTEX);
	//g_pDirect3DDevice->SetFVF( D3DFVF_DIF_VERTEX );
	g_pDirect3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwNumFogDomeVertices, m_dwNumFogDomePrimitives, m_pFogDomeIndies, 
										 D3DFMT_INDEX16, m_pFogDomeVertices, sizeof(e3d_dif_vertex));

    g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );


    //--------------------------------------------------------------------------------------------------
    // Moon
    g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	//E3DAPI_Alpha(true);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	true );
	//E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x01 );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );

    D3DXMATRIXA16 matCloud;
	D3DXMatrixIdentity( &matCloud );

    g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	g_Device.SetFVF(SKY_VERTEX::FVF);
    //g_pDirect3DDevice->SetFVF( SKY_VERTEX::FVF );

	D3DXMatrixInverse( &matWorld, NULL, &matView );
    matWorld._41 += m_vMoonPos.x;
	matWorld._42 += m_vMoonPos.y;
	matWorld._43 += m_vMoonPos.z;
    g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&matWorld );

    g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	//E3DAPI_AlphaBlend( D3DBLEND_ONE, D3DBLEND_ONE );
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_ONE );
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x01 );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );

    g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_MoonColor );
	g_Device.SetTexture(0, m_pMoonTexture);
    //g_pDirect3DDevice->SetTexture( 0, m_pMoonTexture );
    g_pDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_MoonVB, sizeof( SKY_VERTEX ) );

    g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );


    //--------------------------------------------------------------------------------------------------
    // Cloud
	//E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x01 );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );

	g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_HighCloudColor );
	g_Device.SetTexture(0, m_pHighTexture);
    //g_pDirect3DDevice->SetTexture( 0, m_pHighTexture );

    for( i = 0; i < NUM_HIGH_CLOUDS; i++ )
    {
        matCloud._41 = m_HighClouds[ i ].pos.x;
		matCloud._42 = m_HighClouds[ i ].pos.y;
		matCloud._43 = m_HighClouds[ i ].pos.z;

        matWorld = m_matWorld * matCloud; // * matOldWorld;
        g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&matWorld );
        g_pDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_HighCloudVB + m_HighClouds[ i ].kind * 4, sizeof( SKY_VERTEX ) );
    }

    g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_LowCloudColor );
	g_Device.SetTexture(0, m_pLowTexture);
    //g_pDirect3DDevice->SetTexture( 0, m_pLowTexture );

    for( i = 0; i < NUM_LOW_CLOUDS; i++ )
    {
        matCloud._41 = m_LowClouds[ i ].pos.x;
		matCloud._42 = m_LowClouds[ i ].pos.y;
		matCloud._43 = m_LowClouds[ i ].pos.z;

        matWorld = m_matWorld * matCloud;
        g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&matWorld );
        g_pDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_LowCloudVB + m_LowClouds[ i ].kind * 4, sizeof( SKY_VERTEX ) );
    }

    g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	//E3DAPI_Alpha(false);
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	false );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	false );
    //E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 0x08);
	g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x08 );
	g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );
	g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, true );

//	월드 행렬을 되돌려 준다.
	D3DXMatrixIdentity( &matWorld );
    g_pDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DXMATRIX* )&matWorld );

	g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, ( D3DXMATRIX* )&matBackupProj );

	return S_OK;
}

/*
HRESULT CSKY::RenderReflection()
{
    int i;
    D3DXMATRIXA16 matWorld, matOldWorld;

	g_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pDirect3DDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 0);

	g_pDirect3DDevice->GetTransform(D3DTS_WORLD, (D3DXMATRIX*)&matOldWorld);
	g_pDirect3DDevice->MultiplyTransform(D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld);

	g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,					false);

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

    D3DXMATRIXA16 matView;
    D3DXMATRIXA16 matScale;

    //--------------------------------------------------------------------------------------------------
    //  Sun
	D3DXVECTOR3 vSun2DPos;

	D3DXVec3Project((D3DXVECTOR3*)&vSun2DPos, 
					(D3DXVECTOR3*)&(m_vSunPos), 
					E3DGetCamera()->GetViewport(), 
					(D3DXMATRIX*)&E3DGetCamera()->GetProjMatrix(), 
					(D3DXMATRIX*)&E3DGetCamera()->GetViewMatrix(),
					(D3DXMATRIX*)&matOldWorld);
	vSun2DPos.z = 0;


	E3DAPI_Alpha(true);
	E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    g_pDirect3DDevice->SetFVF(PLANET_VERTEX::FVF);

    g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    g_pDirect3DDevice->GetTransform(D3DTS_VIEW, (D3DXMATRIX *) &matView);
    MatrixInverse(&matWorld, &matView);
    matWorld.m_VT += (matOldWorld * m_vSunPos);

    m_SunHaloVB[0].p = D3DXVECTOR3(-m_vSunHaloScale.x, m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + vSun2DPos;
    m_SunHaloVB[1].p = D3DXVECTOR3(-m_vSunHaloScale.x,-m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + vSun2DPos;
    m_SunHaloVB[2].p = D3DXVECTOR3( m_vSunHaloScale.x,-m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + vSun2DPos;
    m_SunHaloVB[3].p = D3DXVECTOR3( m_vSunHaloScale.x, m_vSunHaloScale.y, 0) * SUN_HALO_SIZE + vSun2DPos;

	g_pDirect3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX *) &(CMatrix::IDENTITY));

    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_SunHaloColor);
    g_pDirect3DDevice->SetTexture(0, m_pSunHaloTexture->GetDXTexture());
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_SunHaloVB, sizeof(PLANET_VERTEX));

    m_SunVB[0].p = D3DXVECTOR3(-SUN_SIZE, SUN_SIZE, 0) + vSun2DPos;
    m_SunVB[1].p = D3DXVECTOR3(-SUN_SIZE,-SUN_SIZE, 0) + vSun2DPos;
    m_SunVB[2].p = D3DXVECTOR3( SUN_SIZE,-SUN_SIZE, 0) + vSun2DPos;
    m_SunVB[3].p = D3DXVECTOR3( SUN_SIZE, SUN_SIZE, 0) + vSun2DPos;

    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_SunColor);
    g_pDirect3DDevice->SetTexture(0, m_pSunTexture->GetDXTexture());
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_SunVB, sizeof(PLANET_VERTEX));


    //--------------------------------------------------------------------------------------------------
    // Moon
    g_pDirect3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	E3DAPI_Alpha(true);
	E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);

    g_pDirect3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

    g_pDirect3DDevice->SetFVF(SKY_VERTEX::FVF);

    MatrixInverse(&matWorld, &matView);
    matWorld.m_VT += (matOldWorld * m_vMoonPos);
    g_pDirect3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX *) &matWorld);

//    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_MoonHaloColor);
//    g_pDirect3DDevice->SetTexture(0, m_pMoonHaloTexture->GetTexture());
//    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_MoonHaloVB, sizeof(SKY_VERTEX));

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	E3DAPI_AlphaBlend(D3DBLEND_ONE, D3DBLEND_ONE);
    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_MoonColor);
    g_pDirect3DDevice->SetTexture(0, m_pMoonTexture->GetDXTexture());
    g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_MoonVB, sizeof(SKY_VERTEX));

    g_pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    //--------------------------------------------------------------------------------------------------
    //  Sky
    D3DXMATRIXA16 matCloud = D3DXMATRIXA16::IDENTITY;

	E3DAPI_Alpha(true);
	E3DAPI_AlphaBlend(D3DBLEND_ONE, D3DBLEND_INVSRCCOLOR);

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pDirect3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

    matWorld = m_matWorld * matOldWorld;
    g_pDirect3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX *) &matWorld);

    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwSkyColor);

	g_pDirect3DDevice->SetTexture(0, NULL);
	g_pDirect3DDevice->SetFVF(D3DFVF_DIF_VERTEX);
	g_pDirect3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwNumFogDomeVertices, m_dwNumFogDomePrimitives, m_pFogDomeIndies, 
										 D3DFMT_INDEX16, m_pFogDomeVertices, sizeof(e3d_dif_vertex));

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    //--------------------------------------------------------------------------------------------------
    // Cloud
    g_pDirect3DDevice->SetFVF(SKY_VERTEX::FVF);
	E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_HighCloudColor );
    g_pDirect3DDevice->SetTexture(0, m_pHighTexture->GetDXTexture());

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    g_pDirect3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

    for(i = 0; i < NUM_HIGH_CLOUDS; i++)
    {
        matCloud.m_VT = m_HighClouds[i].pos;

        matWorld = m_matWorld * matCloud * matOldWorld;

        g_pDirect3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX *) &matWorld);

        g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_HighCloudVB + m_HighClouds[i].kind * 4, sizeof(SKY_VERTEX));
    }

    g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_LowCloudColor );
    g_pDirect3DDevice->SetTexture(0, m_pLowTexture->GetDXTexture());

    for(i = 0; i < NUM_LOW_CLOUDS; i++)
    {
        matCloud.m_VT = m_LowClouds[i].pos;

        matWorld = m_matWorld * matCloud * matOldWorld;

        g_pDirect3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX *) &matWorld);

        g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_LowCloudVB + m_LowClouds[i].kind * 4, sizeof(SKY_VERTEX));
    }

    g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	E3DAPI_Alpha(false);
	E3DAPI_AlphaBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 0x08);
	g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,					true);

//	월드 행렬을 되돌려 준다.
	g_pDirect3DDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&matOldWorld);

    g_pDirect3DDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	g_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}
*/

XIAHGE_API void CSKY::SetColor(DWORD dwColor)
{
	m_dwSkyColor	= dwColor;

    BYTE c = dwColor & 0xFF;

    m_LowCloudColor = D3DCOLOR_RGBA( c, c, c, 255);
    m_HighCloudColor = D3DCOLOR_RGBA( c, c, c, 255);
}
}