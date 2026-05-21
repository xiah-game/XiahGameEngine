//*********************************************************//
//		Effect Module version 1.04 Xiah Client Version	   //
//		Date 2003-09-04									   //
//		By AceChangth, Jae Woon Chang					   //
//*********************************************************//
#pragma once

#ifndef __EffectRes_H
#define __EffectRes_H

//#include <vector>
//#include "ResIndex.h"
//#include "AnimationSound.h"

#include "EffectCommonDef.h"


namespace XiahGameEngine
{

#pragma warning( disable: 4018 )
#pragma warning( disable: 4127 )
#pragma warning( disable: 4238 )
#pragma warning( disable: 4239 )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4267 )


#define		NEW_EFFECT_PATH		_T("")


#define		ELEMENT_INIT_SIZE	0.8f


// -- Enum Type
enum EFFECT_LEVELUP_TYPE
{
	LEVELUP_GAPJA,
	LEVELUP_TP,
	LEVELUP_OUTGONG,
	LEVELUP_INGONG
};

// ------- Type and Struct ---------
/*
	Effect Resource Data
    NOT!!! Instance Rendered Data
*/

enum eParticleElementType
{
	PET_Billboard,
	PET_Mesh,
	PET_Count
};

/*
	모든 Effect는 다같이 Light를 먹어 준다
*/
enum eParticleElementRenderType
{
	PERT_Normal,	// No Alpha Blending
	PERT_Black,		// Specular
	PERT_White,		// White Alpha (Inverse Specular)
	PERT_AlphaChannel,

	PERT_Count
};

enum eParticleElementLifeTimeType
{
	PELTT_LifeTime,
	PELTT_Custom
};

enum eElementSpawnType
{
	EST_One,		// 폭팔 Effect같이 한번 나오고 마는것
	EST_Multi,		// 시간에 따라서 계속 나오는것
	EST_Count,
};

enum eParticleLifeTimeType
{
	PLTT_OnlyOne,	// 폭팔같이 하나만들었다가 그거 끝나면 같이 죽는거.
	PLTT_LifeTime,	// 특정 시간이 될때까지
	PLTT_Permanent
};

enum eParticleSpawnVolumeType
{
	PSVT_Point,
	PSVT_Box,
	PSVT_Sphere
};

enum eParticleSpawnPositionAtVolumeType
{
	PSPAVT_OnlyOutSide,
	PSPAVT_InVolume,
	PSPAVT_Custom
};

enum eParticleSpawnDirectionType
{
	PSDT_ToOut,
	PSDT_InOut,
	PSDT_Custom
};

enum eRenderBlend
{
	RB_ZERO,
	RB_ONE,
	RB_SRCCOLOR,
	RB_INVSRCCOLOR,
	RB_SRCALPHA,
	RB_INVSRCALPHA,
	RB_DESTALPHA,
	RB_INVDESTALPHA,
	RB_DESTCOLOR,
	RB_INVDESTCOLOR,
	RB_SRCALPHASAT,
};

enum eRenderBlendOP
{
	RBO_ADD,
	RBO_SUBTRACT,
	RBO_REVSUBTRACT,
	RBO_MIN,
	RBO_MAX
};

enum eTextureBlendOP
{
	TB_DISABLE,
	TB_SELECTARG1,
	TB_SELECTARG2,
	TB_MODULATE,
	TB_MODULATE2X,
	TB_MODULATE4X,
	TB_ADD,
	TB_ADDSIGNED,
	TB_ADDSIGNED2X,
	TB_SUBTRACT,
	TB_ADDSMOOTH,
	TB_BLENDDIFFUSEALPHA,
	TB_BLENDTEXTUREALPHA,
	TB_BLENDFACTORALPHA,
	TB_BLENDTEXTUREALPHAPM,
	TB_BLENDCURRENTALPHA,
	TB_DOTPRODUCT3,
	TB_MULTIPLYADD,
	TB_LERP
};

enum eTextureBlendArg
{
	TBA_CURRENT,
	TBA_DIFFUSE,
	TBA_TEXTURE,
};

enum eHitEnum
{
	eGumYung,
	eYunrang,
	eYager,
	eTuo,
	eGyu,
	eWestGwyin,
	eHagolgwuy,
	eSagal,
	eYuoma,
	eToChung,
	eAlrue,
	eBakranggyun,
	eGwainggyun,
	eGumwawa,
	eGumgunsujang,
	eMadoninja,
	eMangho,
	eBingjo,
	eGunyeja,
	eNwyhwa,
	eBackangjamsi,
	eMooToo,
	eYoihee,
	eHksabong,
	eJuparyuong,
	eHwanyu,
	eNwysin,
	eBumado,
	eGungon,
	eBakho,
	eJaso,
	eGwanhungin,
	eHaegolSerize,
	eBantankangki_Hit,
	eSantaGwanHung,
	eShinjo,
	eYagon,
	eChunshinsulsa,
	eJinmoin,
	ePyo,
	eGonlyeongja,
	eYacha,
	eGyuryuong,
	eRedTiger,
	eMetalMonster,
	eMouseMonster,
	eTreeMonster,
	eFireballTiger,
	eArmorGiant,
	eGolem,
	eAdultAttack1,
	eAdultAttack2,
	eAdultAttack3,
	eFireMonster1,
	eWaterMonster1,
	eTreeMonster1,
	eMetalMonster1,
	eEarthMonster1,

	// 검영, 무투 특화 무공 히트 이펙트
	eGumyongSpecial,	
	eMutuSpecial,

	eHitEnumMax
};

enum eAppearEnum	// 한번 나오는 이펙트
{
	eSmall, eMiddle, eBig,						// NPC 등장 이펙트
	eItemGround,								// 바닥 아이템 등장(?) 이펙트
	eMulYak_HP, eMulYak_IP, eMulYak_HPIP,		// 물약 이펙트
	eTeleport,									// 텔레보트 이펙트
	eShinjo_Explode,							// 신조가 죽을때 폭발 이펙트, 얘는 disappear
	eRebirthItem1,								// 각성 아이템
	eRebirthItem2,
	eRebirthItem3,
	eRebirthItem4,
	eRebirthItem5,
	eRebirthItem6,

