#include "stdafx.h"
#include "MapObjectData.h"
#include "XiahPak.h"

namespace XiahGameEngine
{
	namespace Map
	{
		CMap3DRes_Tile::CMap3DRes_Tile()
		{
			m_hTilePackage = NULL;
			m_hMeshPackage = NULL;
		}
		CMap3DRes_Tile::~CMap3DRes_Tile()
		{
			Release();
		}

		BOOL CMap3DRes_Tile::Create(LPCTSTR tile_index,LPCTSTR tile_pak,LPCTSTR mesh_index,LPCTSTR mesh_pak)
		{
			try
			{
				FILE *fp;

				int i;
				int total_count;

				// Tile
				fp = _tfopen(tile_index, _T("rb"));

				if( !fp)
					throw _T("TileRes: File OpenError");
				
				fread( &total_count, 4, 1, fp);

				for(i = 0; i < total_count; i++)
				{
					Map3D_TileResIndex TileResIndex;

					fread( &TileResIndex.nID, 4, 1, fp);
					fread( &TileResIndex.nTileSetID, 4, 1, fp);
					fread( &TileResIndex.nTileSetIndex, 1, 1, fp);
					fread( &TileResIndex.nRuleIndex, 4, 1, fp);
					fread( &TileResIndex.nSeason, 1, 1, fp);
					fread( &TileResIndex.nFigureCount, 4, 1, fp);
					fread( &TileResIndex.nTileEffectCount, 4, 1, fp );
					fread( &TileResIndex.nOffset, 4, 1, fp);
				
					m_TileResIndexList.insert( MAP3D_TILERESINDEX::value_type( TileResIndex.nID, TileResIndex));
				
					MAP3D_TILERESINDEX::iterator it;

					it = m_TileResIndexList.find( TileResIndex.nID);

					Map3D_TileResIndex *pTileResIndex = &it->second;

					Map3D_TileSetIndex *pTileSetIndex = NULL;
					
					MAP3D_TILESETINDEXLIST::iterator tsit;
					
					tsit = m_TileSetIndexList.find( pTileResIndex->nTileSetID);

					if( tsit == m_TileSetIndexList.end())
					{
						pTileSetIndex = new Map3D_TileSetIndex;

						pTileSetIndex->nID = pTileResIndex->nTileSetID;

						m_TileSetIndexList.insert( MAP3D_TILESETINDEXLIST::value_type( pTileSetIndex->nID, pTileSetIndex));
					}
					else
						pTileSetIndex = tsit->second;


					pTileSetIndex->m_SubSet[ pTileResIndex->nTileSetIndex].m_TileResList[ pTileResIndex->nSeason].push_back( pTileResIndex);
				}
				fclose(fp);

				// Tile Mesh
				// 더이상 Index File이 필요 없다.
//				fp = _tfopen(mesh_index, _T("rb"));
				fp = _tfopen( mesh_pak, _T("rb" ));

				if( !fp)
					throw _T("TileRes: File OpenError");

				fread( &total_count, 4, 1, fp);

				for(i = 0; i < total_count; i++)
				{
					Map3D_TileMeshIndex TileMeshIndex;

					fread( &TileMeshIndex.nID, 4, 1, fp);
					fread( &TileMeshIndex.nOffset, 4, 1, fp);

					TileMeshIndex.nLength = 0;

					m_TileMeshIndexList.insert( MAP3D_TILEMESHINDEX::value_type( TileMeshIndex.nID, TileMeshIndex));
				}

				m_hMeshPackage = fp;
				m_hTilePackage = _tfopen( tile_pak, _T("rb"));

				if( !m_hMeshPackage || !m_hTilePackage)
					throw _T("TileRes: File OpenError");
			}
			catch(LPCTSTR strError)
			{
				DBG_Put( (TCHAR *)strError);
				return FALSE;
			}

			return TRUE;
		}

