#include "stdafx.h"
#include "MapData.h"
#include "MapRender.h"
#include "MapObjectData.h"
#include "XiahPak.h"

#define TEST_PERFORMANCE	0

//#define SHOWBOUNDBOX	// Bound BOx를 그린다

namespace XiahGameEngine
{
	namespace Map
	{
		unsigned short g_TempFaceBuffer[ 30000];

#if TEST_PERFORMANCE
		BOOL		g_bLog = FALSE;
		DWORD		g_LastLogTime;
		DWORD		g_StartTime;
#endif

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

		CMapTileSubMesh::CMapTileSubMesh(Map3DRes_TileMeshBlock* pMeshBlock)
		{
			m_pMeshBlock = NULL;
			m_pIndexBuffer = NULL;
			Create( pMeshBlock );
		}

		CMapTileSubMesh::CMapTileSubMesh()
		{
			m_pMeshBlock = NULL;
			m_pIndexBuffer = NULL;
		}

		CMapTileSubMesh::~CMapTileSubMesh()
		{
			Clear();
		}

		void CMapTileSubMesh::Clear()
		{
			m_pMeshBlock = NULL;

			if( m_pIndexBuffer)
			{
				m_pIndexBuffer->Release();
				m_pIndexBuffer = NULL;
			}
		}

		BOOL CMapTileSubMesh::Create(Map3DRes_TileMeshBlock* pMeshBlock)
		{
			Clear();
			if(pMeshBlock == NULL)
			{
				DBG_LogFile( _T("CMapTileSubMesh::Create fail"));
			}
			m_pMeshBlock = pMeshBlock;

			// Physique이 들어 있으면 vertex type 에러가 나서 하드웨어 가속을 받을 수 없다.
			// 즉, 피직이 있으면 무조건 D3DPOOL_SYSTEMMEM 이다. 
			HRESULT hr;
			if( m_pMeshBlock->m_bPhysique )
				hr = g_pDirect3DDevice->CreateIndexBuffer( m_pMeshBlock->m_nFace * 2 * 3, 0, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIndexBuffer, NULL);
			else
                hr = g_pDirect3DDevice->CreateIndexBuffer( m_pMeshBlock->m_nFace * 2 * 3, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);

			if( FAILED( hr))
				return FALSE;

			// LOD level 이 1일때를 위해서 복사한다. 
			BYTE* pFace_ptr;

			hr = m_pMeshBlock->m_pIndexBuffer->Lock( 0, 0, (void**)&pFace_ptr, 0 );

			if( FAILED( hr) )return FALSE;

			LPBYTE pIndexData;

			hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, 0);

			if( FAILED( hr) )return FALSE;

			memcpy( pIndexData, pFace_ptr, m_pMeshBlock->m_nFace * 6 );

			m_pIndexBuffer->Unlock();
			m_pMeshBlock->m_pIndexBuffer->Unlock();

			//
			SetLodLevel( 1.0f);

			return TRUE;
		}

