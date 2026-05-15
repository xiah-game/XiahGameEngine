#include "stdafx.h"
#include "MapData.h"

namespace XiahGameEngine
{
	namespace Map
	{

		//////////////////////////////////////////////////////////////////////////


#define MAPCELL_TEMPBUFFER_SIZE	(400 * 1024)	// 대충 따져 보니까 400k면 뒤집어 쓴다

		BYTE g_MapCell_TempBuffer[ MAPCELL_TEMPBUFFER_SIZE];

		//--------------------------------------------------------------------------------------------
		class CDetailTextureList : public std::hash_map<int,IDirect3DTexture9*>
		{
		public:
			XIAHGE_API CDetailTextureList()
			{
			}

			XIAHGE_API ~CDetailTextureList()
			{
				Release();
			}
		
			XIAHGE_API BOOL LoadDetailTexture(LPCTSTR filename)
			{
				FILE *fp = _tfopen( filename, _T("rb"));

				if( !fp)
					return FALSE;

				while( 1)
				{
					int id;

					fread( &id, 4, 1, fp);

					if( id == -1)
						break;

					int xsize, ysize;

					fread( &xsize, 4, 1, fp);
					fread( &ysize, 4, 1, fp);

					IDirect3DTexture9 *pTexture;

/*
			HRESULT hr = g_pDirect3DDevice->CreateTexture( newSize.cx, newSize.cy, 1, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &m_pTexture, 0);

			IDirect3DSurface9 *pSurface;

			m_pTexture->GetSurfaceLevel( 0, &pSurface);
			sRect srcRect( 0, 0, 256, 256);
			D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, pData, D3DFMT_R5G6B5, 256 * 2, NULL, &srcRect, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0);
			pData += 256 * 256 * 2;
*/
					HRESULT hr;

					sSize newSize = TextureRescaleByQuality( sSize( xsize, ysize));
					UINT width, height, mipmap;
					D3DFORMAT format;

					width = newSize.cx;
					height = newSize.cy;
					mipmap = 0;
					format = D3DFMT_R5G6B5;

					D3DXCheckTextureRequirements( g_pDirect3DDevice, &width, &height, &mipmap, 0, &format, D3DPOOL_MANAGED);

					// Detail Texture도 D3DPOOL_MANAGED
					hr = g_pDirect3DDevice->CreateTexture( newSize.cx, newSize.cy, mipmap, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &pTexture, NULL);

					if( FAILED( hr))
						throw _T("Create Texture Fail");
					
					fread( g_MapCell_TempBuffer, xsize * ysize * 2, 1, fp);

					sRect srcRect( 0, 0, xsize, ysize);
					IDirect3DSurface9 *pSurface;

					for(int i = 0; i < mipmap; i++)
					{
						pTexture->GetSurfaceLevel( i, &pSurface);
						
						if( pSurface)
							D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, g_MapCell_TempBuffer, D3DFMT_R5G6B5, xsize * 2, NULL, &srcRect, D3DX_FILTER_LINEAR, 0);
					}

					pSurface->Release();

					insert( value_type( id, pTexture));
				}

				return TRUE;
			}
			
			XIAHGE_API BOOL Release()
			{
				iterator it;

				for(it = begin(); it != end(); it ++)
				{
					IDirect3DTexture9* pTexture = it->second;

					if( pTexture)
						pTexture->Release();
				}

				clear();
			
				return TRUE;
			}

			XIAHGE_API IDirect3DTexture9* GetDetailTexture(int id)
			{
				iterator it = find( id);

				if( it == end())
					return NULL;

				return it->second;
			}
		};
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMap3DRes_Tile		g_TileRes;
		XIAHGE_API CDetailTextureList  g_DetailTextureList;
		XIAHGE_API CMapRes	g_MapRes;