		void CMap3DRes_Tile::Release()
		{
			if( m_hTilePackage)
				fclose( m_hTilePackage);

			if( m_hMeshPackage)
				fclose( m_hMeshPackage);

			m_hTilePackage = NULL;
			m_hMeshPackage = NULL;

			MAP3D_TILESETINDEXLIST::iterator tsit;

			for(tsit = m_TileSetIndexList.begin(); tsit != m_TileSetIndexList.end(); tsit ++)
			{
				Map3D_TileSetIndex *pTileSetIndex = tsit->second;
				if(pTileSetIndex == NULL)
				{
					DBG_LogFile( _T("CMap3DRes_Tile::Release1 fail"));
				}
				delete pTileSetIndex;
				pTileSetIndex = NULL;
			}

			m_TileSetIndexList.clear();

			MAP3D_TILEMESHLIST::iterator mit;

			for(mit = m_TileMeshList.begin(); mit != m_TileMeshList.end(); mit ++)
			{
				Map3DRes_TileMesh *pTileMesh = mit->second;
				if(pTileMesh == NULL)
				{
					DBG_LogFile( _T("CMap3DRes_Tile::Release2 fail"));
				}
				delete pTileMesh;
				pTileMesh = NULL;
			}

			m_TileMeshList.clear();

			MAP3D_TILERESLIST::iterator trit;

			for(trit = m_TileResList.begin(); trit != m_TileResList.end(); trit++)
			{
				Map3DRes_TileRes *pTileRes = trit->second;
				if(pTileRes == NULL)
				{
					DBG_LogFile( _T("CMap3DRes_Tile::Release3 fail"));
				}
				delete pTileRes;
				pTileRes = NULL;
			}

			m_TileResList.clear();
		}

		XIAHGE_API BOOL CMap3DRes_Tile::Clear()
		{
			MAP3D_TILEMESHLIST::iterator mit;

			for(mit = m_TileMeshList.begin(); mit != m_TileMeshList.end(); mit ++)
			{
				Map3DRes_TileMesh *pTileMesh = mit->second;
				if(pTileMesh == NULL)
				{
					DBG_LogFile( _T("CMap3DRes_Tile::Clear fail"));
				}
				delete pTileMesh;
				pTileMesh = NULL;
			}

			m_TileMeshList.clear();

			MAP3D_TILERESLIST::iterator trit;

			for(trit = m_TileResList.begin(); trit != m_TileResList.end(); trit++)
			{
				Map3DRes_TileRes *pTileRes = trit->second;

				XiahPak::ReleaseRes( pTileRes->nMapID);

				for(int i = 0; i < pTileRes->nTexCount; i++)
				{
					XiahPak::ReleaseRes( pTileRes->pTexList[ i]);
				}

				
				delete pTileRes;
				pTileRes = NULL;
			}

			m_TileResList.clear();
		
			return TRUE;
		};

		XIAHGE_API Map3DRes_TileMesh *CMap3DRes_Tile::GetTileMesh(int nID)
		{
			MAP3D_TILEMESHLIST::iterator it;

			it = m_TileMeshList.find( nID);

			if( it == m_TileMeshList.end())
			{
				Map3DRes_TileMesh *pTileMesh = new Map3DRes_TileMesh;
				if(pTileMesh == NULL)
				{
					DBG_LogFile( _T("CMap3DRes_Tile::GetTileMesh fail"));
				}

				fseek( m_hMeshPackage, GetTileMeshOffset( nID), SEEK_SET);

				pTileMesh->Load( m_hMeshPackage);
				
				m_TileMeshList.insert( MAP3D_TILEMESHLIST::value_type( nID, pTileMesh));

				return pTileMesh;
			}

			return it->second;
		}

		int CMap3DRes_Tile::GetTileMeshOffset(int nID)
		{
			MAP3D_TILEMESHINDEX::iterator it = m_TileMeshIndexList.find( nID);

			if( it != m_TileMeshIndexList.end())
			{
				Map3D_TileMeshIndex &index = it->second;
				return index.nOffset;
			}

			return 0;
		}

