#pragma once
#include "EffectRes.h"
#include "MapDecal.h"
#include "Grass.h"

namespace XiahGameEngine
{
	namespace Map
	{
		struct sMapRenderInfo
		{
			D3DCOLOR m_DiffuseColor;

			D3DCOLOR m_FogColor;
			BOOL	 m_bFog;
			float	 m_fFogDensity;

			unsigned short	m_CameraBoundSize;
			float			m_fDetailMapRatio;

			// 암흑무 상태가 되면 화면이 어두워진다.
			BOOL	m_bAmhukmuFog;
		};

		#define MAPOBJECT_MAX_SUBMESH_COUNT 10
		#define MAPOBJECT_MAX_COLLIDEBOX_COUNT 50

		//---------------------------------------------------------------------------------------
		// 난중에 이따식이 SubMesh를 갖고 있어서 LOD도 되야 것다
		// 속도가 많이 느리니까. 일단 Mesh하고 Texture는 Partial로 해준다
		class CMapTileSubMesh
		{
		public:
			CMapTileSubMesh(Map3DRes_TileMeshBlock* pMeshBlock);
			CMapTileSubMesh();
			~CMapTileSubMesh();

			BOOL Create(Map3DRes_TileMeshBlock* pMeshBlock);
			BOOL SetLodLevel(float fLevel);

			Map3DRes_TileMeshBlock* m_pMeshBlock;

			void Clear();

			unsigned short			m_nLodVertex;	// Lod가 적용된 Vertex수
			unsigned short			m_nLodFace;		// Lod가 적용된 Face수
			IDirect3DIndexBuffer9*	m_pIndexBuffer;	// Lod가 적용된 FaceList
		};

		class CMapObjectRender : public CRenderObject
		{
		DECLARE_RENDERTYPE( eRT_NormalSort)
		public:
			XIAHGE_API CMapObjectRender();
			XIAHGE_API ~CMapObjectRender();

			XIAHGE_API BOOL Create(sObjTileInstance* pObj);
			XIAHGE_API BOOL Release();

			// 이제 맵 오브젝트를 애니메이션 시킬꺼다.
			XIAHGE_API BOOL PrepareRender();

			XIAHGE_API BOOL Render();
			//XIAHGE_API BOOL SetDetailLevel(float fLevel);

			XIAHGE_API BOOL IsValid(){ return m_pObj && m_pTileRes && m_pMesh;}

			XIAHGE_API BOOL Realize();

			XIAHGE_API BBoxAABB3*	GetObjectBound(){ return &m_BoundBox;};
			XIAHGE_API BOOL StopEffect();

			inline void VisibleEffect(bool bVisible = true)
			{
				if( m_pEffectPackagePair )
					m_pEffectPackagePair->bIsVisible = bVisible;			
			}

			BOOL SpawnEffect();

			XIAHGE_API BOOL RenderCollisionBound();
			XIAHGE_API int GetCollideBoxCount() { return m_nCollideBoxCount;}
			XIAHGE_API BBoxOBB3* GetCollideBoxList() { return &m_CollideBox[ 0];}

			XIAHGE_API inline BYTE GetAlphaBlendType()
			{
				switch( m_pTileRes->nRenderOption )
				{
				case 0:			// No Alpha
					return 1;
				case 1:			// Alpha Channel
					return 3;
				case 2:			// Alpha Test
					return 2;
				};// switch

				return 1;
			}

//		protected:
			sObjTileInstance* m_pObj;
			Map3DRes_TileRes*  m_pTileRes;


		protected:
			BOOL			   m_bRealized;

			Map3DRes_TileMesh* m_pMesh;
			int					m_nSubMesh;
			CMapTileSubMesh    m_SubMesh[MAPOBJECT_MAX_SUBMESH_COUNT];
			IDirect3DTexture9* m_pTexture[ MAPOBJECT_MAX_SUBMESH_COUNT];

			D3DMATERIAL9		   m_Material;
			
			int			m_nCollideBoxCount;

			Matrix4x4	m_ObjectTM;
			Matrix4x4	m_ObjectTM_NoScale;

			BBoxAABB3	m_BoundBox;
			BBoxOBB3	m_CollideBox[ MAPOBJECT_MAX_COLLIDEBOX_COUNT];

			// effect
			_EFFECTPACKAGEPAIR*		m_pEffectPackagePair;

