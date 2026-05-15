#pragma once

#include "RenderObject.h"
#include "EffectRes.h"

// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"

namespace XiahGameEngine
{
	//-------------------------------------------------------------
	// 하나의 캐릭터는 여러개의 SubMesh를 가질 수 있다
	class CCharSubMesh
	{
	public:
		CCharSubMesh(Res_SkinMesh* pSubMesh);
		CCharSubMesh();
		~CCharSubMesh();
		
		BOOL Create(Res_SkinMesh* pSubMesh);
		BOOL SetLodLevel(float fLevel);

		void Clear();
	//protected:
		Res_SkinMesh* m_pResMesh;	// 실제 Mesh데이터

		unsigned short			m_nLodVertex;	// Lod가 적용된 Vertex수
		unsigned short			m_nLodFace;		// Lod가 적용된 Face수
		IDirect3DIndexBuffer9*	m_pIndexBuffer;	// Lod가 적용된 FaceList
	};

#define MAX_CHAR_SUBMESH	10	// Mesh하나가 가지는 SubMes의 최대수

	//-------------------------------------------------------------
	// 한개의 캐릭터당 하나씩 (MeshType에 의해 세팅)
	class CCharMesh
	{
	public:
		XIAHGE_API CCharMesh(Res_Mesh* pMesh);
		XIAHGE_API CCharMesh();
		XIAHGE_API ~CCharMesh();

		BOOL Create(Res_Mesh* pMesh);
		BOOL SetLodLevel(float fLevel);	// 그냥 SubMesh돌면서 호출
		BOOL Clear();

		inline BOOL IsValid()
		{
			if( m_pResMesh == NULL )
				return FALSE;
			else
				return TRUE;
		}

		Res_Mesh* GetResMesh() { return m_pResMesh; }

	//protected:
		Res_Mesh*	m_pResMesh;
		
		int			 m_nSubMesh;	// SubMesh의 수
		CCharSubMesh m_SubMesh[ MAX_CHAR_SUBMESH];
	
		Vector3		m_MeshCenter;
	};

	//-------------------------------------------------------------
	// 단일 캐릭터가 사용하는 texture전체 (XiahPak으로 부터 얻은 Texture의 포인터를 가지고 있다)
	// Release할때 Texture를 Release않함!!
	// XiahPak으로 부터 Texture를 얻어오는 과정은 매우 늘릴때도 있음으로
	// 그릴때마다 얻어오는게 아니라 이처럼 생성될때만 얻어 온다
	
	class CCharTexture
	{
	public:
		CCharTexture(Res_CharTexture *pTexture);
		CCharTexture();
		~CCharTexture();

		BOOL Create(Res_CharTexture* pTexture);
		BOOL Clear();	// 굳이 Texture를 Release할 필요 없음
	//protected:
		Res_CharTexture* m_pResTexture;
	
		IDirect3DTexture9*	m_pTexture[ MAX_CHAR_SUBMESH];
	};

	//-------------------------------------------------------------
	// 모션 Blending이 들어가면 이쪽 모듈이 많이 바뀔 것임
	// 현재는 특별히 하는게 없음
	// 정해진 시간에 해당하는 Animation의 MatrixList만 만들어 줌
	class CCharAnimation
	{
	public:
		CCharAnimation(Res_Skeleton* pSkeleton,Res_Animation* pAnimation);
		CCharAnimation();
		~CCharAnimation();

		BOOL Create(Res_Skeleton* pSkeleton,Res_Animation* pAnimation)
		{
			m_pResSkeleton = pSkeleton;
			m_pResAnimation = pAnimation;

			return TRUE;
		}

