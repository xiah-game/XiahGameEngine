#pragma once
#define		MAX_FILE_MEM_BUFFER		(3 * 1024 * 1024)		// 2 Mega byte => Max size of BCF

namespace XiahGameEngine
{
	extern BYTE	g_FileBuffer[ MAX_FILE_MEM_BUFFER ];	// for reading BCF file

	//---------------------------------------------------------
	// Character Mesh 구조체
	struct Res_SkinMesh
	{
		unsigned char	is_physique;
		unsigned long	skin_mesh_data_size;// Client에서 버퍼를 잡고 한번에 읽기 위해.

		unsigned short	vertex_count;		// 버텍스 수
		unsigned short	face_count;			// Face 수

		IDirect3DVertexBuffer9* vertex_ptr;
		unsigned short	*face_ptr;			// Face 리스트 Lod연산을 위해서 이거는 SystemMemory로 만듬

/*
		unsigned short	lod_count;
		unsigned short  *lod_collapse_ptr;	// Lod Collapse리스트		-> Vertex수 만큼
		unsigned short  *lod_facecount_ptr;	// Lod FaceCount수리스트	-> Vertex수 만큼
*/

	};

	//--------------------------------------------------------
    // Skeleton 및 Animation정보

	// 회전 Key
	struct Bone_RotKey
	{
		float frame;			// 전체 Animation에서의 상태적인 frame값 (0~1.0사이값)
		float quaternion[ 4];	// 위치 Quaternion
		float flip;				// mirror여부
	};

	// Position Key
	struct Bone_PosKey
	{
		float frame;			// 전체 Animation에서의 상태적인 frame값 (0~1.0사이값)
		float position[ 3];		// 위치
		
		Bone_PosKey()
		{
			frame = 0.0f;
			position[0] = 0.0f;
			position[1] = 0.0f;
			position[2] = 0.0f;
		}
	};

	// Scale Key
	struct Bone_ScaleKey
	{
		float frame;			// 전체 Animation에서의 상태적인 frame값 (0~1.0사이값)
		float scale[ 3];		// 스케일

		Bone_ScaleKey()
		{
			frame = 0.0f;
			scale[0] = 0.0f;
			scale[1] = 0.0f;
			scale[2] = 0.0f;
		}
	};

	// 하나의 본에 대한 Animation Key들의 모임
	struct Bone_AniController
	{
		unsigned short rotkey_count;		// rotkey 갯수
		unsigned short poskey_count;		// poskey 갯수
		unsigned short scalekey_count;		// scalekey 갯수

		Bone_RotKey		*rotkey_ptr;			// rotkey 리스트
		Bone_PosKey		*poskey_ptr;			// poskey 리스트
		Bone_ScaleKey	*scalekey_ptr;		// scalekey 리스트

		Bone_AniController()
		{
			rotkey_count = 0;
			poskey_count = 0;
			scalekey_count = 0;

			rotkey_ptr = NULL;
			poskey_ptr = NULL;
			scalekey_ptr = NULL;
		}
	};

	// 본객체 (본들끼리의 Hierachy 및 기본 Figure Matrix)
	struct Bone_Object
	{
		unsigned short	parent_bone;				// 부모 Bone의 Index
		float			offset_matrix[ 16];			// 부모 Bone과의 상대 Matrix !!!
		Bone_Object()
		{
			parent_bone = 0;
			for(int i=0;i<16;i++) offset_matrix[i] = 0;
		}
	};

	// 뼈대 객체
	struct Res_Skeleton
	{
		unsigned long  skeleton_data_size;	// Client에서 버퍼를 잡고 한번에 읽기 위해. 현재 이 변수의 사이즈는 제외되었다.
		unsigned short bone_count;			// 본의수
		Bone_Object*   bone_ptr;			// 본 리스트 (이 리스트안에서의 Index가 Bone의 parent_bone의 값으로 사용된다)
		Matrix4x4*	   world_matrix;
		Matrix4x4*	   world_inv_matrix;