		Map3D_TileResIndex *CMap3DRes_Tile::GetTileResIndex(int nID)
		{
			MAP3D_TILERESINDEX::iterator it = m_TileResIndexList.find( nID);

			if( it != m_TileResIndexList.end())
			{
				return &it->second;
			}
			
			return NULL;
		}

		Map3DRes_TileRes *CMap3DRes_Tile::GetTileRes(int nID)
		{
			MAP3D_TILERESLIST::iterator it = m_TileResList.find( nID);

			if( it == m_TileResList.end())
			{
				Map3D_TileResIndex *pTileResIndex = GetTileResIndex( nID);

				if( pTileResIndex == NULL)
					return NULL;

				Map3DRes_TileRes *pTileRes = new Map3DRes_TileRes;
#ifdef TRACE_LOG
				if(pTileRes == NULL)
				{
					DBG_LogFile( _T("CMap3DRes_Tile::GetTileRes fail"));
				}
#endif
				fseek( m_hTilePackage, pTileResIndex->nOffset, SEEK_SET);

				pTileRes->nFigureCount = pTileResIndex->nFigureCount;
				pTileRes->nEffectCount = pTileResIndex->nTileEffectCount;
				pTileRes->Load( m_hTilePackage);

				m_TileResList.insert( MAP3D_TILERESLIST::value_type( nID, pTileRes));
			
				return pTileRes;
			}

			return it->second;
		}

		Map3D_TileSetIndex *CMap3DRes_Tile::GetTileSetIndex(int nID)
		{
			MAP3D_TILESETINDEXLIST::iterator it = m_TileSetIndexList.find( nID);

			if( it == m_TileSetIndexList.end())
				return NULL;
			
			return it->second;
		}

		Map3DRes_TileRes *CMap3DRes_Tile::GetTileRes(int nTileSetID,int nSubSet,int nRuleIndex,int nSeason)
		{
			Map3D_TileSetIndex *pTileSetIndex = GetTileSetIndex( nTileSetID);

			if( pTileSetIndex == NULL)	// 젠장 아주 우껴서
				return NULL;

			int nID = pTileSetIndex->m_SubSet[ nSubSet].GetTile( nRuleIndex, nSeason);

			return GetTileRes( nID);
		}