	// 이벤트 아이템을 사용할때 발동 이펙트
	eAttackKindItem_start,
	eGrowthKindItem_start,
	eMonsterKindItem_start,
	eEconomiKindItem_start,

	eAppearEnumMax
};

enum eOutGongPersistEnum	// 애초에 지속 무공만 하기로 했는데, 필요한거 다 넣자^^
{
	// 검영
	eMusuhon,
	ePoksahon,
	eKuymgangruk,
	eIlyuidogang,	// 일위도강

	// 연랑
	eLeekwangum_heal_recv,
	eJunuoum_recv,
	eYuenoyueng,
	eYuenoyueng_recv,
	eLeetasaeng_damage,
	eKyugamsu,
	eKyugamsu_recv,
	eWonkisingang_recv,
	eMihonsul_recv,
	eWonkisingang,
	eHwansoou_appear,
	eYuesusinyung,	// 유수신영

	// 무투
	eBantankangki,
	eJukwonkangki,
	ePachunso_recv,
	eKumnasu_recv,
	eAmhukmu_recv,
	eTalbacin_recv,
	eMarulkak_recv,
	eJilpungbo,		// 질풍보

	// 야차
	eChosangbi,
	eOdokchim,
	eDokmu,
	eDokhyulgong,
	eDoknaegong,
	eSsangdosu,
	eMandokbuljin,

	// 오행
	eFEPrepareFire,
	eFEPrepareWater,
	eFEPrepareTree,
	eFEPrepareMetal,
	eFEPrepareEarth,
	eFEFire,
	eFEWater,
	eFETree,
	eFEMetal,
	eFEEarth,
	eFEFire1,
	eFEWater1,
	eFETree1,
	eFEMetal1,
	eFEEarth1,
	eFEFire2,
	eFEWater2,
	eFETree2,
	eFEMetal2,
	eFEEarth2,
	eFEFire3,
	eFEWater3,
	eFETree3,
	eFEMetal3,
	eFEEarth3,

	ePotionBegine,
	ePotion,

	eSpiritBegine,
	eSpirit,

	// 기타 지속 이펙트
	// 이벤트 아이템.
	eAttackKindItem_keepup,
	eGrowthKindItem_keepup,
	eMonsterKindItem_keepup,
	eEconomiKindItem_keepup,

	//HT_0524 각성 외공 무공 대상 이펙트
	eWha_Dragon,
	eBing_Dragon,
	eDok_Dragon,
	eNoi_Dragon,

	// 연랑 특화 무공 지속 이펙트
	eYunrangSpecial,

	eYunSajangsingong,
	eMuKihubkangki,
	eYaEunsinsul,
	eYaHojungkangki,

	eOutGongPersistEnumMax,  //외공 지속 이펙트 MAX  
};

// vertex, mesh and texture struct
struct _TLVERTEX
{
	float		x, y, z, Rhw;
	D3DCOLOR	Specular, Diffuse;
	float		tu, tv;
};
#define		D3DFVF_TLVERTEXDS		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

struct _ELEMENTVERTEX	// Mesh
{
	VECTOR Position;
	VECTOR Normal;
	float tu, tv;
};

struct _ELEMENTVERTEX2	// Billboard
{
	VECTOR Position;
	VECTOR Normal;
	D3DCOLOR diffuse;
	float tu, tv;
};

struct _FACEDATA
{
	short v[3];
};

struct _MESHBUFFER
{
	_MESHBUFFER()
	{
		pVertex = NULL;
		pFace = NULL;
		pVB = NULL;
		pIB = NULL;
	}
	~_MESHBUFFER()
	{
		if( pVertex ) delete []pVertex;
		if( pFace ) delete []pFace;
		if( pVB ) pVB->Release();
		if( pIB ) pIB->Release();
	}

	int					nVertexCount;
	int					nFaceCount;
	int					nTexID;

	LPDIRECT3DVERTEXBUFFER9		pVB;
	LPDIRECT3DINDEXBUFFER9		pIB;

	_ELEMENTVERTEX*		pVertex;
	_FACEDATA*			pFace;
};

struct _EFFECTTEXTURE
{
	_EFFECTTEXTURE()
	{
		m_pTexture = NULL;
		m_nAllocSize = 0;
		m_nRefCount = 0;
	}
	~_EFFECTTEXTURE()
	{
//		Xiah Game Engine에서 Release해준다.
//		if( m_pTexture ) m_pTexture->Release();
		m_pTexture = NULL;
	}

	LPDIRECT3DTEXTURE9		m_pTexture;
	MyString				m_strFileName;
	int						m_nAllocSize;
	int						m_nRefCount;
};
typedef std::map<MyString, _EFFECTTEXTURE *> EFFECTTEXTUREMAP;

struct _EFFECTMESH
{
	_EFFECTMESH()
	{
		m_nAllocSize = 0;
		m_nRefCount = 0;
		m_pMeshBuffer = NULL;
	}
	~_EFFECTMESH()
	{
		if( m_pMeshBuffer )
		{
			delete []m_pMeshBuffer;
			m_pMeshBuffer = NULL;
		}
	}

	MyString				m_strFileName;
	int						m_nMeshCount;
	int						m_nFrameCount;
	int						m_nAllocSize;
	int						m_nRefCount;

	_MESHBUFFER*			m_pMeshBuffer;
};
typedef std::map<MyString, _EFFECTMESH *> EFFECTMESHMAP;


// data struct
struct EFFECTDATA
{
public:
	EFFECTDATA(){};
	EFFECTDATA( float a,float b){ fTime = a;fValue = b;};

	float fTime;
	float fValue;
};
typedef std::list<EFFECTDATA> EFFECTDATALIST;


struct _ELEMENT
{
	int			m_Type;
	MyString	m_MeshPath;
	MyString	m_TexturePath;

	//--------- Texture
	int	m_WrapCount_X;
	int	m_WrapCount_Y;

