#include "stdafx.h"
#include "CharMan.h"


namespace XiahGameEngine
{
	BYTE	g_FileBuffer[ MAX_FILE_MEM_BUFFER ];	// for reading BCF file

	CHARACTERLIST		g_CharacterList;		// character data from package
	FILEHANDLELIST		g_FileHandleList;		// Package File Handle List


	// Load Xiah Character Package FIle => only package header
	XIAHGE_API BOOL InitializeCharacter(LPCTSTR *pPakList, int nCount)
	{
		FILE* fp;
		for(int i=0; i<nCount; i++)	// all character package file
		{
			if( (fp = _tfopen( pPakList[i], _T("rb"))) == NULL)
			{
				TCHAR str[100];
				memset( str, 0, sizeof(str) );
				_stprintf( str, _T("%s can't open"), pPakList[i] );

				MessageBox( GetForegroundWindow(), str, _T("Character Package Loading Module"), MB_OK | MB_ICONERROR);
				continue;
			}

			//
			g_FileHandleList.push_back( fp );

			int nCharCount;
			fread( &nCharCount, sizeof(nCharCount), 1, fp );
			for(int j=0; j<nCharCount; j++)	// all character
			{
				CRes_Character* pResChar = new CRes_Character;

#ifdef TRACE_LOG
				if(pResChar == NULL)
				{
					DBG_LogFile( _T("InitializeCharacter %d fail"),i);
				}
#endif
				fread( &pResChar->character_id, sizeof(pResChar->character_id), 1, fp );
				fread( &pResChar->mesh_count, sizeof(pResChar->mesh_count), 1, fp );
				fread( &pResChar->ani_count, sizeof(pResChar->ani_count), 1, fp );

				g_CharacterList.insert( CHARACTERLIST::value_type( pResChar->character_id, pResChar ) );

				// Res_Mesh
				for(int h=0;h<pResChar->mesh_count;h++)
				{
					Res_Mesh* pResMesh = new Res_Mesh;
#ifdef TRACE_LOG
					if(pResMesh == NULL)
					{
						DBG_LogFile( _T("InitializeCharacter2 fail"));
					}
#endif
					pResMesh->m_hFileHandle = fp;

					fread( &pResMesh->mesh_type, sizeof(pResMesh->mesh_type), 1, fp );
					fread( &pResMesh->texture_count, sizeof(pResMesh->texture_count), 1, fp );

					// Res_CharTexture
					pResMesh->texture_ptr = new Res_CharTexture [pResMesh->texture_count];
					for(int k=0; k<pResMesh->texture_count; k++)
					{
						Res_CharTexture* pResTex = &pResMesh->texture_ptr[k];

						fread( &pResTex->texture_type, sizeof(pResTex->texture_type), 1, fp );
						fread( &pResTex->texture_sub_count, sizeof(pResTex->texture_sub_count), 1, fp );

						// Res_CharTexture_Sub
						pResTex->texture_sub_ptr = new Res_CharTexture_Sub [pResTex->texture_sub_count];
						for(int g=0; g<pResTex->texture_sub_count; g++)
						{
							Res_CharTexture_Sub* pResTexSub = &pResTex->texture_sub_ptr[g];

							fread( &pResTexSub->texture_id, sizeof(pResTexSub->texture_id), 1, fp );
							fread( &pResTexSub->is_alpha, sizeof(pResTexSub->is_alpha), 1, fp );
							fread( &pResTexSub->cull_mode, sizeof(pResTexSub->cull_mode), 1, fp );
						}// for(pResTex->texture_sub_count)

					}// for(pResMesh->texture_count)

					//
					fread( &pResMesh->mesh_size[0], sizeof(pResMesh->mesh_size[0]), 1, fp );
					fread( &pResMesh->mesh_size[1], sizeof(pResMesh->mesh_size[1]), 1, fp );
					fread( &pResMesh->mesh_size[2], sizeof(pResMesh->mesh_size[2]), 1, fp );

					// logical_bone_index
					fread( &pResMesh->logical_bone_index_count, sizeof(pResMesh->logical_bone_index_count), 1, fp );

					if( pResMesh->logical_bone_index_count > 0)
					{
						pResMesh->logical_bone_index_ptr = new unsigned short [ pResMesh->logical_bone_index_count ];
						fread( pResMesh->logical_bone_index_ptr, sizeof(unsigned short)*pResMesh->logical_bone_index_count, 1, fp );
					}
					else
						pResMesh->logical_bone_index_ptr = NULL;

					// Mesh Effect
					fread( &pResMesh->effect_count, sizeof(pResMesh->effect_count), 1, fp );

					pResMesh->effect_ptr = NULL;
					if( pResMesh->effect_count > 0 )
					{
						pResMesh->effect_ptr = new Res_AniEffect [ pResMesh->effect_count ];

						fread( &pResMesh->effect_data_size, sizeof(pResMesh->effect_data_size), 1, fp );

						for(int k=0; k<pResMesh->effect_count; k++)
						{
							Res_AniEffect* pMeshEffect = &pResMesh->effect_ptr[k];

							fread( &pMeshEffect->nEffectID, sizeof(pMeshEffect->nEffectID), 1, fp );
							fread( &pMeshEffect->nPosX, sizeof(pMeshEffect->nPosX), 1, fp );
							fread( &pMeshEffect->nPosY, sizeof(pMeshEffect->nPosY), 1, fp );
							fread( &pMeshEffect->nPosZ, sizeof(pMeshEffect->nPosZ), 1, fp );
							fread( &pMeshEffect->nBoneIndex, sizeof(pMeshEffect->nBoneIndex), 1, fp );
							fread( &pMeshEffect->nStartTime, sizeof(pMeshEffect->nStartTime), 1, fp );
						}
					}// if

					//
					fread( &pResMesh->skin_mesh_count, sizeof(pResMesh->skin_mesh_count), 1, fp );

					fread( &pResMesh->skin_mesh_offset, sizeof(pResMesh->skin_mesh_offset), 1, fp );
					fread( &pResMesh->skeleton_offset, sizeof(pResMesh->skeleton_offset), 1, fp );

					//
					pResMesh->skin_mesh_ptr = NULL;
					pResMesh->skeleton_ptr = NULL;
					pResChar->MeshList.insert( CRes_Character::MESHLIST::value_type( pResMesh->mesh_type, pResMesh ) );
				}// for(pResChar->mesh_count)

				// Res_Animation
				for(h=0; h<pResChar->ani_count; h++)
				{
					Res_Animation* pResAni = new Res_Animation;
					pResAni->m_hFileHandle = fp;

					fread( &pResAni->animation_type, sizeof(pResAni->animation_type), 1, fp );

					// Attached Effect data
					fread( &pResAni->effect_count, sizeof(pResAni->effect_count), 1, fp );

					pResAni->effect_ptr = NULL;
					if( pResAni->effect_count != 0 )
					{
						pResAni->effect_ptr = new Res_AniEffect [ pResAni->effect_count ];

						fread( &pResAni->effect_data_size, sizeof(pResAni->effect_data_size), 1, fp );
						fread( &pResAni->effect_char_id, sizeof(pResAni->effect_char_id), 1, fp );

						for(int k=0; k<pResAni->effect_count; k++)
						{
							Res_AniEffect* pAniEffect = &pResAni->effect_ptr[k];

							fread( &pAniEffect->nEffectID, sizeof(pAniEffect->nEffectID), 1, fp );
							fread( &pAniEffect->nPosX, sizeof(pAniEffect->nPosX), 1, fp );
							fread( &pAniEffect->nPosY, sizeof(pAniEffect->nPosY), 1, fp );
							fread( &pAniEffect->nPosZ, sizeof(pAniEffect->nPosZ), 1, fp );
							fread( &pAniEffect->nBoneIndex, sizeof(pAniEffect->nBoneIndex), 1, fp );
							fread( &pAniEffect->nStartTime, sizeof(pAniEffect->nStartTime), 1, fp );
						}
					}// if

					// Attached Sound Data
					fread( &pResAni->sound_count, sizeof(pResAni->sound_count), 1, fp );

					pResAni->sound_ptr = NULL;
					if( pResAni->sound_count != 0 )
					{
						pResAni->sound_ptr = new Res_AniSound [ pResAni->sound_count ];

						fread( &pResAni->sound_data_size, sizeof(pResAni->sound_data_size), 1, fp );

						for(int k=0; k<pResAni->sound_count; k++)
						{
							Res_AniSound* pAniSound = &pResAni->sound_ptr[k];

							fread( &pAniSound->nSoundID, sizeof(pAniSound->nSoundID), 1, fp );
							fread( &pAniSound->nEffectDBID, sizeof(pAniSound->nEffectDBID), 1, fp );
						}
					}// if

					// Attached Timer Trigger Data
					fread( &pResAni->trigger_count, sizeof(pResAni->trigger_count), 1, fp );

					pResAni->trigger_ptr = NULL;
					if( pResAni->trigger_count != 0 )
					{
						pResAni->trigger_ptr = new Res_TimerTrigger [ pResAni->trigger_count ];

						fread( &pResAni->trigger_data_size, sizeof(pResAni->trigger_data_size), 1, fp );

						for(int k=0; k<pResAni->trigger_count; k++)
						{
							Res_TimerTrigger* pTrigger = &pResAni->trigger_ptr[k];

							fread( &pTrigger->nType, sizeof(pTrigger->nType), 1, fp );
							fread( &pTrigger->nStartTime, sizeof(pTrigger->nStartTime), 1, fp );
							fread( &pTrigger->nLength, sizeof(pTrigger->nLength), 1, fp );
						}
					}// if

					fread( &pResAni->animation_offset, sizeof(pResAni->animation_offset), 1, fp );
					//
					pResAni->animation_ptr = NULL;
					pResChar->AnimationList.insert( CRes_Character::ANIMATIONLIST::value_type( pResAni->animation_type, pResAni ) );
				}// for(pResChar->ani_count)

			}// for(nCharCount)

		}// for(nCount)

		return TRUE;
	}// InitializeCharacter