		// Animation Blending을 위한 함수
		/*
			현재 Setting된 Animation과 Blending된 Animation을 설정해 준다
		*/
		BOOL SetBlend(Res_Animation* pAnimation,float fStartFrame,float fLength);
		// 요넘이 이뻐야 됨
		// 졸라 인자가 많이 졌따 T_T.
		BOOL BuildMatrix(float pre_frame,
						 float frame,
						 Matrix4x4* pMatrixList,
						 Matrix4x4* pRootMatrix,
						 Matrix4x4* pEffectMatrixList,
						 float fLocalAngle,
						 Matrix4x4* pLocalCenterTM,	
						 CTriggerList *pTriggerList,
						 BOOL bOnlyPositionUpdate = FALSE	// Position만 Update시켜준다
						 );	// 전체 Animation길이를 1로 봤을때의 시간 0 ~ 1사이의 값
		BOOL BuildMatrix_Scale(Vector3 scale,
						float pre_frame,
						 float frame,
						 Matrix4x4* pMatrixList,
						 Matrix4x4* pRootMatrix,
						 Matrix4x4* pEffectMatrixList,
						 float fLocalAngle,
						 Matrix4x4* pLocalCenterTM,	
						 CTriggerList *pTriggerList,
						 BOOL bOnlyPositionUpdate = FALSE	// Position만 Update시켜준다
						 );	// 전체 Animation길이를 1로 봤을때의 시간 0 ~ 1사이의 값
		Matrix4x4 GetRootDelta(float pre_frame,float frame);
		BOOL Clear()
		{
			m_pResAnimation = NULL;
			m_pResSkeleton  = NULL;
			m_pResAnimation_BlendSrc = NULL;

			return TRUE;
		}

	//protected:
		Res_Skeleton*	m_pResSkeleton;
		Res_Animation*	m_pResAnimation;

		Res_Animation*	m_pResAnimation_BlendSrc;
		float			m_fBlend_StartFrame;
		float			m_fBlend_Length;
		float			m_fBlend_CurFrame;
	protected:
		BOOL	EvalBlendAnimation(Res_Animation *pSrc,Res_Animation *pDest,float srcframe,float dstframe,float blend_factor);
		BOOL	EvalAnimation(Res_Animation *pAnimation,float cur_frame);

		Quaternion*		m_pEvalBoneAni_Rot;
		Vector3*		m_pEvalBoneAni_Pos;
		unsigned char*	m_pEvalBoneAni_RotFlip;
	};

	//
	enum eCharRender_TriggerEvent
	{
		eTE_CharRender_EndAnimation = 0,	// Animation의 한 Loop가 끝날때
		eTE_CharRender_EndAlphaEffect,		// Alpha Effect가 끝났을때
		eTE_CharRender_TMUpdate,			// 비등속 운동시 캐릭터의 위치이동값 적용 직전
		eTE_CharRender_Timer,				// Timer Trigger
		eTE_CharRender_Count
	};
	
	//-------------------------------------------------------------

	class cShadow
	{
	public:
		XIAHGE_API BOOL Create();
		XIAHGE_API BOOL Destroy();
		XIAHGE_API BOOL Render(LPDIRECT3DINDEXBUFFER9	m_pIndexBuffer);
		XIAHGE_API BOOL Detect(LPDIRECT3DVERTEXBUFFER9 pbPoints, int numVertex, int numFace);

		XIAHGE_API cShadow();
		XIAHGE_API virtual ~cShadow();

		LPDIRECT3DTEXTURE9      m_pShadowTexture; 
	protected:

		int						m_numvertex;
		int						m_numfaces;

		D3DXMATRIX matLocalToWorld; // [0] 그림자 객체 좌표 -> 월드
		D3DXMATRIX matWorldToLight; // [1] 월드             -> 광원 좌표
		D3DXMATRIX matShadowProj;   // [3] 광원 좌표        -> 텍스쳐 좌표

		BOOL					m_bDetectSize;
		LPD3DXRENDERTOSURFACE   m_pRenderToSurface;
		LPDIRECT3DSURFACE9      m_pShadowSurface;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	};



	//-------------------------------------------------------------
	class CCharRender : public CRenderObject
	{
		DECLARE_RENDERTYPE(eRT_NormalSort)
	public:
		XIAHGE_API CCharRender();
		XIAHGE_API ~CCharRender();

		//YS_0728 : BUGFIX
		XIAHGE_API void Init();
	
		// 아래의 3함수를 써서 캐릭터를 세팅할 수 있다
		XIAHGE_API BOOL SetChar(int char_id);
		XIAHGE_API BOOL SetMesh(int mesh_type,int texture_type);
		XIAHGE_API BOOL SetAnimation(int ani_type,float fAnimationSpeed = 1.0f, bool bStopEffect = true);
		
		XIAHGE_API inline void SetAnimationSpeed(float fAnimationSpeed)
		{
			m_fAnimationSpeed = fAnimationSpeed;
		}
		
		XIAHGE_API BOOL Render(bool bGray = false);			// 실제 그려주기
		//XIAHGE_API BOOL ShadowRender();						// Real Shadow