		BOOL CMapTileSubMesh::SetLodLevel(float fLevel)
		{
			m_nLodVertex = m_pMeshBlock->m_nVertex;
			m_nLodFace = m_pMeshBlock->m_nFace;

			// LOD 엄따.
/*
			if( fLevel < 0)
				fLevel = 0;

			if( fLevel > 1)
				fLevel = 1;

			int lodcount = (int)((1.0f - fLevel) * m_pMeshBlock->lod_count);

			if( lodcount == 0)
			{
				m_nLodVertex = m_pMeshBlock->m_nVertex;
				m_nLodFace = m_pMeshBlock->m_nFace;

				return TRUE;
			}

			m_nLodVertex = m_pMeshBlock->m_nVertex - lodcount;
			m_nLodFace   = m_pMeshBlock->m_nFace;

			int i;

			for(i = 0; i < lodcount; i++)
			{
				m_nLodFace -= m_pMeshBlock->lod_facecount_ptr[ m_pMeshBlock->m_nVertex - i - 1];
			}

			if( m_nLodFace <= 0)
			{
				m_nLodFace = 0;
				return TRUE;
			}

			// face pointer
			WORD* pFace_ptr;
#ifdef TL_ACCEL
			m_pMeshBlock->m_pIndexBuffer->Lock( 0, 0, (void**)&pFace_ptr, D3DLOCK_DISCARD );
#else
			m_pMeshBlock->m_pIndexBuffer->Lock( 0, 0, (void**)&pFace_ptr, 0 );
#endif

			//
			int collapse_id;
			int new_collapse_id;
			for(i = m_nLodFace - 1; i >= 0; i--)
			{
				for(int j = 0; j < 3; j++)
				{
					collapse_id =  pFace_ptr[ i * 3 + j];

					while( collapse_id >= m_nLodVertex)
					{
						new_collapse_id = m_pMeshBlock->lod_collapse_ptr[ collapse_id];

						if( new_collapse_id == collapse_id)
							break;

						collapse_id = new_collapse_id;
					}

					if( collapse_id < 0 || collapse_id > m_pMeshBlock->m_nVertex - 1)
						g_TempFaceBuffer[ i * 3 + j] = 0;
					else
						g_TempFaceBuffer[ i * 3 + j] = collapse_id;
				}
			}

			m_pMeshBlock->m_pIndexBuffer->Unlock();

			//Lock과 Unlock사이의 시간이 길어지면 그 만큼 비디오 카드가 Blocking되는 시간이 길어지기 때문에
			//전체적인 성능이 떨어지게 된다
			//Lock이나 특정 Command가 비디오 카드로 전송되는 과정을 제외하고는 
			//원칙적으로 비디오 카드와 CPU는 서로 독립적으로 돌아 가기때문에.
			//항상 그걸 염두해 두어야 한다!!

			LPBYTE pIndexData;
#ifdef TL_ACCEL
			HRESULT hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, D3DLOCK_DISCARD);
#else
			HRESULT hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, 0);
#endif
			if( FAILED( hr))
				return FALSE;

			memcpy( pIndexData, g_TempFaceBuffer, m_nLodFace * 3 * 2);

			m_pIndexBuffer->Unlock();
*/

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
		XIAHGE_API CMapObjectRender::CMapObjectRender()
		{
			m_pObj = NULL;
			m_pTileRes = NULL;
			m_pMesh = NULL;
			m_nCollideBoxCount = 0;

			ZeroMemory( m_pTexture, sizeof( IDirect3DTexture9*) * MAPOBJECT_MAX_SUBMESH_COUNT);

			m_bRealized = FALSE;

			m_nSubMesh = 0;

			m_pEffectPackagePair = NULL;

//			m_CurFrame = 0;
//			m_PreFrame = 0;
			m_pMatrixList = NULL;

			m_nCurFrame = 0;
			m_dwTimeSum = 0;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapObjectRender::~CMapObjectRender()
		{
			Release();
		}

		XIAHGE_API BOOL CMapObjectRender::Realize()
		{
			DBG_Assert( m_pObj != NULL && m_pTileRes != NULL);

			if( m_bRealized)
				return TRUE;

			//DBG_Put("Realize Tile : %d %d %d", m_pTileRes->nTileSetID, m_pTileRes->nTileSetIndex, m_pTileRes->nRuleIndex);

			// 이과정이 느리다
			if( m_pMesh == NULL)
			{
				m_pMesh = g_TileRes.GetTileMesh( m_pTileRes->nMeshID);

				if( m_pMesh)
				{
					m_nSubMesh = m_pMesh->m_nMeshBlock;
					for(int i=0; i<m_nSubMesh; i++)
					{
						m_SubMesh[i].Create( &m_pMesh->m_pMeshBlockList[i] );
					}

					// 맵 오브젝트 Animation Matrix
					if( m_pMesh->m_bPhysique && m_pMesh->m_nFrame > 1 )
					{
						int nSize = m_pMesh->m_nFrame * m_pMesh->m_nBones;
						m_pMatrixList = new Matrix4x4[ nSize ];

						memcpy( m_pMatrixList, m_pMesh->m_pMatrix, sizeof(Matrix4x4)*nSize );

						for(int i=0; i<nSize; i++)
							m_pMatrixList[i] = m_pMatrixList[i] * m_ObjectTM;
					}

				}// if
			}
			
			IDirect3DTexture9* pMainTexture = XiahPak::GetTexture( m_pTileRes->nMapID);

			DBG_Assert( m_pMesh->m_nMeshBlock <= MAPOBJECT_MAX_SUBMESH_COUNT);

			int i;
			for(i = 0; i < m_pTileRes->nTexCount; i++)
			{
				IDirect3DTexture9* pTexture = NULL;
				
				if( m_pTileRes->pTexList[ i] != 0)
					pTexture = XiahPak::GetTexture( m_pTileRes->pTexList[ i]);
				
				if( pTexture)
					m_pTexture[ i] = pTexture;
				else
					m_pTexture[ i] = pMainTexture;
			}

			// spawn effect of this tile
			SpawnEffect();

			//
			m_bRealized = TRUE;
			//m_fDetailLevel = 1;
		
			return TRUE;
		}

		BOOL CMapObjectRender::SpawnEffect()
		{
			if( m_pTileRes->nEffectCount > 0 && !m_pEffectPackagePair )
			{
				// 여러 이펙트를 하나로 묶어서 처리한다.
				g_EffectManager.MakeSharedPackagePair( 0, 0, 0 );

				for(int i=0; i<m_pTileRes->nEffectCount; i++)
				{
					Map3DRes_TileEffect* pTileEffect = m_pTileRes->pEffectList + i;

					int nEffectID = pTileEffect->nEffectID;
					int nPosX = pTileEffect->nPosX;
					int nPosY = pTileEffect->nPosY;
					int nPosZ = pTileEffect->nPosZ;

					_EFFECT* pEffect = g_EffectManager.GetEffect( nEffectID );
					if( pEffect )
						g_EffectManager.EnqEffectImmediately( pEffect, 0, nPosX, nPosY, nPosZ );
				}// for(m_pTileRes->nEffectCount)

				m_pEffectPackagePair = g_EffectManager.GetCurEffectPackagePair();

				g_EffectManager.OffSharedPackagePair();

				// Set effect matrix
				if( m_pEffectPackagePair )
				{
					m_pEffectPackagePair->WorldMatrix = (MATRIX)m_ObjectTM_NoScale;
				}

			}// if m_pTileRes->nEffectCount ()

			return TRUE;
		}

		int CollideBoxCompare(const void *a,const void *b)
		{
			BBoxOBB3* pBoxA = (BBoxOBB3*)a;
			BBoxOBB3* pBoxB = (BBoxOBB3*)b;

			if( pBoxA->GetHeightMax().y > pBoxB->GetHeightMax().y)
				return -1;

			if( pBoxA->GetHeightMax().y < pBoxB->GetHeightMax().y)
				return 1;

			return 0;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapObjectRender::Create(sObjTileInstance* pObj)
		{
			Release();
		
			if(pObj == NULL)
			{
				DBG_LogFile( _T("CMapObjectRender::Create fail"));
			}

			m_pTileRes = g_TileRes.GetTileRes( pObj);

//			DBG_Assert( m_pTileRes != NULL);

			if( m_pTileRes == NULL)
				return FALSE;

			m_pObj = pObj;
			
			// 충돌 박스 만들어 주기, 앗싸 드뎌 숙원사업이!!
			int tx,ty,tz;
			int tr_x,tr_y,tr_z;

			tx = pObj->nPosX;
			ty = pObj->nPosY;
			tz = pObj->nPosZ;

			tr_x = pObj->nRotX;
			tr_y = pObj->nRotY;
			tr_z = pObj->nRotZ;

			Matrix4x4 obj_tm;
			obj_tm.SetRotationEuler( Vector3( (float)tr_x * 3.141592f / 180.0f, 
									 (float)tr_y * 3.141592f / 180.0f, 
									 (float)tr_z * 3.141592f / 180.0f));			
			obj_tm.t = Vector3( tx, ty, -tz);
			
			m_nCollideBoxCount = 0;

			DBG_Assert( m_pTileRes->nFigureCount <= MAPOBJECT_MAX_COLLIDEBOX_COUNT);

			int i;
			for(i = 0; i < m_pTileRes->nFigureCount && i < MAPOBJECT_MAX_COLLIDEBOX_COUNT; i++)
			{
				Map3DRes_Figure &figure = m_pTileRes->pFigureList[ i];

				if( figure.nType != 1)
					continue;

				BBoxOBB3 &box = m_CollideBox[ m_nCollideBoxCount ++];

				Vector3 local_pos;
				Vector3 local_size;

				local_size = Vector3( figure.nXSize, 
									  figure.nYSize, 
									  figure.nZSize);

				local_pos = Vector3( figure.nPosX + (float)figure.nXSize / 2.0f, 
									 figure.nPosY + (float)figure.nYSize / 2.0f,
									 figure.nPosZ - (float)figure.nZSize / 2.0f);

				box = BBoxOBB3( local_pos, local_size / 2, obj_tm);
			
				if( i == 0)
					m_BoundBox = box.m_BBoxAABB;
				else
					m_BoundBox += box.m_BBoxAABB;
			}

			if( m_BoundBox.Size() == Vector3( 0, 0, 0))	// 우쨰 이런일이
			{
				BBoxOBB3 box( Vector3( 0, 1, 0), Vector3( 1, 1, 1), obj_tm);
				m_BoundBox = box.m_BBoxAABB;
			}

			qsort( m_CollideBox, m_nCollideBoxCount, sizeof( BBoxOBB3), CollideBoxCompare);
			
			m_ObjectTM.SetScale( Vector3( 1.0f / 0.816f, 1.0f / 0.816f, 1.0f / 0.816f));
			m_ObjectTM *= obj_tm;
			m_ObjectTM_NoScale = obj_tm;

			m_bRealized = FALSE;

			// Material
			ZeroMemory( &m_Material, sizeof( D3DMATERIAL9));

			m_Material.Ambient.r = 1;
			m_Material.Ambient.g = 1;
			m_Material.Ambient.b = 1;
			m_Material.Ambient.a = 1;
			m_Material.Diffuse.r = 1;
			m_Material.Diffuse.g = 1;
			m_Material.Diffuse.b = 1;
			m_Material.Diffuse.a = 1;
			m_Material.Specular.r = 1;
			m_Material.Specular.g = 1;
			m_Material.Specular.b = 1;
			m_Material.Specular.a = 1;

			//
			m_LastUpdateTime = g_dwCurTime;
			m_dwTimeSum = 0;

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapObjectRender::Release()
		{
			m_pObj = NULL;
			m_pTileRes = NULL;
			m_pMesh = NULL;
			m_nCollideBoxCount = 0;
			m_bRealized = FALSE;
			ZeroMemory( m_pTexture, sizeof( IDirect3DTexture9*) * MAPOBJECT_MAX_SUBMESH_COUNT);

			for(int i=0; i<m_nSubMesh; i++)
				m_SubMesh[i].Clear();
			m_nSubMesh = 0;

			StopEffect();

			if( m_pMatrixList )
				delete []m_pMatrixList;
			m_pMatrixList = NULL;
		
			return TRUE;
		}

//-------------------------------------------------------------------------------------
#define EVAL_BONE_ANI_KEY( controller, evalframe, index)	\
			preRotKey = &controller->rotkey_ptr[ 0];\
			nextRotKey = NULL;\
								\
			for(j = 1; j < controller->rotkey_count; j++)\
			{\
				tempRotKey = &controller->rotkey_ptr[ j];\
				if( evalframe < tempRotKey->frame)\
				{\
					nextRotKey = tempRotKey;\
					break;\
				}\
				else\
					preRotKey = tempRotKey;\
			}\
			\
			if( preRotKey && nextRotKey)\
			{\
				alpha = (evalframe - preRotKey->frame) / (nextRotKey->frame - preRotKey->frame);\
			\
				Quaternion q1( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);\
				Quaternion q2( nextRotKey->quaternion[0], nextRotKey->quaternion[1], nextRotKey->quaternion[2], nextRotKey->quaternion[3]);\
				\
				EvalRotKey[ index] = q1.Slerp( q2, alpha );\
				EvalRotFlip[ index] = preRotKey->flip < 0;\
			}\
			else\
			{\
				EvalRotKey[ index] = Quaternion( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);\
				EvalRotFlip[ index] = preRotKey->flip < 0;\
			}\
				\
			prePosKey = &controller->poskey_ptr[ 0];\
			nextPosKey = NULL;\
				\
			for(j = 1; j < controller->poskey_count; j++)\
			{\
				tempPosKey = &controller->poskey_ptr[ j];\
				if( evalframe < tempPosKey->frame)\
				{\
					nextPosKey = tempPosKey;\
					break;\
				}\
				else\
					prePosKey = tempPosKey;\
			}\
				\
			if( prePosKey && nextPosKey)\
			{\
				float alpha = (double)(evalframe - prePosKey->frame) / (double)(nextPosKey->frame - prePosKey->frame);\
				Vector3 pos1( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );\
				Vector3 pos2( nextPosKey->position[0], nextPosKey->position[1], nextPosKey->position[2] );\
				\
				EvalPosKey[ index] = pos1 * ( 1 - alpha) + pos2 * alpha;\
			}\
			else\
			{\
				EvalPosKey[ index] = Vector3( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );\
			}
//-------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
		// 맵 오브젝트 애니메이션.
		XIAHGE_API BOOL CMapObjectRender::PrepareRender()
		{
/*
			if( m_pMesh->animation_ptr == NULL ) return FALSE;

			int i,j;
			float fLength = m_pMesh->animation_ptr->time_length;
			float fPreFrame = m_PreFrame / fLength;
			float fCurFrame = m_CurFrame / fLength;

			// [ Build Animation Matrix ] 현재 프레임에 해당하는 매트릭스를 계산한다.
			// 1. Root Bone의 움직임을 읽는다
			Matrix4x4 PreFrameDelta;
			Matrix4x4 FrameDelta;

			Bone_AniController* pAniCon = &m_pMesh->animation_ptr->bone_anicontroller_ptr[0];
			float frame = fCurFrame;
			float pre_frame = fPreFrame;

			// Pos Key
			Bone_PosKey *prePosKey = &pAniCon->poskey_ptr[0];
			Bone_PosKey *nextPosKey = NULL;

			for(j=1; j<pAniCon->poskey_count; j++)
			{
				Bone_PosKey &key = pAniCon->poskey_ptr[j];
				if( frame < key.frame)
				{
					nextPosKey = &key;
					break;
				}
				else
					prePosKey = &key;
			}

			if( prePosKey && nextPosKey )
			{
				float alpha = (double)(frame - prePosKey->frame) / (double)(nextPosKey->frame - prePosKey->frame);
				Vector3 pos1( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
				Vector3 pos2( nextPosKey->position[0], nextPosKey->position[1], nextPosKey->position[2] );

				FrameDelta.t = pos1 * (1-alpha) + pos2 * alpha;
			}
			else
			{
				FrameDelta.t = Vector3( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
			}

			prePosKey = &pAniCon->poskey_ptr[0];
			nextPosKey = NULL;

			//
			for(j=1; j<pAniCon->poskey_count; j++)
			{
				Bone_PosKey &key = pAniCon->poskey_ptr[j];
				if( pre_frame < key.frame)
				{
					nextPosKey = &key;
					break;
				}
				else
					prePosKey = &key;
			}

			if( prePosKey && nextPosKey )
			{
				float alpha = (double)(pre_frame - prePosKey->frame) / (double)(nextPosKey->frame - prePosKey->frame);
				Vector3 pos1( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
				Vector3 pos2( nextPosKey->position[0], nextPosKey->position[1], nextPosKey->position[2] );

				PreFrameDelta.t = pos1 * (1-alpha) + pos2 * alpha;
			}
			else
			{
				PreFrameDelta.t = Vector3( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
			}

			FrameDelta = FrameDelta * PreFrameDelta.GetInverse();

			// 2. EvalAnimation
			// 맵 오브젝트에서는 대부분의 본이 1이므로 그다지 많은 부하는 아닐것임.
			int bone_count = m_pMesh->animation_ptr->bone_count;
			Bone_RotKey* preRotKey;
			Bone_RotKey* nextRotKey;
			Bone_RotKey* tempRotKey;
//			Bone_PosKey* prePosKey;
//			Bone_PosKey* nextPosKey;
			Bone_PosKey* tempPosKey;
//			Bone_AniController* pAniCon;
			float	alpha;

			m_pEvalBoneAni_Rot	= ((Quaternion *)g_TempFaceBuffer);
			m_pEvalBoneAni_Pos	= (Vector3 *)(m_pEvalBoneAni_Rot + bone_count);
			m_pEvalBoneAni_RotFlip = (unsigned char*)(m_pEvalBoneAni_Pos + bone_count);
			unsigned char* pFlip = m_pEvalBoneAni_RotFlip;

			Quaternion* pRot = m_pEvalBoneAni_Rot;
			Vector3*	pPos = m_pEvalBoneAni_Pos;

			Quaternion		EvalRotKey[ 2];
			unsigned char	EvalRotFlip[ 2];	// 1이면 flip이다
			Vector3			EvalPosKey[ 2];

			for(i=0; i<bone_count; i++, pRot++, pPos++, pFlip++)
			{
				pAniCon = &m_pMesh->animation_ptr->bone_anicontroller_ptr[ i];

				EVAL_BONE_ANI_KEY( pAniCon, fCurFrame, 0);	// 이놈이 다 한다네. ^^;

				*pRot = EvalRotKey[ 0];
				*pPos = EvalPosKey[ 0];
				*pFlip = EvalRotFlip [ 0];
			}// for

			// 3.
			Quaternion* pRotKey = m_pEvalBoneAni_Rot;
			Vector3*	pPosKey = m_pEvalBoneAni_Pos;
			pFlip = m_pEvalBoneAni_RotFlip;

			for(i=0; i<m_pMesh->animation_ptr->bone_count; i++, pRotKey ++, pPosKey ++, pFlip ++)
			{
				m_pMatrixList[ i].SetRotationQuaternion_Flip( *pRotKey, *pFlip == TRUE);
				m_pMatrixList[ i].t = *pPosKey;

				Bone_AniController* pAniCon = &m_pMesh->animation_ptr->bone_anicontroller_ptr[ i];
				m_pMatrixList[i].m[0][0] *= pAniCon->scalekey_ptr->scale[0];
				m_pMatrixList[i].m[0][1] *= pAniCon->scalekey_ptr->scale[0];
				m_pMatrixList[i].m[0][2] *= pAniCon->scalekey_ptr->scale[0];

				m_pMatrixList[i].m[1][0] *= pAniCon->scalekey_ptr->scale[0];
				m_pMatrixList[i].m[1][1] *= pAniCon->scalekey_ptr->scale[0];
				m_pMatrixList[i].m[1][2] *= pAniCon->scalekey_ptr->scale[0];

				m_pMatrixList[i].m[2][0] *= pAniCon->scalekey_ptr->scale[0];
				m_pMatrixList[i].m[2][1] *= pAniCon->scalekey_ptr->scale[0];
				m_pMatrixList[i].m[2][2] *= pAniCon->scalekey_ptr->scale[0];

				if( m_pMesh->skeleton_ptr->bone_ptr[ i].parent_bone != 65535)
				{
					m_pMatrixList[ i] = m_pMatrixList[ i] * m_pMatrixList[ m_pMesh->skeleton_ptr->bone_ptr[ i].parent_bone ];
				}
				else
				{
					m_pMatrixList[ i].t.x = 0;
					m_pMatrixList[ i].t.z = 0;
				}
			}// for

			// 4.
			m_ObjectTM = FrameDelta * m_ObjectTM;

			for(i = 0; i < m_pMesh->animation_ptr->bone_count; i++)
				m_pMatrixList[ i] = m_pMesh->skeleton_ptr->world_inv_matrix[ i] * m_pMatrixList[ i] * m_ObjectTM;

			// 그 다음 프레임 계산.
			m_PreFrame = m_CurFrame;

			float fDelta = (g_dwCurTime - m_LastUpdateTime);
			m_LastUpdateTime = g_dwCurTime;

			m_CurFrame += fDelta;
			if( m_CurFrame > fLength )
			{
				// 기본적으로 맵 오브젝트는 계속 반복이다.
				m_CurFrame -= fLength;
				m_PreFrame = m_CurFrame;
			}
*/

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapObjectRender::Render()
		{
			if( m_bRealized == FALSE || m_pObj == NULL || m_pTileRes == NULL || m_pMesh == NULL)
				return FALSE;

			// 잠시만.
//			if( m_pMesh->m_bPhysique ) return FALSE;

/*
			// 잠시 보류, 
			// 매 프레임마다 캐릭터처럼 애니메이션 매트릭스를 만들면 계산량이 많아지므로
			// 아예 매트릭스를 계산하고 메모리에 저장해서 바로 쓰자.
			// 애니메이션 매트릭스 만들기.
			if( m_pMesh->m_nFrame > 1 )
				PrepareRender();
*/

			// 임시
			//m_Material.Diffuse.a = 0.3f;
			//m_Material.Ambient.a = 0.3f;
			//m_Material.Specular.a = 0.3f;

			//
			g_pDirect3DDevice->SetMaterial( &m_Material);
			g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

			switch( m_pTileRes->nRenderOption )
			{
			case 0:			// No Alpha
				g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE);

				g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE  );
				g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
				break;
			case 1:			// Alpha Channel
				g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);

				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
				g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				break;
			case 2:			// Alpha Test
				g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
				g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF, 0x80 ); //0x0000000F ); //0xfd);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);

				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT   );
				g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				break;
			};// switch

			for(int i=0; i<m_pMesh->m_nMeshBlock; i++)
			{
				g_Device.SetTexture(0, m_pTexture[i]);
				//g_pDirect3DDevice->SetTexture( 0, m_pTexture[ i]);

				if( m_SubMesh[i].m_pMeshBlock->m_bPhysique )
				{
					g_pDirect3DDevice->SetSoftwareVertexProcessing( TRUE );

					g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );
					g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

					// frame animation
					DWORD dwDelta = g_dwCurTime - m_LastUpdateTime;
					m_LastUpdateTime = g_dwCurTime;

					m_dwTimeSum += dwDelta;
					if( m_dwTimeSum >= m_pTileRes->nSpeedPerFrame )
					{
						while( m_dwTimeSum >= m_pTileRes->nSpeedPerFrame )
						{
							m_nCurFrame++;
							m_dwTimeSum -= m_pTileRes->nSpeedPerFrame;

							if( m_nCurFrame > m_pMesh->m_nFrame-1 )
								m_nCurFrame = 0;
						}
					}

					for(int m=0; m<m_pMesh->m_nBones; m++)
					{
						Matrix4x4 matWorld = m_pMatrixList[ m + m_nCurFrame * m_pMesh->m_nBones ];

						g_pDirect3DDevice->SetTransform( D3DTS_WORLDMATRIX(m), (D3DMATRIX *)&matWorld );
					}

					g_Device.SetFVF(FVF_SKINVERTEX);
					//g_pDirect3DDevice->SetFVF( FVF_SKINVERTEX );
					g_Device.SetStreamSource( m_SubMesh[i].m_pMeshBlock->m_pVertexBuffer, sizeof( SkinVertex ));
				}
				else
				{
					g_pDirect3DDevice->SetSoftwareVertexProcessing( FALSE );

					g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
					g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);

					g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&m_ObjectTM);

					g_Device.SetFVF(D3DFVF_VERTEX);
					//g_pDirect3DDevice->SetFVF( D3DFVF_VERTEX);
					g_Device.SetStreamSource( m_SubMesh[i].m_pMeshBlock->m_pVertexBuffer, sizeof( VT_Normal));
				}

				g_Device.SetIndices( m_SubMesh[i].m_pIndexBuffer );

				if( m_SubMesh[i].m_nLodVertex >= 1 && m_SubMesh[i].m_nLodFace >= 3 )
				{
                    g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_SubMesh[i].m_nLodVertex, 0, m_SubMesh[i].m_nLodFace );

					g_EngineInfo.m_nRenderedVertex	+= m_SubMesh[i].m_nLodVertex;
					g_EngineInfo.m_nRenderedFace	+= m_SubMesh[i].m_nLodFace;
				}
			}// for(int i=0; i<m_pMesh->m_nMeshBlock; i++)

			g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