	// free memory of character data
	XIAHGE_API void ReleaseCharacterAll()
	{
		CHARACTERLIST::iterator it;
		for(it=g_CharacterList.begin(); it!=g_CharacterList.end(); it++)
		{
			CRes_Character* pResChar = it->second;

			pResChar->Release();

			delete pResChar;
		}// for( g_CharacterList )
		g_CharacterList.clear();
	}// ReleaseCharacter

	XIAHGE_API void CloseCharacterPakFileAll()
	{
		FILEHANDLELIST::iterator it;

		for(it=g_FileHandleList.begin(); it!=g_FileHandleList.end(); it++)
		{
			FILE* fp = *it;

			fclose( fp );
		}// for( g_FileHandleList )
		g_FileHandleList.clear();
	}//

	XIAHGE_API CRes_Character* GetCharacter(int nCharID)
	{
		CHARACTERLIST::iterator it = g_CharacterList.find(nCharID);

		if( it == g_CharacterList.end())
			return NULL;

		CRes_Character* pChar = it->second;

		pChar->Realize();

		return pChar;
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

	//-----------------------------------------------------------
	// class CRes_Character
	CRes_Character::CRes_Character()
	{
	}

	CRes_Character::~CRes_Character()
	{
		Release();
	}

	XIAHGE_API Res_Mesh* CRes_Character::GetMesh(int nMeshType)
	{
		MESHLIST::iterator it = MeshList.find( nMeshType );

		if( it == MeshList.end() )
			return NULL;
		else
		{
			Res_Mesh* pResMesh = it->second;

			try
			{
				pResMesh->Realize();
			}
			catch(LPCTSTR strError)
			{
				DBG_Put( (TCHAR *)strError);
			}

			return pResMesh;
		}
	}// GetMesh

	Res_Animation* CRes_Character::GetAnimation(int nAniType)
	{
		ANIMATIONLIST::iterator it = AnimationList.find( nAniType );

		if( it == AnimationList.end())
			return NULL;
		else
		{
			Res_Animation* pResAni = it->second;
			
			try
			{
				pResAni->Realize();
			}
			catch( LPCTSTR strError)
			{
				DBG_Put( (TCHAR *)strError);
			}

			return pResAni;
		}
	}// GetAnimation

	void CRes_Character::Release()
	{
		ReleaseMeshAll();
		ReleaseAnimationAll();
	}//

	void CRes_Character::ReleaseMeshAll()
	{
		MESHLIST::iterator mit;
		for(mit=MeshList.begin(); mit!=MeshList.end(); mit++)
		{
			Res_Mesh* pResMesh = mit->second;

			ReleaseMesh( pResMesh );
		}// for(MeshList)
		MeshList.clear();
	}//

	void CRes_Character::ReleaseMesh(int nMeshType)
	{
		MESHLIST::iterator mit = MeshList.find(nMeshType);

		if( mit != MeshList.end() )
		{
			Res_Mesh* pResMesh = mit->second;

			ReleaseMesh( pResMesh );

			MeshList.erase( mit );
		}
	}//

	void CRes_Character::ReleaseMesh(Res_Mesh* pResMesh)
	{
		for(int j=0; j<pResMesh->texture_count; j++)
		{
			Res_CharTexture* pTex = &pResMesh->texture_ptr[j];

			delete []pTex->texture_sub_ptr;
			pTex->texture_sub_ptr = NULL;

		}
		delete []pResMesh->texture_ptr;
		pResMesh->texture_ptr = NULL;

		if( pResMesh->logical_bone_index_ptr != NULL)
		{
			delete []pResMesh->logical_bone_index_ptr;
			pResMesh->logical_bone_index_ptr = NULL;
		}

		if( pResMesh->effect_ptr )
		{
			delete []pResMesh->effect_ptr;
			pResMesh->effect_ptr = NULL;
		}

		if( pResMesh->skin_mesh_ptr )
		{
			for(j=0; j<pResMesh->skin_mesh_count; j++)
			{
				Res_SkinMesh* pSkinMesh = &pResMesh->skin_mesh_ptr[j];

				pSkinMesh->vertex_ptr->Release();
				delete []pSkinMesh->face_ptr;
				pSkinMesh->face_ptr = NULL;
/*
				delete []pSkinMesh->lod_collapse_ptr;
				delete []pSkinMesh->lod_facecount_ptr;
*/
			}
			delete []pResMesh->skin_mesh_ptr;
			pResMesh->skin_mesh_ptr = NULL;
		}

		if( pResMesh->skeleton_ptr )
		{
            delete []pResMesh->skeleton_ptr->bone_ptr;
			pResMesh->skeleton_ptr->bone_ptr = NULL;

			delete []pResMesh->skeleton_ptr->world_inv_matrix;
			pResMesh->skeleton_ptr->world_inv_matrix = NULL;

			delete []pResMesh->skeleton_ptr->world_matrix;
			pResMesh->skeleton_ptr->world_matrix = NULL;

			delete pResMesh->skeleton_ptr;
			pResMesh->skeleton_ptr = NULL;
		}

		//
		delete pResMesh;
		pResMesh = NULL;
	}//

	void CRes_Character::ReleaseAnimationAll()
	{
		ANIMATIONLIST::iterator ait;
		for(ait=AnimationList.begin(); ait!=AnimationList.end(); ait++)
		{
			Res_Animation* pResAni = ait->second;

			ReleaseAnimation( pResAni );
		}// for(AnimationList)
		AnimationList.clear();
	}//

	void CRes_Character::ReleaseAnimation(int nAniType)
	{
		ANIMATIONLIST::iterator ait = AnimationList.find( nAniType );

		if( ait != AnimationList.end() )
		{
			Res_Animation* pResAni = ait->second;

			ReleaseAnimation( pResAni );

			AnimationList.erase( ait );
		}
	}//

	void CRes_Character::ReleaseAnimation(Res_Animation* pResAni)
	{
		if( pResAni->animation_ptr )
		{
			for(int j=0; j<pResAni->animation_ptr->bone_count; j++)
			{
				Bone_AniController* pAniCon = &pResAni->animation_ptr->bone_anicontroller_ptr[j];

				delete []pAniCon->poskey_ptr;
				pAniCon->poskey_ptr = NULL;
				delete []pAniCon->rotkey_ptr;
				pAniCon->rotkey_ptr = NULL;
				delete []pAniCon->scalekey_ptr;
				pAniCon->scalekey_ptr = NULL;
			}
			delete []pResAni->animation_ptr->bone_anicontroller_ptr;
			pResAni->animation_ptr->bone_anicontroller_ptr = NULL;

			if( pResAni->effect_ptr )
			{
				delete []pResAni->effect_ptr;
				pResAni->effect_ptr = NULL;
			}
			if( pResAni->sound_ptr )
			{
				delete []pResAni->sound_ptr;
				pResAni->sound_ptr = NULL;
			}
			if( pResAni->trigger_ptr )
			{
				delete []pResAni->trigger_ptr;
				pResAni->trigger_ptr = NULL;
			}

			delete pResAni->animation_ptr;
			pResAni->animation_ptr = NULL;
		}

		//
		delete pResAni;
		pResAni = NULL;
	}//

	void CRes_Character::Realize()
	{

	}//


};