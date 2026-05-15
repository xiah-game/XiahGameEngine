#include "stdafx.h"
#include "CharMan.h"
#include "CharRender.h"
#include "XiahPak.h"

#include "SoundPak.h"
#include "XiahBGM.h"

namespace XiahGameEngine
{
	unsigned short g_TempFaceBuffer[ 6000];	// 폴리곤 3천개 짜리 캐릭터는 없겠지 ㅡ,,ㅡ

	//-------------------------------------------------------------
	CCharSubMesh::CCharSubMesh(Res_SkinMesh* pMesh)
	{
		m_pIndexBuffer = NULL;	//??
		Create(pMesh);
	}

	CCharSubMesh::CCharSubMesh()
	{
		m_pResMesh = NULL;
		m_pIndexBuffer = NULL;
	}

	CCharSubMesh::~CCharSubMesh()
	{
		Clear();
	}

	void CCharSubMesh::Clear()
	{
		m_pResMesh = NULL;

		if( m_pIndexBuffer)
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = NULL;
		}
	}

	// Res_SkinMesh로 부터 InstanceData를 만들어 낸다 
	BOOL CCharSubMesh::Create(Res_SkinMesh* pMesh)
	{
		Clear();

		m_pResMesh = pMesh;

		HRESULT hr;

		// 디폴트로 D3DPOOL_MANAGED 로 생성한다.
		// 피직이 있으면 현재 하드웨어 가속을 받을 수 없다. ㅠㅠ;
		if( m_pResMesh->is_physique )
			hr = g_pDirect3DDevice->CreateIndexBuffer( pMesh->face_count * 2 * 3, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIndexBuffer, NULL);
			//hr = g_pDirect3DDevice->CreateIndexBuffer( pMesh->face_count * 2 * 3, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
		else
		{
//			hr = g_pDirect3DDevice->CreateIndexBuffer( pMesh->face_count * 2 * 3, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL);
			hr = g_pDirect3DDevice->CreateIndexBuffer( pMesh->face_count * 2 * 3, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
		}

		if( FAILED( hr))
			return FALSE;

		LPBYTE pIndexData;
		hr = m_pIndexBuffer->Lock( 0, 0, (void **)&pIndexData, 0);

		if( FAILED( hr))
			return FALSE;

		memcpy( pIndexData, m_pResMesh->face_ptr, m_pResMesh->face_count * 3 * 2);

		m_pIndexBuffer->Unlock();

		SetLodLevel( 1.0f);
	
		return TRUE;
	}

	BOOL CCharSubMesh::SetLodLevel(float fLevel)
	{
		m_nLodVertex = m_pResMesh->vertex_count;
		m_nLodFace = m_pResMesh->face_count;

		// LOD 엄따.
/*
		if( fLevel < 0)
			fLevel = 0;

		if( fLevel > 1)
			fLevel = 1;
		
		int lodcount = (int)((1.0f - fLevel) * m_pResMesh->lod_count);

		if( lodcount == 0)
		{
			m_nLodVertex = m_pResMesh->vertex_count;
			m_nLodFace = m_pResMesh->face_count;

			return TRUE;
		}

		m_nLodVertex = m_pResMesh->vertex_count - lodcount;
		m_nLodFace = m_pResMesh->face_count;

		int i;

		for(i = 0; i < lodcount; i++)
		{
			m_nLodFace -= m_pResMesh->lod_facecount_ptr[ m_pResMesh->vertex_count - i - 1];
		}
		
		if( m_nLodFace <= 0)
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
				collapse_id = m_pResMesh->face_ptr[ i * 3 + j];

				while( collapse_id >= m_nLodVertex)
				{
					new_collapse_id = m_pResMesh->lod_collapse_ptr[ collapse_id];

					if( new_collapse_id == collapse_id)
						break;

					collapse_id = new_collapse_id;
				}

				if( collapse_id < 0 || collapse_id > m_pResMesh->vertex_count - 1)
					g_TempFaceBuffer[ i * 3 + j] = 0;
				else
					g_TempFaceBuffer[ i * 3 + j] = collapse_id;
			}
		}

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

	CCharMesh::CCharMesh(Res_Mesh* pMesh)
	{
		Create(pMesh);
	}

	CCharMesh::CCharMesh()
	{
		m_pResMesh = NULL;
		m_nSubMesh = 0;
	}

	CCharMesh::~CCharMesh()
	{
		Clear();
	}

	BOOL CCharMesh::Clear()
	{
		// 이 클래스들은 인스턴스이므로 실제 캐릭터 데이터들을 Release 하면 안된다!!!
		m_pResMesh = NULL;

		for(int i=0; i<m_nSubMesh; i++)
			m_SubMesh[i].Clear();

		return TRUE;
	}

    BOOL CCharMesh::Create(Res_Mesh* pMesh)
	{
		Clear();

		m_pResMesh = pMesh;
		m_nSubMesh = m_pResMesh->skin_mesh_count;

		for(int i=0; i<m_nSubMesh; i++)
		{
			Res_SkinMesh* pSkinMesh = &m_pResMesh->skin_mesh_ptr[i];

			BOOL bOK = m_SubMesh[i].Create( pSkinMesh );
			if( !bOK ) return bOK;
		}// for(m_nSubMesh)

		SetLodLevel( 1.0f );
		
		// mesh center구하기

		Vector3 vMin,vMax;

		vMin = Vector3( 10000.0f, 10000.0f, 10000.0f);
		vMax = -vMin;

		for(i = 0; i < m_nSubMesh; i++)
		{
			if( m_SubMesh[ i].m_pResMesh->is_physique)
			{
				SkinVertex *pVertex;

				if( !FAILED( m_SubMesh[ i].m_pResMesh->vertex_ptr->Lock( 0, 0, (VOID**)&pVertex, 0 )) )
				{
					for(int j = 0; j < m_SubMesh[ i].m_pResMesh->vertex_count; j++)
					{
						if( pVertex->position[ 0] < vMin.x) vMin.x = pVertex->position[ 0];
						if( pVertex->position[ 1] < vMin.y) vMin.y = pVertex->position[ 1];
						if( pVertex->position[ 2] < vMin.z) vMin.z = pVertex->position[ 2];

						if( pVertex->position[ 0] > vMax.x) vMax.x = pVertex->position[ 0];
						if( pVertex->position[ 1] > vMax.y) vMax.y = pVertex->position[ 1];
						if( pVertex->position[ 2] > vMax.z) vMax.z = pVertex->position[ 2];
					
						pVertex ++;
					}
					
					m_SubMesh[ i].m_pResMesh->vertex_ptr->Unlock();
				}
			}
			else
			{
				VT_Normal *pVertex;

				if( !FAILED( m_SubMesh[ i].m_pResMesh->vertex_ptr->Lock( 0, 0, (VOID**)&pVertex, 0)) )
				{
					for(int j = 0; j < m_SubMesh[ i].m_pResMesh->vertex_count; j++)
					{
						if( pVertex->pos.x < vMin.x) vMin.x = pVertex->pos.x;
						if( pVertex->pos.y < vMin.y) vMin.y = pVertex->pos.y;
						if( pVertex->pos.z < vMin.z) vMin.z = pVertex->pos.z;

						if( pVertex->pos.x > vMax.x) vMax.x = pVertex->pos.x;
						if( pVertex->pos.y > vMax.y) vMax.y = pVertex->pos.y;
						if( pVertex->pos.z > vMax.z) vMax.z = pVertex->pos.z;
						
						pVertex ++;
					}

					m_SubMesh[ i].m_pResMesh->vertex_ptr->Unlock();
				}
			}
		}

		m_MeshCenter = (vMin + vMax) / 2;
		m_MeshCenter.y = 0;

		return TRUE;
	}

	BOOL CCharMesh::SetLodLevel(float fLevel)
	{
		for(int i=0; i<m_nSubMesh; i++)
		{
			BOOL bOK = m_SubMesh[i].SetLodLevel( fLevel );
			if( !bOK ) return FALSE;
		}

		return TRUE;
	}

	/*
	BOOL CCharMesh::IsValid()
	{
		if( m_pResMesh == NULL ) return FALSE;
		else return TRUE;
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

	CCharTexture::CCharTexture(Res_CharTexture *pTexture)
	{
		Create( pTexture );
	}

	CCharTexture::CCharTexture()
	{
		m_pResTexture = NULL;

		for(int i=0; i<MAX_CHAR_SUBMESH; i++)
            m_pTexture[i] = NULL;
	}

	CCharTexture::~CCharTexture()
	{
		Clear();
	}

	BOOL CCharTexture::Clear()// 굳이 Texture를 Release할 필요 없음
	{
		m_pResTexture = NULL;

		return TRUE;
	}

	BOOL CCharTexture::Create(Res_CharTexture* pTexture)
	{
		Clear();

		m_pResTexture = pTexture;

		for(int i=0; i<m_pResTexture->texture_sub_count; i++)
			m_pTexture[i] = XiahPak::GetTexture( m_pResTexture->texture_sub_ptr[i].texture_id );

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

	CCharAnimation::CCharAnimation(Res_Skeleton* pSkeleton,Res_Animation* pAnimation)
	{
		Create( pSkeleton, pAnimation );
	}

	CCharAnimation::CCharAnimation()
	{
		Clear();
	}

	CCharAnimation::~CCharAnimation()
	{
		Clear();
	}

	// Root Bone의 움직임을 읽는다
	Matrix4x4 CCharAnimation::GetRootDelta(float pre_frame,float frame)
	{
		Matrix4x4 PreFrameDelta;
		Matrix4x4 FrameDelta;

		Bone_AniController* pAniCon = &m_pResAnimation->animation_ptr->bone_anicontroller_ptr[0];
		int j;

		// Pos Key
		Bone_PosKey *prePosKey = &pAniCon->poskey_ptr[0];
		Bone_PosKey *nextPosKey = NULL;

		// 다음 Ani Key를 찾는다
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

		// 모션블렌딩을 위한 interpolation
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

		return FrameDelta * PreFrameDelta.GetInverse();
	}

	BOOL CCharAnimation::BuildMatrix_Scale(Vector3 scale,float pre_frame,float frame,Matrix4x4* pMatrixList,Matrix4x4* pRootMatrix,Matrix4x4* pEffectMatrixList,float fLocalAngle,Matrix4x4* pLocalCenterTM,CTriggerList* pTriggerList,BOOL bOnlyPositionUpdate)
	{
		if( m_pResSkeleton == NULL || m_pResAnimation == NULL ) return FALSE;
		
		Matrix4x4 LocalScale;
		Matrix4x4 LocalRotateTM;
		Matrix4x4 FrameDelta;
		if( pRootMatrix)
			FrameDelta = GetRootDelta( pre_frame, frame);

		if( pTriggerList)
		{
			int r = pTriggerList->Invoke(eTE_CharRender_TMUpdate, (unsigned long)&FrameDelta);

			if( r != 0 && r != -1)	// Client가 Animation를 새로 세팅했다!!
				return TRUE;
		}
		
		if( bOnlyPositionUpdate && pRootMatrix)
		{
			*pRootMatrix = FrameDelta * (*pRootMatrix); 
			return TRUE;	// 앗싸
		}
		
		LocalScale.SetScale( scale);

		if( fLocalAngle != 0)
			LocalRotateTM.SetRotationY( fLocalAngle);	

		if( pLocalCenterTM)
			LocalRotateTM = (*pLocalCenterTM) * LocalRotateTM;

		// Animation Key를 만들어 준다
		if( m_pResAnimation_BlendSrc != NULL)
		{
			m_fBlend_CurFrame += ABS( frame - pre_frame);
			
			float cur_frame = m_fBlend_CurFrame;
			float alpha = ABS(m_fBlend_CurFrame - m_fBlend_StartFrame) / m_fBlend_Length;

			if( alpha > 1.0f) alpha = 1.0f;
			if( cur_frame > 1.0f) cur_frame = 1.0f;
			if( cur_frame < 0.0f) cur_frame = 0.0f;

			EvalBlendAnimation( m_pResAnimation_BlendSrc, m_pResAnimation, cur_frame, frame, alpha);

			if( alpha >= 1)
			{
				m_pResAnimation_BlendSrc = NULL;
			}
		}
		else
		{
			EvalAnimation( m_pResAnimation, frame);
		}

		Quaternion* pRotKey = m_pEvalBoneAni_Rot;
		Vector3*	pPosKey = m_pEvalBoneAni_Pos;
		unsigned char* pFlip = m_pEvalBoneAni_RotFlip;

		for(int i = 0; i < m_pResAnimation->animation_ptr->bone_count; i++, pRotKey ++, pPosKey ++, pFlip ++)
		{
			pMatrixList[ i].SetRotationQuaternion_Flip( *pRotKey, *pFlip == TRUE);
			pMatrixList[ i].t = *pPosKey;

			// 이건 젠장할 사갈때문에 넣었다
			Bone_AniController* pAniCon = &m_pResAnimation->animation_ptr->bone_anicontroller_ptr[ i];
			pMatrixList[i].m[0][0] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[0][1] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[0][2] *= pAniCon->scalekey_ptr->scale[0];

			pMatrixList[i].m[1][0] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[1][1] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[1][2] *= pAniCon->scalekey_ptr->scale[0];

			pMatrixList[i].m[2][0] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[2][1] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[2][2] *= pAniCon->scalekey_ptr->scale[0];

			if( m_pResSkeleton->bone_ptr[ i].parent_bone != 65535)
			{
				pMatrixList[ i] = pMatrixList[ i] * pMatrixList[ m_pResSkeleton->bone_ptr[ i].parent_bone];
			}
			else
			{
				pMatrixList[ i].t.x = 0;
				pMatrixList[ i].t.z = 0;
			}
		}

		//FrameDelta.t.y = 0;
		if( pRootMatrix)
		{
			*pRootMatrix = FrameDelta * (*pRootMatrix); 

			for(i = 0; i < m_pResAnimation->animation_ptr->bone_count; i++)
			{
				pEffectMatrixList[i] = pMatrixList[ i] * LocalScale * LocalRotateTM * (*pRootMatrix);
				pMatrixList[ i] = m_pResSkeleton->world_inv_matrix[ i] * pMatrixList[ i] * LocalScale * LocalRotateTM * (*pRootMatrix);
			}
		}
		else
		{
			for(i = 0; i < m_pResAnimation->animation_ptr->bone_count; i++)
			{
				pEffectMatrixList[i] = pMatrixList[ i] * LocalScale * LocalRotateTM ;
				pMatrixList[ i] = m_pResSkeleton->world_inv_matrix[ i] * pMatrixList[ i] * LocalScale * LocalRotateTM;
			}
		}

		return TRUE;
	}

	// frame별 matrix 생성
	BOOL CCharAnimation::BuildMatrix(float pre_frame,float frame,Matrix4x4* pMatrixList,Matrix4x4* pRootMatrix,Matrix4x4* pEffectMatrixList,float fLocalAngle,Matrix4x4* pLocalCenterTM,CTriggerList* pTriggerList,BOOL bOnlyPositionUpdate) // 전체 Animation길이를 1로 봤을때의 시간 0 ~ 1사이의 값
	{
		if( m_pResSkeleton == NULL || m_pResAnimation == NULL ) return FALSE;
		
		Matrix4x4 LocalRotateTM;
		Matrix4x4 FrameDelta;

		if( pRootMatrix)
			FrameDelta = GetRootDelta( pre_frame, frame);

		if( pTriggerList)
		{
			int r = pTriggerList->Invoke(eTE_CharRender_TMUpdate, (unsigned long)&FrameDelta);
			if( r != 0 && r != -1)	// Client가 Animation를 새로 세팅했다!!
				return TRUE;
		}

		if( bOnlyPositionUpdate && pRootMatrix)
		{
			*pRootMatrix = FrameDelta * (*pRootMatrix); 
			return TRUE;	// 앗싸
		}

		if( fLocalAngle != 0)
			LocalRotateTM.SetRotationY( fLocalAngle);	

		if( pLocalCenterTM)
			LocalRotateTM = (*pLocalCenterTM) * LocalRotateTM;

		// Animation Key를 만들어 준다
		if( m_pResAnimation_BlendSrc != NULL)
		{
			m_fBlend_CurFrame += ABS( frame - pre_frame);
			
			float cur_frame = m_fBlend_CurFrame;
			float alpha = ABS(m_fBlend_CurFrame - m_fBlend_StartFrame) / m_fBlend_Length;

			if( alpha > 1)
				alpha = 1;

			if( cur_frame > 1)
				cur_frame = 1;

			if( cur_frame < 0)
				cur_frame = 0;

			EvalBlendAnimation( m_pResAnimation_BlendSrc, m_pResAnimation, cur_frame, frame, alpha);

			if( alpha >= 1)
			{
				m_pResAnimation_BlendSrc = NULL;
			}
		}
		else
		{
			EvalAnimation( m_pResAnimation, frame);
		}

		Quaternion* pRotKey = m_pEvalBoneAni_Rot;
		Vector3*	pPosKey = m_pEvalBoneAni_Pos;
		unsigned char* pFlip = m_pEvalBoneAni_RotFlip;

		for(int i = 0; i < m_pResAnimation->animation_ptr->bone_count; i++, pRotKey ++, pPosKey ++, pFlip ++)
		{
			pMatrixList[ i].SetRotationQuaternion_Flip( *pRotKey, *pFlip == TRUE);
			pMatrixList[ i].t = *pPosKey;

			// 이건 젠장할 사갈때문에 넣었다
			Bone_AniController* pAniCon = &m_pResAnimation->animation_ptr->bone_anicontroller_ptr[ i];
			pMatrixList[i].m[0][0] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[0][1] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[0][2] *= pAniCon->scalekey_ptr->scale[0];

			pMatrixList[i].m[1][0] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[1][1] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[1][2] *= pAniCon->scalekey_ptr->scale[0];

			pMatrixList[i].m[2][0] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[2][1] *= pAniCon->scalekey_ptr->scale[0];
			pMatrixList[i].m[2][2] *= pAniCon->scalekey_ptr->scale[0];

			if( m_pResSkeleton->bone_ptr[ i].parent_bone != 65535)
			{
				pMatrixList[ i] = pMatrixList[ i] * pMatrixList[ m_pResSkeleton->bone_ptr[ i].parent_bone];
			}
			else
			{
				pMatrixList[ i].t.x = 0;
				pMatrixList[ i].t.z = 0;
			}
		}

		//FrameDelta.t.y = 0;
		if( pRootMatrix)
		{
			*pRootMatrix = FrameDelta * (*pRootMatrix); 

			for(i = 0; i < m_pResAnimation->animation_ptr->bone_count; i++)
			{
				pEffectMatrixList[i] = pMatrixList[ i] * LocalRotateTM * (*pRootMatrix);
				pMatrixList[ i] = m_pResSkeleton->world_inv_matrix[ i] * pMatrixList[ i] * LocalRotateTM * (*pRootMatrix);
			}
		}
		else
		{
			for(i = 0; i < m_pResAnimation->animation_ptr->bone_count; i++)
			{
				pEffectMatrixList[i] = pMatrixList[ i] * LocalRotateTM ;
				pMatrixList[ i] = m_pResSkeleton->world_inv_matrix[ i] * pMatrixList[ i] * LocalRotateTM;
			}
		}

		return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL	CCharAnimation::EvalAnimation(Res_Animation *pAnimation,float cur_frame)
	{
		DBG_Assert( pAnimation != NULL);

		int i;
		int j;
		int bone_count = pAnimation->animation_ptr->bone_count;
		Bone_RotKey* preRotKey = NULL;
		Bone_RotKey* nextRotKey = NULL;
		Bone_RotKey* tempRotKey = NULL;
		Bone_PosKey* prePosKey = NULL;
		Bone_PosKey* nextPosKey = NULL;
		Bone_PosKey* tempPosKey = NULL;
		Bone_AniController* pAniCon = NULL;
		float alpha = 0.0f;
	
		m_pEvalBoneAni_Rot	= ((Quaternion *)g_TempFaceBuffer);
		m_pEvalBoneAni_Pos	= (Vector3 *)(m_pEvalBoneAni_Rot + bone_count);
		m_pEvalBoneAni_RotFlip = (unsigned char*)(m_pEvalBoneAni_Pos + bone_count);
		unsigned char* pFlip = m_pEvalBoneAni_RotFlip;
	
		Quaternion* pRot = m_pEvalBoneAni_Rot;
		Vector3*	pPos = m_pEvalBoneAni_Pos;

		Quaternion		EvalRotKey[ 2];
		unsigned char	EvalRotFlip[ 2] = {0,};	// 1이면 flip이다
		Vector3			EvalPosKey[ 2];

		for(i = 0; i < bone_count; i++, pRot++, pPos++, pFlip++)
		{
			pAniCon = &pAnimation->animation_ptr->bone_anicontroller_ptr[ i];
			if(pAniCon == NULL) continue;

			preRotKey = &pAniCon->rotkey_ptr[ 0];
			nextRotKey = NULL;

			for(j = 1; j < pAniCon->rotkey_count; j++)
			{
				tempRotKey = &pAniCon->rotkey_ptr[ j];
				if( cur_frame < tempRotKey->frame)
				{
					nextRotKey = tempRotKey;
					break;
				}
				else
				{
					if(tempRotKey)
						preRotKey = tempRotKey;
				}
			}

			// [3/28/2005] 촉음(뱀)에서 어쩌다 Bone_RotKey 깨져버린다.
			if(!IsBadReadPtr(preRotKey, 24)) //preRotKey != NULL)
			{
				if(!IsBadReadPtr(nextRotKey, 24)) //nextRotKey)
				{
					// [3/28/2005] 0 나누기가 가능하다. 실제로 0나누기가 일어났다.
					float fTemp = nextRotKey->frame - preRotKey->frame;
					if(fTemp)
						alpha = (cur_frame - preRotKey->frame) / fTemp; //(nextRotKey->frame - preRotKey->frame);

					Quaternion q1( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);\
					Quaternion q2( nextRotKey->quaternion[0], nextRotKey->quaternion[1], nextRotKey->quaternion[2], nextRotKey->quaternion[3]);\

					EvalRotKey[ 0] = q1.Slerp( q2, alpha );
					EvalRotFlip[ 0] = preRotKey->flip < 0;
				}
				else
				{
					EvalRotKey[ 0] = Quaternion( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);
					EvalRotFlip[ 0] = preRotKey->flip < 0;
				}
			}
			else
			{
				return FALSE;
			}

			prePosKey = &pAniCon->poskey_ptr[ 0];
			nextPosKey = NULL;

			for(j = 1; j < pAniCon->poskey_count; j++)
			{
				tempPosKey = &pAniCon->poskey_ptr[ j];
				if( cur_frame < tempPosKey->frame)
				{
					nextPosKey = tempPosKey;
					break;
				}
				else
				{
					prePosKey = tempPosKey;
				}
			}
			
			// [3/28/2005] 촉음(뱀)에서 어쩌다 Bone_RotKey 깨져버린다.
			if(!IsBadReadPtr(preRotKey, 24)) //prePosKey != NULL)
			{
				if(!IsBadReadPtr(nextPosKey, 24)) //nextPosKey)
				{
					// [3/28/2005] 0 나누기가 가능하다. 실제로 0나누기가 일어났다.
					//float alpha = (double)(cur_frame - prePosKey->frame) / (double)(nextPosKey->frame - prePosKey->frame);
					float fTemp = nextPosKey->frame - prePosKey->frame;
					if(fTemp)
						alpha = (cur_frame - prePosKey->frame) / fTemp;

					Vector3 pos1( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
					Vector3 pos2( nextPosKey->position[0], nextPosKey->position[1], nextPosKey->position[2] );

					EvalPosKey[ 0] = pos1 * ( 1 - alpha) + pos2 * alpha;
				}
				else
				{
					EvalPosKey[ 0] = Vector3( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
				}
			}
			else
			{
				return FALSE;
			}

			*pRot = EvalRotKey[ 0];
			*pPos = EvalPosKey[ 0];
			*pFlip = EvalRotFlip [ 0];
		}

		return TRUE;
	}

	BOOL	CCharAnimation::EvalBlendAnimation(Res_Animation *pSrc,Res_Animation *pDest,float srcframe,float dstframe,float blend_factor)
	{
		DBG_Assert( pSrc != NULL && pDest != NULL);
		int i;
		int j;
		int bone_count = pSrc->animation_ptr->bone_count;
		Bone_RotKey* preRotKey = NULL;
		Bone_RotKey* nextRotKey = NULL;
		Bone_RotKey* tempRotKey = NULL;
		Bone_PosKey* prePosKey = NULL;
		Bone_PosKey* nextPosKey = NULL;
		Bone_PosKey* tempPosKey = NULL;
		Bone_AniController* pSrcAniCon = NULL;
		Bone_AniController* pDstAniCon = NULL;
		float alpha = 0.0f;

		/// 메모리를 줄여본다 (Single Thread니까 되는거다)
		/// Blending을 위해서 index는 0또는 1이되겠다 (저장할 공간 index이다)
		m_pEvalBoneAni_Rot	= ((Quaternion *)g_TempFaceBuffer);
		m_pEvalBoneAni_Pos	= (Vector3 *)(m_pEvalBoneAni_Rot + bone_count);
		m_pEvalBoneAni_RotFlip = (unsigned char*)(m_pEvalBoneAni_Pos + bone_count);
		
		Quaternion* pRot = m_pEvalBoneAni_Rot;
		Vector3*	pPos = m_pEvalBoneAni_Pos;
		unsigned char* pFlip = m_pEvalBoneAni_RotFlip;

		Quaternion		EvalRotKey[ 2];
		unsigned char	EvalRotFlip[ 2] = {0,};	// 1이면 flip이다
		Vector3			EvalPosKey[ 2];

		for(i = 0; i < bone_count; i++, pRot++, pPos++, pFlip++)
		{
			pSrcAniCon = &pSrc->animation_ptr->bone_anicontroller_ptr[ i];
			pDstAniCon = &pDest->animation_ptr->bone_anicontroller_ptr[ i];

			if(pSrcAniCon == NULL || pDstAniCon == NULL ) continue;

			/*//////////////////////////////////////////////////////////////////////////
				SOURCE KEY의 ROT , POS KEY
			//////////////////////////////////////////////////////////////////////////*/

			preRotKey = &pSrcAniCon->rotkey_ptr[ 0];
			nextRotKey = NULL;

			for(j = 1; j < pSrcAniCon->rotkey_count; j++)
			{
				tempRotKey = &pSrcAniCon->rotkey_ptr[ j];
				if( srcframe < tempRotKey->frame)
				{
					nextRotKey = tempRotKey;
					break;
				}
				else
				{
					if(!IsBadReadPtr(tempRotKey, 24))
						preRotKey = tempRotKey;
				}
			}
			
			// [3/28/2005] 촉음(뱀)에서 어쩌다 Bone_RotKey 깨져버린다.
			if(!IsBadReadPtr(preRotKey, 24)) //preRotKey != NULL)
			{
				if(!IsBadReadPtr(nextRotKey, 24)) //nextRotKey)
				{
					// [3/28/2005] 0 나누기가 가능하다. 실제로 0나누기가 일어났다.
					float fTemp = nextRotKey->frame - preRotKey->frame;
					if(fTemp)
						alpha = (srcframe - preRotKey->frame) / fTemp; //(nextRotKey->frame - preRotKey->frame);

					Quaternion q1( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);
					Quaternion q2( nextRotKey->quaternion[0], nextRotKey->quaternion[1], nextRotKey->quaternion[2], nextRotKey->quaternion[3]);

					EvalRotKey[ 0] = q1.Slerp( q2, alpha );
					EvalRotFlip[ 0] = preRotKey->flip < 0;
				}
				else
				{
					EvalRotKey[0].x = preRotKey->quaternion[ 0];
					EvalRotKey[0].y = preRotKey->quaternion[ 1];
					EvalRotKey[0].z = preRotKey->quaternion[ 2];
					EvalRotKey[0].w = preRotKey->quaternion[ 3];

					//EvalRotKey[ 0] = Quaternion( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);
					EvalRotFlip[ 0] = preRotKey->flip < 0;
				}
			}
			else
			{
				return FALSE;
			}
			
			prePosKey = &pSrcAniCon->poskey_ptr[ 0];
			nextPosKey = NULL;
			
			for(j = 1; j < pSrcAniCon->poskey_count; j++)
			{
				tempPosKey = &pSrcAniCon->poskey_ptr[ j];
				if( srcframe < tempPosKey->frame)
				{
					nextPosKey = tempPosKey;
					break;
				}
				else
					prePosKey = tempPosKey;
			}
			
			if( prePosKey && nextPosKey)
			{
				float alpha = (double)(srcframe - prePosKey->frame) / (double)(nextPosKey->frame - prePosKey->frame);
				Vector3 pos1( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
				Vector3 pos2( nextPosKey->position[0], nextPosKey->position[1], nextPosKey->position[2] );
				
				EvalPosKey[ 0] = pos1 * ( 1 - alpha) + pos2 * alpha;
			}
			else
			{
				EvalPosKey[ 0] = Vector3( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
			}

			//////////////////////////////////////////////////////////////////////////

			/*//////////////////////////////////////////////////////////////////////////
				DEST KEY의 ROT , POS KEY
			//////////////////////////////////////////////////////////////////////////*/
			// 본의 rotate Key
			preRotKey = &pDstAniCon->rotkey_ptr[ 0];
			nextRotKey = NULL;

			// Rotate Key의 갯수만큼 반복
			for(j = 1; j < pDstAniCon->rotkey_count; j++)
			{
				// 새로운 rotate 키를 받고
				tempRotKey = &pDstAniCon->rotkey_ptr[ j];

				if( dstframe < tempRotKey->frame)
				{
					// 만일 destframe 보다 tempRotKey의 프레임이 크면 이것으로 변환하도록 지정
					nextRotKey = tempRotKey;
					break;
				}
				else
				{
					if(!IsBadReadPtr(tempRotKey, 24)) //if(NULL != tempRotKey)
					{
						preRotKey = tempRotKey;
					}
				}
			}
			
			// [3/28/2005] 촉음(뱀)에서 어쩌다 Bone_RotKey 깨져버린다.
			if(!IsBadReadPtr(preRotKey, 24)) //preRotKey != NULL)
			{
				if(!IsBadReadPtr(nextRotKey, 24)) //if(nextRotKey)
				{
					// [3/28/2005] 0 나누기가 가능하다. 실제로 0나누기가 일어났다.
					float fTemp = nextRotKey->frame - preRotKey->frame;
					if(fTemp)
						alpha = (dstframe - preRotKey->frame) / fTemp; //(nextRotKey->frame - preRotKey->frame);

					Quaternion q1( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);
					Quaternion q2( nextRotKey->quaternion[0], nextRotKey->quaternion[1], nextRotKey->quaternion[2], nextRotKey->quaternion[3]);

					EvalRotKey[ 1] = q1.Slerp( q2, alpha );
					EvalRotFlip[ 1] = preRotKey->flip < 0;
				}
				else
				{
					// debug용
					EvalRotKey[1].x = preRotKey->quaternion[ 0];
					EvalRotKey[1].y = preRotKey->quaternion[ 1];
					EvalRotKey[1].z = preRotKey->quaternion[ 2];
					EvalRotKey[1].w = preRotKey->quaternion[ 3];

					//EvalRotKey[ 1] = Quaternion( preRotKey->quaternion[ 0], preRotKey->quaternion[ 1], preRotKey->quaternion[ 2], preRotKey->quaternion[ 3]);
					EvalRotFlip[ 1] = preRotKey->flip < 0;
				}
			}
			else
			{
				return FALSE;
			}
			
			// 본의 POSITION Key
			prePosKey = &pDstAniCon->poskey_ptr[ 0];
			nextPosKey = NULL;
			
			for(j = 1; j < pDstAniCon->poskey_count; j++)
			{
				tempPosKey = &pDstAniCon->poskey_ptr[ j];
				if( dstframe < tempPosKey->frame)
				{
					nextPosKey = tempPosKey;
					break;
				}
				else
				{
					if(NULL != tempPosKey)
					{
						prePosKey = tempPosKey;
					}
				}
			}
			
			if( prePosKey && nextPosKey)
			{
				float alpha = (double)(dstframe - prePosKey->frame) / (double)(nextPosKey->frame - prePosKey->frame);
				Vector3 pos1( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
				Vector3 pos2( nextPosKey->position[0], nextPosKey->position[1], nextPosKey->position[2] );
			
				EvalPosKey[ 1] = pos1 * ( 1 - alpha) + pos2 * alpha;
			}
			else
			{
				// 이상함.
				if(NULL == prePosKey) return FALSE;

				EvalPosKey[ 1] = Vector3( prePosKey->position[0], prePosKey->position[1], prePosKey->position[2] );
			}

			//////////////////////////////////////////////////////////////////////////
			

			// 만들어진 두개의 key를 Blending시켜 준다
			*pRot = EvalRotKey[ 0].Slerp( EvalRotKey[ 1], blend_factor);
			*pPos = EvalPosKey[ 0] * ( 1 - blend_factor) + EvalPosKey[ 1] * blend_factor;
			*pFlip = EvalRotFlip [ 0];
		}
	
		return TRUE;
	}	

	BOOL CCharAnimation::SetBlend(Res_Animation* pAnimation,float fStartFrame,float fLength)
	{
		m_pResAnimation_BlendSrc = pAnimation;
		m_fBlend_StartFrame		 = fStartFrame;
		m_fBlend_Length			 = fLength;
		m_fBlend_CurFrame		 = fStartFrame;
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
	//////////////////////////////////////////////////////////////////////////	REAL SHADOW

	XIAHGE_API cShadow::cShadow()
	{
		m_bDetectSize = FALSE;
		Create();
	}

	XIAHGE_API cShadow::~cShadow()
	{
		Destroy();
	}

	XIAHGE_API BOOL cShadow::Create()
	{
		// Create Renderable Texture
		D3DXCreateTexture(
							g_pDirect3DDevice, 
							128, 128,
							1,
							D3DUSAGE_RENDERTARGET,
							D3DFMT_A8R8G8B8,
							//g_EngineInfo.m_nFormat, 
							D3DPOOL_DEFAULT,
							&m_pShadowTexture);

		D3DSURFACE_DESC desc;
		m_pShadowTexture->GetSurfaceLevel(0, &m_pShadowSurface);
		m_pShadowSurface->GetDesc( &desc );

		D3DXCreateRenderToSurface(
								g_pDirect3DDevice, 
								desc.Width, 
								desc.Height, 
								desc.Format, 
								TRUE, 
								D3DFMT_D16, 
								&m_pRenderToSurface );
								/*
								128,128,
								g_EngineInfo.m_nFormat,
								FALSE,
								D3DFMT_UNKNOWN,
								&m_pRenderToSurface);
								*/


		return TRUE;
	}

	XIAHGE_API BOOL cShadow::Destroy()
	{
		m_pRenderToSurface->Release();
		m_pShadowTexture->Release();
		m_pShadowSurface->Release();
		return TRUE;
	}

	XIAHGE_API BOOL cShadow::Detect(LPDIRECT3DVERTEXBUFFER9 buf, int numVertex, int numFace)
	{
		// 메시가 텍스처에 들어가도록 지정

		m_numvertex = numVertex;
		m_numfaces = numFace;

		D3DXVECTOR3 Mesh( 1.0f,1.0f, 0.0f);		// 렌더 객체의 위치
		D3DXVECTOR3 Light (0.0f,2.0f,0.0f);	// Shdow Light위치

		D3DXMatrixTranslation( &matLocalToWorld, Mesh.x, Mesh.y, Mesh.z );
		D3DXMatrixLookAtLH( &matWorldToLight, &Light, &Mesh, &D3DXVECTOR3(0.0f,1.0f,0.0f));


		float fXMax = 0.0f; //최대값을 구할것이므로 0으로 시작하면 된다.
		float fYMax = 0.0f;

		// 객체가 텍스쳐에 딱 들어오도록 수평수직 FOV를 계산한다.
		// [그림자 객체 좌표] 들을 [광원 좌표]로 변환한다음 적당한 FOV를 계산해냄.

		DWORD fvfsize  = sizeof(SkinVertex);	// D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1

		int i;

		BYTE *pbPoints,*pbCur;
		D3DXVECTOR3 *pvCur;
		D3DXVECTOR4 VOut;

		m_pVertexBuffer = buf;

		void *pVertices = NULL;
		D3DXMATRIX matLocalToLight; // [그림자 객체 좌표] -> [광원 좌표] 행렬
		D3DXMatrixMultiply( &matLocalToLight, &matLocalToWorld, &matWorldToLight );

		buf->Lock(0,0 ,(void **)&pbPoints,0);
		for( i = 0, pbCur = pbPoints ; i < m_numvertex ; i++, pbCur += fvfsize)
		{	
			pvCur = (D3DXVECTOR3*)pbCur;
			D3DXVec3Transform( &VOut, pvCur, &matLocalToLight );

			if((float)fabs(VOut.x/VOut.z) > fXMax) fXMax = (float)fabs(VOut.x/VOut.z);
			if((float)fabs(VOut.y/VOut.z) > fYMax) fYMax = (float)fabs(VOut.y/VOut.z);
		}

		buf->Unlock();

		//	fXMax = 0.21715602;
		//	fYMax = 0.26048914f;

		D3DXMatrixPerspectiveFovLH( &matShadowProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
		matShadowProj.m[0][0] = 0.98f/fXMax;
		matShadowProj.m[1][1] = 0.98f/fYMax;

		return TRUE;
	}

	XIAHGE_API BOOL cShadow::Render(LPDIRECT3DINDEXBUFFER9	Index)
	{

		m_pRenderToSurface->BeginScene(m_pShadowSurface,NULL);
		g_pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET , D3DCOLOR_ARGB(0,255,255,255), 0, 0 );

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

		// TEXTUREFACTOR 를 색상으로 사용해서 단색 렌더링을 할것임.
		g_Device.SetTexture(0, NULL);
		//g_pDirect3DDevice->SetTexture(0,NULL);
		g_pDirect3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255,1,1,1) );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		// 변환 행렬 설정.
		// [그림자 객체 좌표] -> [월드좌표] -> [광원좌표] -> [텍스쳐 좌표]
		g_pDirect3DDevice->SetTransform( D3DTS_WORLD, &matLocalToWorld );
		g_pDirect3DDevice->SetTransform( D3DTS_VIEW, &matWorldToLight );
		g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, &matShadowProj );

		// 렌더링.
		g_pDirect3DDevice->SetSoftwareVertexProcessing(TRUE);
		g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );
		g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
		g_Device.SetStreamSource( m_pVertexBuffer, sizeof(SkinVertex) );
		g_Device.SetFVF(FVF_SKINVERTEX);
		//g_pDirect3DDevice->SetFVF(FVF_SKINVERTEX);
		g_Device.SetIndices(Index);
		g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_numvertex, 0, m_numfaces);

		m_pRenderToSurface->EndScene(0);

		//  그림자 맵이 제대로 만들어졌는지 테스트 출력.
		float vertex2D[4][4+2] = 
		{
			{   0,  0,0,1, 0,0 },
			{ 128,  0,0,1, 1,0 },
			{ 128,128,0,1, 1,1 },
			{   0,128,0,1, 0,1 },
		};
		
		g_Device.SetTexture(0, m_pShadowTexture);
		//g_pDirect3DDevice->SetTexture(0,m_pShadowTexture);
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_Device.SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		//g_pDirect3DDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		g_pDirect3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertex2D,sizeof(float)*(4+2));

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

	XIAHGE_API CCharRender::CCharRender()
	{
		m_pResCharacter = NULL;
		m_pMatrixList = NULL;
		m_pEffectMatrixList = NULL;
		m_pCharTM = NULL;
		m_CurFrame = 0;
		m_PreFrame = 0;
		m_pEffectPackagePair = NULL;
		m_pMeshEffectPackagePair = NULL;
		m_bSpawnEffect = false;

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

		m_LocalCenterTM.Identity();

		m_TriggerList.SetSize( eTE_CharRender_Count);

		m_bAlphaEffect = FALSE;
		m_fAlphaEffect = 1.0f;
		m_fAlphaEffectSpeed = 1.0f;
		m_bSpecularEffect = FALSE;

		m_nAnimationSound = 0;
		for(int i=0; i<MAX_ANIMATION_SOUND;i++)
			m_pAnimationSoundBufferList[i] = NULL;

		m_RightFoot = FALSE;
		m_LeftFoot = FALSE;

		m_nMeshEffectIndex = -1;

		m_bApplyMaterialColor = FALSE;

		m_bEnableAnimationSound = true;
	}

	XIAHGE_API CCharRender::~CCharRender()
	{
		Clear();
	}

	//YS_0728 : BUGFIX
	XIAHGE_API void CCharRender::Init()
	{
		//m_fDetailLevel = 1.0f;
		m_bVisible = FALSE;
		m_bNeedUpdate = FALSE;

		m_pResCharacter = NULL;
		m_pMatrixList = NULL;
		m_pEffectMatrixList = NULL;
		m_pCharTM = NULL;
		m_CurFrame = 0;
		m_PreFrame = 0;
		m_pEffectPackagePair = NULL;
		m_pMeshEffectPackagePair = NULL;
		m_bSpawnEffect = false;

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

		m_LocalCenterTM.Identity();

		//m_TriggerList.SetSize( eTE_CharRender_Count);

		m_bAlphaEffect = FALSE;
		m_fAlphaEffect = 1.0f;
		m_fAlphaEffectSpeed = 1.0f;
		m_bSpecularEffect = FALSE;

		m_nAnimationSound = 0;
		for(int i=0; i<MAX_ANIMATION_SOUND;i++)
			m_pAnimationSoundBufferList[i] = NULL;

		m_RightFoot = FALSE;
		m_LeftFoot = FALSE;
	}

	XIAHGE_API BOOL CCharRender::Clear()
	{
		if( m_pMatrixList)
			delete []m_pMatrixList;
		m_pMatrixList = NULL;

		if( m_pEffectMatrixList)
			delete []m_pEffectMatrixList;
		m_pEffectMatrixList = NULL;

//		m_pResCharacter->Release(); 이걸 하면 데이타가 없어짐.
		m_pResCharacter = NULL;
		
		m_bSpawnEffect		= FALSE;
		m_bAlphaEffect		= FALSE;
		m_bSpecularEffect	= FALSE;
		m_bGlowEffect		= FALSE;

		m_nMeshEffectIndex = -1;

		m_LocalScale = Vector3(1.0f, 1.0f, 1.0f);

		ClearMeshEffect();

		StopEffect();
		ReleaseAnimationSound();

		//YS_0728 : BUGFIX
		//m_fDetailLevel	= 1.0f;
		m_bVisible		= FALSE;
		m_bNeedUpdate	= FALSE;
		SetRenderType();

		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::ClearMeshEffect()
	{
		// Mesh Effect
		if( m_pMeshEffectPackagePair )
		{
			g_EffectManager.DeqEffectPackagePair( m_pMeshEffectPackagePair );
			m_pMeshEffectPackagePair = NULL;
		}

		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::SetChar(int char_id)
	{
		Clear();

		m_pResCharacter = GetCharacter( char_id );

		if( m_pResCharacter == NULL ) return FALSE;
		else return TRUE;
	}

	XIAHGE_API BOOL CCharRender::SetMesh(int mesh_type,int texture_type)
	{
		DBG_Assert( m_pResCharacter != NULL);	// 캐릭이 세팅된 상태에서만.

		Res_Mesh* pMesh = m_pResCharacter->GetMesh( mesh_type);
		if( pMesh == NULL ) return FALSE;

		Res_CharTexture* pTexture = pMesh->GetTexture( texture_type);
		if( pTexture == NULL ) return FALSE;

		m_Mesh.Create( pMesh);
		m_Texture.Create( pTexture);

		// 미리 만들자.
		m_pMatrixList = new Matrix4x4[ pMesh->skeleton_ptr->bone_count ];
		m_pEffectMatrixList = new Matrix4x4[ pMesh->skeleton_ptr->bone_count ];

		m_LocalCenterTM.Identity();
		if( m_Mesh.IsValid())
		{
			if( pMesh->skeleton_ptr)
			{
				if( pMesh->skeleton_ptr->bone_count >0)
				{
					if( pMesh->skeleton_ptr->world_matrix[ 0].t.z != 0)
					{
//						m_LocalCenterTM.t.z = (float)pMesh->mesh_size[ 2] / 2 - pMesh->skeleton_ptr->world_matrix[ 0].t.z;
//						m_LocalCenterTM.t.z = pMesh->skeleton_ptr->world_matrix[ 0].t.z;
						m_LocalCenterTM.t = Vector3( pMesh->skeleton_ptr->world_matrix[ 0].t.x, 0, pMesh->skeleton_ptr->world_matrix[ 0].t.z)
									-m_Mesh.m_MeshCenter;
						//m_LocalCenterTM.t.z /= 2;
					}
				}
			}// if
		}// if

		//EnableSpecularEffect();
		m_Animation.Clear();

		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::MakeMeshEffect(int nIndex)
	{
		Res_Mesh* pMesh = m_Mesh.GetResMesh();
		if( pMesh == NULL ) return FALSE;

		if( pMesh->effect_count > 0 && pMesh->effect_count >= nIndex+1 )
		{
			m_nMeshEffectIndex = nIndex;

			// 이펙트를 굳이 이름을 비교하지 않아도 될것 같음.
			// Character Studio2에서 등록할때 순서대로 등록을 하면 패키지 할때에도 순서대로 들어가니깐.
			g_EffectManager.MakeSharedPackagePair( 0, 0, 0 );

			int nEffectID = pMesh->effect_ptr[nIndex].nEffectID;
			int nPosX = pMesh->effect_ptr[nIndex].nPosX;
			int nPosY = pMesh->effect_ptr[nIndex].nPosY;
			int nPosZ = pMesh->effect_ptr[nIndex].nPosZ;
			int nBoneIndex = pMesh->effect_ptr[nIndex].nBoneIndex;
			int nStartTime = pMesh->effect_ptr[nIndex].nStartTime;

			_EFFECT* pEffect = g_EffectManager.GetEffect( nEffectID );
			_EFFECTPACKAGE* pPackage = g_EffectManager.EnqEffectImmediately(pEffect, nStartTime, nPosX, nPosY, nPosZ, nBoneIndex );

			m_pMeshEffectPackagePair = g_EffectManager.GetCurEffectPackagePair();
#ifdef TRACE_LOG
			if(m_pMeshEffectPackagePair == NULL)
			{
				DBG_LogFile( _T("CCharRender::MakeMeshEffect2 fail"));
			}
#endif
			g_EffectManager.OffSharedPackagePair();
		}// if

		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::MakeMeshEffect(int nCount, int *IndexAry)
	{
		// 메시 이펙트를 여러개 뿌릴때 쓴다.
		Res_Mesh* pMesh = m_Mesh.GetResMesh();
		if( pMesh == NULL ) return FALSE;

		if( pMesh->effect_count > 0 ) // && pMesh->effect_count >= nIndex+1 )
		{
			m_nMeshEffectIndex = IndexAry[0];

			// 이펙트를 굳이 이름을 비교하지 않아도 될것 같음.
			// Character Studio2에서 등록할때 순서대로 등록을 하면 패키지 할때에도 순서대로 들어가니깐.
			g_EffectManager.MakeSharedPackagePair( 0, 0, 0 );

			for(int i=0; i<nCount; i++)
			{
				int nIndex = IndexAry[i];

				// 먼저 현재 인덱스가 유효한가?
				if( nIndex >= 0 && nIndex < pMesh->effect_count )
				{
					int nEffectID = pMesh->effect_ptr[nIndex].nEffectID;
					int nPosX = pMesh->effect_ptr[nIndex].nPosX;
					int nPosY = pMesh->effect_ptr[nIndex].nPosY;
					int nPosZ = pMesh->effect_ptr[nIndex].nPosZ;
					int nBoneIndex = pMesh->effect_ptr[nIndex].nBoneIndex;
					int nStartTime = pMesh->effect_ptr[nIndex].nStartTime;

					_EFFECT* pEffect = g_EffectManager.GetEffect( nEffectID );
					if( pEffect )
						_EFFECTPACKAGE* pPackage = g_EffectManager.EnqEffectImmediately(pEffect, nStartTime, nPosX, nPosY, nPosZ, nBoneIndex );

				}// if
			}// for

			m_pMeshEffectPackagePair = g_EffectManager.GetCurEffectPackagePair();
#ifdef TRACE_LOG
			if(m_pMeshEffectPackagePair == NULL)
			{
				DBG_LogFile( _T("CCharRender::MakeMeshEffect2 fail"));
			}
#endif
			g_EffectManager.OffSharedPackagePair();

		}// if

		return TRUE;
	}

	/*
	XIAHGE_API BOOL CCharRender::SetAnimationSpeed( float fAnimationSpeed)
	{
		m_fAnimationSpeed = fAnimationSpeed;
		return TRUE;
	}
	*/

	void CCharRender::SetTimeTrigger()
	{
		ZeroMemory( m_bTimeTrigger, sizeof( BOOL) * 10);
		if( m_Animation.m_pResAnimation == NULL)
			return;

		for(int t_index = 0; t_index < 	m_Animation.m_pResAnimation->trigger_count; t_index++)
		{
			m_bTimeTrigger[ t_index] = TRUE;
		}
	}

	XIAHGE_API float CCharRender::GetAnimationLength()
	{
		if( m_pResCharacter == NULL)
			return 1000.0f;

		if( m_Mesh.IsValid() == FALSE)
			return 1000.0f;

		if( m_Animation.m_pResAnimation == NULL)
			return 1000.0f;

		return (float)m_Animation.m_pResAnimation->animation_ptr->time_length * m_fAnimationSpeed;
	}

	XIAHGE_API BOOL CCharRender::SetAnimation(int ani_type,float fAnimationSpeed,bool bStopEffect)
	{
//		DBG_Assert( m_pResCharacter != NULL);
//		DBG_Assert( m_Mesh.IsValid() );

		//HT_TEST
		if(ani_type == 2)
			int a = 0;

		if( m_pResCharacter == NULL)
			return TRUE;

		if( m_Mesh.IsValid() == FALSE)
			return TRUE;

		if( m_pResCharacter == NULL)
		{
			DBG_LogFile( _T("m_pResCharacter 가 NULL이다 : %d"), ani_type);
		}

		if( m_Mesh.IsValid() == FALSE)
		{
			DBG_LogFile( _T("Mesh가 Invalid함"));
		}

		if( m_pResCharacter->GetAnimation( ani_type) == NULL)
		{
			if(ani_type > 0)
				DBG_LogFile( "Animation이 없음 %d, %d", m_pResCharacter->character_id, ani_type);

			return FALSE;
		}

		Res_Animation *pBlendSrcAnimation = NULL;
		float	blend_src_startframe;
		float	blend_src_length;

		if( m_Animation.m_pResAnimation != NULL)
		{
			pBlendSrcAnimation = m_Animation.m_pResAnimation;
			
			blend_src_startframe = m_CurFrame / pBlendSrcAnimation->animation_ptr->time_length;
			blend_src_length	 = 150.0f / 1000.0f;

			if( m_bReverseAnimation)
				blend_src_startframe = 1 - blend_src_startframe;
		}
		
		BOOL bOK = m_Animation.Create( m_Mesh.GetResMesh()->skeleton_ptr, m_pResCharacter->GetAnimation( ani_type));
		
		if( pBlendSrcAnimation)
			m_Animation.SetBlend( pBlendSrcAnimation, blend_src_startframe, blend_src_length);

		m_CurFrame = 0;
		m_PreFrame = 0;
		m_LastUpdateTime = g_dwCurTime;

		m_fAnimationSpeed = fAnimationSpeed;

		// effect
		if( bStopEffect ) 
			StopEffect();

		SpawnEffect();
		SpawnAnimationSound();	// Animation Sound Duplicate버퍼 만들어 주기
		SetTimeTrigger();

		m_bBoneAnimation = TRUE;
		m_bReverseAnimation = FALSE;
		m_bLoopAnimation = TRUE;

		return bOK;
	}
	
	/*
	XIAHGE_API BOOL CCharRender::SetPosition(Vector3 vPos)
	{
		if( m_pCharTM)
			m_pCharTM->t = vPos;

		return TRUE;
	}
	*/

	XIAHGE_API BOOL CCharRender::SpawnEffect()
	{
		// spawn effect of this animation
		if( m_Animation.m_pResAnimation == NULL)
		{
			m_bSpawnEffect = true;
			return TRUE;
		}

		if( m_Animation.m_pResAnimation->effect_count != 0 )
		{
			// 여러 이펙트를 하나로 묶어서 처리한다.
			g_EffectManager.MakeSharedPackagePair( 0, 0, 0 );

			for(int i=0; i<m_Animation.m_pResAnimation->effect_count; i++)
			{
				int nEffectID = m_Animation.m_pResAnimation->effect_ptr[i].nEffectID;
				int nPosX = m_Animation.m_pResAnimation->effect_ptr[i].nPosX;
				int nPosY = m_Animation.m_pResAnimation->effect_ptr[i].nPosY;
				int nPosZ = m_Animation.m_pResAnimation->effect_ptr[i].nPosZ;
				int nBoneIndex = m_Animation.m_pResAnimation->effect_ptr[i].nBoneIndex;
				int nStartTime = m_Animation.m_pResAnimation->effect_ptr[i].nStartTime;

				_EFFECT* pEffect = g_EffectManager.GetEffect( nEffectID );
				if( pEffect )
				{
					_EFFECTPACKAGE* pPackage = g_EffectManager.EnqEffectImmediately(pEffect, nStartTime, nPosX, nPosY, nPosZ, nBoneIndex );

					if( !pPackage )
					{
						// 혹시나 이펙트 메모리를 할당하다가 없으면 아예 현재 이펙트가 없도록 한다.
						DBG_LogFile( _T("CCharRender::SpawnEffect fail EffenctID : %d"), nEffectID);
						break;
					}
					else
					if( m_fAnimationSpeed != 1.0f )
					{
                        pPackage->m_LifeTimeChange = pEffect->m_LifeTime / m_fAnimationSpeed;
					}
				}
			}// for

			m_pEffectPackagePair = g_EffectManager.GetCurEffectPackagePair();
			
			g_EffectManager.OffSharedPackagePair();
		}// if

		m_bSpawnEffect = true;

		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::StopEffect()
	{
		if( m_pEffectPackagePair )
		{
			g_EffectManager.DeqEffectPackagePair( m_pEffectPackagePair );

			m_pEffectPackagePair = NULL;
		}

		m_bSpawnEffect = false;

		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::PrepareRender(BOOL bOnlyPositionUpdate,int AttackType,char ground_type)
	{		
		if( m_pResCharacter == NULL ) return FALSE;
		if( m_Animation.m_pResAnimation == NULL) return FALSE;

		// 현재 에니메이션의 Total Play Time을 구하고
		float fLength = m_Animation.m_pResAnimation->animation_ptr->time_length;
		
		// m_bReverseAnimation
		float fPreFrame = m_PreFrame / fLength;
		float fCurFrame = m_CurFrame / fLength;

		/*	꺼꾸로 돌아가는 에니메이션 */
		if( m_bReverseAnimation)
		{
			fPreFrame = 1 - fPreFrame;
			fCurFrame = 1 - fCurFrame;
		}
		
		if( m_LocalScale != Vector3( 1.0f, 1.0f, 1.0f))
		{
			m_Animation.BuildMatrix_Scale( m_LocalScale,
									fPreFrame, 
									fCurFrame, 
									m_pMatrixList, 
									m_pCharTM, 
									m_pEffectMatrixList, 
									m_LocalAngle,
									&m_LocalCenterTM,
									&m_TriggerList,
									bOnlyPositionUpdate
									);	// 위치값을 변형 시켜준다
		}
		else
		{
			m_Animation.BuildMatrix( fPreFrame, 
									fCurFrame, 
									m_pMatrixList, 
									m_pCharTM, 
									m_pEffectMatrixList, 
									m_LocalAngle,
									&m_LocalCenterTM,
									&m_TriggerList,
									bOnlyPositionUpdate
									);	// 위치값을 변형 시켜준다
		}
		
		m_PreFrame = m_CurFrame;

		// 시간을 직접 구해서 더했기 때문에. FrameScale이 필요가 없다
		float fDelta = (g_dwCurTime - m_LastUpdateTime);

		// 위치보정 Frame skip시의 위치를 보정하여 준다.
		if(fDelta > fLength)
		{			
			//DBG_LogFile("fdelta:%f fLength:%f m_fAnimationSpeed:%f",fDelta,fLength,m_fAnimationSpeed);

			int temp_delta = (int)((g_dwCurTime - m_LastUpdateTime) / fLength);
			//DBG_LogFile("delta : %d",temp_delta );

			for(int i=0; i<temp_delta; i++)
			{
				fPreFrame = 0.0f;
				fCurFrame = 0.999f;	// 거의 마지막 까지

				if( m_LocalScale != Vector3( 1.0f, 1.0f, 1.0f))
				{
					m_Animation.BuildMatrix_Scale( m_LocalScale,
											fPreFrame, 
											fCurFrame, 
											m_pMatrixList, 
											m_pCharTM, 
											m_pEffectMatrixList, 
											m_LocalAngle,
											&m_LocalCenterTM,
											&m_TriggerList,
											bOnlyPositionUpdate
											);	// 위치값을 변형 시켜준다
				}
				else
				{
					m_Animation.BuildMatrix( fPreFrame, 
											fCurFrame, 
											m_pMatrixList, 
											m_pCharTM, 
											m_pEffectMatrixList, 
											m_LocalAngle,
											&m_LocalCenterTM,
											&m_TriggerList,
											bOnlyPositionUpdate
											);	// 위치값을 변형 시켜준다
				}
			}
			fDelta -= (float)temp_delta * fLength;
			//DBG_LogFile("fdelta result : %f",fDelta);
		}

		fDelta *= m_fAnimationSpeed;
		m_LastUpdateTime = g_dwCurTime;

		// Alpha Effect
		if( m_bAlphaEffect)
		{
			m_Material.Diffuse.a = m_fAlphaEffect;
			m_Material.Ambient.a = m_fAlphaEffect;
			m_Material.Specular.a = m_fAlphaEffect;
			
			m_fAlphaEffect -= (fDelta / 2000.0f) * m_fAlphaEffectSpeed;

			if( m_fAlphaEffect <= 0)
			{
				int r =  m_TriggerList.Invoke( eTE_CharRender_EndAlphaEffect);

				m_bAlphaEffect = TRUE;
				m_fAlphaEffect = 0;

				if( r == 0)
					return TRUE;
			}
		}
		
		//eTE_CharRender_Timer
		//Timer Trigger
		if(m_Animation.m_pResAnimation)
		{
			for(int t_index = 0; t_index < 	m_Animation.m_pResAnimation->trigger_count; t_index++)
			{
				Res_TimerTrigger* pTrigger = &m_Animation.m_pResAnimation->trigger_ptr[ t_index];
				//nType
				if( m_CurFrame >= (float)pTrigger->nStartTime && m_bTimeTrigger[ t_index])
				{
					m_TriggerList.Invoke( eTE_CharRender_Timer, pTrigger->nType);
					m_bTimeTrigger[ t_index ] = FALSE;
				}
			}
		}

		//
		m_CurFrame += fDelta;

		// Frame이 최고 프레임을 넘을때
		if( m_CurFrame > fLength )
		{
			int r = m_TriggerList.Invoke( eTE_CharRender_EndAnimation);

			SetTimeTrigger();
			if( r == -1 || r == 0) // Trigger가 없거나, 별다른 처리가 없으면 반복
			{
				if( m_bLoopAnimation)
				{
					m_CurFrame -= fLength;
					m_PreFrame = m_CurFrame;
				}
				else
				{
					m_CurFrame = fLength;
					m_PreFrame = m_CurFrame;
				}
				// 반복시 사운드의 초기화
				RespawnAnimationSound();
				// 리턴하지 않으면 대기동작 사운드 이상해짐.
				return TRUE;
			}
			else	// 다른 Animation이 세팅 되었다
				return TRUE;
		}

		// 화면에 캐릭터는 안보이고 해당 이펙트가 있으면 이것도 안보이게 한다.
		if( bOnlyPositionUpdate && m_bSpawnEffect )
		{
			if( m_pEffectPackagePair && m_pEffectPackagePair->bNowUsing )
				m_pEffectPackagePair->bIsVisible = false;
		}
		else
		if( !bOnlyPositionUpdate && m_bSpawnEffect )
		{
			if( m_pEffectPackagePair && m_pEffectPackagePair->bNowUsing )
				m_pEffectPackagePair->bIsVisible = true;
		}

		// Set effect matrix
		if( m_pEffectPackagePair )
		{
			if( m_pCharTM )
			{
				Matrix4x4 LocalRotateTM;
				LocalRotateTM.SetRotationY( m_LocalAngle);     
				m_pEffectPackagePair->WorldMatrix = (MATRIX)(m_LocalCenterTM * LocalRotateTM * (*m_pCharTM));
/*
				m_pEffectPackagePair->WorldMatrix = (MATRIX)(Matrix4x4());
				m_pEffectPackagePair->WorldMatrix._41 = m_pCharTM->t.x;
				m_pEffectPackagePair->WorldMatrix._42 = m_pCharTM->t.y;
				m_pEffectPackagePair->WorldMatrix._43 = m_pCharTM->t.z;
*/
			}

			EFFECTPACKAGELIST::iterator eit;
			for(eit=m_pEffectPackagePair->PackageList.begin(); eit!=m_pEffectPackagePair->PackageList.end(); eit++)
			{
				_EFFECTPACKAGE* pPackage = *eit;
#ifdef TRACE_LOG
				if(pPackage == NULL)
				{
					DBG_LogFile( _T("CCharRender::PrepareRender2 fail"));
				}
#endif
				if( pPackage->nBoneIndex != -1 )
					pPackage->BoneMatrix = (MATRIX)m_pEffectMatrixList[ pPackage->nBoneIndex ];
			}// for
		}// if

		// Mesh Effect
		if( m_pMeshEffectPackagePair )
		{
			EFFECTPACKAGELIST::iterator eit;
			for(eit=m_pMeshEffectPackagePair->PackageList.begin(); eit!=m_pMeshEffectPackagePair->PackageList.end(); eit++)
			{
				_EFFECTPACKAGE* pPackage = *eit;
#ifdef TRACE_LOG
				if(pPackage == NULL)
				{
					DBG_LogFile( _T("CCharRender::PrepareRender3 fail"));
				}
#endif
				if( pPackage->nBoneIndex != -1 )
					pPackage->BoneMatrix = (MATRIX)m_pEffectMatrixList[ pPackage->nBoneIndex ];
			}// for
		}// if

		// SOUND PLAY! 볼륨이 0 이면 아에 플레이 하지 않는다.
		if(g_EngineInfo.m_dwFXVolume > 0)
			PlayAnimationSound(AttackType,ground_type);

		return TRUE;
	}

	/*
	XIAHGE_API BOOL CCharRender::EnableBoneAnimation(BOOL bEnable)
	{
		m_bBoneAnimation = bEnable;
		return TRUE;
	}
	*/

	/*
	XIAHGE_API BOOL CCharRender::SetLocalCenter(Matrix4x4 centerTM)
	{
		m_LocalCenterTM = centerTM;
		return TRUE;
	}
	*/

	/*
	XIAHGE_API BOOL CCharRender::ShadowRender()
	{
		// NOT YET!

		return TRUE;
	}
	*/

	XIAHGE_API BOOL CCharRender::Render(bool bGray)
	{
		if( m_pResCharacter == NULL ) return FALSE;

//		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

		DWORD dwTemp;
		g_pDirect3DDevice->GetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, &dwTemp );

		if( m_fAlphaEffect != 1.0f)
		{
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		}

		g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );

		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

		if( m_bApplyMaterialColor )
			g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		else
			g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

		g_pDirect3DDevice->SetMaterial( &m_Material);

		if( m_bSpecularEffect)
		{
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD  );
		
			g_Device.SetTexture(1, m_pSpecularTexture);
			//g_pDirect3DDevice->SetTexture( 1, m_pSpecularTexture);
		}

		if( m_Mesh.m_SubMesh[0].m_pResMesh->is_physique && m_bBoneAnimation)
		{
			for(int j=0; j<m_Animation.m_pResSkeleton->bone_count; j++)
				g_pDirect3DDevice->SetTransform( D3DTS_WORLDMATRIX(j+1), (D3DMATRIX *)&m_pMatrixList[j] );
		}
		else
		{
			if( m_pCharTM)
				g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)m_pCharTM);
			else
			{
				Matrix4x4 iTM;
				g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&iTM);
			}
		}

		if( m_bAlphaEffect)
			m_bGlowEffect = FALSE;

		if( m_bGlowEffect)
		{
			int glow_level = 4;// GlowEffect의 정밀도
			float glow_color = 0.02f;
			float glow_scale = 0.02f;
			
/*
			static float glow_color = 0.05f;
			static float glow_scale = 0.11f;
			static int pre = 0;
			if( GetAsyncKeyState( '1') < 0)
			{
				glow_color += 0.01f;
			}

			if( GetAsyncKeyState( '2') < 0)
			{
				glow_color -= 0.01f;
			}
			
			if( GetAsyncKeyState( '3') < 0)
			{
				glow_scale += 0.005f;
			}

			if( GetAsyncKeyState( '4') < 0)
			{
				glow_scale -= 0.005f;
			}

			int cur = GetAsyncKeyState( VK_RETURN) < 0;

			if( cur && pre == 0)
			{
				DBG_LogFile("%.2f %.2f", glow_color, glow_scale);
			}

			pre = cur;
*/
			g_Device.SetTexture(0, NULL);
			//g_pDirect3DDevice->SetTexture( 0, NULL );

			Matrix4x4 backProj;
			Matrix4x4 view;
			Matrix4x4 proj;

			g_pDirect3DDevice->GetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&backProj);
			g_pDirect3DDevice->GetTransform( D3DTS_VIEW, (D3DMATRIX*)&view);

			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE);
			g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

			g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
			g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
			//g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
			
			g_pDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
			D3DMATERIAL9 backMat;
			D3DMATERIAL9 Mat;

			g_pDirect3DDevice->GetMaterial( &backMat);
			Mat = backMat;

			Mat.Diffuse.r = 0.00f;
			Mat.Diffuse.g = 0.00f;
			Mat.Diffuse.b = 0.00f;
			Mat.Diffuse.a = 0.00f;
			Mat.Ambient.r = glow_color;
			Mat.Ambient.g = glow_color;
			Mat.Ambient.b = glow_color;
			Mat.Ambient.a = glow_color;

			g_pDirect3DDevice->SetMaterial( &Mat);

			D3DLIGHT9 backLight;
			D3DLIGHT9 Light;

			g_pDirect3DDevice->GetLight( 0, &backLight);

			Light = backLight;

			Light.Ambient.r = GetBValue( m_GlowColor) / 255.0f;
			Light.Ambient.g = GetGValue( m_GlowColor) / 255.0f;
			Light.Ambient.b = GetRValue( m_GlowColor) / 255.0f;
			Light.Ambient.a = 1.0f;
			Light.Diffuse.r = 0.0f;
			Light.Diffuse.g = 0.0f;
			Light.Diffuse.b = 0.0f;
			Light.Diffuse.a = 0.0f;
			Light.Direction.x = 1.0f;
			Light.Direction.y = 1.0f;
			Light.Direction.z = 1.0f;

			g_pDirect3DDevice->SetLight( 0, &Light);
			g_pDirect3DDevice->LightEnable( 1, FALSE);

			g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

			Vector3 scPos;
			float	projection_scale = 1;

			if( m_Mesh.m_SubMesh[0].m_pResMesh->is_physique && m_bBoneAnimation)
			{
				scPos = m_pEffectMatrixList[ 0].t;
				scPos = g_pCurrentCamera->WorldToScreen( scPos);
				projection_scale = 1 + (scPos.z - g_pCurrentCamera->m_fNear) * 0.2f / (g_pCurrentCamera->m_fFar - g_pCurrentCamera->m_fNear);

				scPos = m_pEffectMatrixList[ 0].t;
				scPos *= view;
				scPos.z = 0;
			}
			else if( m_pCharTM)
			{
				scPos = m_pCharTM->t;
				scPos = g_pCurrentCamera->WorldToScreen( scPos);
				projection_scale = 1 + (scPos.z - g_pCurrentCamera->m_fNear) * 0.2f / (g_pCurrentCamera->m_fFar - g_pCurrentCamera->m_fNear);

				scPos = m_pCharTM->t;
				scPos *= view;
				scPos.z = 0;
			}

			Matrix4x4 temp1, temp2;

			temp1.t = -scPos;
			temp2.t = scPos;

			for(int glow_index = 0; glow_index < glow_level; glow_index++)
			{
				Matrix4x4 scale;
				Matrix4x4 tempMatrix;
				float fScale = 1.00f + (float)(glow_level - glow_index) * glow_scale / (float)glow_level;
				
				fScale *= projection_scale;

				scale.SetScale( Vector3( fScale, fScale, 1));
				//scale.SetRotation( _PI / 4);

				proj = temp1 * scale * temp2 * backProj;

				g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&proj);