	EFFECTDATALIST	m_TexIndex;
	
	//--------- Rendering Option
	int m_RenderType;
	EFFECTDATALIST	m_Opacity;

	//--------- Local Translation
	EFFECTDATALIST	m_Rotation[ 3];	// x. y, z
	EFFECTDATALIST	m_Size[ 3];

	//--------- Life Time
	float	m_LifeTime;
	int		m_LifeTimeType;

	BOOL	m_bCullMode;
	BOOL	m_bCenterMove;

	// Element Blend Option
	DWORD		m_nRenderSrc;
	DWORD		m_nRenderDest;
	DWORD		m_nRenderOp;
	DWORD		m_nTextureCOP;
	DWORD		m_nTextureCA1;
	DWORD		m_nTextureCA2;
	DWORD		m_nTextureAOP;
	DWORD		m_nTextureAA1;
	DWORD		m_nTextureAA2;

	// 2003-09-02 New Option
	BOOL		m_bEmitter;		// 이펙트 에디터에서 나가는 방향이 매트릭스에 고정된다. 

};
typedef std::list<_ELEMENT *> ELEMENTLIST;


struct _PARTICLE
{
	ELEMENTLIST m_ElementList;

	//--------- Translation
	VECTOR			m_Position;	// Local Position From Effect
	EFFECTDATALIST	m_Rotation[ 3];

	//--------- Element Spawning
	int		m_LifeTimeType;
	DWORD	m_LifeTime;
	DWORD	m_DelayTime;
	BOOL	m_bForceFirst;

	int		m_ElementSpawnType;			// Element생성 종류
	int		m_ElementSpawnCount;
	int		m_ElementSpawnCountBy;		// 한번에 몇개?
	DWORD	m_ElementSpawnDelay;		// Element생성 시간차

	VECTOR			m_SpawnDirection;
	EFFECTDATALIST	m_SpawnSpeed;		// Element를 처음 만들때 초기 속도를 준다. 방향은 따로
	
	int				m_SpawnVolumeType;	// Element의 생성 위치모양
	EFFECTDATALIST	m_SpawnVolumeSize[3];// PointType에서는 무시되겠지?

	int		m_SpawnPositionAtVolumeType;
	int		m_SpawnDirectionType;

	EFFECTDATALIST	m_Gravity[3];
};
typedef std::list<_PARTICLE *> PARTICLELIST;

//typedef std::list<CWaterfall *> WATERFALLLIST;
//typedef std::list<CThunder *> THUNDERLIST;

class CEffectLight;

typedef std::list<CEffectLight *> LIGHTLIST;

typedef std::list<VECTOR> VECTORLIST;

typedef std::map<MyString, int> STRINGINTLIST;

typedef std::map<MyString, DWORD> STRINGDWORDLIST;

enum EffectType
{
	eCharacterEffect, eTileEffect, eExpAcquireEffect, eCharacterEffect2, eCharacterEffect3
};

struct _EFFECT	// 이펙트의 원본 데이터, 모든 이펙트 포인터는 이것을 참조한다.
{
	_EFFECT()
	{
		m_nLightIndex = 2;
		m_bLoadResource = false;
		m_TextureResIDList.clear();
		m_MeshList.clear();
	}

	// list
	PARTICLELIST	m_ParticleList;

	// other list...
//	WATERFALLLIST	m_WaterfallList;
//	THUNDERLIST		m_ThunderList;
	LIGHTLIST		m_LightList;

	// data
	EFFECTDATALIST	m_Position[ 3];	
	EFFECTDATALIST	m_Rotation[ 3];
	EFFECTDATALIST	m_Opacity;		// 이펙트 전체 투명도. 각 엘리먼트의 material, diffuse에 영향을 준다.

	MyString		m_EffectName;
	DWORD			m_EffectID;			// Effect Editor Management ID
	DWORD			m_EffectManageID;	// total ref id
	DWORD			m_LifeTime;
	DWORD			m_DBID;				// NEW_EFFECT_INFO table id
	int				m_nLightIndex;		// Light Index. start number is 2

	//  Xiah에서는 캐릭터 이펙트와 타일 이펙트로 분류가 되는데, 이것을 분류해야 어떤 파일에서 partial loading
	// 할지를 결정할 수 있다.
	__int8			m_nEffectType;

	// in Xiah, 텍스쳐와 메시는 partial loading이므로 이 안에 데이터가 존재해야한다.
	bool				m_bLoadResource;
	STRINGINTLIST		m_TextureResIDList;
	STRINGDWORDLIST		m_MeshList;

	// More Option
	// 1. Repeat
	// 2. Target Move => 목표 지점으로 이동하는 기능인데, 툴에서는 직접 설정치를 넣어주고, 
	//	  클라이언트에서는 내부적으로 직접 데이터를 설정해 준다. 
	// 3. Trace Move => 궤적 이동인데, 툴에서 ASE파일에서 경로를 읽어서 VECTOR 리스트에 넣고
	//	  이것을 이펙트 파일에 저장을 시켜놓는다.
	BOOL			m_bRepeat;
	BOOL			m_bTraceMove;
	float			m_fTraceMoveSpeed;
	float			m_fTargetMoveSpeed;
	BOOL			m_bTargetMove;
	VECTORLIST		m_TraceList;
	BOOL			m_bRepeatTraceMove;

	// 2003-09-02 New Option
	BOOL			m_bPositionFixToParentObject;	// 이펙트의 매트릭스 고정.

};
typedef std::map<DWORD, _EFFECT *> EFFECTLIST;
typedef std::list<_EFFECT *> EFFECTPOOLLIST;

struct _ELEMENTRENDER
{
	bool			m_bPlay;
	DWORD			m_ElapsedTime;
	VECTOR			m_Position;
	VECTOR			m_ExtraPosition;	// exp acquire effect used
	VECTOR			m_vGravity;
	VECTOR			m_Velocity;