		Res_Skeleton()
		{
			skeleton_data_size = 0;
			bone_count = 0;
			bone_ptr = NULL;
			world_matrix = NULL;;
			world_inv_matrix = NULL;;
		}
	};

	// 뼈대 Animation
	struct Res_Skeleton_Animation
	{
		unsigned long		skeleton_ani_data_size;// Client에서 버퍼를 잡고 한번에 읽기 위해.
		unsigned short		time_length;			// Animation전체 길이 ( 시간 단위: ms (1/1000초))
		unsigned short		bone_count;			// 본의 수
		Bone_AniController* bone_anicontroller_ptr;	// 본 Ani Controll 리스트

		Res_Skeleton_Animation()
		{
			skeleton_ani_data_size = 0;
			time_length = 0;
			bone_count = 0;
			bone_anicontroller_ptr = NULL;
		}
	};

	//----------------------------------------------
	// Character Studio 에서 설정된 정보들
	// 하나의 SubMeshBlock에 연결되는 TextureMap 정보
	struct Res_CharTexture_Sub
	{
		int				texture_id;		// Resource ID (TetxureID)
		unsigned char	is_alpha;		// Alpha Blending이면 1 아니면 0
		unsigned char	cull_mode;		// Backface Culling이 들어가면 1 아니면 0

		Res_CharTexture_Sub()
		{
			texture_id = 0;
			is_alpha = 0;
			cull_mode = 0;
		}
	};

	// SubMesBlock에 연결되는 TextureMap들의 모임
	struct Res_CharTexture
	{
		unsigned short			texture_type;		// texture type
		unsigned short			texture_sub_count;	// texture sub 갯수
		Res_CharTexture_Sub*	texture_sub_ptr;	// texture sub 리스트
		
		Res_CharTexture()
		{
			texture_type = 0;
			texture_sub_count = 0;
			texture_sub_ptr = NULL;
		}
	};

	// 애니메이션에 연결된 이펙트 정보 
	struct Res_AniEffect
	{
		int					nEffectID;
		__int8				nPosX;
		__int8				nPosY;
		__int8				nPosZ;
		__int8				nBoneIndex;
		unsigned short		nStartTime;

		Res_AniEffect()
		{
			nEffectID = 0;
			nPosX = 0;
			nPosY = 0;
			nPosZ = 0;
			nBoneIndex = 0;
			nStartTime = 0;
		}
	};

	// 애니메이션에 연결된 사운드 정보.
	struct Res_AniSound
	{
		int			nSoundID;		// ANIMATION2_SOUND table nID
		int			nEffectDBID;

		Res_AniSound()
		{
			nSoundID = 0;
			nEffectDBID = 0;
		}
	};
	
	// 애니메이션에 연결된 Timer Trigger 정보.
	struct Res_TimerTrigger
	{
		unsigned short		nType;
		unsigned short		nStartTime;
		unsigned short		nLength;
	};

	//-------------------------------------------------------
	// this is XiahMesh in character studio2
	struct Res_Mesh
	{
		FILE *m_hFileHandle;

        unsigned short		mesh_type;					// mesh type
		unsigned short		texture_count;				// texture 수
		Res_CharTexture*	texture_ptr;				// texture 리스트

		unsigned short		mesh_size[ 3];				// mesh크기

		unsigned short		logical_bone_index_count;	// bone index 수
		unsigned short*		logical_bone_index_ptr;		// bone index리스트

		unsigned short		effect_count;				// Mesh에 연결된 이펙트 수, 아이템 이펙트
		unsigned long		effect_data_size;
		Res_AniEffect*		effect_ptr;

		unsigned short		skin_mesh_count;
		//
		Res_SkinMesh*		skin_mesh_ptr;				// 실제 정보

		Res_Skeleton*		skeleton_ptr;				// 실제 skeleton정보