			// 애니메이션에 필요한 데이타.
/*
			float			m_CurFrame;
			float			m_PreFrame;
*/
			DWORD			m_LastUpdateTime;
			DWORD			m_dwTimeSum;
			Matrix4x4*		m_pMatrixList;	// 애니메이션 용 매트릭스
/*
			Quaternion*		m_pEvalBoneAni_Rot;
			Vector3*		m_pEvalBoneAni_Pos;
			unsigned char*	m_pEvalBoneAni_RotFlip;
*/
			int				m_nCurFrame;
		};
		//---------------------------------------------------------------------------------------
		typedef std::vector<CMapObjectRender *> MAPRENDER_MAPOBJECTLIST;
		typedef std::hash_set<CMapObjectRender *> MAPRENDER_MAPOBJECT_VISIBLETEST_LIST;

		//---------------------------------------------------------------------------------------
		class CMapCellRender_MeshBlock : public CRenderObject
		{
		DECLARE_RENDERTYPE( eRT_NormalSort);
		public:
			XIAHGE_API CMapCellRender_MeshBlock();
			XIAHGE_API ~CMapCellRender_MeshBlock();

			XIAHGE_API BOOL Create(CRes_MeshBlock* pMeshBlock,IDirect3DTexture9* pDetailTexture);
			XIAHGE_API BOOL Release();
			
			XIAHGE_API BOOL Render();

			//XIAHGE_API BOOL SetDetailLevel(float fLevel);

			XIAHGE_API inline BBoxAABB3* GetBoundBox()
			{
				DBG_Assert( m_pMeshBlock != NULL);

				return &m_pMeshBlock->m_BoundBox;
			}

			XIAHGE_API BOOL SetDiffuseColor(D3DCOLOR color);

			XIAHGE_API inline void ShowDetail(BOOL bShow)
			{
				m_bShowDetail = bShow;				
			}

			XIAHGE_API MAPRENDER_MAPOBJECTLIST* GetMapObjectList()
			{
				return &m_MapObjectList;
			}

		protected:
			CRes_MeshBlock* m_pMeshBlock;
		
			unsigned short m_nLodVertex;
			unsigned short m_nLodFace;

			IDirect3DIndexBuffer9* m_pIndexBuffer;
			IDirect3DTexture9*	   m_pDetailTexture;

			D3DCOLOR				m_DiffuseColor;
			BOOL					m_bShowDetail;
			
		public:
			MAPRENDER_MAPOBJECTLIST	m_MapObjectList;
		};

		//---------------------------------------------------------------------------------------
		typedef std::vector<CMapCellRender_MeshBlock *> MAPRENDER_MESHBLOCK_LIST;
		

		//
		//---------------------------------------------------------------------------------------
		typedef std::map<float, float>	DFLOATMAP;

		typedef std::list<CGrass*> GRASSLIST;

		#define	GRASSZONE_MAPMAX	50	// max of list. 하나의 GrassZone이 가질수 있는 리스트 최대

		struct sGrassZoneInfo
		{
			sGrassZoneInfo()
			{
			}

			~sGrassZoneInfo()
			{
				for(int i=0; i<byGrassPosListIndex; i++)
					GrassPosList[i].clear();
			}

			BYTE			byPositionByCamera;					// 카메라와 풀과의 위치 관계
			bool			bRender;							// 렌더링 할까?
			BYTE			byGrassTextureIndex;				// 0-7 사이의 값.
			DFLOATMAP		GrassPosList[ GRASSZONE_MAPMAX ];	// 풀 위치를 저장하는 리스트
			BYTE			byGrassPosListIndex;				// 리스트의 개수
			BYTE			byGrassZoneDensity;					// 풀 밀집도

			GRASSLIST		GrassZoneList;						// 사용할 CGrass List
		};

		//---------------------------------------------------------------------------------------
		class CMapCellRender : public CRenderObject
		{
		DECLARE_RENDERTYPE( eRT_NormalSort);
		public:
			XIAHGE_API CMapCellRender();
			XIAHGE_API virtual ~CMapCellRender();

			XIAHGE_API BOOL Create(CRes_MapCell *pMapCell);

			XIAHGE_API BOOL Render(BYTE byType);
			XIAHGE_API BOOL RenderWater();
			XIAHGE_API BOOL PrepareRender();

			XIAHGE_API BOOL SetGrassZonePos();
			XIAHGE_API BOOL UpdateGrass();
			XIAHGE_API BOOL ResetGrassZone();
			XIAHGE_API BOOL RenderGrass();

			XIAHGE_API BOOL Release();

			XIAHGE_API BOOL QueryVisibleMeshblock_Level2(CCamera *pCamera,unsigned short CameraBoundSize);
			XIAHGE_API BOOL QueryVisibleMeshblock_Level1(sRect *pBound);