	VECTOR			m_Size;
	int				m_TexIndex;
	int				m_nCurMeshFrame;
	D3DMATERIAL9	m_Material;
	MATRIX			m_MeshTM;
//	_TLVERTEX		m_TLVertex[4];	// 숫자 이펙트용

	_ELEMENTVERTEX2	m_BillboardVertex[4];
	_ELEMENT*		m_pElement;
	_EFFECTTEXTURE*	m_pTexture;
	_EFFECTMESH*	m_pMesh;
};
typedef std::list<_ELEMENTRENDER *> ELEMENTRENDERLIST;


struct _VERTEXRENDER
{
	_VERTEXRENDER()
	{
		nVerticesNum = 0;
		nPrimitiveCount = 0;
		VB = NULL;
		IB = NULL;
		pElement = NULL;
	}
	~_VERTEXRENDER()
	{
		if( VB ) VB->Release();
		if( IB ) IB->Release();
		VB = NULL;
		IB = NULL;
	}

	_ELEMENT*					pElement;
	int							nVerticesNum;
	int							nPrimitiveCount;
	LPDIRECT3DVERTEXBUFFER9		VB;
	LPDIRECT3DINDEXBUFFER9		IB;
};
typedef std::list<_VERTEXRENDER *> VERTEXRENDERLIST;


struct _PARTICLERENDER
{
	bool		m_bPlay;
	VECTOR		m_Position;
	VECTOR		m_Rotation;
	DWORD		m_ElementCreatedCount;	// for Count
	DWORD		m_ElementRenderCount;
	DWORD		m_ElapsedTime;
	DWORD		m_LastSpawnTime;
	BOOL		m_bPassDelay;
	BOOL		m_bPassFirst;

	_PARTICLE	*pParticle;
	ELEMENTRENDERLIST		m_ElementList;
	VERTEXRENDERLIST		m_VertexRenderList;
};
typedef std::list<_PARTICLERENDER *> PARTICLERENDERLIST;

struct _EFFECTPACKAGEPAIR;
struct _EFFECTPACKAGE;

struct _EFFECTRENDER			// When rendering Effect, save effect data and manipulate
{
	bool		m_bPlay;
	bool		m_bOKStart;
	DWORD		m_ElapsedTime;
	VECTOR		m_CustomPosition;
	VECTOR		m_Position;
	VECTOR		m_Rotation;

//	int			m_nCharUniqID;
//	int			m_nAniType;
	int			m_nStartTime;			// start playing time
	int			m_nAttachedBoneIndex;	// bone index attached mesh
	VECTOR		m_vMoveStartPos;		// if Client use a new start position of effect, set this data
										// only client usage and this data maybe loaded DB or client itself
	VECTOR		m_vTargetMovePos;		// 이펙트의 전체 position을 변화시켜야 함. World Matrix Point
	VECTOR		m_vTargetMoveDelta;		// 이동하는 양을 누적시켜놓는다.
	bool		m_bTraceMoveTurn;		// 참이면 위치 이동이고, 거짓이면 목표 위치를 변경한다.
	int			m_nTraceListIndex;
	VECTORLIST::iterator	TraceListIT;

	// 근데 아래 3개의 변수들을 앞으로 안쓸것 같당.
	// 이펙트를 타켓 위치로 이동시킬때 필요한 변수들. 
	VECTOR		m_vCurPos;				// 현재 위치.
	VECTOR		m_vDestPos;				// 이동하는 목표 위치.
	VECTOR		m_vDir;					// 이때 이동 속도.

	// pointer
	_EFFECTPACKAGEPAIR*	pPackagePair;
	_EFFECTPACKAGE*		pPackage;
	_EFFECT		*pEffect;				// 이 데이타는 읽기 전용이다.

	// sub render list
	PARTICLERENDERLIST		m_ParticleList;	// Particle
	LIGHTLIST				m_LightList;	// 이걸루 라이팅한다.

};
typedef std::list<_EFFECTRENDER *> EFFECTRENDERLIST;


typedef std::map<MyString, int> EPACKAGERESMAP;	// save texture path and res id, this is mapping data
												// this res id is nResID in RES table
struct _EFFECTPACKAGEMESH
{
	MyString strMeshPath;
	DWORD dwOffset;
	int nLen;
};
typedef std::list<_EFFECTPACKAGEMESH *> EPACKAGEMESHLIST;	// mesh path, offset of effect_msh.dat, length

struct _EFFECTPACKAGE		// effect_ch.dat
{
	int				nEffectManageID;	// uniq id
	_EFFECT*		pEffect;
	_EFFECTRENDER*	pEffectRender;

	// below, two value is key to find character
	int		nCharID;		// ANIMATION2 table nCharID of ani mesh
	int		nAniType;		// ANIMATION2 table nAniType
	int		nEffectID;		// NEW_EFFECT_INFO table nEffectID of effect
	int		nPosX;			// fixed position X of effect
	int		nPosY;			// fixed position Y of effect
	int		nPosZ;			// fixed position Z of effect
	int		nBoneIndex;		// bone index of attached mesh
	int		nStartTime;		// play time or delay time to animate effect with ani mesh
	DWORD	dwEffOffset;	// offset to effect file data

	MyString				strEffectName;

	DWORD	m_LifeTimeChange;	// 해당 캐릭터의 애니메이션 시간이 변경되면 그 시간에 맞춰서 이펙트 플레이 시간도 같이 변경시켜준다.
								// 이 값은 패키지 파일을 읽을때 캐릭터 애니메이션 시간을 먼저 계산해서 클라이언트에서 직접 설정한다.

	MATRIX	BoneMatrix;		// 이펙트가 붙는 캐릭터의 본 Matrix

	_EFFECTPACKAGE()
	{
		pEffect = NULL;
		pEffectRender = NULL;
		m_LifeTimeChange = 0;
		D3DXMatrixIdentity( &BoneMatrix );
	}
};
typedef std::list<_EFFECTPACKAGE *> EFFECTPACKAGELIST;

struct _EFFECTPACKAGEPAIR
{
	// 외공 지속 이펙트일때, 지속 시간이 끝나갈때 알파를 적용시키기 위해.
	DWORD	dwTotalTime, dwElapsedTime;