		// LodSetting및 Animation만들어주기 등등
		XIAHGE_API BOOL PrepareRender(BOOL bOnlyPositionUpdate = FALSE,int AttackType=0,char ground_type = 0);
		
		XIAHGE_API BOOL Clear();

		//XIAHGE_API BOOL SetDetailLevel(float fDetailLevel);	// LOD Level 0 ~ 1사이의 값이다

		XIAHGE_API BOOL IsValid(){ return m_pResCharacter != NULL;}

		XIAHGE_API inline void SetPosition(Vector3 vPos)
		{
			if( m_pCharTM)
				m_pCharTM->t = vPos;
		}

		XIAHGE_API inline void SetPosition(Matrix4x4* pObjectTM)
		{
			m_pCharTM = pObjectTM;

			// 이때 Mesh Effect가 있으면 위치도 같이 세팅.
			if( m_pMeshEffectPackagePair )
			{
				m_pMeshEffectPackagePair->pWorldMatrix = (MATRIX*)m_pCharTM;
				//memcpy( &m_pMeshEffectPackagePair->WorldMatrix, m_pCharTM, sizeof(Matrix4x4) );
			}
		}

		XIAHGE_API BBoxAABB3 GetLocalBound();	// Mesh에 설정된 Bound구하기

		XIAHGE_API float GetLocalAngle(){ return m_LocalAngle;}
		XIAHGE_API BOOL  SetLocalAngle(float fAngle){ return m_LocalAngle = fAngle;}

		XIAHGE_API BOOL SetTrigger(int nEvent,CTrigger* pTrigger)
		{
			return m_TriggerList.SetTrigger( nEvent, pTrigger);
		}

		XIAHGE_API inline void StartAlphaEffect(float fAlphaEffectSpeed=1.0f)
		{
			m_bAlphaEffect		= TRUE;
			m_fAlphaEffect		= 1.0f;
			m_fAlphaEffectSpeed = fAlphaEffectSpeed;
		}

		XIAHGE_API BOOL IsAlphaEffect()
		{
			return m_bAlphaEffect;
		}

		XIAHGE_API BOOL SetAlphaValue(float val);

		XIAHGE_API Matrix4x4* GetChildBoneMatrix(int nLogicalIndex);
		XIAHGE_API BOOL ChangeTexture(int nSubMeshIndex,int res_id);

		XIAHGE_API BOOL EnableSpecularEffect();

		XIAHGE_API inline void EnableGlowEffect(BOOL bTrue, D3DCOLOR glow_color = D3DCOLOR_XRGB( 255, 255, 255))
		{
			m_bGlowEffect	= bTrue;
			m_GlowColor		= glow_color;
		}

		XIAHGE_API BOOL SpawnEffect();
		XIAHGE_API BOOL StopEffect();

		XIAHGE_API inline int GetMeshType()
		{
			if( m_Mesh.IsValid() == FALSE)
				return -1;

			return m_Mesh.m_pResMesh->mesh_type;
		}

		XIAHGE_API inline int GetTextureType()
		{
			if( m_Texture.m_pResTexture == NULL)
				return -1;

			return m_Texture.m_pResTexture->texture_type;
		}

		XIAHGE_API inline int GetCharID()
		{
			if( IsValid() == FALSE)
				return -1;

			return	m_pResCharacter->character_id;
		}

		XIAHGE_API inline void EnableBoneAnimation(BOOL bEnable)
		{
			m_bBoneAnimation = bEnable;
		}

		XIAHGE_API inline void SetLocalCenter(Matrix4x4 centerTM)
		{
			m_LocalCenterTM = centerTM;
		}

		XIAHGE_API inline void SetLocalScale(Vector3 scale)
		{
			m_LocalScale = scale;
		}

		XIAHGE_API int GetTimerTriggerCount();

		XIAHGE_API int SetReverseAnimation() { m_bReverseAnimation = true; return true;}
		XIAHGE_API int SetLoopAnimation(bool bLoop) { m_bLoopAnimation = bLoop; return true;}

		XIAHGE_API float GetAnimationLength();

		XIAHGE_API inline Matrix4x4* GetCharTM()
		{
			return m_pCharTM;
		}

		XIAHGE_API BOOL MakeMeshEffect(int nIndex = 0);
		XIAHGE_API BOOL MakeMeshEffect(int nCount, int *IndexAry);

		XIAHGE_API BOOL ClearMeshEffect();

		XIAHGE_API float Get_AlphaEffect() { return m_fAlphaEffect;	}