			XIAHGE_API inline BOOL IsValid()
			{
				return m_pMapCell != NULL;
			}

			XIAHGE_API int GetVisibleMeshblockCount_Level1()
			{
				return m_VisibleMeshBlockList_Level1.size();
			}

			XIAHGE_API int GetVisibleMeshBlockCount_Level2()
			{
				return m_VisibleMeshBlockList_Level2.size();
			}

			XIAHGE_API int GetVisibleObjectList_Level2()
			{
//				return m_VisibleObjectList_Level2.size();
				return m_VisibleObjectListNoAlpha.size() + m_VisibleObjectListAlphaTest.size() + m_VisibleObjectListAlpha.size();
			}

			XIAHGE_API BOOL		SetDiffuseColor(D3DCOLOR color);

			XIAHGE_API inline void	SetDetailRatio(float fDetailRatio)
			{
				m_fDetailRatio = fDetailRatio;
			}

			XIAHGE_API BOOL		ClearVisible();
			XIAHGE_API BOOL		ReleaseEffect();

			XIAHGE_API MAPRENDER_MAPOBJECTLIST* GetMeshBlockObjectList(int meshblock_x,int meshblock_y)
			{
				return m_MeshBlock[ meshblock_x + meshblock_y * 8].GetMapObjectList();
			}

			// 바운드 박스 보이기
			XIAHGE_API void Show_BoundBox()
			{
				m_ShowBoundBox = !m_ShowBoundBox;
			}

			// 마우스 위치 커서가 맵 오브젝트위에 있을때 검사
			BOOL CheckMapObjectForMouseCursor(BBoxAABB3 MouseBound, float fCharHeight, float fCharPositionY);

			// 마우스 위치 커서가 맵 오브젝트 위에 있을때 버텍스를 새롭게 계산해준다.
			BOOL MakeMouseCursorVertexOnObject(Vector2 vSmall, Vector2 vBig, D3DCOLOR dColor, VT_LVertex* pVertex, WORD* pFace, int& nVertex, int& nFace);

			// 두개의 직사각형의 겹치는 정점을 구한다. 2차원 계산.
			BOOL FindIntersectVector(Vector2 vSmall1, Vector2 vBig1, Vector2 vSmall2, Vector2 vBig2, int& nTotalX, int& nTotalY, float *fXArray, float *fYArray, float *fIXArray, float *fIYArray);

			// 마우스 위치 커서 데칼을 직접 저장한다.
			BOOL SetDecalOnMouseCursor(CMapDecal* pDecal);

		protected:
			CRes_MapCell* m_pMapCell;

			CMapCellRender_MeshBlock m_MeshBlock[ MAPCELL_MESHBLOCK_COUNT];

			MAPRENDER_MESHBLOCK_LIST m_VisibleMeshBlockList_Level1;	// Bound에 의해
			MAPRENDER_MESHBLOCK_LIST m_VisibleMeshBlockList_Level2; // 카메라 Frustum에 의해

			MAPRENDER_MAPOBJECTLIST	m_VisibleObjectListNoAlpha;		// 1
			MAPRENDER_MAPOBJECTLIST	m_VisibleObjectListAlphaTest;	// 2
			MAPRENDER_MAPOBJECTLIST	m_VisibleObjectListAlpha;		// 3

			// 요넘만 동적 할당 하겠다
			int					 m_nMapObject;
			CMapObjectRender	*m_pMapObject[ 1000];

			// 졸라 떡대쟁이 Object들은 문제가 있겠다			
//			MAPRENDER_MAPOBJECTLIST m_VisibleObjectList_Level2;
			MAPRENDER_MAPOBJECT_VISIBLETEST_LIST	m_InvisibleObjectList;

			// Diffuse Color
			D3DCOLOR			m_DiffuseColor;
			float				m_fDetailRatio;

			// grass zone
			sGrassZoneInfo*		m_pGrassZoneInfo;

			// Misc
			BOOL					m_ShowBoundBox;

			CMapObjectRender*	m_pMapObjectWidthMouseCursor;	// 마우스 커서가 있는 맵 오브젝트
			CMapDecal*			m_pDecalOnMapObject;			// 마우스 위치 커서 Decal

		};

		//---------------------------------------------------------------------------------------
		typedef std::vector<CMapCellRender*> MAPRENDER_MAPCELL_LIST;

		#define MAX_MAPCELL_COUNT	64

		// g_pCurrentCamera에 대해서 해준다
		#define CAMERA_BOUND_GRID_SIZE	128	// 반경 128
		//---------------------------------------------------------------------------------------
		