		unsigned long		skin_mesh_offset;
		unsigned long		skeleton_offset;

		Res_Mesh()
		{
			m_hFileHandle = NULL;
			mesh_type = 0;
			texture_count = 0;
			texture_ptr = NULL;

			mesh_size[0] = 0;
			mesh_size[1] = 0;
			mesh_size[2] = 0;

			logical_bone_index_count = 0;
			logical_bone_index_ptr = NULL;

			effect_count = 0;
			effect_data_size = 0;
			effect_ptr = NULL;

			skin_mesh_count = 0;

			skin_mesh_ptr = NULL;
			skeleton_ptr = NULL;

			skin_mesh_offset = 0;
			skeleton_offset = 0;
		}

		// func
		void Realize()	// Load SkinMesh and Skeleton data from file when called
		{
			if( skin_mesh_ptr == NULL )
			{
				// Move Res_SkinMesh data position
				fseek( m_hFileHandle, skin_mesh_offset, SEEK_SET );

				skin_mesh_ptr = new Res_SkinMesh [skin_mesh_count];
				for(int i=0; i<skin_mesh_count; i++)
				{
					Res_SkinMesh* pSkinMesh = &skin_mesh_ptr[i];

					fread( &pSkinMesh->skin_mesh_data_size, sizeof(pSkinMesh->skin_mesh_data_size), 1, m_hFileHandle );
					DWORD dwBufPos = 0;
					// 크기만큼 왕창 읽고, 버퍼에서 데이타를 가져오자.
					fread( g_FileBuffer, pSkinMesh->skin_mesh_data_size, 1, m_hFileHandle );

					//
					memcpy( &pSkinMesh->is_physique, &g_FileBuffer[dwBufPos], sizeof(pSkinMesh->is_physique) );
					dwBufPos += sizeof(pSkinMesh->is_physique);

					memcpy( &pSkinMesh->vertex_count, &g_FileBuffer[dwBufPos], sizeof(pSkinMesh->vertex_count) );
					dwBufPos += sizeof(pSkinMesh->vertex_count);

					memcpy( &pSkinMesh->face_count, &g_FileBuffer[dwBufPos], sizeof(pSkinMesh->face_count) );
					dwBufPos += sizeof(pSkinMesh->face_count);

					//
					HRESULT hr;

					// Physique이 들어 있으면 vertex type 에러가 나서 하드웨어 가속을 받을 수 없다.
					// 즉, 피직이 있으면 무조건 D3DPOOL_SYSTEMMEM 이다. 
					DWORD dwVertexSize;
					if( pSkinMesh->is_physique )
					{
						dwVertexSize = sizeof(SkinVertex) * pSkinMesh->vertex_count;

						// 윽.. ㅠㅠ 눈물을 머금고,,,
						hr = g_pDirect3DDevice->CreateVertexBuffer( dwVertexSize, 0, FVF_SKINVERTEX, D3DPOOL_SYSTEMMEM, &pSkinMesh->vertex_ptr, NULL);
						
						if( FAILED( hr))
							throw _T("Create Character Vertex Buffer Fail");
					}
					else
					{
						dwVertexSize = sizeof(VT_Normal) * pSkinMesh->vertex_count;

						// 모두 D3DPOOL_MANAGED 닷!
//						hr = g_pDirect3DDevice->CreateVertexBuffer( dwVertexSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_DEFAULT, &pSkinMesh->vertex_ptr, NULL);
						hr = g_pDirect3DDevice->CreateVertexBuffer( dwVertexSize, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_MANAGED, &pSkinMesh->vertex_ptr, NULL);

						if( FAILED( hr))
							throw _T("Create Character Vertex Buffer Fail");
					}

					SkinVertex *pVertexData;

					// 다이내믹으로 만든것은 데이터가 AGP 메모리로 올라가는데, 쓸것을 바로 만들어서 없애는 형식을 쓴다. 
					// AGP 메모리가 한정되어 있어서 데이터를 저장하지 않고 현재 필요한 데이타를
					// Lock, Unlock 해서 만들어서 쓰고 D3DLOCK_DISCARD 해주면 쓴 다음에 지운다.
					// 만약 첨에 한번만 Lock해서 만들고 바꾸지 않고 계속 쓴다면 D3DLOCK_NOOVERWRITE해준다.
					hr = pSkinMesh->vertex_ptr->Lock( 0, 0, (void **)&pVertexData, 0 );

					if( FAILED( hr))
						throw _T("Character Vertex Buffer Lock Fail");

					memcpy( pVertexData, &g_FileBuffer[ dwBufPos], dwVertexSize);

					// 이건 뭐지?
/*
					for(int j = 0; j < pSkinMesh->vertex_count; j++)
					{
						pVertexData->weight[ 3] = 0;
						pVertexData->indices = pVertexData->indices & 0x00FFFFFF;

						pVertexData++;
					}
*/

					pSkinMesh->vertex_ptr->Unlock();
					
					dwBufPos += dwVertexSize;

					//
					pSkinMesh->face_ptr = new WORD [ 3 * pSkinMesh->face_count ];
					memcpy( pSkinMesh->face_ptr, &g_FileBuffer[dwBufPos], sizeof(WORD)*3*pSkinMesh->face_count );
					dwBufPos += sizeof(WORD)*3*pSkinMesh->face_count;

					// 이제 LOD 안쓴다.
/*
					memcpy( &pSkinMesh->lod_count, &g_FileBuffer[dwBufPos], sizeof(pSkinMesh->lod_count) );
					dwBufPos += sizeof(pSkinMesh->lod_count);

					pSkinMesh->lod_collapse_ptr = new WORD [ pSkinMesh->vertex_count ];
					memcpy( pSkinMesh->lod_collapse_ptr, &g_FileBuffer[dwBufPos], sizeof(WORD)*pSkinMesh->vertex_count );
					dwBufPos += sizeof(WORD)*pSkinMesh->vertex_count;

					pSkinMesh->lod_facecount_ptr = new WORD [ pSkinMesh->vertex_count ];
					memcpy( pSkinMesh->lod_facecount_ptr, &g_FileBuffer[dwBufPos], sizeof(WORD)*pSkinMesh->vertex_count );
					dwBufPos += sizeof(WORD)*pSkinMesh->vertex_count;
*/

				}// for(skin_mesh_count)

			}// if( skin_mesh_ptr == NULL )

			if( skeleton_ptr == NULL )
			{
				// Res_Skeleton data position
				fseek( m_hFileHandle, skeleton_offset, SEEK_SET );

				skeleton_ptr = new Res_Skeleton;

				fread( &skeleton_ptr->skeleton_data_size, sizeof(skeleton_ptr->skeleton_data_size), 1, m_hFileHandle );
				DWORD dwBufPos = 0;
				// 크기만큼 왕창 읽고, 버퍼에서 데이타를 가져오자.
				fread( g_FileBuffer, skeleton_ptr->skeleton_data_size, 1, m_hFileHandle );

				//
				memcpy( &skeleton_ptr->bone_count, &g_FileBuffer[dwBufPos], sizeof(skeleton_ptr->bone_count) );
				dwBufPos += sizeof(skeleton_ptr->bone_count);

				skeleton_ptr->bone_ptr = new Bone_Object[ skeleton_ptr->bone_count ];
				memcpy( skeleton_ptr->bone_ptr, &g_FileBuffer[dwBufPos], sizeof(Bone_Object)*skeleton_ptr->bone_count );
				dwBufPos += sizeof(Bone_Object)*skeleton_ptr->bone_count;
			
				skeleton_ptr->world_matrix = new Matrix4x4[ skeleton_ptr->bone_count];
				skeleton_ptr->world_inv_matrix = new Matrix4x4[ skeleton_ptr->bone_count];

				// Skeleton데이터의 Matrix만들어 주기
				for( int i = 0; i < skeleton_ptr->bone_count; i++)
				{
					memcpy( &skeleton_ptr->world_matrix[ i], skeleton_ptr->bone_ptr[ i].offset_matrix, sizeof( float) * 16);

					if( skeleton_ptr->bone_ptr[ i].parent_bone != 0xffff)
					{
						skeleton_ptr->world_matrix[ i] *= skeleton_ptr->world_matrix[ skeleton_ptr->bone_ptr[ i].parent_bone];
						skeleton_ptr->world_inv_matrix[ i] = skeleton_ptr->world_matrix[ i].GetInverse();
					}
				}

			
			}// if( skeleton_ptr == NULL )
		}// void Realize()