/*
				if( m_Mesh.m_SubMesh[0].m_pResMesh->is_physique && m_bBoneAnimation)
				{
					for(int j=0; j<m_Animation.m_pResSkeleton->bone_count; j++)
					{
						tempMatrix = scale * m_pMatrixList[j];
						g_pDirect3DDevice->SetTransform( D3DTS_WORLDMATRIX(j+1), (D3DMATRIX *)&tempMatrix );
					}
				}
				else
				{
					if( m_pCharTM)
					{
						tempMatrix = scale * (*m_pCharTM);
						g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&tempMatrix);
					}
					else
					{
						tempMatrix = scale;
						g_pDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&tempMatrix);
					}
				}
*/
				for(int i = 0; i < m_Mesh.m_nSubMesh; i++)
				{
					if( m_Mesh.m_SubMesh[0].m_pResMesh->is_physique )
					{
						g_pDirect3DDevice->SetSoftwareVertexProcessing(TRUE);

						g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );
						g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

						g_Device.SetFVF(FVF_SKINVERTEX);
						//g_pDirect3DDevice->SetFVF( FVF_SKINVERTEX );
						g_Device.SetStreamSource( m_Mesh.m_SubMesh[i].m_pResMesh->vertex_ptr, sizeof(SkinVertex) );
						g_Device.SetIndices( m_Mesh.m_SubMesh[i].m_pIndexBuffer );
					}
					else
					{
						g_pDirect3DDevice->SetSoftwareVertexProcessing(FALSE);

						g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
						g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);

						g_Device.SetFVF(D3DFVF_VERTEX);
						//g_pDirect3DDevice->SetFVF( D3DFVF_VERTEX );
						g_Device.SetStreamSource( m_Mesh.m_SubMesh[i].m_pResMesh->vertex_ptr, sizeof(VT_Normal) );
						g_Device.SetIndices( m_Mesh.m_SubMesh[i].m_pIndexBuffer );
					}

					g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_Mesh.m_SubMesh[i].m_nLodVertex, 0, m_Mesh.m_SubMesh[i].m_nLodFace );

					g_EngineInfo.m_nRenderedVertex += m_Mesh.m_SubMesh[i].m_nLodVertex;
					g_EngineInfo.m_nRenderedFace += m_Mesh.m_SubMesh[i].m_nLodFace;
				}// for
			}// for( glow_index )
			
			g_pDirect3DDevice->SetLight( 0, &backLight);
			g_pDirect3DDevice->LightEnable( 1, TRUE);
			
			g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

			g_pDirect3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&backProj);
			g_pDirect3DDevice->SetMaterial( &backMat);

			g_pDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
			g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);
		}// if( m_bGlowEffect)
		
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);

		// 현재 Mesh의 SubMesh(SkinMesh)를 돌면서 렌더링한다.
		for(int i=0; i<m_Mesh.m_nSubMesh; i++)
		{
			if( m_Mesh.m_SubMesh[i].m_pResMesh->is_physique )
			{
				g_pDirect3DDevice->SetSoftwareVertexProcessing(TRUE);

				g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );
				g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

				g_Device.SetFVF(FVF_SKINVERTEX);
				//g_pDirect3DDevice->SetFVF( FVF_SKINVERTEX );
				g_Device.SetStreamSource( m_Mesh.m_SubMesh[i].m_pResMesh->vertex_ptr, sizeof(SkinVertex) );
				g_Device.SetIndices( m_Mesh.m_SubMesh[i].m_pIndexBuffer );
			}
			else
			{
				g_pDirect3DDevice->SetSoftwareVertexProcessing(FALSE);

				g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
				g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);

				g_Device.SetFVF(D3DFVF_VERTEX);
				//g_pDirect3DDevice->SetFVF( D3DFVF_VERTEX );
				g_Device.SetStreamSource( m_Mesh.m_SubMesh[i].m_pResMesh->vertex_ptr, sizeof(VT_Normal) );
				g_Device.SetIndices( m_Mesh.m_SubMesh[i].m_pIndexBuffer );
			}

			g_Device.SetTexture(0, m_Texture.m_pTexture[i]);
			//g_pDirect3DDevice->SetTexture( 0, m_Texture.m_pTexture[i] );