//			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
//			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
			g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
			g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
			g_pDirect3DDevice->SetSoftwareVertexProcessing( FALSE );

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		/*
		XIAHGE_API BYTE CMapObjectRender::GetAlphaBlendType()
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
		*/

		//---------------------------------------------------------------------------------------
		/*
		XIAHGE_API BOOL CMapObjectRender::SetDetailLevel(float fLevel)
		{
			// 이렇게하면  절대로 LOD를 안탄다.
			fLevel = 1.0f;

			if( m_fDetailLevel == fLevel)
				return TRUE;

			m_fDetailLevel = fLevel;
			fLevel *= g_EngineInfo.m_fPolygonDetail;

			// sub mesh LOD
			for(int i=0; i<m_nSubMesh; i++)
				m_SubMesh[i].SetLodLevel( fLevel );
//				m_SubMesh[i].SetLodLevel( 1.0f );

			return CRenderObject::SetDetailLevel( fLevel);
		}
		*/
		
		XIAHGE_API BOOL CMapObjectRender::StopEffect()
		{
			if( m_pEffectPackagePair )
			{
				g_EffectManager.DeqEffectPackagePair( m_pEffectPackagePair );

				m_pEffectPackagePair = NULL;
			}

			return TRUE;
		}

		XIAHGE_API BOOL CMapObjectRender::RenderCollisionBound()
		{
			for(int i = 0; i < m_nCollideBoxCount; i++)
			{
				DrawBound( m_CollideBox[ i], D3DCOLOR_XRGB( 255, 0, 0));
			}
		
			return TRUE;
		}

		/*
		BOOL CMapObjectRender::VisibleEffect(bool bVisible)
		{
			if( m_pEffectPackagePair )
				m_pEffectPackagePair->bIsVisible = bVisible;

			return TRUE;
		}
		*/


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
		XIAHGE_API CMapCellRender_MeshBlock::CMapCellRender_MeshBlock()
		{
			m_pMeshBlock = NULL;
			m_pIndexBuffer = NULL;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapCellRender_MeshBlock::~CMapCellRender_MeshBlock()
		{
			Release();
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender_MeshBlock::Create(CRes_MeshBlock* pMeshBlock,IDirect3DTexture9* pDetailTexture)
		{
			m_pMeshBlock = pMeshBlock;

			HRESULT hr;
			// 여긴 터레인 인데.
//			hr = g_pDirect3DDevice->CreateIndexBuffer( m_pMeshBlock->m_nFace * 3 * 2, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL);
			hr = g_pDirect3DDevice->CreateIndexBuffer( m_pMeshBlock->m_nFace * 3 * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);

			if( FAILED( hr))
				return FALSE;

			LPBYTE pIndexData;

			hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, 0);

			if( FAILED( hr))
				return FALSE;

			memcpy( pIndexData, pMeshBlock->m_pFace, pMeshBlock->m_nFace * 2 * 3);

			m_pIndexBuffer->Unlock();

			//SetDetailLevel( 1.0f);
			
			m_pDetailTexture = pDetailTexture;
			m_nLodVertex = m_pMeshBlock->m_nVertex;
			m_nLodFace = m_pMeshBlock->m_nFace;

			m_DiffuseColor = D3DCOLOR_XRGB( 255, 255, 255);

			return TRUE;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender_MeshBlock::Release()
		{
			m_pMeshBlock = NULL;

			if( m_pIndexBuffer)
			{
				m_pIndexBuffer->Release();
				m_pIndexBuffer = NULL;
			}

			return TRUE;
		}

		//---------------------------------------------------------------------------------------

//		XIAHGE_API BOOL CMapCellRender_MeshBlock::SetDetailLevel(float fLevel)
//		{
//			m_nLodVertex = m_pMeshBlock->m_nVertex;
//			m_nLodFace = m_pMeshBlock->m_nFace;

			// LOD 엄따.
/*
			fLevel *= g_EngineInfo.m_fPolygonDetail;

			if( fLevel < 0)
				fLevel = 0;

			if( fLevel > 1)
				fLevel = 1;

			if( fabsf( m_fDetailLevel - fLevel) < 0.05f)	// 5% 차이는 무시한다 
				return TRUE;

			CRenderObject::SetDetailLevel( fLevel);

			int lodcount = (int)(( 1.0f - fLevel) * m_pMeshBlock->m_nLodCount);

			if( lodcount == 0)
			{
				m_nLodVertex = m_pMeshBlock->m_nVertex;
				m_nLodFace = m_pMeshBlock->m_nFace;
			
				return TRUE;
			}

			m_nLodVertex = m_pMeshBlock->m_nVertex - lodcount;
			m_nLodFace = m_pMeshBlock->m_nFace;

			int i;

			for(i = 0; i < lodcount; i++)
			{
				m_nLodFace -= m_pMeshBlock->m_pLodFaceList[ m_pMeshBlock->m_nVertex - i - 1];
			}

			if( m_nLodFace > m_pMeshBlock->m_nFace)
			{
				m_nLodFace = 0;
				return TRUE;
			}

			int collapse_id;
			int new_collapse_id;

			for(i = m_nLodFace - 1; i >= 0; i--)
			{
				for(int j = 0; j < 3; j++)
				{
					collapse_id = m_pMeshBlock->m_pFace[ i * 3 + j];

					while( collapse_id >= m_nLodVertex)
					{
						new_collapse_id = m_pMeshBlock->m_pCollapsedIDList[ collapse_id];

						if( new_collapse_id == collapse_id)
							break;

						collapse_id = new_collapse_id;
					}

					if( collapse_id < 0 || collapse_id > m_pMeshBlock->m_nVertex - 1)
						g_TempFaceBuffer[ i * 3 + j] = 0;
					else
						g_TempFaceBuffer[ i * 3 + j] = collapse_id;
				}
			}

			LPBYTE pIndexData;
#ifdef TL_ACCEL
			HRESULT hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, D3DLOCK_DISCARD);
#else
			HRESULT hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, 0);
#endif
			if( FAILED( hr))
				return FALSE;

			memcpy( pIndexData, g_TempFaceBuffer, m_nLodFace * 3 * 2);

			m_pIndexBuffer->Unlock();

*/
//			return TRUE;
//		}


		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender_MeshBlock::Render()
		{
			DBG_Assert( m_pMeshBlock != NULL);

			if( m_bShowDetail)	// 호호호호호ㅗ
			{
				g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
				g_Device.SetTexture(1, m_pDetailTexture);
				//g_pDirect3DDevice->SetTexture( 1, m_pDetailTexture);
			}
			else
			{
				g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				g_Device.SetTexture(1, NULL);
				//g_pDirect3DDevice->SetTexture( 1, NULL);
			}

			//g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			g_Device.SetStreamSource( m_pMeshBlock->m_pVertex, sizeof( sMapData_Vertex));
			g_Device.SetIndices( m_pIndexBuffer);
			g_Device.SetFVF(D3DFVF_MAPDATA_VERTEX);
			//g_pDirect3DDevice->SetFVF(D3DFVF_MAPDATA_VERTEX);

			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nLodVertex, 0, m_nLodFace);

			g_EngineInfo.m_nRenderedVertex	+= m_nLodVertex;
			g_EngineInfo.m_nRenderedFace	+= m_nLodFace;

		/* DUMP VERTEX
			if(GetAsyncKeyState(VK_UP) < 0)
			{
				FILE *fp;
				fp = fopen("t.txt","wt");
				unsigned char *pp;
				m_pMeshBlock->m_pVertex->Lock(0,0,(void**)&pp,0);
				for(int i=0; i< m_nLodVertex;i++)
				{
					sMapData_Vertex *k;
					k = (sMapData_Vertex*)pp++;
					fprintf(fp,"x:%f y:%f z:%f %d\n",k->pos.x,k->pos.y,k->pos.z);
				}

				fclose(fp);
				m_pMeshBlock->m_pVertex->Unlock();
			}
		*/
		
			return TRUE;
		}

		/*
		XIAHGE_API BBoxAABB3* CMapCellRender_MeshBlock::GetBoundBox()
		{
			DBG_Assert( m_pMeshBlock != NULL);

			return &m_pMeshBlock->m_BoundBox;
		}
		*/

		XIAHGE_API BOOL CMapCellRender_MeshBlock::SetDiffuseColor(D3DCOLOR color)
		{
			if( m_DiffuseColor == color)
				return TRUE;

			m_DiffuseColor = color;

			sMapData_Vertex *pVertex = NULL;

			HRESULT hr = m_pMeshBlock->m_pVertex->Lock( 0, 0, (void **)&pVertex, 0);

			if( FAILED( hr))
				return FALSE;

			for(int i = 0; i < m_pMeshBlock->m_nVertex; i++)
			{
				pVertex->color = color;
				pVertex ++;	
			}

			m_pMeshBlock->m_pVertex->Unlock();

			return TRUE;
		}

		/*
		XIAHGE_API BOOL CMapCellRender_MeshBlock::ShowDetail(BOOL bShow)
		{
			m_bShowDetail = bShow;
			return TRUE;
		}
		*/


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
		XIAHGE_API CMapCellRender::CMapCellRender()
		{
			m_pMapCell = NULL;
			ZeroMemory( m_pMapObject, sizeof(CMapObjectRender*) * 1000);
			m_nMapObject = 0;

			m_pGrassZoneInfo = NULL;
			m_ShowBoundBox = FALSE;

			m_pMapObjectWidthMouseCursor = NULL;
			m_pDecalOnMapObject = NULL;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapCellRender::~CMapCellRender()
		{
			Release();	
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender::Create(CRes_MapCell *pMapCell)
		{
			if( m_pMapCell == pMapCell)
			{
				DBG_Put(_T("왜 똑같은데 Create또 불르냐?"));
				return TRUE;
			}

			m_pMapCell = pMapCell;

			int i;

			m_DiffuseColor = D3DCOLOR_XRGB( 255, 255, 255);

			// Object만들어 주기 졸라 느림
			m_nMapObject = pMapCell->m_nObject;

			if( m_nMapObject > 0)
			{
				DBG_Assert( m_nMapObject < 1000);

//				m_pMapObject = new CMapObjectRender[ m_nMapObject];

				for(i = 0; i < m_pMapCell->m_nObject; i++)
				{
					m_pMapObject[ i] = new CMapObjectRender;
					if( !m_pMapObject[ i]->Create( pMapCell->m_pObject + i))
					{
						DBG_Put(_T("MapObject 만들어 주기 실패"));
						delete m_pMapObject[ i];
						m_pMapObject[ i] = NULL;

					}
				}
			}

			// MeshBlock만들어 주기
			for(i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
			{
				if( !m_MeshBlock[ i].Create( &m_pMapCell->m_pHeightMeshBlock[ i], pMapCell->m_pDetailTexture[ i]))
					return FALSE;
			
				BBoxAABB3* pMeshBlockBound = m_MeshBlock[ i].GetBoundBox();

				m_MeshBlock[ i].m_MapObjectList.clear();

				for(int j = 0; j < m_nMapObject; j++)
				{
					if(m_pMapObject[ j])
					{
						BBoxAABB3* pObjectBound = m_pMapObject[ j]->GetObjectBound();

						BOOL IsBound = TRUE;

						if( pObjectBound->m_vMin.x > pMeshBlockBound->m_vMax.x ||
							pObjectBound->m_vMax.x < pMeshBlockBound->m_vMin.x ||
							pObjectBound->m_vMin.z > pMeshBlockBound->m_vMax.z ||
							pObjectBound->m_vMax.z < pMeshBlockBound->m_vMin.z)
							IsBound = FALSE;
						if( IsBound)
						{
							m_MeshBlock[ i].m_MapObjectList.push_back( m_pMapObject[ j]);
						}
					}
				}
			}

			m_VisibleObjectListNoAlpha.clear();
			m_VisibleObjectListAlphaTest.clear();
			m_VisibleObjectListAlpha.clear();
			m_InvisibleObjectList.clear();

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender::Release()
		{
			int i;
			ReleaseEffect();
			m_VisibleMeshBlockList_Level1.clear();
			m_VisibleMeshBlockList_Level2.clear();
//			m_VisibleObjectList_Level2.clear();


			if( m_pGrassZoneInfo ) 
			{
				for(i=0; i<m_pMapCell->m_nGrassZoneCount; i++)
				{
					GRASSLIST::iterator it;
					for(it=m_pGrassZoneInfo[i].GrassZoneList.begin(); it!=m_pGrassZoneInfo[i].GrassZoneList.end(); it++)
					{
						CGrass* pGrass = *it;

						g_MapRender.m_GrassZoneList.push_back( pGrass );
					}

					m_pGrassZoneInfo[i].GrassZoneList.clear();
				}

				delete []m_pGrassZoneInfo;
				m_pGrassZoneInfo = NULL;
			}

			for(i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
				m_MeshBlock[ i].Release();

			if( m_pMapObject)
			{
				for(i = 0; i < m_nMapObject; i++)
				{
					if( m_pMapObject[ i] )
					{
						delete m_pMapObject[ i];
						m_pMapObject[ i] = NULL;
					}
				}
//				delete [] m_pMapObject;
//				m_pMapObject = NULL;
			}

			m_nMapObject = 0;
			m_pMapCell = 0;

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender::Render(BYTE byType)
		{
			DBG_Assert( m_pMapCell != NULL);

			// 이제부터 
			// 지형 (0), 
			// No Alpha Map Object (1),
			// Alpha Test Map Object (2),
			// Alpha Map Object (3)
			// 으루 그린다.
			switch( byType )
			{
			case 0:		// Terrain
				{
					D3DMATERIAL9 material;
					memset( &material, 0, sizeof(D3DMATERIAL9) );

					material.Ambient.r = 1;
					material.Ambient.g = 1;
					material.Ambient.b = 1;
					material.Diffuse.r = 1;
					material.Diffuse.g = 1;
					material.Diffuse.b = 1;

					g_pDirect3DDevice->SetMaterial( &material);

					g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
					//g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
					g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
					g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

					g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
					//g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );

					g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);

					g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
					g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
					//g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

					g_Device.SetTexture(0, m_pMapCell->m_pTexture);
					//g_pDirect3DDevice->SetTexture( 0, m_pMapCell->m_pTexture);
					g_Device.SetFVF(D3DFVF_MAPDATA_VERTEX);
					//g_pDirect3DDevice->SetFVF( D3DFVF_MAPDATA_VERTEX);

					Matrix4x4 iTM;
					g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&iTM);

#if TEST_PERFORMANCE
					g_StartTime = timeGetTime();
#endif

					MAPRENDER_MESHBLOCK_LIST::iterator it;

					for(it = m_VisibleMeshBlockList_Level2.begin(); it != m_VisibleMeshBlockList_Level2.end(); it++)
					{
						CMapCellRender_MeshBlock *pMeshBlock = *it;
						pMeshBlock->Render();
					}

#if TEST_PERFORMANCE
					g_StartTime = timeGetTime() - g_StartTime;

					if( g_bLog)
					{
						DBG_LogFile("MapCellRender : MeshBlock : Render : %d", g_StartTime);
					}

					g_StartTime = timeGetTime();
#endif

					g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

//					g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
//					if( GetAsyncKeyState( VK_SPACE) < 0)
//					{
//						for(it = m_VisibleMeshBlockList_Level2.begin(); it != m_VisibleMeshBlockList_Level2.end(); it++)
//						{
//							CMapCellRender_MeshBlock *pMeshBlock = *it;
//							DrawBound( *pMeshBlock->GetBoundBox(), D3DCOLOR_XRGB( 0, 0, 255));
//						}
//					}
				}
				break;
			case 1:		// No Alpha Map Object
				{
					g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

					MAPRENDER_MAPOBJECTLIST::iterator object_it;
					for(object_it = m_VisibleObjectListNoAlpha.begin(); object_it != m_VisibleObjectListNoAlpha.end(); object_it ++)
					{
						CMapObjectRender* pMapObjectRender = *object_it;
						if(pMapObjectRender == NULL)
						{
							DBG_LogFile( _T("CMapCellRender::Render fail"));
						}

#ifdef SHOWBOUNDBOX
						if(1)// ?
						{
							pMapObjectRender->RenderCollisionBound();
							DrawBound( *pMapObjectRender->GetObjectBound(), D3DCOLOR_XRGB( 255, 255, 0));
						}
						//DrawBound( *pMapObjectRender->GetObjectBound(), D3DCOLOR_XRGB( 255, 255, 0));
#endif

						pMapObjectRender->Render();

						// 마우스 위치 커서 Decal, 해당 오브젝트 그린후 같이 그린다.
						if( m_pMapObjectWidthMouseCursor && m_pDecalOnMapObject &&
							m_pMapObjectWidthMouseCursor == pMapObjectRender )
							m_pDecalOnMapObject->Render();
					}

#if TEST_PERFORMANCE
					g_StartTime = timeGetTime() - g_StartTime;

					if( g_bLog)
						DBG_LogFile("MapCellRender : MapObject No Alpha : Render : %d", g_StartTime);
#endif
				}
				break;
			case 2:		// Alpha Test Map Object
				{
					g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

					MAPRENDER_MAPOBJECTLIST::iterator object_it;
					for(object_it = m_VisibleObjectListAlphaTest.begin(); object_it != m_VisibleObjectListAlphaTest.end(); object_it ++)
					{
						CMapObjectRender* pMapObjectRender = *object_it;
						if(pMapObjectRender == NULL)
						{
							DBG_LogFile( _T("CMapCellRender::Render2 fail"));
						}
#ifdef SHOWBOUNDBOX
						if(1)// ?
						{
							pMapObjectRender->RenderCollisionBound();
							DrawBound( *pMapObjectRender->GetObjectBound(), D3DCOLOR_XRGB( 255, 255, 0));
						}
						//DrawBound( *pMapObjectRender->GetObjectBound(), D3DCOLOR_XRGB( 255, 255, 0));
#endif

						pMapObjectRender->Render();

						// 마우스 위치 커서 Decal, 해당 오브젝트 그린후 같이 그린다.
						if( m_pMapObjectWidthMouseCursor && m_pDecalOnMapObject &&
							m_pMapObjectWidthMouseCursor == pMapObjectRender )
							m_pDecalOnMapObject->Render();
					}

#if TEST_PERFORMANCE
					g_StartTime = timeGetTime() - g_StartTime;

					if( g_bLog)
						DBG_LogFile("MapCellRender : MapObject Alpha Test : Render : %d", g_StartTime);
#endif
				}
				break;
			case 3:		// Alpha Map Object
				{
					g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
					g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

					MAPRENDER_MAPOBJECTLIST::iterator object_it;
					for(object_it = m_VisibleObjectListAlpha.begin(); object_it != m_VisibleObjectListAlpha.end(); object_it ++)
					{
						CMapObjectRender* pMapObjectRender = *object_it;
						if(pMapObjectRender == NULL)
						{
							DBG_LogFile( _T("CMapCellRender::Render3 fail"));
						}
#ifdef SHOWBOUNDBOX
						if(1)// ?
						{
							pMapObjectRender->RenderCollisionBound();
							DrawBound( *pMapObjectRender->GetObjectBound(), D3DCOLOR_XRGB( 255, 255, 0));
						}
						//DrawBound( *pMapObjectRender->GetObjectBound(), D3DCOLOR_XRGB( 255, 255, 0));
#endif

						pMapObjectRender->Render();
					}

#if TEST_PERFORMANCE
					g_StartTime = timeGetTime() - g_StartTime;

					if( g_bLog)
						DBG_LogFile("MapCellRender : MapObject Alpha : Render : %d", g_StartTime);
#endif
				}
				break;
			};// switch( byType )

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender::RenderWater()
		{
			if( m_pMapCell == NULL)
				return TRUE;

			if( m_pMapCell->m_nWaterVertex <= 0)
				return TRUE;

//			g_pDirect3DDevice->SetTexture( 0, NULL);

			g_Device.SetFVF(D3DFVF_MAPDATA_VERTEX);
			//g_pDirect3DDevice->SetFVF( D3DFVF_MAPDATA_VERTEX);
			g_Device.SetStreamSource( m_pMapCell->m_pWaterVertex, sizeof( sMapData_Vertex));
			g_Device.SetIndices( m_pMapCell->m_pWaterFace);

			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pMapCell->m_nWaterVertex, 0, m_pMapCell->m_nWaterFace / 3);

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender::PrepareRender()
		{
		
			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		/*
		XIAHGE_API BOOL CMapCellRender::IsValid()
		{
			return m_pMapCell != NULL;
		}
		*/

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapCellRender::QueryVisibleMeshblock_Level1(sRect *pBound)
		{
			// 이 Bound에 해당되는 MeshBlock을 모두 추가 시켜준다 꽤 빠름
			m_VisibleMeshBlockList_Level1.clear();
			m_VisibleMeshBlockList_Level2.clear();
			
//			MAPRENDER_MAPOBJECTLIST::iterator object_it;
//			for(object_it = m_VisibleObjectList_Level2.begin(); object_it != m_VisibleObjectList_Level2.end(); object_it++)
//			{
//				CMapObjectRender *pRender = *object_it;
//				pRender->StopEffect();
//			}
//			m_VisibleObjectList_Level2.clear();
			
			for(int i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
			{
				sRect rcMeshBlockBound( sPoint( (i % MAPCELL_MESHBLOCK_XSIZE) * (MESHBLOCK_TILE_XSIZE * TILE_GRID_SIZE), 
												(i / MAPCELL_MESHBLOCK_XSIZE) * (MESHBLOCK_TILE_YSIZE * TILE_GRID_SIZE)
												),
										sSize( MESHBLOCK_TILE_XSIZE * TILE_GRID_SIZE, MESHBLOCK_TILE_YSIZE * TILE_GRID_SIZE));

				rcMeshBlockBound += sPoint( m_pMapCell->m_XPos * TILE_GRID_SIZE, m_pMapCell->m_YPos * TILE_GRID_SIZE);
			
				if( rcMeshBlockBound.IsIntersect( *pBound))
					m_VisibleMeshBlockList_Level1.push_back( &m_MeshBlock[ i]);
			}

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		// 카메라와 실제 메쉬 크기만한 BoundBox와의 VisibleTest이기 때문에 느림
		XIAHGE_API BOOL CMapCellRender::QueryVisibleMeshblock_Level2(CCamera *pCamera,unsigned short CameraBoundSize)
		{
			m_VisibleMeshBlockList_Level2.clear();

			MAPRENDER_MAPOBJECT_VISIBLETEST_LIST object_test_list;
//			MAPRENDER_MAPOBJECT_VISIBLETEST_LIST invisible_object_list;
			MAPRENDER_MAPOBJECTLIST::iterator object_it;
			MAPRENDER_MAPOBJECT_VISIBLETEST_LIST::iterator invisible_object_list_iterator;

/*
			for(object_it = m_VisibleObjectList_Level2.begin(); object_it != m_VisibleObjectList_Level2.end(); object_it++)
				invisible_object_list.insert( *object_it);
			m_VisibleObjectList_Level2.clear();
*/

			// 현재 보이는 맵 오브젝트들을 안보인다고 가정하고, 카메라에 들어가는 맵 오브젝트를 추출한 후
			// 안보이는 리스트에서 밴다. 그리고 최종적으로 안보이는 맵 오브젝트의 이펙트를 보이지 않게 한다.
			for(object_it = m_VisibleObjectListNoAlpha.begin(); object_it != m_VisibleObjectListNoAlpha.end(); object_it++)
				m_InvisibleObjectList.insert( *object_it );
			m_VisibleObjectListNoAlpha.clear();

			for(object_it = m_VisibleObjectListAlphaTest.begin(); object_it != m_VisibleObjectListAlphaTest.end(); object_it++)
				m_InvisibleObjectList.insert( *object_it );
			m_VisibleObjectListAlphaTest.clear();

			for(object_it = m_VisibleObjectListAlpha.begin(); object_it != m_VisibleObjectListAlpha.end(); object_it++)
				m_InvisibleObjectList.insert( *object_it );
			m_VisibleObjectListAlpha.clear();

			//
			sRect rcCamera;

			rcCamera.left = g_pCurrentCamera->m_vFrom.x;
			rcCamera.top  = -g_pCurrentCamera->m_vFrom.z;
			rcCamera.right = rcCamera.left + CameraBoundSize * 14 / 10;
			rcCamera.bottom = rcCamera.top + CameraBoundSize * 14 / 10;

			rcCamera -= sPoint( CameraBoundSize * 7 / 10, CameraBoundSize * 7 / 10);

			//
			MAPRENDER_MESHBLOCK_LIST::iterator it;

			for(it = m_VisibleMeshBlockList_Level1.begin(); it != m_VisibleMeshBlockList_Level1.end(); it++)
			{
				CMapCellRender_MeshBlock *pMeshBlock = *it;
				if(pMeshBlock == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::QueryVisibleMeshblock_Level2 fail"));
				}

				BBoxAABB3 *pBound = pMeshBlock->GetBoundBox();

				if( pCamera->Visible( *pBound))
				{
					Vector3 Distance = pCamera->m_vFrom - pBound->Center();
					float fDistance = Distance.GetLength();
					
					float fLevel = fDistance / (CameraBoundSize);

					// LOD 엄따.
//					int nLevel = (1 - fLevel) * 10.0f;
//					pMeshBlock->SetDetailLevel( (float)nLevel / 10.0f);
					
					pMeshBlock->ShowDetail( fLevel <= m_fDetailRatio);

					//
					for(int i = 0; i < pMeshBlock->m_MapObjectList.size(); i++)
					{
						CMapObjectRender *pMapObjectRender = pMeshBlock->m_MapObjectList[ i];

						DBG_Assert( ::_CrtIsValidPointer( pMapObjectRender, sizeof( CMapObjectRender), TRUE));
						DBG_Assert( ::_CrtIsValidPointer( pMapObjectRender->GetObjectBound(), sizeof( BBoxAABB3), TRUE));
						
						sRect rcObject;
						BBoxAABB3 *aabb = pMapObjectRender->GetObjectBound();

						rcObject.left = aabb->m_vMin.x;
						rcObject.top = -aabb->m_vMax.z;
						rcObject.right = aabb->m_vMax.x;
						rcObject.bottom = -aabb->m_vMin.z;

						if( !rcObject.IsIntersect( rcCamera))
							continue;
						
						if( pCamera->Visible( *pMapObjectRender->GetObjectBound()))
						{
							if( object_test_list.find( pMapObjectRender) == object_test_list.end())
							{
								object_test_list.insert( pMapObjectRender);
								
								if( pMapObjectRender->Realize())
								{
									// LOD 엄따.
/*
									//
									Vector3 vCenter = pMapObjectRender->GetObjectBound()->Center();

									Vector3 Distance = pCamera->m_vFrom - vCenter;

									float fDistance = Distance.GetLength();
									float fLevel = fDistance / (CameraBoundSize);
									int nLevel = (1 - fLevel) * 10.0f;

									pMapObjectRender->SetDetailLevel( (float)nLevel / 10.0f);
*/

									//
									DBG_Assert( ::_CrtIsValidPointer( pMapObjectRender, sizeof( CMapObjectRender), TRUE));

									invisible_object_list_iterator = m_InvisibleObjectList.find( pMapObjectRender);

									// 안보이는줄 알았더니 보이는 놈이네.
									if( invisible_object_list_iterator != m_InvisibleObjectList.end() )
										m_InvisibleObjectList.erase( pMapObjectRender );

//									m_VisibleObjectList_Level2.push_back( pMapObjectRender);
									BYTE byAlphaType = pMapObjectRender->GetAlphaBlendType();
									switch( byAlphaType )
									{
									case 2:
										m_VisibleObjectListAlphaTest.push_back( pMapObjectRender);
										break;
									case 3:
										m_VisibleObjectListAlpha.push_back( pMapObjectRender);
										break;
									case 1:
									default:
										m_VisibleObjectListNoAlpha.push_back( pMapObjectRender);
										break;
									};// switch

									pMapObjectRender->VisibleEffect();
								}// if( pMapObjectRender->Realize())

							}// if
						}// if( camera visible )
					}// for( mapobjectlist )
					
					m_VisibleMeshBlockList_Level2.push_back( pMeshBlock);
				}// if( pCamera->Visible( *pBound))
			}// for( m_VisibleMeshBlockList_Level1 )

			object_test_list.clear();

			for( invisible_object_list_iterator = m_InvisibleObjectList.begin(); invisible_object_list_iterator != m_InvisibleObjectList.end(); invisible_object_list_iterator++)
			{
				CMapObjectRender *pInvisibleObject = *invisible_object_list_iterator;
				if(pInvisibleObject == NULL)
				{
					DBG_LogFile( _T("QueryVisibleMeshblock_Level2-2 fail"));
				}

				// 안보이는 오브젝트의 이펙트도 안그린다.
				pInvisibleObject->VisibleEffect(false);
			}

			return TRUE;
		}

		XIAHGE_API BOOL CMapCellRender::SetDiffuseColor(D3DCOLOR color)
		{
			if( m_DiffuseColor == color)
				return TRUE;

			for(int i = 0; i < MAPCELL_MESHBLOCK_COUNT; i++)
				m_MeshBlock[ i].SetDiffuseColor( color);

			m_DiffuseColor = color;

			return TRUE;
		}

		/*
		XIAHGE_API BOOL CMapCellRender::SetDetailRatio(float fDetailRatio)
		{
			m_fDetailRatio = fDetailRatio;
			
			return TRUE;
		}
		*/

		XIAHGE_API BOOL	CMapCellRender::ClearVisible()
		{
			m_VisibleMeshBlockList_Level1.clear();
			m_VisibleMeshBlockList_Level2.clear();
			
			MAPRENDER_MAPOBJECTLIST::iterator it;
/*
			for(it = m_VisibleObjectList_Level2.begin(); it != m_VisibleObjectList_Level2.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;

				// 이펙트를 지운다.
				pObjRender->StopEffect();

				// 이펙트만 보이지 않게 한다.
//				pObjRender->VisibleEffect(false);
			}
*/

			for(it = m_VisibleObjectListNoAlpha.begin(); it != m_VisibleObjectListNoAlpha.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectListNoAlpha.clear();

			for(it = m_VisibleObjectListAlphaTest.begin(); it != m_VisibleObjectListAlphaTest.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible2 fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectListAlphaTest.clear();

			for(it = m_VisibleObjectListAlpha.begin(); it != m_VisibleObjectListAlpha.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible3 fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectListAlpha.clear();

			return TRUE;
		}

		XIAHGE_API BOOL	CMapCellRender::ReleaseEffect()
		{
			m_VisibleMeshBlockList_Level1.clear();
			m_VisibleMeshBlockList_Level2.clear();
			
			MAPRENDER_MAPOBJECTLIST::iterator it;
/*
			for(it = m_VisibleObjectList_Level2.begin(); it != m_VisibleObjectList_Level2.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;

				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectList_Level2.clear();
*/

			for(it = m_VisibleObjectListNoAlpha.begin(); it != m_VisibleObjectListNoAlpha.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible4 fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectListNoAlpha.clear();

			for(it = m_VisibleObjectListAlphaTest.begin(); it != m_VisibleObjectListAlphaTest.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible5 fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectListAlphaTest.clear();

			for(it = m_VisibleObjectListAlpha.begin(); it != m_VisibleObjectListAlpha.end(); it++)
			{
				CMapObjectRender* pObjRender = *it;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible6 fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_VisibleObjectListAlpha.clear();

			MAPRENDER_MAPOBJECT_VISIBLETEST_LIST::iterator vit;
			for(vit = m_InvisibleObjectList.begin(); vit != m_InvisibleObjectList.end(); vit++)
			{
				CMapObjectRender* pObjRender = *vit;
				if(pObjRender == NULL)
				{
					DBG_LogFile( _T("CMapCellRender::ClearVisible6 fail"));
				}
				// 이펙트를 지운다.
				pObjRender->StopEffect();
			}
			m_InvisibleObjectList.clear();

			return TRUE;
		}

		XIAHGE_API BOOL CMapCellRender::SetGrassZonePos()
		{
			m_pGrassZoneInfo = new sGrassZoneInfo [ m_pMapCell->m_nGrassZoneCount ];

			for(int i=0; i<m_pMapCell->m_nGrassZoneCount; i++)
			{
				Vector3 vStart = Vector3( m_pMapCell->m_pGrassZoneInfo[i].fX1, 0, -m_pMapCell->m_pGrassZoneInfo[i].fZ1 );
				Vector3 vEnd   = Vector3( m_pMapCell->m_pGrassZoneInfo[i].fX2, 0, -m_pMapCell->m_pGrassZoneInfo[i].fZ2 );

				vStart.x += 3;
				vStart.z -= 3;
				vEnd.x -= 3;
				vEnd.z += 3;

				//
				sGrassZoneInfo* pGrassZoneInfo = &m_pGrassZoneInfo[i];

				pGrassZoneInfo->byPositionByCamera = 0;
				pGrassZoneInfo->bRender = false;
				pGrassZoneInfo->byGrassTextureIndex = m_pMapCell->m_pGrassZoneInfo[i].nType; // rand() % 8;
				pGrassZoneInfo->byGrassPosListIndex = 0;
				pGrassZoneInfo->byGrassZoneDensity  = m_pMapCell->m_pGrassZoneInfo[i].nDensity; // rand() % 35;	// 수치가 작을수록 밀집도가 높게. 계산상 편의.
				pGrassZoneInfo->GrassZoneList.clear();

				// x는 작은 것에서 큰 것 순서로, z는 큰 것에서 작은 순서로.
				// 또한 밀집도가 크면 풀을 많이, 작으면 풀을 적게.
				float fZDist = 1.0f + ( rnd() * (pGrassZoneInfo->byGrassZoneDensity+1) * 0.25f );
				float fXDist = 1.0f + ( rnd() * (pGrassZoneInfo->byGrassZoneDensity+1) * 0.25f );
				float fPosX, fPosZ, fTempX, fTempZ;
				for(float fZ=vStart.z; fZ>vEnd.z; fZ-=fZDist)
				{
					for(float fX=vStart.x; fX<vEnd.x; fX+=fXDist)
					{
						fTempX = 0.65f + ( rnd() * (pGrassZoneInfo->byGrassZoneDensity+1) * 0.4f );
						fTempZ = 0.65f + ( rnd() * (pGrassZoneInfo->byGrassZoneDensity+1) * 0.4f );

						if( rand() / 2 ) fTempX = -fTempX;
						if( rand() / 2 ) fTempZ = -fTempZ;

						fPosX = fX + fTempX;
						fPosZ = fZ + fTempZ;

						pGrassZoneInfo->GrassPosList[ pGrassZoneInfo->byGrassPosListIndex ].insert( DFLOATMAP::value_type( fPosX, fPosZ ) );
					}// for(fX)

					if( pGrassZoneInfo->byGrassPosListIndex < GRASSZONE_MAPMAX-1 )
						pGrassZoneInfo->byGrassPosListIndex++;
					else
						break;
				}// for(fZ)

			}// for( m_pMapCell->m_nGrassZoneCount )

			return TRUE;
		}

		XIAHGE_API BOOL CMapCellRender::UpdateGrass()
		{
			if( m_pMapCell->m_nGrassZoneCount == 0 ) return TRUE;

			// 풀 위치 정보가 생성되었는지 확인
			if( m_pGrassZoneInfo == NULL )
			{
				SetGrassZonePos();
				ResetGrassZone();
			}// if

			// GrassZone이 카메라에 비춰서 그려야 할지 말지를 검사.
			for(int i=0; i<m_pMapCell->m_nGrassZoneCount; i++)
			{
				sGrassZoneInfo* pGrassZoneInfo = &m_pGrassZoneInfo[i];

				Vector3 vStart = Vector3( m_pMapCell->m_pGrassZoneInfo[i].fX1, 0, -m_pMapCell->m_pGrassZoneInfo[i].fZ1 );
				Vector3 vEnd   = Vector3( m_pMapCell->m_pGrassZoneInfo[i].fX2, 0, -m_pMapCell->m_pGrassZoneInfo[i].fZ2 );

				bool bRender = true;

				if( g_pCurrentCamera->m_vFrom.x <= vStart.x &&
					g_pCurrentCamera->m_vFrom.z >= vStart.z )	// 1
				{
					if( g_pCurrentCamera->m_vAt.x < g_pCurrentCamera->m_vFrom.x &&
						g_pCurrentCamera->m_vAt.z > g_pCurrentCamera->m_vFrom.z   )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.z > vStart.z+30 &&
						g_pCurrentCamera->m_vAt.z > g_pCurrentCamera->m_vFrom.z+15 )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.x < vStart.x-30 &&
						g_pCurrentCamera->m_vAt.x < g_pCurrentCamera->m_vFrom.x-15 )
						bRender = false;
				}
				else
				if( g_pCurrentCamera->m_vFrom.x >= vEnd.x &&
					g_pCurrentCamera->m_vFrom.z >= vStart.z )	// 2
				{
					if( g_pCurrentCamera->m_vAt.x > g_pCurrentCamera->m_vFrom.x &&
						g_pCurrentCamera->m_vAt.z > g_pCurrentCamera->m_vFrom.z   )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.z > vStart.z+30 &&
						g_pCurrentCamera->m_vAt.z > g_pCurrentCamera->m_vFrom.z+15 )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.x > vEnd.x+30 &&
						g_pCurrentCamera->m_vAt.x > g_pCurrentCamera->m_vFrom.x+15 )
						bRender = false;
				}
				else
				if( g_pCurrentCamera->m_vFrom.x <= vStart.x &&
					g_pCurrentCamera->m_vFrom.z <= vEnd.z )		// 3
				{
					if( g_pCurrentCamera->m_vAt.x < g_pCurrentCamera->m_vFrom.x &&
						g_pCurrentCamera->m_vAt.z < g_pCurrentCamera->m_vFrom.z   )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.z < vEnd.z-30 &&
						g_pCurrentCamera->m_vAt.z < g_pCurrentCamera->m_vFrom.z-15 )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.x < vStart.x-30 &&
						g_pCurrentCamera->m_vAt.x < g_pCurrentCamera->m_vFrom.x-15 )
						bRender = false;
				}
				else
				if( g_pCurrentCamera->m_vFrom.x >= vEnd.x &&
					g_pCurrentCamera->m_vFrom.z <= vEnd.z )		// 4
				{
					if( g_pCurrentCamera->m_vAt.x > g_pCurrentCamera->m_vFrom.x &&
						g_pCurrentCamera->m_vAt.z < g_pCurrentCamera->m_vFrom.z   )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.z < vEnd.z-30 &&
						g_pCurrentCamera->m_vAt.z < g_pCurrentCamera->m_vFrom.z-15 )
						bRender = false;

					if( bRender &&
						g_pCurrentCamera->m_vFrom.x > vEnd.x+30 &&
						g_pCurrentCamera->m_vAt.x > g_pCurrentCamera->m_vFrom.x+15 )
						bRender = false;
				}
				else
				if( g_pCurrentCamera->m_vFrom.z <= vStart.z &&
                    g_pCurrentCamera->m_vFrom.z >= vEnd.z     )
				{
					if( g_pCurrentCamera->m_vAt.x >= g_pCurrentCamera->m_vFrom.x )	// 5
					{
						if( g_pCurrentCamera->m_vFrom.x > vEnd.x+30 &&
							g_pCurrentCamera->m_vAt.x > g_pCurrentCamera->m_vFrom.x+15 )
							bRender = false;
					}
					else	// 6
					{
						if( g_pCurrentCamera->m_vFrom.x < vStart.x-30 &&
							g_pCurrentCamera->m_vAt.x < g_pCurrentCamera->m_vFrom.x-15 )
							bRender = false;
					}
				}
				else
				if( g_pCurrentCamera->m_vFrom.x >= vStart.x &&
                    g_pCurrentCamera->m_vFrom.x <= vEnd.x     )
				{
					if( g_pCurrentCamera->m_vAt.z >= g_pCurrentCamera->m_vFrom.z )	// 7
					{
						if( g_pCurrentCamera->m_vFrom.z > vStart.z+30 &&
							g_pCurrentCamera->m_vAt.z > g_pCurrentCamera->m_vFrom.z+15 )
							bRender = false;
					}
					else	// 8
					{
						if( g_pCurrentCamera->m_vFrom.z < vEnd.z-30 &&
							g_pCurrentCamera->m_vAt.z < g_pCurrentCamera->m_vFrom.z-15 )
							bRender = false;
					}
				}

				//
				pGrassZoneInfo->bRender = bRender;

				// 렌더링될때만 VB를 만든다.
				if( bRender )
				{
					GRASSLIST::iterator git;
					for(git=pGrassZoneInfo->GrassZoneList.begin(); git!=pGrassZoneInfo->GrassZoneList.end(); git++)
					{
						CGrass* pGrass = *git;

						pGrass->Update();
					}
				}// if

			}// for( m_pMapCell->m_nGrassZoneCount )

			return TRUE;
		}

		XIAHGE_API BOOL CMapCellRender::ResetGrassZone()
		{
			// 모든 GrassZone을 매 프레임마다 변경시키면 넘 과부하이므로
			// 각 GrassZone 별로 카메라 위치가 바뀌면 업데이트 되도록 설정.
			CGrass* pGrassZone = NULL;
			float fXSize = 3.2f, fHeight = 6.0f;

			for(int i=0; i<m_pMapCell->m_nGrassZoneCount; i++)
			{
				sGrassZoneInfo* pGrassZoneInfo = &m_pGrassZoneInfo[i];

				// 총 리스트에서 하나를 가져와서 쓴다.
				if( g_MapRender.m_GrassZoneList.size() == 0 ) break;

				pGrassZone = g_MapRender.m_GrassZoneList.front();
				g_MapRender.m_GrassZoneList.pop_front();

				pGrassZone->ClearAllGrass();
				pGrassZoneInfo->GrassZoneList.push_back( pGrassZone );

				pGrassZone->VBLock();

				// 리스트의 순서대로, 각 리스트의 첫 부분부터.
				for(int j=0; j<=pGrassZoneInfo->byGrassPosListIndex; j++)
				{
					DFLOATMAP::iterator mit;
					for(mit=pGrassZoneInfo->GrassPosList[j].begin(); mit!=pGrassZoneInfo->GrassPosList[j].end(); mit++)
					{
						float fX = mit->first;
						float fZ = mit->second;

						float fY = Map::g_MapRes.GetHeight( fX+(fXSize/2.0f), fZ );

						Vector3 vV1 = Vector3( fX, fY, fZ );
						Vector3 vV2 = vV1;
						vV2.x += fXSize;

						BOOL bOK = pGrassZone->AddGrass( pGrassZoneInfo->byGrassTextureIndex, vV1, vV2, fHeight );
						if( !bOK )
						{
							pGrassZone->VBUnlock();

							// 총 리스트에서 하나를 가져와서 쓴다.
							if( g_MapRender.m_GrassZoneList.size() == 0 ) break;

							pGrassZone = g_MapRender.m_GrassZoneList.front();
							g_MapRender.m_GrassZoneList.pop_front();

							pGrassZone->ClearAllGrass();
							pGrassZoneInfo->GrassZoneList.push_back( pGrassZone );

							pGrassZone->VBLock();
							pGrassZone->AddGrass( pGrassZoneInfo->byGrassTextureIndex, vV1, vV2, fHeight );
						}// if

					}// for( mit )
				}// for( pGrassZoneInfo->byGrassPosListIndex )

				pGrassZone->VBUnlock();
			}// for( m_pMapCell->m_nGrassZoneCount )

			return TRUE;
		}

		XIAHGE_API BOOL CMapCellRender::RenderGrass()
		{
			if( m_pMapCell->m_nGrassZoneCount == 0 ) return TRUE;

			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF, 0x00000080 ); //0xfd);
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			g_pDirect3DDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1 );// D3DMCS_MATERIAL D3DMCS_COLOR1
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );//D3DBLEND_SRCCOLOR
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			g_pDirect3DDevice->SetRenderState( D3DRS_BLENDOP,	D3DBLENDOP_ADD );
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

			for(int i=0; i<m_pMapCell->m_nGrassZoneCount; i++)
			{
				sGrassZoneInfo* pGrassZoneInfo = &m_pGrassZoneInfo[i];

				if( pGrassZoneInfo->bRender )
				{
					GRASSLIST::iterator git;
					for(git=pGrassZoneInfo->GrassZoneList.begin(); git!=pGrassZoneInfo->GrassZoneList.end(); git++)
					{
						CGrass* pGrass = *git;

						pGrass->Render();
					}
				}// if
			}// for

//			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
//			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

			return TRUE;
		}

		// 맵셀안의 보이는 오브젝트와 마우스 위치 커서를 검사한다.
		BOOL CMapCellRender::CheckMapObjectForMouseCursor(BBoxAABB3 MouseBound, float fCharHeight, float fCharPositionY)
		{
			m_pMapObjectWidthMouseCursor = NULL;
			m_pDecalOnMapObject = NULL;

			float fCharHeightHalf = fCharHeight / 2.0f;
			Vector3 vCenter = ( MouseBound.m_vMax - MouseBound.m_vMin ) / 2;
			vCenter = MouseBound.m_vMin + vCenter;

			// No alpha list
			MAPRENDER_MAPOBJECTLIST::iterator mit;
			for(mit=m_VisibleObjectListNoAlpha.begin(); mit!=m_VisibleObjectListNoAlpha.end(); mit++)
			{
				CMapObjectRender* pMapObject = *mit;

				BBoxAABB3* pObjectBound = pMapObject->GetObjectBound();

				BBoxAABB3 MouseBound2 = MouseBound;

				// 다리를 올라 갈때를 위해서 바운드 박스를 새롭게 계산할 필요가 있다.
				// 클라이언트에서는 XiahGameObject.cpp안에 OnTMUpdate()함수에서는 
				// 캐릭터가 올라 갈 수 있는 높이 차이를 4로두고 있다. 근데, 여기서는 캐릭터 키의 절반으로 하자.
				if( pObjectBound->m_vMax.y <= fCharPositionY + fCharHeightHalf && // 캐릭터가 올라 갈 수있는 높이
					pObjectBound->m_vMin.y > MouseBound.m_vMax.y && // 마우스 커서가 있는 지형 위치가 오브젝트 하단 높이보다 낮다.
					pObjectBound->m_vMin.x <= vCenter.x && // 마우스 위치가 오브젝트의 넓은 면적 안에 있다.
					pObjectBound->m_vMin.z <= vCenter.z &&
					pObjectBound->m_vMax.x >= vCenter.x &&
					pObjectBound->m_vMax.z >= vCenter.z   )
				{
					// 마우스의 현재 지형 위치를 캐릭터의 지형 위치로 바뀌준다.
					MouseBound2.m_vMin.y = MouseBound2.m_vMax.y = fCharPositionY;
				}

				// 들어왔군.
				// 면적으로 계산해야 된다. 왜냐면, 마우스 위치가 오브젝트 근처에 있을 수도 있기 때문.
				if( pObjectBound->m_vMax.y <= MouseBound2.m_vMin.y + fCharHeightHalf && // 올라 갈 수 있는 높이
					pObjectBound->Intersect( MouseBound2 ) )
				{
					m_pMapObjectWidthMouseCursor = pMapObject;
					return TRUE;
				}

			}// for


			// 아직 이녀석들은 검색하지 말자. 마우스 위치 커서가 나오는 것들은 No alpha!
//			m_VisibleObjectListAlphaTest;
//			m_VisibleObjectListAlpha

			return FALSE;
		}

		// 마우스 위치 커서가 맵 오브젝트 위에 있을때 버텍스를 새롭게 계산해준다.
		// 복잡하게 하려다가 속도 땜시. 주로 직사각형의 오브젝트 위에 올라간다고 가정하고
		// 오브젝트의 바운드 박스와 계산해서 버텍스를 만든다.
		BOOL CMapCellRender::MakeMouseCursorVertexOnObject(Vector2 vSmall, Vector2 vBig, D3DCOLOR dColor, VT_LVertex* pVertex, WORD* pFace, int& nVertex, int& nFace)
		{
			if( !m_pMapObjectWidthMouseCursor ) return FALSE;

			nVertex = 0;
			nFace = 0;
			BBoxAABB3* pObjectBound = m_pMapObjectWidthMouseCursor->GetObjectBound();

			// 경우를 나누자
			// 1. 맵 오브젝트가 마우스 위치 커서를 완전히 포함할 때.
			if( pObjectBound->m_vMin.x <= vSmall.x &&
				pObjectBound->m_vMin.z <= vSmall.y &&
				pObjectBound->m_vMax.x >= vBig.x &&
				pObjectBound->m_vMax.z >= vBig.y     )
			{
				// 이럴땐 4개 버텍스로만으로도 되겠군.
				//   2  3
				//   0  1
				pVertex[0].pos = Vector3( vSmall.x, pObjectBound->m_vMax.y + 0.11f, vSmall.y );
				pVertex[1].pos = Vector3( vBig.x,   pObjectBound->m_vMax.y + 0.11f, vSmall.y );
				pVertex[2].pos = Vector3( vSmall.x, pObjectBound->m_vMax.y + 0.11f, vBig.y   );
				pVertex[3].pos = Vector3( vBig.x,   pObjectBound->m_vMax.y + 0.11f, vBig.y   );

				for(int i=0; i<4; i++)
                    pVertex[i].diffuse = dColor;

				pVertex[0].tex = Vector2( 0, 1 );
				pVertex[1].tex = Vector2( 1, 1 );
				pVertex[2].tex = Vector2( 0, 0 );
				pVertex[3].tex = Vector2( 1, 0 );

				pFace[0] = 0;
				pFace[1] = 2;
				pFace[2] = 1;
				pFace[3] = 2;
				pFace[4] = 3;
				pFace[5] = 1;

				nVertex = 4;
				nFace = 6;
			}// if
			else	// 2. 마우스 위치 커서가 맵 오브젝트를 완전히 포함할 때.
			if( pObjectBound->m_vMin.x > vSmall.x &&
				pObjectBound->m_vMin.z > vSmall.y &&
				pObjectBound->m_vMax.x < vBig.x &&
				pObjectBound->m_vMax.z < vBig.y     )
			{
				// 가운데가 직사각형으로 솟은 모형. 직접 만들어주려니깐 힘들군. 
				// 그려려니 생각하셔요. 내 노트에 그림 그려놓은 것 참조.  ㅡ,.ㅡ^;
				pVertex[ 0].pos = Vector3(vSmall.x, g_MapRes.GetHeight(vSmall.x,pObjectBound->m_vMax.z)+0.11f, pObjectBound->m_vMax.z);
				pVertex[ 1].pos = Vector3(pObjectBound->m_vMin.x, g_MapRes.GetHeight(pObjectBound->m_vMin.x,pObjectBound->m_vMax.z)+0.11f, pObjectBound->m_vMax.z);
				pVertex[ 2].pos = Vector3(vSmall.x, g_MapRes.GetHeight(vSmall.x,vBig.y)+0.11f, vBig.y); // Top-Left
				pVertex[ 3].pos = Vector3(pObjectBound->m_vMin.x, g_MapRes.GetHeight(pObjectBound->m_vMin.x,vBig.y)+0.11f, vBig.y);

				pVertex[ 4].pos = Vector3(pObjectBound->m_vMax.x, g_MapRes.GetHeight(pObjectBound->m_vMax.x,pObjectBound->m_vMax.z)+0.11f, pObjectBound->m_vMax.z);
				pVertex[ 5].pos = Vector3(pObjectBound->m_vMax.x, g_MapRes.GetHeight(pObjectBound->m_vMax.x,vBig.y)+0.11f, vBig.y);
				pVertex[ 6].pos = Vector3(vBig.x, g_MapRes.GetHeight(vBig.x,pObjectBound->m_vMax.z)+0.11f, pObjectBound->m_vMax.z);
				pVertex[ 7].pos = Vector3(vBig.x, g_MapRes.GetHeight(vBig.x,vBig.y)+0.11f, vBig.y);

				pVertex[ 8].pos = Vector3(vSmall.x, g_MapRes.GetHeight(vSmall.x,pObjectBound->m_vMin.z)+0.11f, pObjectBound->m_vMin.z);
				pVertex[ 9].pos = Vector3(pObjectBound->m_vMin.x, g_MapRes.GetHeight(pObjectBound->m_vMin.x,pObjectBound->m_vMin.z)+0.11f, pObjectBound->m_vMin.z);
				pVertex[10].pos = Vector3(pObjectBound->m_vMax.x, g_MapRes.GetHeight(pObjectBound->m_vMax.x,pObjectBound->m_vMin.z)+0.11f, pObjectBound->m_vMin.z);
				pVertex[11].pos = Vector3(vBig.x, g_MapRes.GetHeight(vBig.x,pObjectBound->m_vMin.z)+0.11f, pObjectBound->m_vMin.z);

				pVertex[12].pos = Vector3(vSmall.x, g_MapRes.GetHeight(vSmall.x,vSmall.y)+0.11f, vSmall.y);
				pVertex[13].pos = Vector3(pObjectBound->m_vMin.x, g_MapRes.GetHeight(pObjectBound->m_vMin.x,vSmall.y)+0.11f, vSmall.y);
				pVertex[14].pos = Vector3(pObjectBound->m_vMax.x, g_MapRes.GetHeight(pObjectBound->m_vMax.x,vSmall.y)+0.11f, vSmall.y);
				pVertex[15].pos = Vector3(vBig.x, g_MapRes.GetHeight(vBig.x,vSmall.y)+0.11f, vSmall.y);	// Bottom-Right

				// 뿔뚝 솟은 애들.
				pVertex[16].pos = Vector3(pObjectBound->m_vMin.x, pObjectBound->m_vMax.y+0.11f, pObjectBound->m_vMin.z);
				pVertex[17].pos = Vector3(pObjectBound->m_vMax.x, pObjectBound->m_vMax.y+0.11f, pObjectBound->m_vMin.z);
				pVertex[18].pos = Vector3(pObjectBound->m_vMin.x, pObjectBound->m_vMax.y+0.11f, pObjectBound->m_vMax.z);
				pVertex[19].pos = Vector3(pObjectBound->m_vMax.x, pObjectBound->m_vMax.y+0.11f, pObjectBound->m_vMax.z);

				nVertex = 20;

				// texture index
				float fXSize = vBig.x - vSmall.x;
				float fX1 = pObjectBound->m_vMin.x - vSmall.x;
				float fX2 = pObjectBound->m_vMax.x - vSmall.x;

				float fYSize = vBig.y - vSmall.y;				// 양수가 나옴.
				float fY1 = vBig.y - pObjectBound->m_vMax.z;
				float fY2 = vBig.y - pObjectBound->m_vMin.z;

				pVertex[ 0].tex = Vector2(          0, fY1/fYSize );
				pVertex[ 1].tex = Vector2( fX1/fXSize, fY1/fYSize );
				pVertex[ 2].tex = Vector2(          0,          0 );
				pVertex[ 3].tex = Vector2( fX1/fXSize,          0 );

				pVertex[ 4].tex = Vector2( fX2/fXSize, fY1/fYSize );
				pVertex[ 5].tex = Vector2( fX2/fXSize,          0 );
				pVertex[ 6].tex = Vector2(          1, fY1/fYSize );
				pVertex[ 7].tex = Vector2(          1,          0 );

				pVertex[ 8].tex = Vector2(          0, fY2/fYSize );
				pVertex[ 9].tex = Vector2( fX1/fXSize, fY2/fYSize );
				pVertex[10].tex = Vector2( fX2/fXSize, fY2/fYSize );
				pVertex[11].tex = Vector2(          1, fY2/fYSize );

				pVertex[12].tex = Vector2(          0,          1 );
				pVertex[13].tex = Vector2( fX1/fXSize,          1 );
				pVertex[14].tex = Vector2( fX2/fXSize,          1 );
				pVertex[15].tex = Vector2(          1,          1 );

				pVertex[16].tex = Vector2( fX1/fXSize, fY2/fYSize );
				pVertex[17].tex = Vector2( fX2/fXSize, fY2/fYSize );
				pVertex[18].tex = Vector2( fX1/fXSize, fY1/fYSize );
				pVertex[19].tex = Vector2( fX2/fXSize, fY1/fYSize );

				// Face
				pFace[ 0] =  0;
				pFace[ 1] =  2;
				pFace[ 2] =  1;
				pFace[ 3] =  2;
				pFace[ 4] =  3;
				pFace[ 5] =  1;

				pFace[ 6] =  1;
				pFace[ 7] =  3;
				pFace[ 8] =  4;
				pFace[ 9] =  3;
				pFace[10] =  5;
				pFace[11] =  4;

				pFace[12] =  4;
				pFace[13] =  5;
				pFace[14] =  6;
				pFace[15] =  5;
				pFace[16] =  7;
				pFace[17] =  6;

				pFace[18] =  8;
				pFace[19] =  0;
				pFace[20] =  9;
				pFace[21] =  0;
				pFace[22] =  1;
				pFace[23] =  9;

				pFace[24] = 10;
				pFace[25] =  4;
				pFace[26] = 11;
				pFace[27] =  4;
				pFace[28] =  6;
				pFace[29] = 11;

				pFace[30] = 12;
				pFace[31] =  8;
				pFace[32] = 13;
				pFace[33] =  8;
				pFace[34] =  9;
				pFace[35] = 13;

				pFace[36] = 13;
				pFace[37] =  9;
				pFace[38] = 14;
				pFace[39] =  8;
				pFace[40] = 10;
				pFace[41] = 14;

				pFace[42] = 14;
				pFace[43] = 10;
				pFace[44] = 15;
				pFace[45] = 10;
				pFace[46] = 11;
				pFace[47] = 15;

				// 뿔뚝 솟은 애들.
				pFace[48] = 18;
				pFace[49] =  1;
				pFace[50] = 19;
				pFace[51] =  1;
				pFace[52] =  4;
				pFace[53] = 19;

				pFace[54] = 19;
				pFace[55] =  4;
				pFace[56] = 17;
				pFace[57] =  4;
				pFace[58] = 10;
				pFace[59] = 17;

				pFace[60] =  9;
				pFace[61] = 16;
				pFace[62] = 10;
				pFace[63] = 16;
				pFace[64] = 17;
				pFace[65] = 10;

				pFace[66] = 16;
				pFace[67] =  9;
				pFace[68] = 18;
				pFace[69] =  9;
				pFace[70] =  1;
				pFace[71] = 18;

				pFace[72] = 16;
				pFace[73] = 18;
				pFace[74] = 17;
				pFace[75] = 18;
				pFace[76] = 19;
				pFace[77] = 17;

				nFace = 78;

				for(int h=0; h<78; h++)
                    pVertex[h].diffuse = dColor;

			}// if
			else	// 3. 서로 겹쳤을때.
			if(!(pObjectBound->m_vMin.x > vBig.x ||
				 pObjectBound->m_vMin.z > vBig.y ||
				 pObjectBound->m_vMax.x < vSmall.x ||
				 pObjectBound->m_vMax.z < vSmall.y  ))
			{
				// 대부분의 경우 여기에 걸린다. 그 다음으로 1번.

				// 일단 두개의 직각형이 만나는 정점을 구하자.
				Vector2 vSmall2 = Vector2( pObjectBound->m_vMin.x, pObjectBound->m_vMin.z );
				Vector2 vBig2   = Vector2( pObjectBound->m_vMax.x, pObjectBound->m_vMax.z );

				// 알고보니 플로트 배열로 데이터를 저장할 수가 있군.
				float fXArray[4]; // 계산상 최대 4개이다.
				float fYArray[4];
				float fIXArray[2];	// 중복되는 직사각형.
				float fIYArray[2];
				int nTotalVectorX, nTotalVectorY;

				// 버텍스를 구성할 벡터를 구한다.
				FindIntersectVector( vSmall, vBig, vSmall2, vBig2, nTotalVectorX, nTotalVectorY, fXArray, fYArray, fIXArray, fIYArray );

				if( nTotalVectorX == 0 || nTotalVectorY == 0 ) return FALSE; // 물론 이런일은 없다.

				// make vertex and tex
				int i,j;
				float fXSize = vBig.x - vSmall.x;
				float fYSize = vBig.y - vSmall.y;

				for(j=0; j<nTotalVectorY; j++)
					for(i=0; i<nTotalVectorX; i++)
					{
						pVertex[nVertex++].pos = Vector3( fXArray[i], g_MapRes.GetHeight(fXArray[i],fYArray[j])+0.11f, fYArray[j] );

						pVertex[nVertex-1].tex = Vector2( (fXArray[i]-vSmall.x)/fXSize, (vBig.y-fYArray[j])/fYSize );
					}

				// 뿔뚝 솟은 부분.
				pVertex[nVertex++].pos = Vector3( fIXArray[0], pObjectBound->m_vMax.y+0.11f, fIYArray[0] );
				pVertex[nVertex++].pos = Vector3( fIXArray[1], pObjectBound->m_vMax.y+0.11f, fIYArray[0] );
				pVertex[nVertex++].pos = Vector3( fIXArray[0], pObjectBound->m_vMax.y+0.11f, fIYArray[1] );
				pVertex[nVertex++].pos = Vector3( fIXArray[1], pObjectBound->m_vMax.y+0.11f, fIYArray[1] );

				// make face, 이게 어렵군. 
				// 겹쳐지는 부분의 1층 시작하는 버텍스 인덱스
				int nFindVertexIndex;
				for(i=0; i<nTotalVectorX*nTotalVectorY; i++)
				{
					if( pVertex[i].pos.x == fIXArray[0] &&
						pVertex[i].pos.z == fIYArray[0]   )
					{
						nFindVertexIndex = i;
						break;
					}// if
				}// for

				// 1층. 불필요한 부분은(겹쳐지는 곳의 1층 바닥) 없애자.
				for(j=0; j<nTotalVectorY-1; j++)
				{
					for(i=0; i<nTotalVectorX-1; i++)
					{
						// 겹쳐지는 부분의 1층
						if( (j*nTotalVectorX) + i == nFindVertexIndex )
							continue;

						pFace[nFace++] = (j*nTotalVectorX) + i+nTotalVectorX;
						pFace[nFace++] = (j*nTotalVectorX) + i;
						pFace[nFace++] = (j*nTotalVectorX) + i+nTotalVectorX + 1;

						pFace[nFace++] = (j*nTotalVectorX) + i;
						pFace[nFace++] = (j*nTotalVectorX) + i+1;
						pFace[nFace++] = (j*nTotalVectorX) + i+nTotalVectorX + 1;
					}
				}

				// 2층을 만들위한 버텍스 인덱스.
				int FirstIndexAry[4];
				FirstIndexAry[0] = nFindVertexIndex;
				FirstIndexAry[1] = nFindVertexIndex + 1;
				FirstIndexAry[3] = nFindVertexIndex + nTotalVectorX;
				FirstIndexAry[2] = nFindVertexIndex + nTotalVectorX + 1;

				int SecondIndexAry[4];
				SecondIndexAry[0] = nVertex - 4;
				SecondIndexAry[1] = nVertex - 3;
				SecondIndexAry[3] = nVertex - 2;
				SecondIndexAry[2] = nVertex - 1;

				for(i=0; i<4; i++)
                    pVertex[ SecondIndexAry[i] ].tex = pVertex[ FirstIndexAry[i] ].tex;

				// 2층 Face, 내 노트 참고  ㅡ,.ㅡ^;
				for(i=0; i<4; i++)
				{
					pFace[nFace++] = SecondIndexAry[i];
					pFace[nFace++] = FirstIndexAry[i];
					pFace[nFace++] = SecondIndexAry[ (i+1)%4 ];

					pFace[nFace++] = FirstIndexAry[i];
					pFace[nFace++] = FirstIndexAry[ (i+1)%4 ];
					pFace[nFace++] = SecondIndexAry[ (i+1)%4 ];
				}

				pFace[nFace++] = SecondIndexAry[3];
				pFace[nFace++] = SecondIndexAry[0];
				pFace[nFace++] = SecondIndexAry[2];

				pFace[nFace++] = SecondIndexAry[0];
				pFace[nFace++] = SecondIndexAry[1];
				pFace[nFace++] = SecondIndexAry[2];

				for(int h=0; h<nVertex; h++)
                    pVertex[h].diffuse = dColor;
			}// if

			return TRUE;
		}

		// 두개의 직사각형의 겹치는 정점을 구한다. 2차원 계산.
		BOOL CMapCellRender::FindIntersectVector(Vector2 vSmall1, Vector2 vBig1, Vector2 vSmall2, Vector2 vBig2, int& nTotalX, int& nTotalY, float *fXArray, float *fYArray, float *fIXArray, float *fIYArray)
		{
			// vSmall1, vBig1 이 마우스 위치 커서의 좌표이다. 꼭!
			// 두 직사각형이 겹쳐질때 계산되는 좌표들은 x좌표, y좌표로 따로 계산될 수 있다.
			nTotalX = 0;
			nTotalY = 0;

			// 1. X만 생각한다. 알아서 해독하도록.
			fXArray[nTotalX++] = vSmall1.x;
			if( vSmall1.x <= vSmall2.x )
			{
				if( vSmall1.x != vSmall2.x )
                    fXArray[nTotalX++] = vSmall2.x;

				fIXArray[0] = vSmall2.x;	// Intersect point

				if( vBig1.x <= vBig2.x )
				{
					fXArray[nTotalX++] = vBig1.x;

					fIXArray[1] = vBig1.x;
				}
				else
				{
					fXArray[nTotalX++] = vBig2.x;
					fXArray[nTotalX++] = vBig1.x;

					fIXArray[1] = vBig2.x;
				}
			}
			else
			{
				fIXArray[0] = vSmall1.x;

				if( vBig1.x <= vBig2.x )
				{
					fXArray[nTotalX++] = vBig1.x;

					fIXArray[1] = vBig1.x;
				}
				else
				{
					fXArray[nTotalX++] = vBig2.x;
					fXArray[nTotalX++] = vBig1.x;

					fIXArray[1] = vBig2.x;
				}
			}

			// 2. Y. Y는 (-)이다. => Z
			fYArray[nTotalY++] = vBig1.y;
			if( vBig1.y >= vBig2.y )
			{
				if( vBig1.y != vBig2.y )
					fYArray[nTotalY++] = vBig2.y;

				fIYArray[0] = vBig2.y;

				if( vSmall1.y >= vSmall2.y )
				{
					fYArray[nTotalY++] = vSmall1.y;

					fIYArray[1] = vSmall1.y;
				}
				else
				{
					fYArray[nTotalY++] = vSmall2.y;
					fYArray[nTotalY++] = vSmall1.y;

					fIYArray[1] = vSmall2.y;
				}
			}
			else
			{
				fIYArray[0] = vBig1.y;

				if( vSmall1.y >= vSmall2.y )
				{
					fYArray[nTotalY++] = vSmall1.y;

					fIYArray[1] = vSmall1.y;
				}
				else
				{
					fYArray[nTotalY++] = vSmall2.y;
					fYArray[nTotalY++] = vSmall1.y;

					fIYArray[1] = vSmall2.y;
				}
			}

			return TRUE;
		}

		BOOL CMapCellRender::SetDecalOnMouseCursor(CMapDecal* pDecal)
		{
			m_pDecalOnMapObject = pDecal;

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
		XIAHGE_API CMapRender::CMapRender()
		{
			m_MapBound = sRect( 0, 0, 2048, 2048);
			m_nVisibleMeshBlock = 0;

			m_MapRenderInfo.m_DiffuseColor = D3DCOLOR_XRGB( 255, 255, 255);
			m_MapRenderInfo.m_CameraBoundSize = CAMERA_BOUND_GRID_SIZE;
			m_MapRenderInfo.m_fDetailMapRatio = 0.5f;

			m_pWaterTexture = NULL;
			m_pYongAmTexture = NULL;
			m_pMarshTexture = NULL;

			m_pMapCellRenderWidthMouseCursor = NULL;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapRender::~CMapRender()
		{
			Release();
		}

		XIAHGE_API BOOL CMapRender::Release()
		{
			for(int i = 0; i < MAX_MAPCELL_COUNT; i++)
			{
				if( m_MapCellRender[ i].IsValid())
					m_MapCellRender[ i].Release();
			}

			m_MapBound = sRect( 0, 0, 2048, 2048);
			m_CameraBound = sRect( 0, 0, 0, 0);
			
			m_VisibleMapCellList.clear();
			m_VisibleMapDecalList.clear();

			m_nVisibleMeshBlock = 0;
			m_nVisibleObject = 0;

			m_pWaterTexture = NULL;
			m_pYongAmTexture = NULL;
			m_pMarshTexture = NULL;

			ReleaseGrassZoneArray();

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::ReleaseVisibleMapCell()
		{
			// 현재 m_VisibleMapCellList안에 있는 맵 셀들을 릴리즈 한다.
			MAPRENDER_MAPCELL_LIST::iterator it;
			for( it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender* pMapCellRender = *it;

				pMapCellRender->Release();
			}
			m_VisibleMapCellList.clear();


			m_VisibleMapDecalList.clear();

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRender::Update()
		{
			DBG_Assert( g_pCurrentCamera != NULL);

			if( !g_pCurrentCamera->IsUpdate())
				return TRUE;
			
#if	TEST_PERFORMANCE
			g_bLog = FALSE;
			
			if( timeGetTime() - g_LastLogTime > 2000)
			{
				g_bLog = TRUE;
				g_LastLogTime = timeGetTime();
			}

			g_StartTime = timeGetTime();
#endif

			sRect rcCamera;

			rcCamera.left = g_pCurrentCamera->m_vFrom.x;
			rcCamera.top  = -g_pCurrentCamera->m_vFrom.z;
			rcCamera.right = rcCamera.left + m_MapRenderInfo.m_CameraBoundSize * 2;
			rcCamera.bottom = rcCamera.top + m_MapRenderInfo.m_CameraBoundSize * 2;

			rcCamera -= sPoint( m_MapRenderInfo.m_CameraBoundSize, m_MapRenderInfo.m_CameraBoundSize);

			if( rcCamera != m_CameraBound)	// 위치가 이동될때
			{
				m_CameraBound = rcCamera;

				PrepareMapCell();
			}

			m_nVisibleMeshBlock = 0;
			m_nVisibleObject = 0;

			// 카메라가 회전 될 수도 있으니까
			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;
				if(pMapCellRender == NULL)
				{
					DBG_LogFile( _T("CMapRender::Update fail"));
				}

				pMapCellRender->QueryVisibleMeshblock_Level2( g_pCurrentCamera, m_MapRenderInfo.m_CameraBoundSize);
				m_nVisibleMeshBlock += pMapCellRender->GetVisibleMeshBlockCount_Level2();
				m_nVisibleObject += pMapCellRender->GetVisibleObjectList_Level2();
			}

			// 라이트 세팅
			Light::g_LightManager.PrepareRender();

#if	TEST_PERFORMANCE
			g_StartTime = timeGetTime() - g_StartTime;
		
			if( g_bLog)
				DBG_LogFile("MapRender : Update : %d", g_StartTime);
#endif
			return TRUE;
		}
		
		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRender::RenderTerrain()
		{
			Light::g_LightManager.Render();

			if( m_VisibleMapCellList.size() == 0)
				return TRUE;

			g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);

			// 메인 케릭터가 암흑무에 걸렸다.
			if( m_MapRenderInfo.m_bAmhukmuFog )
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, m_MapRenderInfo.m_bAmhukmuFog );
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGCOLOR,  D3DCOLOR_XRGB( 50, 50, 50 ) );

				g_pDirect3DDevice->SetRenderState( D3DRS_FOGSTART,		1.0f  );
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGEND,		30.0f );
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGDENSITY,	0.07f );

				g_pDirect3DDevice->SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_NONE );
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );// D3DFOG_NONE
			}
			else
			if( m_MapRenderInfo.m_bFog)
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_EXP2);
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
//				g_pDirect3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
//				g_pDirect3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
				
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGDENSITY, *((DWORD *)(&m_MapRenderInfo.m_fFogDensity)));
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGCOLOR, m_MapRenderInfo.m_FogColor);
				g_pDirect3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE);

				float fStart, fEnd;

				fStart = (float)m_MapRenderInfo.m_CameraBoundSize * 0.6f;
				fEnd   = (float)m_MapRenderInfo.m_CameraBoundSize;

				g_pDirect3DDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD *)(&fStart)));
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGEND, *((DWORD *)(&fEnd)));
			}

