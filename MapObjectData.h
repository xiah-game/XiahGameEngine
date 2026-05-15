#pragma once

#include "CharMan.h"

/*
	MapObject Data처리

	TileEditor에서 등록된 넘들이다


*/

/*
	졸라 원래는 여기다 다 바꿔야 되는데.

	시간 여유가 않된다... !!

	일단 복사!
*/

/*
	로딩된 Tile들 다시 Release하는 모듈이 필요함
*/

namespace XiahGameEngine
{
	namespace Map
	{
		//-------- Object Tile정보
		struct Map3D_TileResIndex
		{
			int nID;
			int nTileSetID;
			BYTE nTileSetIndex;
			int nRuleIndex;
			BYTE nSeason;
			int nFigureCount;
			int nTileEffectCount;
			int nOffset;
		};

		//---------------------------------------------------------------------------------------
		typedef std::map<int,Map3D_TileResIndex> MAP3D_TILERESINDEX;
		typedef std::vector<Map3D_TileResIndex *> MAP3D_TILERESINDEXLIST;

		//---------------------------------------------------------------------------------------
		struct Map3D_TileSubSetIndex
		{
			int GetTile(int nRuleIndex,int nSeason)
			{
				MAP3D_TILERESINDEXLIST *pList;
				std::vector<int> idlist;
				unsigned long i;

				pList = &m_TileResList[ nSeason];
				
				if( pList->size() > 0)
				{
					for(i = 0; i < pList->size(); i++)
					{
						if( pList->operator []( i)->nRuleIndex == nRuleIndex)
							idlist.push_back( pList->operator []( i)->nID);
					}
				}
				
				if( idlist.size() == 0)
				{
					pList = &m_TileResList[ 4];
					for(i = 0; i < pList->size(); i++)
					{
						if( pList->operator []( i)->nRuleIndex == nRuleIndex)
							idlist.push_back( pList->operator []( i)->nID);
					}
				}

				if( idlist.size() == 0)
				{
					pList = &m_TileResList[ 0];
					for(i = 0; i < pList->size(); i++)
					{
						if( pList->operator []( i)->nRuleIndex == nRuleIndex)
							idlist.push_back( pList->operator []( i)->nID);
					}
				}

				if( idlist.size() == 0)
					return -1;

				return idlist[ 0];
			}

			MAP3D_TILERESINDEXLIST m_TileResList[ 5];	// 5개절
		};

		//---------------------------------------------------------------------------------------
		struct Map3D_TileSetIndex
		{
			int nID;

			Map3D_TileSubSetIndex m_SubSet[ 5];
		};

		//---------------------------------------------------------------------------------------
		typedef std::map<int,Map3D_TileSetIndex *> MAP3D_TILESETINDEXLIST;

		//---------------------------------------------------------------------------------------
		struct Map3D_TileMeshIndex
		{
			int nID;
			int nOffset;
			int nLength;
		};

		//---------------------------------------------------------------------------------------
		typedef std::map<int,Map3D_TileMeshIndex> MAP3D_TILEMESHINDEX;

		//---------------------------------------------------------------------------------------
		struct Map3DRes_TileMeshBlock
		{
			BOOL m_bPhysique;
			int m_nVertex;
			int m_nFace;

			IDirect3DVertexBuffer9* m_pVertexBuffer;
			IDirect3DIndexBuffer9*  m_pIndexBuffer;

			// LOD 엄따.
/*
			// 2003-04-21 acechangth add
			unsigned short	lod_count;
			unsigned short  *lod_collapse_ptr;	// Lod Collapse리스트		-> Vertex수 만큼
			unsigned short  *lod_facecount_ptr;	// Lod FaceCount수리스트	-> Vertex수 만큼
*/

			Map3DRes_TileMeshBlock()
			{
				m_pVertexBuffer = NULL;
				m_pIndexBuffer = NULL;
//				lod_collapse_ptr = NULL;
//				lod_facecount_ptr = NULL;
			}

			~Map3DRes_TileMeshBlock()
			{
				if( m_pVertexBuffer)
				{
					m_pVertexBuffer->Release();
					m_pVertexBuffer = NULL;
				}

				if( m_pIndexBuffer)
				{
					m_pIndexBuffer->Release();
					m_pIndexBuffer = NULL;
				}

//				if( lod_collapse_ptr ) delete []lod_collapse_ptr;
//				if( lod_facecount_ptr ) delete []lod_facecount_ptr;
			}
		};

		//---------------------------------------------------------------------------------------
		struct Map3DRes_TileMesh
		{
			Map3DRes_TileMesh()
			{
				m_nMeshBlock = 0;
				m_pMeshBlockList = NULL;
//				skeleton_ptr = NULL;
//				animation_ptr = NULL;
				m_pMatrix = NULL;
				m_bPhysique = false;
			}