		void Map3DRes_TileMesh::Load(FILE *fp)
		{
			// 타일 메시는 본과 피직이 없다는 가정이다. 애니메이션 데이타도 없다.
			// 아니다. 있다.
			DWORD dwDataSize;
			fread( &dwDataSize, 4, 1, fp );

			// 데이타를 통째로 읽자.
			fread( g_FileBuffer, dwDataSize, 1, fp );

			DWORD dwBufPos = 0;

			// Now Copy data from Memory
			unsigned short skin_mesh_count;
			memcpy( &skin_mesh_count, &g_FileBuffer[dwBufPos], sizeof(skin_mesh_count) );
			dwBufPos += sizeof(skin_mesh_count);

			m_nMeshBlock = skin_mesh_count;
			m_pMeshBlockList = new Map3DRes_TileMeshBlock[ m_nMeshBlock];
			if(m_pMeshBlockList == NULL)
			{
				DBG_LogFile( _T("Map3DRes_TileMesh::Load fail"));
			}

			for(int i=0; i<m_nMeshBlock; i++)
			{
				Map3DRes_TileMeshBlock *pMeshBlock = &m_pMeshBlockList[ i ];

				// physique
				unsigned char is_physique;
				memcpy( &is_physique, &g_FileBuffer[dwBufPos], sizeof(is_physique) );
				dwBufPos += sizeof(is_physique);

				pMeshBlock->m_bPhysique = is_physique;
				if( pMeshBlock->m_bPhysique )
                    m_bPhysique = true;

				// vertex count
				unsigned short vertex_count;
				memcpy( &vertex_count, &g_FileBuffer[dwBufPos], sizeof(vertex_count) );
				dwBufPos += sizeof(vertex_count);

				pMeshBlock->m_nVertex = vertex_count;

				// face count
				unsigned short face_count;
				memcpy( &face_count, &g_FileBuffer[dwBufPos], sizeof(face_count) );
				dwBufPos += sizeof(face_count);

				pMeshBlock->m_nFace = face_count;

				// Physique이 들어 있으면 vertex type 에러가 나서 하드웨어 가속을 받을 수 없다.
				// 즉, 피직이 있으면 무조건 D3DPOOL_SYSTEMMEM 이다. 
				HRESULT hr;
				bool bPhysique = false;
				// vertex buffer
				if( pMeshBlock->m_bPhysique )
				{
					bPhysique = true;

					hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof(SkinVertex) * pMeshBlock->m_nVertex, 
											0, FVF_SKINVERTEX, D3DPOOL_SYSTEMMEM, 
											&pMeshBlock->m_pVertexBuffer, NULL);

					if( FAILED( hr))
						throw _T("TileMesh Load Fail");

					SkinVertex* pVertex;

					pMeshBlock->m_pVertexBuffer->Lock( 0, 0, (void**)&pVertex, 0 );

					memcpy( pVertex, &g_FileBuffer[dwBufPos], sizeof(SkinVertex) * pMeshBlock->m_nVertex );
					dwBufPos += ( sizeof(SkinVertex) * pMeshBlock->m_nVertex );

					pMeshBlock->m_pVertexBuffer->Unlock();
				}
				else
				{
/*
					hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof( VT_Normal) * pMeshBlock->m_nVertex, 
						D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_DEFAULT,
						&pMeshBlock->m_pVertexBuffer, NULL);
*/
					hr = g_pDirect3DDevice->CreateVertexBuffer( sizeof( VT_Normal) * pMeshBlock->m_nVertex, 
											D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_MANAGED, 
											&pMeshBlock->m_pVertexBuffer, NULL);

					if( FAILED( hr))
					{
						throw _T("TileMesh Load Fail");
					}

					VT_Normal* pVertex;

					hr = pMeshBlock->m_pVertexBuffer->Lock( 0, 0, (void**)&pVertex, 0 );

					if( !FAILED( hr))
					{
						memcpy( pVertex, &g_FileBuffer[dwBufPos], sizeof(VT_Normal) * pMeshBlock->m_nVertex );
						dwBufPos += ( sizeof(VT_Normal) * pMeshBlock->m_nVertex );

						pMeshBlock->m_pVertexBuffer->Unlock();
					}
				}

				// index buffer
				if( bPhysique )
					hr = g_pDirect3DDevice->CreateIndexBuffer( 2 * 3 * pMeshBlock->m_nFace, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &pMeshBlock->m_pIndexBuffer, NULL);
				else
					hr = g_pDirect3DDevice->CreateIndexBuffer( 2 * 3 * pMeshBlock->m_nFace, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pMeshBlock->m_pIndexBuffer, NULL);

				if( FAILED( hr))
				{
					throw _T("Create Index Buffer Fail");
				}

				BYTE *pIndex;

				hr = pMeshBlock->m_pIndexBuffer->Lock( 0, 0, (void**)&pIndex, 0 );

				memcpy( pIndex, &g_FileBuffer[dwBufPos], 2*3*pMeshBlock->m_nFace );
				dwBufPos += ( 2*3*pMeshBlock->m_nFace );

				hr = pMeshBlock->m_pIndexBuffer->Unlock();

				// 이젠 LOD가 없다. 주석.
/*
				// LOD count
				memcpy( &pMeshBlock->lod_count, &g_FileBuffer[dwBufPos], sizeof(pMeshBlock->lod_count) );
				dwBufPos += sizeof(pMeshBlock->lod_count);

				// 
				pMeshBlock->lod_collapse_ptr = new WORD [ pMeshBlock->m_nVertex ];
				memcpy( pMeshBlock->lod_collapse_ptr, &g_FileBuffer[dwBufPos], 2*pMeshBlock->m_nVertex );
				dwBufPos += (2*pMeshBlock->m_nVertex);

				pMeshBlock->lod_facecount_ptr = new WORD [ pMeshBlock->m_nVertex ];
				memcpy( pMeshBlock->lod_facecount_ptr, &g_FileBuffer[dwBufPos], 2*pMeshBlock->m_nVertex );
				dwBufPos += (2*pMeshBlock->m_nVertex);
*/

			}// for(m_nMeshBlock)