		XIAHGE_API Res_CharTexture* GetTexture(int texture_type)
		{
			for(int i=0; i<texture_count; i++)
			{
				if( texture_ptr[i].texture_type == texture_type )
					return &texture_ptr[i];
			}// for

			return NULL;
		}
	};

	// XiahAnimation in character studio2
	struct Res_Animation
	{
		FILE *m_hFileHandle;

        unsigned short				animation_type;			// animation type
		Res_Skeleton_Animation		*animation_ptr;			// 실제 animation 정보

		// 현재 애니메이션에 연결된 이펙트 정보.
		unsigned short		effect_count;
		unsigned long		effect_data_size;			// effect_ptr 데이타 사이즈. + int
		int					effect_char_id;
		Res_AniEffect*		effect_ptr;

		// 현재 애니메이션에 연결된 사운드 정보. 
		unsigned long		sound_data_size;			// sound_ptr data size
		unsigned short		sound_count;
		Res_AniSound*		sound_ptr;

		// 현재 애니메이션에 연결된 Timer Trigger 정보.
		unsigned long		trigger_data_size;			// trigger_ptr data size
		unsigned short		trigger_count;
		Res_TimerTrigger*	trigger_ptr;

		// Package를 저장할때 사용할 Offset(실제 데이터가 존재하는 파일의 위치)
		unsigned long				animation_offset;