			~Map3DRes_TileMesh()
			{
				if( m_pMeshBlockList)
				{
					delete [] m_pMeshBlockList;
				}
				m_pMeshBlockList = NULL;

/*				잠시 보류
				if( skeleton_ptr )
				{
					delete []skeleton_ptr->bone_ptr;

					delete []skeleton_ptr->world_inv_matrix;
					delete []skeleton_ptr->world_matrix;

					delete skeleton_ptr;
				}
				skeleton_ptr = NULL;

				if( animation_ptr )
				{
					for(int j=0; j<animation_ptr->bone_count; j++)
					{
						Bone_AniController* pAniCon = &animation_ptr->bone_anicontroller_ptr[j];

						delete []pAniCon->poskey_ptr;
						delete []pAniCon->rotkey_ptr;
						delete []pAniCon->scalekey_ptr;
					}
					delete []animation_ptr->bone_anicontroller_ptr;

					delete animation_ptr;
				}
				animation_ptr = NULL;
*/

				if( m_pMatrix )
					delete []m_pMatrix;
				m_pMatrix = NULL;
			}
			
			void Load(FILE *fp);

			int						 m_nID;

			unsigned short			m_nFrame;
			int						m_nMeshBlock;
			Map3DRes_TileMeshBlock *m_pMeshBlockList;

			bool					m_bPhysique;
			unsigned short			m_nBones;
			Matrix4x4*				m_pMatrix;

			// 애니메이션 데이타
//			Res_Skeleton*				skeleton_ptr;
//			Res_Skeleton_Animation*		animation_ptr;
		};

		//---------------------------------------------------------------------------------------
		typedef std::map<int,Map3DRes_TileMesh *> MAP3D_TILEMESHLIST;

		//---------------------------------------------------------------------------------------
		struct Map3DRes_Figure
		{
			int nID;
			int nTileResID;
			BYTE nType;
			BYTE nDirection;
			int nPosX;
			int nPosY;
			int nPosZ;
			BYTE nXSize;
			BYTE nYSize;
			BYTE nZSize;
			BYTE nHightPoint;
		};

		// tile effect data
		struct Map3DRes_TileEffect
		{
			int		nEffectID;
			int		nPosX;
			int		nPosY;
			int		nPosZ;
			int		nRepeatTime;
		};

		//---------------------------------------------------------------------------------------
		struct Map3DRes_TileRes
		{
			Map3DRes_TileRes()
			{
				pTexList = NULL;
				pFigureList = NULL;
				pEffectList = NULL;
			}

			~Map3DRes_TileRes()
			{
				if( pTexList)
				{
					delete []pTexList;
					pTexList = NULL;
				}

				if( pFigureList)
				{
					delete []pFigureList;
					pFigureList = NULL;
				}

				if( pEffectList )
				{
					delete []pEffectList;
					pEffectList = NULL;
				}
			}
			
			void Load(FILE *fp);

			int nID;
			int nTileSetID;
			BYTE nTileSetIndex;
			BYTE nSeason;
			int nMeshID;
			int nMapID;
			BYTE nXSize;
			BYTE nYSize;
			BYTE nZSize;
			int nRuleIndex;
			int nRenderOption;
			int nResType;
			unsigned short	nSpeedPerFrame;	// BCF animation speed per frame

			int nTexCount;
			int *pTexList;
			
			int nFigureCount;
			Map3DRes_Figure *pFigureList;

			int nEffectCount;
			Map3DRes_TileEffect	*pEffectList;
		};

		//---------------------------------------------------------------------------------------
		typedef std::map<int,Map3DRes_TileRes *> MAP3D_TILERESLIST;

		//---------------------------------------------------------------------------------------
		struct sObjTileInstance
		{
		public:
			int nPosX;
			int nPosY;
			int nPosZ;
			int nRotX;
			int nRotY;
			int nRotZ;
			
			int  nTileSetID;	
			BYTE nSubSet;		//	Billow Comments	: 한 TileSet안에서 Type별 Index
			int  nRuleIndex;	//	Billow Comments	: Variation
		};

		//---------------------------------------------------------------------------------------
		class CMap3DRes_Tile
		{
		public:
			CMap3DRes_Tile();
			virtual ~CMap3DRes_Tile();

			BOOL Create(LPCTSTR tile_index,LPCTSTR tile_pak,LPCTSTR mesh_index,LPCTSTR mesh_pak);
			void Release();

			int GetTileMeshOffset(int nID);
			XIAHGE_API Map3DRes_TileMesh *GetTileMesh(int nID);	

			Map3D_TileResIndex *GetTileResIndex(int nID);
			Map3DRes_TileRes *GetTileRes(int nID);

			Map3DRes_TileRes *GetTileRes(int nTileSetID,int nSubSet,int nRuleIndex,int nSeason);

			Map3DRes_TileRes *GetTileRes(sObjTileInstance *pObjTile)
			{
				return GetTileRes( pObjTile->nTileSetID, pObjTile->nSubSet, pObjTile->nRuleIndex, 0);
			}

			Map3D_TileSetIndex *GetTileSetIndex(int nID);

			XIAHGE_API BOOL Clear();
		public:
			FILE *m_hTilePackage;
			FILE *m_hMeshPackage;

			MAP3D_TILERESINDEX m_TileResIndexList;
			MAP3D_TILESETINDEXLIST m_TileSetIndexList;

			MAP3D_TILEMESHINDEX	m_TileMeshIndexList;
			
			MAP3D_TILEMESHLIST	m_TileMeshList;
			MAP3D_TILERESLIST   m_TileResList;
		};

		extern XIAHGE_API CMap3DRes_Tile		g_TileRes;

	};
};