			m_nFrame = 1;

			memcpy( &m_nFrame, &g_FileBuffer[dwBufPos], sizeof(m_nFrame) );
			dwBufPos += sizeof(m_nFrame);

			if( m_bPhysique && m_nFrame > 1 )
			{
				// Tile Editor에서 매트릭스 정보를 바로 저장했다.  ^^;
				fread( &m_nBones, sizeof(m_nBones), 1, fp );

				int nSize = m_nFrame * m_nBones;
				m_pMatrix = new Matrix4x4 [ nSize ];

				fread( m_pMatrix, sizeof(Matrix4x4)*nSize, 1, fp );
			}// if( m_nFrame > 1 )
		}

		void Map3DRes_TileRes::Load(FILE *fp)
		{
			DWORD dwDataSize;
			fread( &dwDataSize, 4, 1, fp );

			// 데이타를 통째로 읽자.
			fread( g_FileBuffer, dwDataSize, 1, fp );

			DWORD dwBufPos = 0;

			memcpy( &nID, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nTileSetID, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nTileSetIndex, &g_FileBuffer[dwBufPos], 1 );
			dwBufPos += 1;
			memcpy( &nSeason, &g_FileBuffer[dwBufPos], 1 );
			dwBufPos += 1;
			memcpy( &nMeshID, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nMapID, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nXSize, &g_FileBuffer[dwBufPos], 1 );
			dwBufPos += 1;
			memcpy( &nYSize, &g_FileBuffer[dwBufPos], 1 );
			dwBufPos += 1;
			memcpy( &nZSize, &g_FileBuffer[dwBufPos], 1 );
			dwBufPos += 1;
			memcpy( &nRuleIndex, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nRenderOption, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nResType, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;
			memcpy( &nSpeedPerFrame, &g_FileBuffer[dwBufPos], 2 );
			dwBufPos += 2;

			//
			memcpy( &nTexCount, &g_FileBuffer[dwBufPos], 4 );
			dwBufPos += 4;

			pTexList = NULL;
			if( nTexCount > 0 )
			{
				pTexList = new int[ nTexCount];

				memcpy( pTexList, &g_FileBuffer[dwBufPos], 4*nTexCount );
				dwBufPos += 4*nTexCount;
			}

			if( nFigureCount > 0)
			{
				pFigureList = new Map3DRes_Figure[ nFigureCount];
				
				for(int i = 0; i < nFigureCount; i++)
				{
					Map3DRes_Figure *pFigure = pFigureList + i;

					memcpy( &pFigure->nID, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pFigure->nTileResID, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pFigure->nType, &g_FileBuffer[dwBufPos], 1 );
					dwBufPos += 1;
					memcpy( &pFigure->nDirection, &g_FileBuffer[dwBufPos], 1 );
					dwBufPos += 1;
					memcpy( &pFigure->nPosX, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pFigure->nPosY, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pFigure->nPosZ, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pFigure->nXSize, &g_FileBuffer[dwBufPos], 1 );
					dwBufPos += 1;
					memcpy( &pFigure->nYSize, &g_FileBuffer[dwBufPos], 1 );
					dwBufPos += 1;
					memcpy( &pFigure->nZSize, &g_FileBuffer[dwBufPos], 1 );
					dwBufPos += 1;
					memcpy( &pFigure->nHightPoint, &g_FileBuffer[dwBufPos], 1 );
					dwBufPos += 1;
				}// for
			}// if

			// load attached effect data
			if( nEffectCount > 0 )
			{
				pEffectList = new Map3DRes_TileEffect [ nEffectCount ];
				for(int i=0; i<nEffectCount; i++)
				{
					Map3DRes_TileEffect* pEffect = pEffectList + i;

					memcpy( &pEffect->nEffectID, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pEffect->nPosX, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pEffect->nPosY, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pEffect->nPosZ, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
					memcpy( &pEffect->nRepeatTime, &g_FileBuffer[dwBufPos], 4 );
					dwBufPos += 4;
				}// for( nEffectCount )
			}// if


			// Original Code
/*
			fread( &nID, 4, 1 ,fp);
			fread( &nTileSetID, 4, 1, fp);
			fread( &nTileSetIndex, 1, 1, fp);
			fread( &nSeason, 1, 1 ,fp);
			fread( &nMeshID, 4, 1, fp);
			fread( &nMapID, 4, 1, fp);
			fread( &nXSize, 1, 1, fp);
			fread( &nYSize, 1, 1 ,fp);
			fread( &nZSize, 1, 1 ,fp);
			fread( &nRuleIndex, 4, 1 ,fp);
			fread( &nRenderOption, 4, 1, fp);
			fread( &nResType, 4, 1, fp);

			// 일단은 고정시켜놓자.
			nSpeedPerFrame = 30;

			fread( &nTexCount, 4, 1, fp);
			pTexList = new int[ nTexCount];
			fread( pTexList, nTexCount, sizeof( int), fp);
			
		//	fread( &nFigureCount, 4, 1, fp);

			if( nFigureCount > 0)
			{
				pFigureList = new Map3DRes_Figure[ nFigureCount];
				
				for(int i = 0; i < nFigureCount; i++)
				{
					Map3DRes_Figure *pFigure = pFigureList + i;

					fread( &pFigure->nID,			sizeof(pFigure->nID) , 1, fp);
					fread( &pFigure->nTileResID,	sizeof(pFigure->nTileResID) , 1, fp);
					fread( &pFigure->nType,			sizeof(pFigure->nType) , 1, fp);
					fread( &pFigure->nDirection,	sizeof(pFigure->nDirection) , 1, fp);
					fread( &pFigure->nPosX,			sizeof(pFigure->nPosX) , 1, fp);
					fread( &pFigure->nPosY,			sizeof(pFigure->nPosY) , 1, fp);
					fread( &pFigure->nPosZ,			sizeof(pFigure->nPosZ) , 1, fp);
					fread( &pFigure->nXSize,		sizeof(pFigure->nXSize) , 1, fp);
					fread( &pFigure->nYSize,		sizeof(pFigure->nYSize) , 1, fp);
					fread( &pFigure->nZSize,		sizeof(pFigure->nZSize) , 1, fp);
					fread( &pFigure->nHightPoint,	sizeof(pFigure->nHightPoint) , 1, fp);
				}

		//		fread( pFigureList, sizeof( Map3DRes_Figure) * nFigureCount, 1, fp);
			}// if

			// load attached effect data
			if( nEffectCount > 0 )
			{
				pEffectList = new Map3DRes_TileEffect [ nEffectCount ];
				for(int i=0; i<nEffectCount; i++)
				{
					Map3DRes_TileEffect* pEffect = pEffectList + i;

					fread( &pEffect->nEffectID,		sizeof(pEffect->nEffectID), 1, fp );
					fread( &pEffect->nPosX,			sizeof(pEffect->nPosX), 1, fp );
					fread( &pEffect->nPosY,			sizeof(pEffect->nPosY), 1, fp );
					fread( &pEffect->nPosZ,			sizeof(pEffect->nPosZ), 1, fp );
					fread( &pEffect->nRepeatTime,	sizeof(pEffect->nRepeatTime), 1, fp );
				}// for( nEffectCount )

			}// if
*/

		}
	};
};