	// below, three value is key to find character
//	int		nCharUniqID;	// 캐릭터 고유 ID
//	int		nCharID;		// ANIMATION2 table nCharID of ani mesh
//	int		nAniType;		// ANIMATION2 table nAniType

	// 대부분의 이펙트는 매트릭스가 변하지 않는다. 
	// 하지만 캐릭터에 고정되는 매트릭스의 경우, 매트릭스 포인터를 사용한다.
	MATRIX	WorldMatrix;	// 해당하는 캐릭터 World matrix
	MATRIX* pWorldMatrix;

	// 2003-10-30 추가.
	// 이녀석이 현재 사용중인지 아님 메모리 풀안에 있는지.
	bool	bNowUsing;

	bool	bIsVisible;	// 현재 화면에 보이는가?

	EFFECTPACKAGELIST		PackageList;
};
typedef std::list<_EFFECTPACKAGEPAIR *> EFFECTPACKAGEPAIRLIST;

//	Billow(2003.01.03) : .NET에서 INTLIST가 이미 정의되어 있다는 에러발생 => INTLIST에서 INT_LIST로 Renaming
typedef std::list<int> INT_LIST;

// ------- Type and Struct ---------


// -- Global Func ---------------

extern float GetEffectData(EFFECTDATALIST &elist,float time);
extern void ScaleEffectDataTime(EFFECTDATALIST &elist,float scale,DWORD time);

extern void Save(EFFECTDATALIST &elist, FILE *fp);
extern void Load(EFFECTDATALIST &elist, FILE *fp);
extern void Save(MyString &str, FILE *fp);
extern void Load(MyString &str, FILE *fp);



// ------------------------------------------------------------

#define VERSION_NO					2	// now no version check, but afterward, maybe do!
#define VERSION_CLOUDS				3	// no more use this version
#define	VERSION_UPGRADE0303			4	// In 2003_03, Upgrade Format
#define	VERSION_20030904			5	// 매트릭스에 고정되는 이펙트 추가.

// 하나의 VB에 1000개 Vertex를 한번에 Render한다.
#define ELEMENT_RENDER_MAX			250	// Max 4-vertexes mesh of one ELEMENT for rendering

//
// 에디터와 클라이언트에서 메모리 사용량을 서로 다르게 한다.
//
//#ifdef	_EDITOR
/*
#define	EFFECTPOOL_MAX				  50
#define PACKAGEPOOL_MAX				  50
#define PACKAGEPAIRPOOL_MAX			  30
#define EFFECTRENDERPOOL_MAX		  50	// effects pre-allocated memory pool for rendering
#define PARTICLERENDERPOOL_MAX		 150	// particles pre-allocated memory pool for rendering
#define ELEMENTRENDERPOOL_MAX		2000	// (mesh)elements pre-allocated memory pool for rendering
#define VERTEXRENDERPOOL_MAX		 100	// (billboard) vertex and index pre-allocated memory pool for rendering
#define LIGHTRENDERPOOL_MAX			  50
*/
//#endif

//#ifdef	_CLIENT
#define	EFFECTPOOL_MAX				 3500
#define PACKAGEPOOL_MAX				 3500
#define PACKAGEPAIRPOOL_MAX			 3000
#define EFFECTRENDERPOOL_MAX		 3500	// effects pre-allocated memory pool for rendering
#define PARTICLERENDERPOOL_MAX		 8000	// particles pre-allocated memory pool for rendering
#define ELEMENTRENDERPOOL_MAX		70000	// (mesh)elements pre-allocated memory pool for rendering
#define VERTEXRENDERPOOL_MAX		 5500	// (billboard) vertex and index pre-allocated memory pool for rendering
#define LIGHTRENDERPOOL_MAX			 3500
//#endif



#define BASIC_FILE_PATH		_T("\\\\venus\\EffectFiles\\")

class CEffectResPool;

class CEffect
{
	// quick pointer
	_EFFECT*			m_pCurEffect;			// indicate created effect
	_EFFECTPACKAGE*		m_pCurEffectPackage;	// indicate current effect package data
	_EFFECTPACKAGEPAIR*	m_pCurEffectPackagepair;
	_EFFECTRENDER*		m_pCurEffectRender;

	// 안쓴다. 
	_EFFECTPACKAGE*		m_pLevelUpGapJaPackage;		// 갑자 상승.
	_EFFECTPACKAGE*		m_pLevelUpTPPackage;		// 수련치 상승.
	_EFFECTPACKAGE*		m_pLevelUpOutGongPackage;	// 외공 수련.
	_EFFECTPACKAGE*		m_pLevelUpInGongPackage;	// 내공 수련.

	_EFFECT*			m_pHitEffect[ eHitEnumMax ];	// 캐릭터별 타격 이펙트
	_EFFECT*			m_pLevelUpEffect[4];			// 레벨 업 이펙트
	_EFFECT*			m_pAppearEffect[eAppearEnumMax];// NPC가 등장할때 나오는 이펙트, 또한 바닥 아이템 이펙트
	_EFFECT*			m_pExpAcquireEffect;			// 경험치 획득 이펙트

	_EFFECT*			m_pOutGongPersistEffect[ eOutGongPersistEnumMax ];		// 외공 지속 이펙트
	short				m_nOutGongPersistEffectXArray[ eOutGongPersistEnumMax ];// 외공 지속 이펙트의 X(Character Studio에서 Setting)
	short				m_nOutGongPersistEffectYArray[ eOutGongPersistEnumMax ];// 외공 지속 이펙트의 Y(Character Studio에서 Setting)
	short				m_nOutGongPersistEffectZArray[ eOutGongPersistEnumMax ];// 외공 지속 이펙트의 Z(Character Studio에서 Setting)

	// 안쓴다.
	_EFFECTPACKAGE*		m_pHitSalPackage;
	_EFFECTPACKAGE*		m_pHitNumberPackage[10];	// 데미지 숫자.
	_EFFECTPACKAGE*		m_pExpAcquirePackage;		// 경험치 획득.