		//---------------------------------------------------------------------------------------
		BOOL InitializeXiahMap(LPCTSTR *pFileList)
		{
			try
			{
				if( !g_TileRes.Create( pFileList[ 0], pFileList[ 1], pFileList[ 2], pFileList[ 3]))
					return FALSE;

				if( !g_DetailTextureList.LoadDetailTexture( pFileList[ 4]))
					return FALSE;

			}
			catch(LPCTSTR strError)
			{
				DBG_Put((TCHAR *)strError);
				return FALSE;
			}

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		BOOL UninitializeXiahMap()
		{
			g_MapRes.Release();
			g_DetailTextureList.Release();
			g_TileRes.Release();

			return TRUE;
		}

		/*************************************************************************************************************
		..............................................................................................................
		......................SSSS...EEEEEE..PPPPP.....AA....RRRRR.....AA....TTTTTT...OOOO...RRRRR....................
		.....................SS..SS..EE......PP..PP...AAAA...RR..RR...AAAA.....TT....OO..OO..RR..RR...................
		.....................SS......EE......PP..PP..AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
		......................SSSS...EEEEEE..PPPPP...AAAAAA..RRRR....AAAAAA....TT....OO..OO..RRRR.....................
		.........................SS..EE......PP......AA..AA..RR.RR...AA..AA....TT....OO..OO..RR.RR....................
		.....................SS..SS..EE......PP......AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
		......................SSSS...EEEEEE..PP......AA..AA..RR..RR..AA..AA....TT.....OOOO...RR..RR...................
		..............................................................................................................
		*************************************************************************************************************/

		//---------------------------------------------------------------------------------------
		XIAHGE_API CRes_MeshBlock::CRes_MeshBlock()
		{
			m_nVertex = 0;
			m_nFace = 0;
			m_pVertex = NULL;
			m_pFace = NULL;

			m_nLodCount = 0;
			m_pCollapsedIDList = 0;
			m_pLodFaceList = 0;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CRes_MeshBlock::~CRes_MeshBlock()
		{
			Release();
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CRes_MeshBlock::Release()
		{
			if( m_pVertex)
			{
				m_pVertex->Release();
				m_pVertex = NULL;
			}

			if( m_pFace)
			{
				delete [] m_pFace;
				m_pFace = NULL;
			}

			if( m_pCollapsedIDList)
			{
				delete [] m_pCollapsedIDList;
				m_pCollapsedIDList = NULL;
			}

			if( m_pLodFaceList)
			{
				delete [] m_pLodFaceList;
				m_pLodFaceList = NULL;
			}
		
			return TRUE;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CRes_MeshBlock::ReadData(LPBYTE &pData)
		{
			m_nVertex = *(unsigned long *)pData;
			pData += 4;
			m_nFace = *(unsigned long*)pData;
			pData += 4;
			m_nLodCount = *(unsigned short* )pData;
			pData += 2;

			HRESULT hr;
/*
			hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof( sMapData_Vertex) * m_nVertex,
														D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
														D3DFVF_MAPDATA_VERTEX,
														D3DPOOL_DEFAULT,
														&m_pVertex, NULL);
*/

			hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof( sMapData_Vertex) * m_nVertex,
														D3DUSAGE_WRITEONLY,
														D3DFVF_MAPDATA_VERTEX,
														D3DPOOL_MANAGED,
														&m_pVertex, NULL);

			if( FAILED( hr))
				throw _T("Vertex Buffer Create Fail");

			m_pFace = new unsigned short[ m_nFace * 3];
			m_pCollapsedIDList = new unsigned short[ m_nVertex];
			m_pLodFaceList = new unsigned short[ m_nVertex];

			LPBYTE pVertex;
			hr = m_pVertex->Lock( 0, 0, (void **)&pVertex, 0);
			if( FAILED( hr))
				throw _T("Vertex Buffer Lock Fail");
			// Vertex data copy. (Vector3 pos,unsigned long color,Vector2 tex_color,Vector2 tex_detail)
			memcpy( pVertex, pData, sizeof( sMapData_Vertex) * m_nVertex);
			m_pVertex->Unlock();
			pData += sizeof( sMapData_Vertex) * m_nVertex;

			// Face data
			memcpy( m_pFace, pData, sizeof( unsigned short) * m_nFace * 3);
			pData += sizeof( unsigned short) * m_nFace * 3;

			// Collapse data
			memcpy( m_pCollapsedIDList, pData, sizeof( unsigned short) * m_nVertex);
			pData += sizeof( unsigned short) * m_nVertex;

			// LOD face list
			memcpy( m_pLodFaceList, pData, sizeof( unsigned short) * m_nVertex);
			pData += sizeof( unsigned short) * m_nVertex;

			// BOUND BOX Data (Mapcell의 바운드)
			memcpy( &m_BoundBox, pData, sizeof( float) * 6);
			pData += sizeof(float) * 6;

			return TRUE;
		}

		/*************************************************************************************************************
		..............................................................................................................
		......................SSSS...EEEEEE..PPPPP.....AA....RRRRR.....AA....TTTTTT...OOOO...RRRRR....................
		.....................SS..SS..EE......PP..PP...AAAA...RR..RR...AAAA.....TT....OO..OO..RR..RR...................
		.....................SS......EE......PP..PP..AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
		......................SSSS...EEEEEE..PPPPP...AAAAAA..RRRR....AAAAAA....TT....OO..OO..RRRR.....................
		.........................SS..EE......PP......AA..AA..RR.RR...AA..AA....TT....OO..OO..RR.RR....................
		.....................SS..SS..EE......PP......AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
		......................SSSS...EEEEEE..PP......AA..AA..RR..RR..AA..AA....TT.....OOOO...RR..RR...................
		..............................................................................................................
		*************************************************************************************************************/
		//---------------------------------------------------------------------------------------
		XIAHGE_API CRes_MapCell::CRes_MapCell(HANDLE hFileHandle)
		{
			m_hFileHandle = hFileHandle;

			ReadHeader();

			m_bRealized = FALSE;
			// 8 * 8개의 Detail Texture 초기화
			for(int i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
			{
				m_pDetailTexture[ i] = NULL;
			}

			m_pTexture = NULL;

			m_nObject = 0;
			m_pObject = NULL;

			m_nWaterVertex = 0;
			m_nWaterFace = 0;
			m_pWaterVertex = 0;
			m_pWaterFace = 0;

			//
			m_nGrassZoneCount = 0;
			m_pGrassZoneInfo  = NULL;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CRes_MapCell::~CRes_MapCell()
		{
			Release();
		}

		//---------------------------------------------------------------------------------------
		BOOL CRes_MapCell::ReadHeader()
		{
			DWORD nReadByte;

			DBG_Assert( m_hFileHandle != INVALID_HANDLE_VALUE);
			
			if( !ReadFile( m_hFileHandle, &m_XPos, 2, &nReadByte, NULL))
				throw _T("File Read Error");

			if( !ReadFile( m_hFileHandle, &m_YPos, 2, &nReadByte, NULL))
				throw _T("File Read Error");

			if( !ReadFile( m_hFileHandle, &m_nOffset, 4, &nReadByte, NULL))
				throw _T("File Read Error");

			if( !ReadFile( m_hFileHandle, &m_nSize, 4, &nReadByte, NULL))
				throw _T("File Read Error");

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CRes_MapCell::Realize()
		{
			if( m_bRealized == TRUE)
				return TRUE;

			DWORD nReadByte;
			
			if( SetFilePointer( m_hFileHandle, m_nOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				throw _T("File Seek Error");

			if( MAPCELL_TEMPBUFFER_SIZE	< m_nSize)
				throw _T("Buffer Size Overflow :Critical Error!!");

			if( !ReadFile( m_hFileHandle, g_MapCell_TempBuffer, m_nSize, &nReadByte, NULL))
				throw _T("File Read Error");

			int i;

			LPBYTE pData = g_MapCell_TempBuffer;

			// 한맵셀을 읽는다. 1개의 맵셀은 8*8개의 Detail
			for(i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
				m_pHeightMeshBlock[ i].ReadData( pData);
		
			// SkipWater
			//for(i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
			//	m_pWaterMeshBlock[ i].ReadData( pData);

			sSize newSize = TextureRescaleByQuality( sSize( 256, 256));

			UINT width, height, mipmap;
			D3DFORMAT format;

			width = newSize.cx;
			height = newSize.cy;
			mipmap = 0;
			format = D3DFMT_R5G6B5;

			D3DXCheckTextureRequirements( g_pDirect3DDevice, &width, &height, &mipmap, 0, &format, D3DPOOL_MANAGED);

			// 이곳은 color map을 읽어 들인다. colormap은 원본 (2048*2048)을 각각의 Mapcell 단위로 잘라서 Mapcell에 종속 시킨다.
			// Texture의 질은 16Bit Bitmap(D3DFMT_R5G6B5)이며 고로 사이즈는 256*256*2(byte)이다.

			HRESULT hr = g_pDirect3DDevice->CreateTexture( newSize.cx, newSize.cy, mipmap, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &m_pTexture, 0);
			IDirect3DSurface9 *pSurface;
			sRect srcRect( 0, 0, 256, 256);
			for(i = 0; i < mipmap; i++)
			{
				m_pTexture->GetSurfaceLevel( i, &pSurface);
				if( pSurface)
					D3DXLoadSurfaceFromMemory( pSurface, NULL, NULL, pData, D3DFMT_R5G6B5, 256 * 2, NULL, &srcRect, D3DX_FILTER_LINEAR, 0 );// D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0);
			}
			// 사용이 끝난 서피스를 Release하지 않으면 Leak!
			pSurface->Release();
			pData += 256 * 256 * 2;

			// Detail Texture
			int *pDetailInfo = (int *)pData;

			// 멥셀안의 디테일 텍스처를 얻는다.
			for(i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
			{
				// detail texture Object
				this->m_pDetailTexture[ i] = g_DetailTextureList.GetDetailTexture( pDetailInfo[ i]);
			}
			pData += sizeof(int) * MAPCELL_MESHBLOCK_COUNT;

			// Grass info
			memcpy( m_Grass, pData, sizeof( sGrass) * MAPCELL_MESHBLOCK_COUNT);
			pData += sizeof( sGrass) * MAPCELL_MESHBLOCK_COUNT;

			// Mapobject 갯수
			m_nObject = *(unsigned short* )pData;
			pData += 2;
			if( m_nObject)
			{
				m_pObject = new sObjTileInstance[ m_nObject];

				memcpy( m_pObject, pData, sizeof( sObjTileInstance) * m_nObject);
			
				pData += sizeof( sObjTileInstance) * m_nObject;
			}
			else
				m_pObject = NULL;

			// Height 정보
			memcpy( m_Height, pData, MAPCELL_HEIGHT_COUNT);
			pData += MAPCELL_HEIGHT_COUNT;

			// 물정보
			m_nWaterVertex = *(unsigned long *)pData;
			pData += 4;
			m_nWaterFace = *(unsigned long*)pData;
			pData += 4;

			if( m_nWaterVertex > 0)
			{
				HRESULT hr;
				hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof( sMapData_Vertex) * m_nWaterVertex,
															D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
															D3DFVF_MAPDATA_VERTEX,
															D3DPOOL_DEFAULT,
															&m_pWaterVertex, NULL);
/*
				hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof( sMapData_Vertex) * m_nWaterVertex,
															0,
															D3DFVF_MAPDATA_VERTEX,
															D3DPOOL_MANAGED,
															&m_pWaterVertex, NULL);
*/
				if( FAILED( hr))
					throw _T("Vertex Buffer Create Fail");

				LPBYTE pVertex;
				hr = m_pWaterVertex->Lock( 0, 0, (void**)&pVertex, 0);
				if( FAILED( hr))
					throw _T("Vertex Buffer Lock Fail");

				// 물 vertex를 물, 용암, 늪지대 물로 표현한다.
				D3DCOLOR WaterColor;
				switch( g_MapRes.m_byWaterType )
				{
				case eWaterType_Water:
					WaterColor = D3DCOLOR_ARGB( 170, 0, 180, 255 );
					break;
				case eWaterType_YongAm:
					WaterColor = D3DCOLOR_ARGB( 192, 255, 255, 255 );
					break;
				case eWaterType_Marsh:
					WaterColor = D3DCOLOR_ARGB( 240, 255, 255, 255 );
					break;
				};// switch

				// 물 Vertex 정보 로드
				// TODO: 지울것 test
				//char strtemp[128] = {0,};
				sMapData_Vertex *pWaterVertex = (sMapData_Vertex*)pData;
				for(i = 0; i < m_nWaterVertex; i++)
				{
					//sprintf(strtemp, "%f, %f, %f\n", pWaterVertex->pos.x, pWaterVertex->pos.y, pWaterVertex->pos.z);
					//OutputDebugString(strtemp);

					pWaterVertex->color = WaterColor;
					pWaterVertex ++;
				}
				memcpy( pVertex, pData, sizeof( sMapData_Vertex) * m_nWaterVertex);
				m_pWaterVertex->Unlock();
				pData += sizeof( sMapData_Vertex) * m_nWaterVertex;

				// Water Index 정보
//				hr = g_pDirect3DDevice->CreateIndexBuffer( 2 * m_nWaterFace, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pWaterFace, NULL);
				hr = g_pDirect3DDevice->CreateIndexBuffer( 2 * m_nWaterFace, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pWaterFace, NULL );

				if( FAILED( hr))
					throw _T("Index Buffer Crate Fail");

				LPBYTE pFace;
//				hr = m_pWaterFace->Lock( 0, 0, (void**)&pFace, D3DLOCK_NOOVERWRITE ); ?
				hr = m_pWaterFace->Lock( 0, 0, (void**)&pFace, 0);

				if( FAILED( hr))
					throw _T("Index Buffer Lock Fail");

				memcpy( pFace, pData, sizeof( unsigned short) * m_nWaterFace);
				m_pWaterFace->Unlock();

				// TODO: 지울것 test
				/*
				unsigned short *pp = (unsigned short*)pData;

				for(int i=0; i < m_nWaterFace; ++i)
				{					
					sprintf(strtemp, "%d\n", *pp);
					OutputDebugString(strtemp);

					pp++;
				}
				*/

				pData += sizeof(unsigned short) * m_nWaterFace;
			}


			// Grass Zone
			m_nGrassZoneCount = *(unsigned short *)pData;
			pData += sizeof(unsigned short);

			if( m_nGrassZoneCount > 0 )
			{
				m_pGrassZoneInfo = new sGrassZonePackageData [ m_nGrassZoneCount ];

				memcpy( m_pGrassZoneInfo, pData, sizeof(sGrassZonePackageData) * m_nGrassZoneCount );
				pData += sizeof(sGrassZonePackageData) * m_nGrassZoneCount;
			}

			//DBG_Put(_T("\n m_XPos=%d, m_YPos=%d, m_nGrassZoneCount=%d"), m_XPos, m_YPos, m_nGrassZoneCount);
			m_bRealized = TRUE;
			return TRUE;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CRes_MapCell::Release()
		{
			for(int i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
			{
				m_pHeightMeshBlock[ i].Release();
				m_pWaterMeshBlock[ i].Release();
			
				m_pDetailTexture[ i] = NULL;
			}

			if( m_pTexture)
			{
				m_pTexture->Release();
				m_pTexture = NULL;
			}

			m_nObject = 0;
			if( m_pObject)
			{
				delete [] m_pObject;
				m_pObject = NULL;
			}
			
			m_bRealized = FALSE;

			if( m_pWaterVertex)
			{
				m_pWaterVertex->Release();
				m_pWaterVertex = NULL;
			}

			if( m_pWaterFace)
			{
				m_pWaterFace->Release();
				m_pWaterFace = NULL;
			}

			if( m_pGrassZoneInfo )
			{
				delete []m_pGrassZoneInfo;
				m_pGrassZoneInfo = NULL;
			}

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API float CRes_MapCell::GetHeight(float x,float y)
		{
			int tx = (int)x / 4;
			int ty = (int)-y / 4;
		
			if( tx < 0 || ty < 0 || tx > 63 || ty > 63)
				return 0;

			float dx = x - tx * 4;
			float dy = -y - ty * 4;

			float h0 = m_Height[ tx + ty * 65];
			float h1 = m_Height[ tx + 1 + ty * 65];
			float h2 = m_Height[ tx + (ty + 1) * 65];
			float h3 = m_Height[ tx + 1 + (ty + 1) * 65];

			if(x > 2047 || -y > 2047)
			{
				DBG_LogFile( _T("CRes_MapCell::GetHeight fail %f %f"),x,y);
				return 0;
			}
		

			if( dx > dy)
				return h1 + (h0 - h1) * (TILE_GRID_SIZE_FLOAT - dx) / TILE_GRID_SIZE_FLOAT 
						+ (h3 - h1) * dy / TILE_GRID_SIZE_FLOAT;
			else
				return h2 + (h3 - h2) * dx / TILE_GRID_SIZE_FLOAT
						+ (h0 - h2) * (TILE_GRID_SIZE_FLOAT - dy) / TILE_GRID_SIZE_FLOAT;

			return 0;
/*
		if( dx > dz)
			return	(height	= h1+(h0-h1)*(TILE_SIZE-dx)/TILE_SIZE+(h3-h1)*dz/TILE_SIZE);
		//	LeftBottom
		else
			return	(height	= h2+(h3-h2)*dx/TILE_SIZE+(h0-h2)*(TILE_SIZE-dz)/TILE_SIZE);
*/

		}



		/*************************************************************************************************************
		..............................................................................................................
		......................SSSS...EEEEEE..PPPPP.....AA....RRRRR.....AA....TTTTTT...OOOO...RRRRR....................
		.....................SS..SS..EE......PP..PP...AAAA...RR..RR...AAAA.....TT....OO..OO..RR..RR...................
		.....................SS......EE......PP..PP..AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
		......................SSSS...EEEEEE..PPPPP...AAAAAA..RRRR....AAAAAA....TT....OO..OO..RRRR.....................
		.........................SS..EE......PP......AA..AA..RR.RR...AA..AA....TT....OO..OO..RR.RR....................
		.....................SS..SS..EE......PP......AA..AA..RR..RR..AA..AA....TT....OO..OO..RR..RR...................
		......................SSSS...EEEEEE..PP......AA..AA..RR..RR..AA..AA....TT.....OOOO...RR..RR...................
		..............................................................................................................
		*************************************************************************************************************/
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapRes::CMapRes()
		{
			m_hFileHandle = INVALID_HANDLE_VALUE;

			m_byWaterType = eWaterType_Water;
			m_bLoadMapFile = false;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapRes::~CMapRes()
		{
			Release();
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRes::Release()
		{
			RES_MAPCELL_LIST::iterator it;

			for(it = m_MapList.begin(); it != m_MapList.end(); it++)
			{
				CRes_MapCell *pMapCell = it->second;
				if(pMapCell) 
				{
					delete pMapCell;
					pMapCell = NULL;
				}
			}

			m_MapList.clear();

			if( m_hFileHandle == INVALID_HANDLE_VALUE)
				CloseHandle( m_hFileHandle);

			m_hFileHandle = INVALID_HANDLE_VALUE;

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRes::LoadMapFile(LPCTSTR mapfile)
		{
			m_byWaterType = eWaterType_Water;
			if( _tcscmp( mapfile, _T("map\\sec1.xmp")) == 0 ||
				_tcscmp( mapfile, _T("map\\sec3.xmp")) == 0 )
                m_byWaterType = eWaterType_YongAm;
            else
            if( _tcscmp( mapfile, "map\\sec5.xmp") == 0 )
                m_byWaterType = eWaterType_Marsh;

			// 이걸 해놓으면 CMapRender에서 물, 용암 텍스쳐를 읽는다.
			m_bLoadMapFile = true;

			try
			{
				m_hFileHandle = CreateFile( mapfile, 
						GENERIC_READ, 
						FILE_SHARE_READ, 
						NULL,
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
						NULL);	
				
				if( m_hFileHandle == INVALID_HANDLE_VALUE)
					throw _T("File Open Error");

				unsigned long temp;
				unsigned long nReadByte;

				if( !ReadFile( m_hFileHandle, &temp, 4, &nReadByte, 0))
					throw _T("File Read Error");

				// MAP cell을 갯수(temp)만큼 만든다.
				for(int i = 0; i < temp; i++)
				{
					CRes_MapCell	*pMapCell = new CRes_MapCell( m_hFileHandle);
					// 각각의 x,y로만든 Key를 기반으로 Mapcell을 hash_map에 등록한다.
					m_MapList.insert( RES_MAPCELL_LIST::value_type( MAKELONG( pMapCell->m_XPos, pMapCell->m_YPos), pMapCell));
				}
			}
			catch(LPCTSTR strError)
			{
				DBG_Put( (TCHAR*)strError);
				return FALSE;
			}

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API CRes_MapCell* CMapRes::GetMapCell(unsigned short x,unsigned short y)
		{
			unsigned long key = MAKELONG( x, y);

			RES_MAPCELL_LIST::iterator it = m_MapList.find( key);

			if( it == m_MapList.end())
				return NULL;

			CRes_MapCell *pMapCell = it->second;
			if(pMapCell == NULL)
			{
				DBG_LogFile( _T("CMapRes::GetMapCell fail"));
				return NULL;
			}

			try
			{
				pMapCell->Realize();
			}
			catch(LPCTSTR strError)
			{
				DBG_Put( (TCHAR*)strError);
				return NULL;
			}

			return pMapCell;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API float CMapRes::GetHeight(float x,float y)
		{
			if(x < 0 || x > 2047 || -y < 0 || -y > 2047 ) 
			{
				// ERROR 이다 그러나 그럴 수 있다. 예) 카메라 같은것
				return 0;
			}

			int mapcell_x = (int)x;
			int mapcell_y = ((int)-y);

			mapcell_x /= 256;
			mapcell_y /= 256;

			mapcell_x *= 64;
			mapcell_y *= 64;

			CRes_MapCell *pMapCell = GetMapCell( mapcell_x, mapcell_y);

			if( pMapCell == NULL)
			{
				//HT_CHEAT : 이 버그는 무슨 내용인지 모르겠넹.. 나중에 기회되면 ... 
			//	DBG_LogFile( _T("CMapRes::GetHeight fail %f %f"),x,y);
				return 0;
			}

			return pMapCell->GetHeight( x - mapcell_x * 4, y + mapcell_y * 4); // y는 음수로
		}
	};
};