#if TEST_PERFORMANCE
			DWORD StartTime = timeGetTime();
#endif

			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;
				if(pMapCellRender == NULL)
				{
					DBG_LogFile( _T("CMapRender::RenderTerrain fail"));
				}

				pMapCellRender->Render(0);
			}

			// 마우스 위치 커서를 따로 세팅.
			if( m_pMapCellRenderWidthMouseCursor )
				m_pMapCellRenderWidthMouseCursor->SetDecalOnMouseCursor( NULL );

			MAPDECAL_LIST::iterator decal_it;
			for(decal_it = m_VisibleMapDecalList.begin(); decal_it != m_VisibleMapDecalList.end(); decal_it++)
			{
				CMapDecal* pDecal = *decal_it;
				if(pDecal == NULL)
				{
					DBG_LogFile( _T("CMapRender::RenderTerrain decal fail"));
				}

				// 만약 맵 오브젝트위에 있는 거라면 안그린다. 이 녀석은 g_PickCursor 이다.
				if( !pDecal->m_bOnMapObject )
					pDecal->Render();
				else
				{
					// 이때 마우스 위치 커서를 따로 세팅.
					if( m_pMapCellRenderWidthMouseCursor )
						m_pMapCellRenderWidthMouseCursor->SetDecalOnMouseCursor( pDecal );
				}
			}

			// Changth : 내가 주석을 달아주지. 역시나 주석이 없군...
			// 클라이언트에서 AddVisibalMapDecal() 함수를 사용하여 매 프레임마나 그려질 그림자를
			// 리스트에 넣어준다. 그러면 엔진에서는 현재 리스트에 있는 그림자를 그리고 리스트를
			// 모두 지운다. 이렇게 되면 굳이 그려질 그림자, 안그려질 그림자로 따로 구분할 필요가 없다.
			ClearVisibleMapDecalList();