	_EFFECTPACKAGEPAIR*	m_pSharedPackagePair;		// character studio2에서 Effect Play All에 사용.

	// data original
	EFFECTLIST			m_EffectList;			// Created and loaded effect List
	EFFECTPACKAGELIST	m_EffectPackageList;	// load package effect file data
	EPACKAGERESMAP		m_EffectPackageResMap;		// res map (texture path, res id)
	EPACKAGEMESHLIST	m_EffectPackageMeshList;	// mesh map (mesh path, offset, length)

	// Tile Effect list.
	EFFECTPACKAGELIST	m_TileEffectPackageList;
	EPACKAGEMESHLIST	m_TileEffectPackageMeshList;

	// in real time, used data, so this is copy of origin
	EFFECTRENDERLIST		m_EffectRenderList;		// List to render effect
	EFFECTPACKAGEPAIRLIST	m_CurPackagePairList;
	EFFECTRENDERLIST		m_CurEffectRenderList;

	// 메모리 반환될 리스트, 사용되고 있는 이펙트가 없어지면 이 리스트에 넣는다. 싱크를 위해서.
	EFFECTPACKAGEPAIRLIST	m_DeleteEffectPackagePairList;

	// this is used in real time and rendering. so pre-memory-allocated needed
	EFFECTRENDERLIST		m_EffectRenderPool;		// Memory Storage, pre-Memory-allocated-pool
	PARTICLERENDERLIST		m_ParticleRenderPool;
	ELEMENTRENDERLIST		m_ElementRenderPool;
	VERTEXRENDERLIST		m_VertexRenderPool;
	EFFECTPACKAGELIST		m_PackagePool;			// copy of original package
	EFFECTPACKAGEPAIRLIST	m_PackagePairPool;
	LIGHTLIST				m_LightRenderPool;			// copy of effect light

	static _EFFECTRENDER		m_PreEffectRenderPool[ EFFECTRENDERPOOL_MAX ];
	static _PARTICLERENDER		m_PreParticleRenderPool[ PARTICLERENDERPOOL_MAX ];
	static _ELEMENTRENDER		m_PreElementRenderPool[ ELEMENTRENDERPOOL_MAX ];
	static _VERTEXRENDER		m_PreVertexRenderPool[ VERTEXRENDERPOOL_MAX ];
	static _EFFECTPACKAGE		m_PrePackagePool[ PACKAGEPOOL_MAX ];
	static _EFFECTPACKAGEPAIR	m_PrePackagePairPool[ PACKAGEPAIRPOOL_MAX ];
	static CEffectLight			m_PreLightRenderPool[ LIGHTRENDERPOOL_MAX ];

	VECTOR				m_vView;
	VECTOR				m_vViewRight;
	VECTOR				m_vViewUp;

	LPDIRECT3DDEVICE9			m_pDevice;

	bool		m_bReleaseCalled;
	int					m_nLastAddedEffectID;
	int					m_nEffectID;		// 관리하는 이펙트 아이디. 같은 이펙트라도 이것은 다를수 있다.
	DWORD			m_dwFogEnable;
	DWORD			m_dwAmbient;

	bool		m_bBoolean;					// 필요할때 불리언 값을 조절해서 상황에 맞게 사용한다.
	VECTOR				m_vTargetMovePos;	// World Coordinate point, Effect Editor Used

	BYTE		m_nCurEffectVer;

	FILE*		m_pCharFileHandle;	// Character Effect file
	// [12/10/2004] DB 이펙트가 날라가서 변형/추가가 필요하다.
	FILE*		m_pCharFileHandle2;	// Character Effect 2 file
	FILE*		m_pCharFileHandle3;	// Character Effect 3 file
	FILE*		m_pTileFileHandle;	// Tile Effect file
	__int8		m_nCurEffectType;

public:
//	CAnimationSound		m_EffectSound;
	CEffectResPool*		m_pEffectResPool;
	MATRIX				m_WorldMatrix;	// 글로벌한 아이덴티티 매트릭스

	DWORD			m_dwVertexBufferMakingTimeVal;		// 이 시간주기로 버텍스 버퍼를 갱신한다.
	DWORD			m_dwVertexBufferMakingElapsedTime;

public:	// debug
	int GetCurPackagePairListSize() { return m_CurPackagePairList.size(); }
	int GetCurEffectRenderListSize() { return m_CurEffectRenderList.size(); }
	int GetPackagePairPoolSize() { return m_PackagePairPool.size(); }
	int GetPackagePoolSize() { return m_PackagePool.size(); }
	int GetLightPoolSize() { return m_LightRenderPool.size(); }
	int GetVertexRenderPoolSize() { return m_VertexRenderPool.size(); }
	int GetElementRenderPoolSize() { return m_ElementRenderPool.size(); }
	int GetParticleRenderPoolSize() { return m_ParticleRenderPool.size(); }
	int GetEffectRenderPoolSize() { return m_EffectRenderPool.size(); }
	int GetAniSoundInstancePoolSize() { return 0; /*m_EffectSound.GetAniSoundInstancePoolSize();*/ }
	int GetAniSoundInstanceListSize() { return 0; /*m_EffectSound.GetAniSoundInstanceListSize();*/ }
	int GetEffectRenderListSize() { return m_EffectRenderList.size(); }
	XIAHGE_API int GetTotalRenderedVertex() { return m_nTotalRenderVertexCount;}
	XIAHGE_API int GetTotalRenderedFace() { return m_nTotalRenderFaceCount;}

protected:
	unsigned long m_nTotalRenderVertexCount;
	unsigned long m_nTotalRenderFaceCount;

public:
	DWORD GetRenderBlend(__int8 nType);
	DWORD GetRenderBlendOP(__int8 nType);
	DWORD GetTextureBlendOP(__int8 nType);
	DWORD GetTextureBlendArg(__int8 nType);