//			if( m_bSpecularEffect)
//			{
//				g_pDirect3DDevice->SetTexture( 1, m_Texture.m_pTexture[i] );
//			}

			g_pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE   );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT   );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE   );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT   );
			g_pDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

			bool bAlphaTest = false;

			if( m_bAlphaEffect == FALSE)
			{
				switch( m_Texture.m_pResTexture->texture_sub_ptr[ i].is_alpha)
				{
				case 0:
					g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
					g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE  );
					g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
					break;
				case 1:
					bAlphaTest = true;
					g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
					g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case 2:
					g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
					g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE);
					g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
				}
			}
			else
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
				g_pDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				g_pDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}

			if( bAlphaTest )
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAREF, 0x00000050 );
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			}

			if( m_Texture.m_pResTexture->texture_sub_ptr[ i].cull_mode >= 1 && m_Texture.m_pResTexture->texture_sub_ptr[ i].cull_mode <= 3)
				g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE,  m_Texture.m_pResTexture->texture_sub_ptr[ i].cull_mode);
			else
				g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);

			// 캐릭터 선택창에서 그레이로 그릴때 사용하기 위해서 이런 짓을 함.
/*			if( bGray )
			{
				D3DMATERIAL9 material;
				memset( &material, 0, sizeof(D3DMATERIAL9) );

				material.Ambient.r = 0.4f;
				material.Ambient.g = 0.4f;
				material.Ambient.b = 0.4f;
				material.Diffuse.r = 0.4f;
				material.Diffuse.g = 0.4f;
				material.Diffuse.b = 0.4f;

				g_pDirect3DDevice->SetMaterial( &material);
				g_pDirect3DDevice->SetTexture( 0, NULL );
			}// if
*/

			g_pDirect3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_Mesh.m_SubMesh[i].m_nLodVertex, 0, m_Mesh.m_SubMesh[i].m_nLodFace );