		Res_Animation()
		{
			m_hFileHandle = NULL;

			animation_type = 0;
			animation_ptr = NULL;			// 실제 animation 정보

			effect_count = NULL;
			effect_data_size = NULL;
			effect_char_id;
			effect_ptr = NULL;

			sound_data_size = 0;
			sound_count = 0;
			sound_ptr = NULL;

			trigger_data_size = 0;
			trigger_count = 0;
			trigger_ptr = NULL;

			animation_offset = 0;
		}

		// func
		void Realize()
		{
			if( animation_ptr == NULL )
			{
				memset(g_FileBuffer,0,sizeof(g_FileBuffer));

				// Move Res_Skeleton_Animation data position
				fseek( m_hFileHandle, animation_offset, SEEK_SET );

				animation_ptr = new Res_Skeleton_Animation;
				fread( &animation_ptr->skeleton_ani_data_size, sizeof(animation_ptr->skeleton_ani_data_size), 1, m_hFileHandle );

				// 크기만큼 왕창 읽고, 버퍼에서 데이타를 가져오자.
				DWORD dwBufPos = 0;
				fread( g_FileBuffer, animation_ptr->skeleton_ani_data_size, 1, m_hFileHandle );

				// 
				memcpy( &animation_ptr->time_length, &g_FileBuffer[dwBufPos], sizeof(animation_ptr->time_length) );
				dwBufPos += sizeof(animation_ptr->time_length);

				memcpy( &animation_ptr->bone_count, &g_FileBuffer[dwBufPos], sizeof(animation_ptr->bone_count) );
				dwBufPos += sizeof(animation_ptr->bone_count);

				animation_ptr->bone_anicontroller_ptr = new Bone_AniController[ animation_ptr->bone_count ];
				for(int i=0; i<animation_ptr->bone_count; i++ )
				{
					Bone_AniController* pAniCon = &animation_ptr->bone_anicontroller_ptr[i];

					// pos key
					memcpy( &pAniCon->poskey_count, &g_FileBuffer[dwBufPos], sizeof(pAniCon->poskey_count) );
					dwBufPos += sizeof(pAniCon->poskey_count);

					pAniCon->poskey_ptr = new Bone_PosKey[ pAniCon->poskey_count ];
					memcpy( pAniCon->poskey_ptr, &g_FileBuffer[dwBufPos], sizeof(Bone_PosKey)*pAniCon->poskey_count );
					dwBufPos += sizeof(Bone_PosKey)*pAniCon->poskey_count;

					// rot key
					memcpy( &pAniCon->rotkey_count, &g_FileBuffer[dwBufPos], sizeof(pAniCon->rotkey_count) );
					dwBufPos += sizeof(pAniCon->rotkey_count);

					pAniCon->rotkey_ptr = new Bone_RotKey[ pAniCon->rotkey_count ];
					memcpy( pAniCon->rotkey_ptr, &g_FileBuffer[dwBufPos], sizeof(Bone_RotKey)*pAniCon->rotkey_count  );
					dwBufPos += sizeof(Bone_RotKey)*pAniCon->rotkey_count;

					// scale key
					memcpy( &pAniCon->scalekey_count, &g_FileBuffer[dwBufPos], sizeof(pAniCon->scalekey_count) );
					dwBufPos += sizeof(pAniCon->scalekey_count);

					pAniCon->scalekey_ptr = new Bone_ScaleKey[ pAniCon->scalekey_count ];
					memcpy( pAniCon->scalekey_ptr, &g_FileBuffer[dwBufPos], sizeof(Bone_ScaleKey)*pAniCon->scalekey_count );
					dwBufPos += sizeof(Bone_ScaleKey)*pAniCon->scalekey_count;
				}// for(animation_ptr->bone_count)

			}// if
		}//void Realize()
	};