	XIAHGE_API void SetAppearEffect(int nType,_EFFECT* pEffect);
	XIAHGE_API void SetLevelUpEffect(int nType,_EFFECT* pEffect);
	XIAHGE_API void SetHitEffect(int nType,_EFFECT* pEffect);
	XIAHGE_API void SetExpAcquireEffect(_EFFECT* pEffect);
	XIAHGE_API void SetOutGongPersistEffect(int nType,_EFFECT* pEffect, int nX, int nY, int nZ);

	XIAHGE_API void SetTargetMovePosition(VECTOR vTargetPos);
	void SetBooleanValue(bool bValue);
	XIAHGE_API void OffSharedPackagePair();
	XIAHGE_API bool MakeSharedPackagePair(int nCharUniqID, int nCharID, int nAniType);
	bool IsEffectInRendering(int nEffectManageID);
	void DeqEffectImmediately(_EFFECT* pEffect);
	void CopyElement(_PARTICLE* pParticle, _ELEMENT* pSrcElement, _ELEMENT* pDestElement);
	void CopyParticle(_EFFECT* pEffect, _PARTICLE* pSrcParticle, _PARTICLE* pDestParticle);
	void CopyEffect(_EFFECT* pSrcEffect, _EFFECT* pDestEffect);

	XIAHGE_API _EFFECTPACKAGE* EnqAppearEffectImmediately(int nType, int nSTime=0, int nX=0, int nY=0, int nZ=0, int nBoneIndex=-1);
	XIAHGE_API _EFFECTPACKAGE* EnqLevelUpEffectImmediately(int nType, int nSTime=0, int nX=0, int nY=0, int nZ=0, int nBoneIndex=-1);
	XIAHGE_API _EFFECTPACKAGE* EnqHitEffectImmediately(int nType, int nSTime=0, int nX=0, int nY=0, int nZ=0, int nBoneIndex=-1);
	XIAHGE_API _EFFECTPACKAGE* EnqEffectImmediately(_EFFECT* pEffect, int nSTime=0, int nX=0, int nY=0, int nZ=0, int nBoneIndex=-1);
	XIAHGE_API _EFFECTPACKAGE* EnqExpAcquireEffectImmediately(VECTOR vTargetPos, int nSTime=0, int nX=0, int nY=0, int nZ=0, int nBoneIndex=-1);
	XIAHGE_API _EFFECTPACKAGE* EnqOutGongPersistEffectImmediately(int nType, int nSTime=0, int nX=0, int nY=0, int nZ=0, int nBoneIndex=-1);

	void EnqExpAcquireEffect(VECTOR vStartPos, VECTOR vDestPos);
	void EnqLevelupEffect(BYTE nType, int nCharUniqID, int nCharID, int nAniType, D3DMATRIX* CharMatrix);
	void DeleteEffectRender(_EFFECTRENDER* pEffectRender);
	void DeletePackagePair(int nCharUniqID);

	void LoadTileEffectMeshFromPackage();
	void LoadTileEffectFromPackage();

	void SetPackagePairMatrix(int nCharUniqID, MATRIX *CharMatrix);
	void SetCurWorldMatrix(MATRIX WorldMatrix);
	XIAHGE_API void LoadEffectTextureFromPackage(/*CResIndex* pResIndex*/);
	bool IsCharUniqIDinPackage(int nCharUniqID);
	XIAHGE_API void LoadEffectMeshFromPackage();
	void DeleteAllPackageData();
	XIAHGE_API void LoadEffectFromPackage();
	void DeleteLight(_EFFECT* pEffect, CEffectLight* pLight);
	CEffectLight* AddLight(_EFFECT* pEffect);
//	void DeleteThunder(_EFFECT* pEffect, CThunder* pThunder);
//	void DeleteWaterfall(_EFFECT* pEffect, CWaterfall* pWaterfall);
//	CThunder * AddThunder(_EFFECT* pEffect);
//	CWaterfall* AddWaterfall(_EFFECT* pEffect);

	void DecMeshRefCount(MyString strFilename);
	void IncMeshRefCount(MyString strFilename);
	void DecTextureRefCount(MyString strFilename);
	void IncTextureRefCount(MyString strFilename);
	void GetAllMesh(EFFECTMESHMAP& meshmap);
	void DeleteMesh(MyString strFilename);
	_EFFECTMESH* GetMesh(LPCTSTR filename);
	void GetAllTexture(EFFECTTEXTUREMAP& texturemap);
	void DeleteTexture(MyString strFilename);
	_EFFECTTEXTURE* GetTexture(LPCTSTR filename);

	//XIAHGE_API int GetEffectRenderCount() { return m_CurEffectRenderList.size();}

	//
	XIAHGE_API void DeqEffectPackagePair(_EFFECTPACKAGEPAIR* pPackagePair);
	void MakeElementVertexToRender(DWORD dTime);
	void DeqEffectFromRender(_EFFECTRENDER* pEffectRender);
	XIAHGE_API void RenderEffect(DWORD dTime);
	XIAHGE_API void RenderOthers(DWORD dTime);
	void SpawnElement(_PARTICLERENDER* pParticleRender);
	XIAHGE_API void UpdateEffect(DWORD dTime);
	void StartEffectToRender(_EFFECTRENDER* pEffectRender);
	_EFFECTRENDER* EnqEffectToRender(_EFFECT* pEffect);
	_EFFECTRENDER* EnqEffectToRender(DWORD id);

	void EffectPackagePairMemoryReturn(_EFFECTPACKAGEPAIR* pPackagePair);
	void DeleteEffectPackagePairList();

	XIAHGE_API void SetVertexBufferRenewTime(DWORD dwRenewTime);

	// create
	_ELEMENT* CreateElement(_PARTICLE* pParticle);
	_PARTICLE* CreateParticle(_EFFECT* pEffect);
	void CreateEffect();// create effect and insert into list but don't save into file