		XIAHGE_API inline BOOL IsAlphaBlendTestObject()
		{
			// 캐릭터의 경우, 알파 채널과 스페큘러가 있는데,
			// 현재 스페큘러는 거의 없으므로. 0이 아닌것은 다 알파로 취급
			for(int i=0; i < m_Mesh.m_nSubMesh; ++i)
			{
				if( m_Texture.m_pResTexture->texture_sub_ptr[i].is_alpha != 0 )
					return TRUE;
			}// for

			return FALSE;
		}

		XIAHGE_API CCharMesh GetMesh() { return m_Mesh; }

		XIAHGE_API int GetMeshEffectIndex() { return m_nMeshEffectIndex; }

		XIAHGE_API void SetMaterialDiffuseColor(BOOL bValue, float r, float g, float b);

		// 2004.08.06 Changth
		// 분신격이 뛰니깐 분신이 뛸때도 걷는 소리가 나는데 이때 에러가 난다. 
		// 그래서 분신이 뛸때는 소리가 없게한다.
		XIAHGE_API void EnableAnimationSound(bool bEnable)
		{
			m_bEnableAnimationSound = bEnable;
		}

	protected:
		BOOL SpawnAnimationSound();
		BOOL PlayAnimationSound(int AttackType,char ground_type);
		BOOL ReleaseAnimationSound();
		BOOL RespawnAnimationSound();
		void SetTimeTrigger();


#define MAX_ANIMATION_SOUND	20

		int						m_nAnimationSound;
		// FMOD 적용		
		FSOUND_SAMPLE*			m_pAnimationSoundBufferList[ MAX_ANIMATION_SOUND];

		int						m_nAnimationSoundStartTime[ MAX_ANIMATION_SOUND];
		BOOL					m_bAnimationSoundPlay[ MAX_ANIMATION_SOUND];
		BOOL					m_RightFoot;
		BOOL					m_LeftFoot;

	protected:
		// 이건 캐릭터 렌더링용 매트릭스
		Matrix4x4* m_pMatrixList;	// 현재 동작의 Animation Matrix 매번 변하겠지?
		// 이건 이펙트 연결용 매트릭스
		Matrix4x4* m_pEffectMatrixList;
		
		Matrix4x4	*m_pCharTM;

		CRes_Character* m_pResCharacter;

		Matrix4x4	m_LocalCenterTM;	// 길쭉이 오브젝트같은 넘들의 중심을 잡아주기 위해서!
		Vector3		m_LocalScale;

		CCharTexture   m_Texture;
		CCharMesh      m_Mesh;
		CCharAnimation m_Animation;

		float			m_CurFrame;
		float			m_PreFrame;
		float			m_fAnimationSpeed;

		// Client에서의 어떤 진행 방향과 다르게 Mesh의 방향을 지정하고 싶을때 사용해준다
		float			m_LocalAngle;

		// 캐릭터의 UpdateTime이 Client프로그램의 Update에서 매 프레임 호출이 되지 않을 수도 있기 때문에
		DWORD			m_LastUpdateTime;

		D3DMATERIAL9	m_Material;

		bool					m_bReverseAnimation;
		bool					m_bLoopAnimation;

		// 2004.08.06 Changth
		// 분신격이 뛰니깐 분신이 뛸때도 걷는 소리가 나는데 이때 에러가 난다. 
		// 그래서 분신이 뛸때는 소리가 없게한다.
		bool		m_bEnableAnimationSound;

	public:
		bool					m_bSpawnEffect;
		_EFFECTPACKAGEPAIR*		m_pEffectPackagePair;
		_EFFECTPACKAGEPAIR*		m_pMeshEffectPackagePair;

		int				m_nMeshEffectIndex;

	protected:
		BOOL				m_bAlphaEffect;
		float				m_fAlphaEffect;
		float				m_fAlphaEffectSpeed; // 특정 캐릭터의 알파 이펙트 속도를 조절하기 위해서.

		BOOL				m_bSpecularEffect;
		IDirect3DTexture9*	m_pSpecularTexture;

		BOOL				m_bGlowEffect;
		D3DCOLOR			m_GlowColor;
		
		BOOL				m_bBoneAnimation;

		BOOL				m_bApplyMaterialColor;
		
		// TimeTrigger
		BOOL				m_bTimeTrigger[ 10];

		// Trigger
		CTriggerList			m_TriggerList;
	};
};