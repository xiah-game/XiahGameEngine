#pragma once

#include "MapObjectData.h"

namespace XiahGameEngine
{
	namespace Map
	{

		//////////////////////////////////////////////////////////////////////////

		struct sMapData_Vertex
		{
			Vector3 pos;
			unsigned long color;
			Vector2 tex_color;
			Vector2 tex_detail;
		};

		#define D3DFVF_MAPDATA_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2)

		struct sGrass
		{
			unsigned long nResID;
			unsigned char density;
		};

		#define MAPCELL_MESHBLOCK_COUNT	64
		#define MAPCELL_HEIGHT_COUNT ((64 + 1) * (64 + 1))

		#define MESHBLOCK_TILE_XSIZE	8
		#define MESHBLOCK_TILE_YSIZE	8

		#define MAPCELL_MESHBLOCK_XSIZE 8
		#define MAPCELL_MESHBLOCK_YSIZE 8

		#define TILE_GRID_SIZE_FLOAT	4.0f	
		#define TILE_GRID_SIZE			4

		class CRes_MeshBlock
		{
		public:
			XIAHGE_API CRes_MeshBlock();
			XIAHGE_API ~CRes_MeshBlock();

			XIAHGE_API BOOL Release();
			XIAHGE_API BOOL ReadData(LPBYTE &pData);

		public:
			unsigned long m_nVertex;
			unsigned long m_nFace;

			IDirect3DVertexBuffer9* m_pVertex;
			unsigned short*			m_pFace;

			unsigned short			m_nLodCount;

			unsigned short*			m_pCollapsedIDList;
			unsigned short*			m_pLodFaceList;

			BBoxAABB3				m_BoundBox;
		};
	

		//
		struct sGrassZonePackageData
		{
			float fX1, fZ1;
			float fX2, fZ2;
			int nType, nDensity;
		};

		//---------------------------------------------------------------------------------------
		// 단인 MapCell정보
		class CRes_MapCell
		{
		public:
			XIAHGE_API CRes_MapCell(HANDLE hFileHandle);
			XIAHGE_API ~CRes_MapCell();

			XIAHGE_API BOOL Realize();
			XIAHGE_API BOOL Release();

		public:
			unsigned short m_XPos;
			unsigned short m_YPos;

			unsigned long m_nOffset;
			unsigned long m_nSize;

			HANDLE	m_hFileHandle;
			
			BOOL m_bRealized;
			
			// 실데이터
			CRes_MeshBlock m_pHeightMeshBlock[ MAPCELL_MESHBLOCK_COUNT];
			CRes_MeshBlock m_pWaterMeshBlock[ MAPCELL_MESHBLOCK_COUNT];

			IDirect3DTexture9* m_pTexture;
			IDirect3DTexture9* m_pDetailTexture[ MAPCELL_MESHBLOCK_COUNT];

			sGrass		   m_Grass[ MAPCELL_MESHBLOCK_COUNT];

			unsigned char  m_Height[MAPCELL_HEIGHT_COUNT];

			// Object
			unsigned short	  m_nObject;
			sObjTileInstance* m_pObject;

			unsigned long	  m_nWaterVertex;
			unsigned long	  m_nWaterFace;

			IDirect3DVertexBuffer9* m_pWaterVertex;
			IDirect3DIndexBuffer9*	m_pWaterFace;

			// GrassZone
			unsigned short	m_nGrassZoneCount;
			sGrassZonePackageData*	m_pGrassZoneInfo;

		public:
			XIAHGE_API float GetHeight(float x,float y);

		protected:
			BOOL ReadHeader();
		};

		typedef std::hash_map<unsigned long,CRes_MapCell*> RES_MAPCELL_LIST;

		//--------------------------------------------------------------------------------------------
		XIAHGE_API extern BOOL InitializeXiahMap(LPCTSTR *pFileList);
		XIAHGE_API extern BOOL UninitializeXiahMap();

		enum eWaterTypeEnum
		{
			eWaterType_Water,
			eWaterType_YongAm,
			eWaterType_Marsh
		};

		// LinkMap구조 이기 때문에. Map파일 단위로 초기화가 이루어 져야 된다
		class CMapRes
		{
		public:
			XIAHGE_API CMapRes();
			XIAHGE_API ~CMapRes();

			XIAHGE_API BOOL LoadMapFile(LPCTSTR mapfile);
			XIAHGE_API BOOL Release();

			XIAHGE_API CRes_MapCell* GetMapCell(unsigned short x,unsigned short y);
			
			XIAHGE_API float GetHeight(float x,float y);

			// variables
			bool	m_bLoadMapFile;
			BYTE	m_byWaterType;

		protected:
			RES_MAPCELL_LIST	m_MapList;
			HANDLE				m_hFileHandle;
		};

		XIAHGE_API extern CMapRes g_MapRes;
	};
};