	// 
	void DeleteAllEffect();
	void DeleteEffect(_EFFECT* pEffect, bool bDeletePointer = true);
	void DeleteEffect(DWORD id);
	XIAHGE_API _EFFECT * GetCurEffect();
	XIAHGE_API _EFFECTPACKAGEPAIR * GetCurEffectPackagePair();
	XIAHGE_API void Release();
	XIAHGE_API void ReleaseAllMeshTexture();
	XIAHGE_API void Initialize(LPDIRECT3DDEVICE9 pDevice, int nMaxTextureSize, int nMaxMeshSize);
	XIAHGE_API _EFFECT * GetEffect(DWORD id);
	int GetFreeEffectID();
	void DeleteParticle(_PARTICLE *pParticleData, bool bDeletePointer = true);
	void DeleteParticle(_EFFECT* pEffect, _PARTICLE* pParticle, bool bDeletePointer = true);
	void DeleteElement(_PARTICLE* pParticle, _ELEMENT* pElement, bool bDeletePointer = true);

	// File I/O Func
	void SaveElement(_ELEMENT *pElementData, FILE *fp);
	void LoadElement(_ELEMENT *pElementData, FILE *fp, bool bLoadRes = true);
	void SaveParticle(_PARTICLE *pParticleData,FILE *fp);
	void LoadParticle(_PARTICLE *pParticleData,FILE *fp, bool bLoadRes = true);
	void SaveEffect(_EFFECT* pEffect,FILE *fp);
	XIAHGE_API bool LoadEffect(FILE *fp, bool bLoadRes = true);// create new effect and load effect data from file
	void SaveEffect(FILE *fp);

	XIAHGE_API CEffect();
	XIAHGE_API virtual ~CEffect();

	XIAHGE_API BOOL LoadXiahTileEffectPackage(LPCTSTR szFilename);
	// [12/10/2004] DB 이펙트가 날라가서 함수를 변형이 필요하다.
	XIAHGE_API BOOL LoadXiahEffectPackage(LPCTSTR szFilename, int nFileType = 1);
	void RealizeEffectInEffectPackage(_EFFECTPACKAGE* pPackage);
	void RealizeEffectInEffectPackage(DWORD dwEffOffset);
	XIAHGE_API void RealizeEffectResouce(_EFFECT* pEffect);
};


extern XIAHGE_API CEffect	g_EffectManager;

//-------------------------------------------------------



class CEffectResPool
{
	LPDIRECT3DDEVICE9	m_pDevice;
	EFFECTTEXTUREMAP	m_TextureMap;
	EFFECTMESHMAP		m_MeshMap;

	int					m_nMaxTextureAllocated;
	int					m_nMaxMeshAllocated;

public:
	void RegisterHCHMeshFromFile(MyString strName, FILE* fp);
	void RegisterTextureFromFile(MyString strName, LPDIRECT3DTEXTURE9 pTexture);
	void RegisterMeshFromFile(MyString strName, MyString strFilename);

	_EFFECTMESH* FindMesh(MyString strMesh);
	void DecMeshRefCount(MyString strFilename);
	void IncMeshRefCount(MyString strFilename);
	void DecTextureRefCount(MyString strFilename);
	void IncTextureRefCount(MyString strFilename);
	_EFFECTTEXTURE* FindTexture(MyString strTexture);
	int					m_nCurTextureAllocated;
	int					m_nCurMeshAllocated;

	void GetAllMesh(EFFECTMESHMAP& meshmap);
	void DeleteMesh(MyString strFilename);
	void GetAllTexture(EFFECTTEXTUREMAP& texturemap);
	void DeleteTexture(MyString strFilename);
	_EFFECTMESH * GetMesh(LPCTSTR filename);
	_EFFECTTEXTURE* GetTexture(LPCTSTR filename);

	void Release();
	void Create(LPDIRECT3DDEVICE9 pDevice, int nMaxTextureSize, int nMaxMeshSize);

	// FILE I/O
	int CreateTextureFromFile(_EFFECTTEXTURE* pEffectTexture, LPCTSTR filename );
	int LoadBCFFromFile(_EFFECTMESH *pEffectMesh, LPCTSTR filename);
	int LoadHCHFromFile(_EFFECTMESH* pEffectMesh, LPCTSTR filename);
	int LoadHCHFromFile(_EFFECTMESH* pEffectMesh, FILE* fp);
	int LoadHCHFromMemory(_EFFECTMESH* pEffectMesh, BYTE* pBuffer);

	CEffectResPool();
	virtual ~CEffectResPool();

	std::vector<MyString> m_MeshTextureList;

};



//-------------------------------------------------------



class CEffectLight  
{
	int		m_nPosX;
	int		m_nPosY;
	int		m_nPosZ;
	int		m_nDiffuseR;
	int		m_nDiffuseG;
	int		m_nDiffuseB;
	int		m_nSpecularR;
	int		m_nSpecularG;
	int		m_nSpecularB;
	int		m_nAmbientR;
	int		m_nAmbientG;
	int		m_nAmbientB;

public:	// variables
	LPDIRECT3DDEVICE9	m_pd3dDevice;
	D3DLIGHT9			m_Light;

	EFFECTDATALIST	m_Range;
	DWORD			m_LifeTime;
	DWORD			m_ElapsedTime;
	bool			m_bTurnOn;
	bool			m_bPlay;
	int				m_nLightID;
	bool			m_bIsSetMatrix;

	MATRIX*			m_pMatrix;

public:
	void SetDevice(LPDIRECT3DDEVICE9 pDevice);
	void Start(bool bStart);
	void SetMatrix(MATRIX* pMatrix);
	void Update(DWORD dwTime);
	void Setting(int x, int y, int z, int dR, int dG, int dB, int sR, int sG, int sB, int aR, int aG, int aB);
	void GetSettingData(int &x, int &y, int &z, int &dR, int &dG, int &dB, int &sR, int &sG, int &sB, int &aR, int &aG, int &aB);
	void Load(FILE *fp, LPDIRECT3DDEVICE9 pDevice);
	void Save(FILE *fp);
	void Init(int x, int y, int z, int dR, int dG, int dB, int sR, int sG, int sB, int aR, int aG, int aB);
	CEffectLight();
	virtual ~CEffectLight();

};

#endif


};// namespace