	//-------------------------------------------------
	class CRes_Character
	{
	public:
		CRes_Character();
		~CRes_Character();

		// variable
		int				character_id;					// character id
		unsigned short	mesh_count;						// mesh수
		unsigned short	ani_count;						// animation수

		typedef std::hash_map<int, Res_Mesh*> MESHLIST;
		typedef std::hash_map<int, Res_Animation*> ANIMATIONLIST;

		MESHLIST		MeshList;
		ANIMATIONLIST	AnimationList;

		// func
		XIAHGE_API Res_Mesh*		GetMesh(int nMeshType);
		Res_Animation*	GetAnimation(int nAniType);

		void Realize();

		void Release();	// free memory of character data
		void ReleaseMeshAll();
		void ReleaseMesh(int nMeshType);
		void ReleaseMesh(Res_Mesh* pResMesh);
		void ReleaseAnimationAll();
		void ReleaseAnimation(int nAniType);
		void ReleaseAnimation(Res_Animation* pResAni);

	};

   
	typedef std::hash_map<int,CRes_Character*> CHARACTERLIST;
	typedef std::vector<FILE*> FILEHANDLELIST;

	//
	extern CHARACTERLIST		g_CharacterList;		// character data from package
	extern FILEHANDLELIST		g_FileHandleList;		// Package File Handle List


	// Load Xiah Character Package FIle => only package header
	extern XIAHGE_API BOOL InitializeCharacter(LPCTSTR *pPakList, int nCount);

	// free memory of character data
	extern XIAHGE_API void ReleaseCharacterAll();

	// close character file
	extern XIAHGE_API void CloseCharacterPakFileAll();

	extern XIAHGE_API CRes_Character* GetCharacter(int nCharID);
};