		typedef std::vector<CMapDecal*> MAPDECAL_LIST;

		#define	GRASSZONE_ARRAY_MAX		30

		class CMapRender
		{
		public:
			XIAHGE_API CMapRender();
			XIAHGE_API virtual ~CMapRender();

			XIAHGE_API virtual BOOL Update();
			XIAHGE_API virtual BOOL RenderTerrain();
			XIAHGE_API virtual BOOL RenderObject(BYTE byType);
			XIAHGE_API virtual BOOL RenderWater();

			XIAHGE_API virtual BOOL UpdateGrassZone();
			XIAHGE_API virtual BOOL RenderGrassZone();

			XIAHGE_API virtual BOOL Release();
			XIAHGE_API virtual BOOL ReleaseVisibleMapCell();
			
			XIAHGE_API int GetVisibleMapCellCount()
			{
				return m_VisibleMapCellList.size();
			}

			XIAHGE_API int GetVisibleMeshBlockCount()
			{
				return m_nVisibleMeshBlock;
			}

			XIAHGE_API int GetVisibleObjectCount()
			{
				return m_nVisibleObject;
			}

			XIAHGE_API BOOL SetRenderInfo( sMapRenderInfo* pInfo);

			XIAHGE_API BOOL AddVisibalMapDecal(CMapDecal* pMapDecal)
			{
				m_VisibleMapDecalList.push_back( pMapDecal);
				return TRUE;
			}

			XIAHGE_API BOOL DeleteVisibleMapDecal(CMapDecal* pMapDecal)
			{
				MAPDECAL_LIST::iterator it;

				for(it = m_VisibleMapDecalList.begin(); it != m_VisibleMapDecalList.end(); it++)
				{
					if( pMapDecal == *it)
					{
						m_VisibleMapDecalList.erase( it);
						break;
					}
				}

				return TRUE;
			}

			XIAHGE_API BOOL ClearVisibleMapDecalList()
			{
				m_VisibleMapDecalList.clear();
				return TRUE;
			}

			// 충돌 처리를 위해서 특정 위치에 해당되는 MeshBlock에 속한 모든 Object들의 list를 준다
			XIAHGE_API BOOL QueryMeshblockObjectList(WORD wPosX,WORD wPosY,MAPRENDER_MAPOBJECTLIST **ppList);

		protected:
			XIAHGE_API virtual BOOL PrepareMapCell();
			XIAHGE_API virtual BOOL UpdateMapCellRenderInfo();

		protected:
			sRect			m_CameraBound;	// 카메라의 현재 위치
			sRect			m_MapBound;		// Map의 현재 사이즈 (위치값은 0,0)
			CMapCellRender	m_MapCellRender[ MAX_MAPCELL_COUNT];

			MAPRENDER_MAPCELL_LIST m_VisibleMapCellList;

			int				m_nVisibleMeshBlock;
			int				m_nVisibleObject;

			sMapRenderInfo m_MapRenderInfo;

			MAPDECAL_LIST	m_VisibleMapDecalList;

			// 물, 용암 texture
			LPDIRECT3DTEXTURE9	m_pWaterTexture;
			LPDIRECT3DTEXTURE9	m_pYongAmTexture;

			// 늪지대 물.
			LPDIRECT3DTEXTURE9	m_pMarshTexture;

		public:	// Grass Zone
			// 총 사용할 수 있는 CGrass를 리스트로 가지고 있고, 필요한 만큼 리스트에서 빼내서 쓴다.
			GRASSLIST		m_GrassZoneList;
			CGrass			m_GrassZonePool[ GRASSZONE_ARRAY_MAX ];

			XIAHGE_API BOOL ReleaseGrassZoneArray();
			XIAHGE_API BOOL CreateGrassZoneArray();

			// 마우스 위치 커서가 맵 오브젝트위에 있을때 검사
			BOOL CheckMapObjectForMouseCursor(BBoxAABB3 MouseBound, float fCharHeight, float fCharPositionY);
			// 마우스 위치 커서가 맵 오브젝트 위에 있을때 버텍스를 새롭게 계산해준다.
			BOOL MakeMouseCursorVertexOnObject(Vector2 vSmall, Vector2 vBig, D3DCOLOR dColor, VT_LVertex* pVertex, WORD* pFace, int& nVertex, int& nFace);

			CMapCellRender*		m_pMapCellRenderWidthMouseCursor;// 마우스 위치 커서가 있는 맵 셀
		};

		XIAHGE_API extern CMapRender g_MapRender;
	};
};