#if	TEST_PERFORMANCE
			StartTime = timeGetTime() - StartTime;
		
			if( g_bLog)
				DBG_LogFile("MapRender Terrain : Render : %d", StartTime);
#endif
	
			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::RenderObject(BYTE byType)
		{
			if( m_VisibleMapCellList.size() == 0)
				return TRUE;

#if TEST_PERFORMANCE
			DWORD StartTime = timeGetTime();
#endif

			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;
				if(pMapCellRender == NULL)
				{
					DBG_LogFile( _T("CMapRender::RenderObject fail"));
				}
				pMapCellRender->Render(byType);
			}

#if	TEST_PERFORMANCE
			StartTime = timeGetTime() - StartTime;
		
			if( g_bLog)
				DBG_LogFile("MapRender : Object Render : %d", StartTime);
#endif

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::RenderWater()
		{
			if( m_VisibleMapCellList.size() == 0)
				return TRUE;

			if( g_MapRes.m_bLoadMapFile )
			{
				g_MapRes.m_bLoadMapFile = false;

				switch( g_MapRes.m_byWaterType )
				{
				case eWaterType_Water:
					m_pWaterTexture  = XiahPak::GetTexture( 50000205 );
					break;
				case eWaterType_YongAm:
					m_pYongAmTexture = XiahPak::GetTexture( 50000801 );
					break;
				case eWaterType_Marsh:
					m_pMarshTexture  = XiahPak::GetTexture( 50001241 );
					break;
				}
			}

			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
			
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE  );
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			g_pDirect3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			
//			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);

			// 물 vertex를 물, 용암, 늪지대 물로 표현한다.
			switch( g_MapRes.m_byWaterType )
			{
			case eWaterType_Water:
				{
					g_Device.SetTexture(0, m_pWaterTexture);
					g_Device.SetTexture(1, m_pWaterTexture);

					//g_pDirect3DDevice->SetTexture( 0, m_pWaterTexture );
					//g_pDirect3DDevice->SetTexture( 1, m_pWaterTexture );
				}				
				break;
			case eWaterType_YongAm:
				{
					g_Device.SetTexture(0, m_pYongAmTexture);
					g_Device.SetTexture(1, m_pYongAmTexture);

					//g_pDirect3DDevice->SetTexture( 0, m_pYongAmTexture );
					//g_pDirect3DDevice->SetTexture( 1, m_pYongAmTexture );
				}				
				break;
			case eWaterType_Marsh:
				{
					g_Device.SetTexture(0, m_pMarshTexture);
					g_Device.SetTexture(1, m_pMarshTexture);

					//g_pDirect3DDevice->SetTexture( 0, m_pMarshTexture );
					//g_pDirect3DDevice->SetTexture( 1, m_pMarshTexture );
				}				
				break;
			};// switch

			//
			Matrix4x4 iTM;

			static float height = 0;
			static int   height_dir = 1;

			if( height_dir == 1)
			{
				height += 0.005f * g_fFrameScale;
				if( height > 0.2f)
					height_dir = 0;
			}
			else if( height_dir == 0)
			{
				height -= 0.005f * g_fFrameScale;
				if( height < -0.2f)
					height_dir = 1;
			}
			
			iTM.t.y = height;

			g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&iTM);

			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE);

			// 물 그려주기
            MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender* pMapCellRender = *it;

				pMapCellRender->RenderWater();
			}

			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);

			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