#ifndef MASTER
			g_EngineInfo.m_nRenderedVertex += m_Mesh.m_SubMesh[i].m_nLodVertex;
			g_EngineInfo.m_nRenderedFace += m_Mesh.m_SubMesh[i].m_nLodFace;
#endif
		}// for(int i=0; i<m_Mesh.m_nSubMesh; i++)

		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);

		if( m_bSpecularEffect)
		{
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
			g_pDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE  );
		}
		g_pDirect3DDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		g_pDirect3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		g_pDirect3DDevice->SetSoftwareVertexProcessing(FALSE);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

		return TRUE;
	}

	/*
	XIAHGE_API BOOL CCharRender::SetDetailLevel(float fDetailLevel)
	{
		DBG_Assert( m_pResCharacter != NULL);
		DBG_Assert( m_Mesh.m_pResMesh != NULL);

		// test
		fDetailLevel = 1.0f;

		fDetailLevel *= g_EngineInfo.m_fPolygonDetail;
		m_Mesh.SetLodLevel( fDetailLevel);

		return TRUE;
	}
	*/

	/*
	XIAHGE_API BOOL CCharRender::SetPosition(Matrix4x4* pObjectTM)
	{
		m_pCharTM = pObjectTM;

		// 이때 Mesh Effect가 있으면 위치도 같이 세팅.
		if( m_pMeshEffectPackagePair )
		{
			m_pMeshEffectPackagePair->pWorldMatrix = (MATRIX*)m_pCharTM;

//			memcpy( &m_pMeshEffectPackagePair->WorldMatrix, m_pCharTM, sizeof(Matrix4x4) );
		}

		return TRUE;
	}
	*/

	XIAHGE_API BBoxAABB3 CCharRender::GetLocalBound()	// Mesh에 설정된 Bound구하기
	{
		DBG_Assert( m_Mesh.IsValid());
		
		return BBoxAABB3( Vector3( 0, 0, 0), 
							Vector3( 
								m_Mesh.m_pResMesh->mesh_size[ 0], 
								m_Mesh.m_pResMesh->mesh_size[ 1], 
								m_Mesh.m_pResMesh->mesh_size[ 2]));
	}

	/*
	XIAHGE_API BOOL CCharRender::StartAlphaEffect(float fAlphaEffectSpeed)
	{
		m_bAlphaEffect = TRUE;
		m_fAlphaEffect = 1.0f;
		m_fAlphaEffectSpeed = fAlphaEffectSpeed;
	
		return TRUE;
	}
	*/

	/*
	XIAHGE_API BOOL CCharRender::IsAlphaEffect()
	{
		return m_bAlphaEffect;
	}
	*/

	XIAHGE_API BOOL CCharRender::SetAlphaValue(float val)
	{
		if(val == 0.0f)	
			m_bAlphaEffect = FALSE;
		else
			m_bAlphaEffect = TRUE;

		m_fAlphaEffect = val;

		return TRUE;
	}

	XIAHGE_API Matrix4x4* CCharRender::GetChildBoneMatrix(int nLogicalIndex)
	{
		DBG_Assert( IsValid());
		DBG_Assert( m_pEffectMatrixList != NULL);
		DBG_Assert( m_Mesh.IsValid());

		DBG_Assert( nLogicalIndex < m_Mesh.m_pResMesh->logical_bone_index_count);

		int nPhysicalIndex = m_Mesh.m_pResMesh->logical_bone_index_ptr[ nLogicalIndex];

		DBG_Assert( nPhysicalIndex < m_Mesh.m_pResMesh->skeleton_ptr->bone_count);

		return &m_pEffectMatrixList[ nPhysicalIndex];
	}

	XIAHGE_API BOOL CCharRender::ChangeTexture(int nSubMeshIndex,int res_id)
	{
		DBG_Assert( IsValid());

		m_Texture.m_pTexture[ nSubMeshIndex] = XiahPak::GetTexture( res_id);
	
		return TRUE;
	}

	XIAHGE_API BOOL CCharRender::EnableSpecularEffect()
	{
		m_bSpecularEffect = TRUE;
		m_pSpecularTexture = XiahPak::GetTexture( 50000177);
	
		return TRUE;
	}

/*
#define MAX_ANIMATION_SOUND	20

		int						m_nAnimationSound;
		IDirectSoundBuffer8*	m_pAnimationSoundBufferList[ MAX_ANIMATION_SOUND];
		int						m_nAnimationSoundStartTime[ MAX_ANIMATION_SOUND];
*/
	
	BOOL CCharRender::SpawnAnimationSound()
	{
		if( m_Animation.m_pResAnimation == NULL)
			return FALSE;

		//ReleaseAnimationSound();

		m_nAnimationSound = 0;

		m_RightFoot = FALSE;
		m_LeftFoot = FALSE;

		for(int i = 0; i < m_Animation.m_pResAnimation->sound_count; i++)
		{

			if( XiahSoundPak::GetSoundBufferInstance( m_Animation.m_pResAnimation->sound_ptr[ i].nSoundID,
								&m_pAnimationSoundBufferList[ i], &m_nAnimationSoundStartTime[ i]))
			{
				m_bAnimationSoundPlay[ i] = FALSE;
				m_nAnimationSound ++;
			}
		}

		return TRUE;
	}
	
	BOOL CCharRender::RespawnAnimationSound()
	{
		// Animation반복과 같은 경우
		if( m_Animation.m_pResAnimation == NULL)
			return FALSE;

		for(int i = 0; i < m_nAnimationSound; i++)
		{
			m_bAnimationSoundPlay[ i] = FALSE;
			m_RightFoot = FALSE;
			m_LeftFoot = FALSE;
		}

		return TRUE;
	}