//			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//			g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRender::PrepareMapCell()
		{
			//현재 Bound에 들어 있는 MapCell의 List를 뽑아준다
			//최대 9 MapCell

			// Effect를 위해 ㅆ_ㅆ
			std::set<CMapCellRender*>	InvisibleMapCellList;
			
			MAPRENDER_MAPCELL_LIST::iterator it;
			std::set<CMapCellRender*>::iterator mapcell_it;

			for( it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				InvisibleMapCellList.insert( *it);
			}

			m_VisibleMapCellList.clear();
			int i,j;
			int start_mapcell_x = (m_CameraBound.Center().x / 256) * 64;
			int start_mapcell_y = (m_CameraBound.Center().y / 256) * 64;

			start_mapcell_x -= 64;
			start_mapcell_y -= 64;

			for(i = start_mapcell_y; i < start_mapcell_y + 64 * 3; i+= 64)
			{
				for(j = start_mapcell_x; j < start_mapcell_x + 64 * 3; j+= 64)
				{
					if( j < 0 || i < 0 ||
						j > 64 * 7 || i > 64 * 7)
						continue;

					sRect rcMapCell( sPoint( j * 4, i * 4), sSize( 256, 256));

					if( !rcMapCell.IsIntersect( m_CameraBound))
						continue;	// 아마 이럴일은 거의 없을듯

					// 일단 Realize시켜서 준비해준다
					CRes_MapCell *pMapCell = g_MapRes.GetMapCell( j, i);

//					없어도 될듯.
//					DBG_Assert( pMapCell != NULL);

					if( pMapCell == NULL)
						continue;

					CMapCellRender *pMapCellRender = &m_MapCellRender[ (j / 64) + (i / 64) * 8];

					if( !pMapCellRender->IsValid())
					{
						pMapCellRender->Create( pMapCell);
					}

					if( !pMapCellRender->QueryVisibleMeshblock_Level1( &m_CameraBound))
					{
						continue;	// 여기 걸리는 경우도 없겠다 적어도 1개는 걸리니까
					}

					mapcell_it = InvisibleMapCellList.find( pMapCellRender);

					if( mapcell_it != InvisibleMapCellList.end())
					{
						InvisibleMapCellList.erase( mapcell_it);
					}

					pMapCellRender->SetDiffuseColor( m_MapRenderInfo.m_DiffuseColor);
					pMapCellRender->SetDetailRatio( m_MapRenderInfo.m_fDetailMapRatio);
					m_VisibleMapCellList.push_back( pMapCellRender);
				}
			}
		
			for(mapcell_it = InvisibleMapCellList.begin(); mapcell_it != InvisibleMapCellList.end(); mapcell_it++)
			{
				CMapCellRender* pMapCellRender = *mapcell_it;
				if(pMapCellRender == NULL)
				{
					DBG_LogFile( _T("CMapRender::PrepareMapCell fail"));
				}

				pMapCellRender->Release();
//				pMapCellRender->ClearVisible();
			}
			
			InvisibleMapCellList.clear();

			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRender::SetRenderInfo( sMapRenderInfo* pInfo)
		{
			BOOL bUpdateRenderInfo = FALSE;

			if(pInfo == NULL)
			{
				DBG_LogFile( _T("CMapRender::SetRenderInfo fail"));
			}

			if( m_MapRenderInfo.m_DiffuseColor != pInfo->m_DiffuseColor)
			{
				bUpdateRenderInfo = TRUE;
			}

			if( m_MapRenderInfo.m_fDetailMapRatio != pInfo->m_fDetailMapRatio)
			{
				bUpdateRenderInfo = TRUE;
			}

			m_MapRenderInfo = *pInfo;
		
			if( bUpdateRenderInfo)
			{
				UpdateMapCellRenderInfo();
/*			
				Light::CLight *pLight = Light::g_LightManager.GetGlobalLight();

				pLight->m_Light.Diffuse.r = (float)GetRValue( m_MapRenderInfo.m_DiffuseColor) / 255.0f;
				pLight->m_Light.Diffuse.g = (float)GetGValue( m_MapRenderInfo.m_DiffuseColor) / 255.0f;
				pLight->m_Light.Diffuse.b = (float)GetBValue( m_MapRenderInfo.m_DiffuseColor) / 255.0f;
				pLight->m_Light.Diffuse.a = 1;

				pLight->m_Light.Ambient.r = pLight->m_Light.Diffuse.r * 0.5f;
				pLight->m_Light.Ambient.g = pLight->m_Light.Diffuse.g * 0.5f;
				pLight->m_Light.Ambient.b = pLight->m_Light.Diffuse.b * 0.5f;
				pLight->m_Light.Ambient.a = 1;
*/
			}
		
			return TRUE;
		}

		//---------------------------------------------------------------------------------------
		XIAHGE_API BOOL CMapRender::UpdateMapCellRenderInfo()
		{
			// 보이는것만 해준다
			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;
				if(pMapCellRender == NULL)
				{
					DBG_LogFile( _T("CMapRender::UpdateMapCellRenderInfo fail"));
				}

				pMapCellRender->SetDiffuseColor( m_MapRenderInfo.m_DiffuseColor);
				pMapCellRender->SetDetailRatio( m_MapRenderInfo.m_fDetailMapRatio);
			}

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::QueryMeshblockObjectList(WORD wPosX,WORD wPosY,MAPRENDER_MAPOBJECTLIST **ppList)
		{
			int mapcell_x;
			int mapcell_y;
			int meshblock_x;
			int meshblock_y;

			mapcell_x = wPosX / 256;
			mapcell_y = wPosY / 256;

			meshblock_x = (wPosX % 256) / 32;
			meshblock_y = (wPosY % 256) / 32;
			
			if( mapcell_x < 0 || mapcell_y < 0 || mapcell_x > 7 || mapcell_y > 7)
				return FALSE;

			if( meshblock_x < 0 || meshblock_y < 0 || meshblock_x > 7 || meshblock_y > 7)
				return FALSE;
			
			CMapCellRender* pMapCellRender = &m_MapCellRender[(mapcell_x) + (mapcell_y) * 8];

			if( !pMapCellRender->IsValid())
				return FALSE;

			*ppList = pMapCellRender->GetMeshBlockObjectList( meshblock_x, meshblock_y);
		
			if( *ppList == NULL)
			{
				DBG_LogFile( _T("CMapRender::QueryMeshblockObjectList fail"));
				return FALSE;
			}

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::CreateGrassZoneArray()
		{
			for(int i=0; i<GRASSZONE_ARRAY_MAX; i++)
				m_GrassZonePool[i].Init();

			for(i=0; i<GRASSZONE_ARRAY_MAX; i++)
                m_GrassZoneList.push_back( &m_GrassZonePool[i] );

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::ReleaseGrassZoneArray()
		{
			for(int i=0; i<GRASSZONE_ARRAY_MAX; i++)
				m_GrassZonePool[i].Release();

			m_GrassZoneList.clear();

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::UpdateGrassZone()
		{
			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;

				pMapCellRender->UpdateGrass();
			}

			return TRUE;
		}

		XIAHGE_API BOOL CMapRender::RenderGrassZone()
		{
			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;

				pMapCellRender->RenderGrass();
			}

			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

			return TRUE;
		}

		// 각 맵셀을 뒤져서 마우스 커서가 있는 오브젝트가 있는 맵셀을 구한다.
		BOOL CMapRender::CheckMapObjectForMouseCursor(BBoxAABB3 MouseBound, float fCharHeight, float fCharPositionY)
		{
			m_pMapCellRenderWidthMouseCursor = NULL;

			MAPRENDER_MAPCELL_LIST::iterator it;
			for(it = m_VisibleMapCellList.begin(); it != m_VisibleMapCellList.end(); it++)
			{
				CMapCellRender *pMapCellRender = *it;

				if(pMapCellRender == NULL) continue;

				if( pMapCellRender->CheckMapObjectForMouseCursor(MouseBound, fCharHeight, fCharPositionY) )
				{
					m_pMapCellRenderWidthMouseCursor = pMapCellRender;
					return TRUE;
				}
			}// for

			return FALSE;
		}

		// 마우스 커서가 있는 오브젝트를 가진 맵 셀이 계산하도록 한다.
		BOOL CMapRender::MakeMouseCursorVertexOnObject(Vector2 vSmall, Vector2 vBig, D3DCOLOR dColor, VT_LVertex* pVertex, WORD* pFace, int& nVertex, int& nFace)
		{
			if( !m_pMapCellRenderWidthMouseCursor ) return FALSE;

			return m_pMapCellRenderWidthMouseCursor->MakeMouseCursorVertexOnObject( vSmall, vBig, dColor, pVertex, pFace, nVertex, nFace );
		}


		//---------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------
		XIAHGE_API CMapRender g_MapRender;
	};

};