#define SWING_SOUNDFX		0
#define CRITICAL_SOUNDFX	4

#define ANI_RUNNING		202
#define ANI_ATTACK1		211	
#define ANI_ATTACK2		212
#define ANI_ATTACK3		213

	//케렉터의 사운드를 플레이 한다.
	BOOL CCharRender::PlayAnimationSound(int AttackType,char ground_type)
	{
		LONG volume;
		LONG pan;

		if( m_Animation.m_pResAnimation == NULL || m_nAnimationSound == 0)
			return FALSE;

		// PC들의 뛸때 처리
		if(m_Animation.m_pResAnimation->animation_type == ANI_RUNNING)
		{
			// 2004.08.06 Changth
			// 분신격이 뛰니깐 분신이 뛸때도 걷는 소리가 나는데 이때 에러가 난다. 
			// 그래서 분신이 뛸때는 소리가 없게한다.
			if( !m_bEnableAnimationSound ) 
				return FALSE;

			float fLength = m_Animation.m_pResAnimation->animation_ptr->time_length;
			float cur_frame = m_CurFrame / fLength;

			//DBG_Put("%f %f %f %d %d",cur_frame,m_CurFrame, fLength,m_RightFoot ,m_LeftFoot );
			//오른발
			if(cur_frame >= 0.4 && m_RightFoot == FALSE)
			{
				g_pCurrentCamera->GetSoundEffectVolume( m_pCharTM->t, &volume, &pan);
				g_pCurrentCamera->RegSoundEffect(m_pCharTM->t,pan,m_pAnimationSoundBufferList[ground_type-1]);
				m_RightFoot = TRUE;
				//DBG_Put("오른발");
				return TRUE;
			}

			//왼발
			if(cur_frame >= 0.90 && m_LeftFoot == FALSE)
			{
				g_pCurrentCamera->GetSoundEffectVolume( m_pCharTM->t, &volume, &pan);
				if(g_pCurrentCamera->RegSoundEffect(m_pCharTM->t,pan,m_pAnimationSoundBufferList[ground_type-1]) == TRUE)
					m_bAnimationSoundPlay[ground_type-1] = TRUE;
				m_LeftFoot = TRUE;
				//DBG_Put("왼발");
				return TRUE;
			}
		}

		// 기본공격시의 다른소리를 위한 CHECK (HARD Coding)
		// 211,212,213 이 공격 에니메이션 번호
		if(m_Animation.m_pResAnimation->animation_type == ANI_ATTACK1 ||
			m_Animation.m_pResAnimation->animation_type == ANI_ATTACK2 ||
			m_Animation.m_pResAnimation->animation_type == ANI_ATTACK3)
		{
			// 공격시!
			if( m_nAnimationSoundStartTime[SWING_SOUNDFX] <= m_CurFrame && m_bAnimationSoundPlay[SWING_SOUNDFX] == FALSE)
			{
				if( m_pAnimationSoundBufferList[SWING_SOUNDFX])
				{
					g_pCurrentCamera->GetSoundEffectVolume( m_pCharTM->t, &volume, &pan);
					if(g_pCurrentCamera->RegSoundEffect(m_pCharTM->t,pan,m_pAnimationSoundBufferList[SWING_SOUNDFX]) == TRUE)
					{
						m_bAnimationSoundPlay[SWING_SOUNDFX] = TRUE;
					}
					else
						m_bAnimationSoundPlay[SWING_SOUNDFX] = FALSE;
				}
			}

			//DBG_Put("%d %d %d %d",AttackType,m_nAnimationSound,bMissed,bCritical);
			// 어텍소리가 잘못된경우 아니면 MISS 가 나왔다! 이러면 타격소리는 없다.
			if(AttackType == 0 || AttackType >= m_nAnimationSound) return TRUE;

			// 두번째 소리
			if( m_nAnimationSoundStartTime[AttackType] <= m_CurFrame && m_bAnimationSoundPlay[AttackType] == FALSE)
			{
				if( m_pAnimationSoundBufferList[AttackType])
				{
					g_pCurrentCamera->GetSoundEffectVolume( m_pCharTM->t, &volume, &pan);
					if(g_pCurrentCamera->RegSoundEffect(m_pCharTM->t,pan,m_pAnimationSoundBufferList[AttackType]) == TRUE)
					{
						m_bAnimationSoundPlay[AttackType] = TRUE;
					}
					else
						m_bAnimationSoundPlay[AttackType] = FALSE;
				}
			}
		}
		else
		if(m_Animation.m_pResAnimation->animation_type != ANI_RUNNING)
		{
			for(int i = 0; i < m_nAnimationSound; i++)
			{
				if( m_nAnimationSoundStartTime[i] <= m_CurFrame && m_bAnimationSoundPlay[i] == FALSE)
				{
					if( m_pAnimationSoundBufferList[i])
					{
						// 카메라 PAN을 얻는다
						g_pCurrentCamera->GetSoundEffectVolume( m_pCharTM->t, &volume, &pan);
						// FX를 등록한다.
						if(g_pCurrentCamera->RegSoundEffect(m_pCharTM->t,pan,m_pAnimationSoundBufferList[i]) == TRUE)
						{
							m_bAnimationSoundPlay[i] = TRUE;
						}
						else
							m_bAnimationSoundPlay[i] = FALSE;

						//return TRUE;
					}
					m_bAnimationSoundPlay[i] = TRUE;
				}
			}
		}
		return TRUE;
	}
	
	BOOL CCharRender::ReleaseAnimationSound()
	{
		if( m_Animation.m_pResAnimation == NULL)
			return FALSE;

		if( m_nAnimationSound == 0)
			return FALSE;

		for(int i = 0; i < m_nAnimationSound; i++)
		{
			if( m_pAnimationSoundBufferList[ i] != NULL)
			{
				// FX가 등록되어있으면 리스트에서 제거해야한다. 안하면 ??? 모름 -_-
				XiahFX::Del_FX(m_pAnimationSoundBufferList[i]);

				//m_pAnimationSoundBufferList[ i]->Stop();
				//m_pAnimationSoundBufferList[ i]->Release();
				//m_pAnimationSoundBufferList[ i] = NULL;
			}

			m_nAnimationSoundStartTime[ i] = 0;
			m_bAnimationSoundPlay[ i] = FALSE;
		}

		m_nAnimationSound = 0;

		return TRUE;
	}

	/*
	XIAHGE_API int CCharRender::GetMeshType()
	{
		if( m_Mesh.IsValid() == FALSE)
			return -1;

		return m_Mesh.m_pResMesh->mesh_type;
	}
	*/

	/*
	XIAHGE_API int CCharRender::GetTextureType()
	{
		if( m_Texture.m_pResTexture == NULL)
			return -1;

		return m_Texture.m_pResTexture->texture_type;
	}
	*/

	/*
	XIAHGE_API int CCharRender::GetCharID()
	{
		if( IsValid() == FALSE)
			return -1;

		return	m_pResCharacter->character_id;
	}
	*/

	XIAHGE_API int CCharRender::GetTimerTriggerCount()
	{
		if( m_pResCharacter == NULL)
			return 0;

		if( m_Mesh.IsValid() == FALSE)
			return 0;

		if( m_Animation.m_pResAnimation == NULL)
			return 0;

		return m_Animation.m_pResAnimation->trigger_count;
	}

	/*
	XIAHGE_API BOOL CCharRender::EnableGlowEffect(BOOL bTrue,D3DCOLOR glow_color)
	{
		m_bGlowEffect = bTrue;
		m_GlowColor = glow_color;
	
		return TRUE;
	}
	*/

	/*
	XIAHGE_API BOOL CCharRender::SetLocalScale(Vector3 scale)
	{
		m_LocalScale = scale;
		return TRUE;
	}
	*/

	/*
	XIAHGE_API Matrix4x4* CCharRender::GetCharTM()
	{
		return m_pCharTM;
	}
	*/

	/*
	XIAHGE_API BOOL CCharRender::IsAlphaBlendTestObject()
	{
		//if( m_Mesh.IsValid() == FALSE) return FALSE;
		//if( m_Texture.m_pResTexture == NULL) return FALSE;
		//if( m_Texture.m_pResTexture->texture_sub_ptr == NULL ) return FALSE;

		// 캐릭터의 경우, 알파 채널과 스페큘러가 있는데,
		// 현재 스페큘러는 거의 없으므로. 0이 아닌것은 다 알파로 취급
		for(int i=0; i<m_Mesh.m_nSubMesh; i++)
		{
			if( m_Texture.m_pResTexture->texture_sub_ptr[i].is_alpha != 0 )
				return TRUE;
		}// for

		return FALSE;
	}
	*/

	XIAHGE_API void CCharRender::SetMaterialDiffuseColor(BOOL bValue, float r, float g, float b)
	{
		m_bApplyMaterialColor = bValue;

		m_Material.Diffuse.r = r;
		m_Material.Diffuse.g = g;
		m_Material.Diffuse.b = b;
		m_Material.Ambient.r = r;
		m_Material.Ambient.g = g;
		m_Material.Ambient.b = b;
	}

};
