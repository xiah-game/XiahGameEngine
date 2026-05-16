
#include "stdafx.h"
#include "EffectRes.h"
#include "HchFile.h"


#ifdef	_EFFECTEDITOR
#include "R_EffectInfo.h"
#endif

#ifdef	_CHARACTERSTUDIO2
#include "ObjectDoc.h"
#include "DlgCommonDef.h"
extern CObjectDoc*	g_pObjectDoc;
#endif

#include "XiahPak.h"


// 재x의 엄청난 더러운 코드군...
// 파일 하나에 다 집어 넣는 센스 멋지군

namespace XiahGameEngine
{
#define		EFFECTLIGHT_INDEX_MAX		8

#define		TYPE_TILE_EFFECT			-1
#define		TYPE_EXPACQUIRE_EFFECT		-3	// 이걸 사용할지 안할지는 좀더 두고보자.



// Needed Global Func
VECTOR operator *(VECTOR &v, MATRIX &m)
{
	VECTOR r;

	r.x = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
	r.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
	r.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;

	return r;
}

void SetRotationEuler(MATRIX &m, VECTOR &angle)
{
	float a = COS( angle.x);
	float b = SIN( angle.x);
	float c = COS( angle.y);
	float d = SIN( angle.y);
	float e = COS( angle.z);
	float f = SIN( angle.z);

	float ad = a * d;
	float bd = b * d;

	m._11 =   c * e;
	m._12 =  -c * f;
	m._13 =  -d;
	m._21 = -bd * e + a * f;
	m._22 =  bd * f + a * e;
	m._23 =  -b * c;
	m._31 =  ad * e + b * f;
	m._32 = -ad * f + b * e;
	m._33 =   a * c;

	m._14 =  m._24 = m._34 = m._41 = m._42 = m._43 = 0;
	m._44 =  1;
}


//-- Global Func -----------------------------
float GetEffectData(EFFECTDATALIST &elist,float time)
{
/*
	if( time > elist.back().fTime)
		return elist.back().fValue;
	
	EFFECTDATA first,end;

	first = elist.front();
	
	EFFECTDATALIST::iterator it;

	for(it = elist.begin(); it != elist.end(); it++)
	{
		EFFECTDATA &value = *it;

		if( time < value.fTime)
		{
			end = value;
			break;
		}
		else
			first = value;
	}

	float alpha = (time - first.fTime) / (end.fTime - first.fTime);
	float r = first.fValue * (1 - alpha) + end.fValue * (alpha);

	return r;
*/
	EFFECTDATA *pPrev, *pNext;

	pPrev = &elist.front();
	pNext = NULL;

	EFFECTDATALIST::iterator it;
	it = elist.begin();
	it ++;

	for(;it != elist.end(); it++)
	{
		EFFECTDATA *pData = &(*it);

		if(pData == NULL)
		{
			DBG_LogFile( _T("GetEffectData fail"));
		}

		if( time < pData->fTime)
		{
			pNext = pData;
			break;
		}
		else
			pPrev = pData;
	}

	if( pPrev && pNext)
	{
		float alpha = (time - pPrev->fTime) / (pNext->fTime - pPrev->fTime);
		
		if( alpha < 0)
			alpha = 0;

		if( alpha > 1.0f)
			alpha = 1.0f;
		return pPrev->fValue * (1 - alpha) + pNext->fValue * alpha;
	}
	else
	{
		return pPrev->fValue;
	}

	return 0;
}

void ScaleEffectDataTime(EFFECTDATALIST &elist,float scale,DWORD time)
{
	EFFECTDATALIST::iterator it;

	for(it = elist.begin(); it != elist.end(); it++)
	{
		EFFECTDATA &value = *it;
		value.fTime *= scale;
	}

	elist.front().fTime = 0;
	elist.back().fTime = time;
}


void Save(EFFECTDATALIST &elist, FILE *fp)
{
	int count = elist.size();

	fwrite( &count, 4, 1, fp);
	EFFECTDATALIST::iterator it;

	for(it = elist.begin(); it != elist.end(); it++)
	{
		EFFECTDATA &effect = *it;

		fwrite( &effect, sizeof( EFFECTDATA), 1, fp);
	}
}

void Load(EFFECTDATALIST &elist, FILE *fp)
{
	int count;
	fread( &count, 4, 1, fp);

	for(int i = 0; i < count; i++)
	{
		EFFECTDATA data;

		fread( &data, sizeof( EFFECTDATA), 1, fp);

		elist.push_back( data);
	}
}

void Save(MyString &str, FILE *fp)
{
	int len = str.size();
	len++;

	LPCTSTR ptr = str.data();

	fwrite( &len, 4, 1, fp );
	fwrite( ptr, len, 1, fp );
}

void Load(MyString &str, FILE *fp)
{
	int len;
	fread( &len, 4, 1, fp );

	TCHAR szString[255];
	memset( szString, 0, sizeof(szString) );
	fread( szString, len, 1, fp );

	str.assign( szString );
}



//
// --- CEffect ----------------------------
//

XIAHGE_API CEffect	g_EffectManager;

_EFFECTRENDER CEffect::m_PreEffectRenderPool[EFFECTRENDERPOOL_MAX];
_PARTICLERENDER CEffect::m_PreParticleRenderPool[PARTICLERENDERPOOL_MAX];
_ELEMENTRENDER CEffect::m_PreElementRenderPool[ELEMENTRENDERPOOL_MAX];
_VERTEXRENDER CEffect::m_PreVertexRenderPool[VERTEXRENDERPOOL_MAX];
_EFFECTPACKAGE CEffect::m_PrePackagePool[PACKAGEPOOL_MAX];
_EFFECTPACKAGEPAIR CEffect::m_PrePackagePairPool[PACKAGEPAIRPOOL_MAX];
CEffectLight CEffect::m_PreLightRenderPool[LIGHTRENDERPOOL_MAX];

XIAHGE_API CEffect::CEffect() : m_pCharFileHandle2(NULL), m_pCharFileHandle3(NULL)
{
	m_pCharFileHandle = NULL;
	m_pTileFileHandle = NULL;

	m_pCurEffect = NULL;
	m_pCurEffectPackage = NULL;
	m_pCurEffectRender = NULL;

	m_nLastAddedEffectID = 0;
	m_nEffectID = 0;
	m_pEffectResPool = NULL;
	m_bReleaseCalled = false;
	m_pSharedPackagePair = NULL;

	for(int i=0; i<eHitEnumMax; i++)
		m_pHitEffect[i] = NULL;

	for(i=0; i<4; i++)
        m_pLevelUpEffect[i] = NULL;

	// 왜 3개만 초기화 했냐??
	//for(i=0; i<3; i++)
	for(i=0; i < eAppearEnumMax; ++i)
        m_pAppearEffect[i] = NULL;

	m_pExpAcquireEffect = NULL;

	for(i=0; i<eOutGongPersistEnumMax; i++)
		m_pOutGongPersistEffect[i] = NULL;

	m_DeleteEffectPackagePairList.clear();

	m_dwVertexBufferMakingTimeVal = 0;
	m_dwVertexBufferMakingElapsedTime = 0;

	// 안쓰는...
	m_pLevelUpGapJaPackage = NULL;
	m_pLevelUpTPPackage = NULL;
	m_pLevelUpOutGongPackage = NULL;
	m_pLevelUpInGongPackage = NULL;

	m_pHitSalPackage = NULL;
	for(i=0; i<10; i++)
		m_pHitNumberPackage[i] = NULL;

	m_pExpAcquirePackage = NULL;
	m_bBoolean = false;

	//
	m_vTargetMovePos = VECTOR(0,0,0);

	D3DXMatrixIdentity( &m_WorldMatrix );

	m_nTotalRenderVertexCount	= 0;
	m_nTotalRenderFaceCount		= 0;
}

XIAHGE_API CEffect::~CEffect()
{
	Release();
}

XIAHGE_API bool CEffect::LoadEffect(FILE *fp, bool bLoadRes)
{
	BYTE ver;
	fread( &ver, 1, 1, fp );

	// test
//	ver = VERSION_NO;

	m_nCurEffectVer = ver;

	// check valid data format
	if( ver < VERSION_NO || ver > VERSION_20030904 )
		return false;

	//
	_EFFECT* pEffect = new _EFFECT;
	if(pEffect == NULL)
	{
		DBG_LogFile( _T("CEffect::LoadEffect fail"));
	}

	m_pCurEffect = pEffect;

	// set current effect type
	pEffect->m_nEffectType = m_nCurEffectType;

	//
	fread( &pEffect->m_EffectID, 4, 1, fp);
	XiahGameEngine::Load( pEffect->m_EffectName, fp);
	fread( &pEffect->m_LifeTime, 4, 1, fp);

	XiahGameEngine::Load( pEffect->m_Position[ 0], fp);
	XiahGameEngine::Load( pEffect->m_Position[ 1], fp);
	XiahGameEngine::Load( pEffect->m_Position[ 2], fp);

	XiahGameEngine::Load( pEffect->m_Rotation[ 0], fp);
	XiahGameEngine::Load( pEffect->m_Rotation[ 1], fp);
	XiahGameEngine::Load( pEffect->m_Rotation[ 2], fp);

	int count;
	fread( &count, 4, 1, fp);

	for( int i=0; i<count; i++)
	{
		_PARTICLE* pParticle = new _PARTICLE;
		if(pParticle == NULL)
		{
			DBG_LogFile( _T("CEffect::LoadEffect fail"));
		}

		LoadParticle( pParticle, fp, bLoadRes );

		pEffect->m_ParticleList.push_back(pParticle);
	}// for

	// add effect list and ID
	m_nEffectID++;
	pEffect->m_EffectManageID = m_nEffectID;
//	if( m_pCurEffectPackage )
//		m_pCurEffectPackage->nEffectManageID = m_nEffectID;	// 아 근데 이거 안쓸지도 모르는데..

	pEffect->m_DBID = m_pCurEffectPackage->nEffectID;
	m_EffectList.insert( EFFECTLIST::value_type( pEffect->m_DBID, pEffect ) );
//	m_EffectList.insert( EFFECTLIST::value_type( pEffect->m_EffectManageID, pEffect ) );
//	m_EffectList.insert( EFFECTLIST::value_type( pEffect->m_EffectID, pEffect ) );

	// Load others... waterfall, thunder, light
	fread( &count, 4, 1, fp );
	for(i=0; i<count; i++)
	{
		// 혹시 에디터에서 만들어 놓으면 에러가 나므로 파일만 읽도록 한다. 
		int dir, PosX, PosY, PosZ, Power, Number, Size, Width, SizeB;
		fread( &dir,	4, 1, fp );
		fread( &PosX,	4, 1, fp );
		fread( &PosY,	4, 1, fp );
		fread( &PosZ,	4, 1, fp );
		fread( &Power,	4, 1, fp );
		fread( &Number,	4, 1, fp );
		fread( &Size,	4, 1, fp );
		fread( &Width,	4, 1, fp );
		fread( &SizeB,	4, 1, fp );

		MyString texturepath;
		XiahGameEngine::Load(texturepath, fp);
/*		지우면 안되
		CWaterfall* pWaterfall = new CWaterfall;
		pWaterfall->Load(fp, bLoadRes);

		pEffect->m_WaterfallList.push_back( pWaterfall );
*/
	}

	fread( &count, 4, 1, fp );
	for(i=0; i<count; i++)
	{
		int x, y, z, ratio, body, subbody, subbodydivision, width;
		DWORD drawgap;

		fread( &x,				4, 1, fp );
		fread( &y,				4, 1, fp );
		fread( &z,				4, 1, fp );
		fread( &ratio,			4, 1, fp );
		fread( &body,			4, 1, fp );
		fread( &subbody,		4, 1, fp );
		fread( &subbodydivision,4, 1, fp );
		fread( &drawgap,		4, 1, fp );
		fread( &width,			4, 1, fp );

		MyString texturepath;
		XiahGameEngine::Load(texturepath, fp);
/*		지우면 안되
		CThunder* pThunder = new CThunder;
		pThunder->Load(fp, bLoadRes);

		pEffect->m_ThunderList.push_back( pThunder );
*/
	}

	fread( &count, 4, 1, fp );
	for(i=0; i<count; i++)
	{
		CEffectLight *pLight = new CEffectLight;
		if(pLight == NULL)
		{
			DBG_LogFile( _T("CEffect::LoadEffect2 fail"));
		}

		pLight->Load(fp, m_pDevice);
		pLight->m_nLightID = pEffect->m_nLightIndex++;

		pEffect->m_LightList.push_back(pLight);
	}

	// 이전 버전일때에는 현재 버전에서 필요한 데이타들을 초기화해준다. 
	if( ver < VERSION_UPGRADE0303 )
	{
		pEffect->m_bRepeat = FALSE;
		pEffect->m_bTraceMove = FALSE;
		pEffect->m_fTraceMoveSpeed = 0.0f;
		pEffect->m_fTargetMoveSpeed = 0.0f;
		pEffect->m_bTargetMove = FALSE;
		pEffect->m_TraceList.clear();
		pEffect->m_bRepeatTraceMove = FALSE;
		pEffect->m_bPositionFixToParentObject = FALSE;

		pEffect->m_Opacity.push_back( EFFECTDATA( 0, 1 ) );
		pEffect->m_Opacity.push_back( EFFECTDATA( pEffect->m_LifeTime, 1 ) );
	}
	else
	if( ver == VERSION_UPGRADE0303 )
	{
		fread( &pEffect->m_bRepeat, 4, 1, fp);
		fread( &pEffect->m_bTraceMove, 4, 1, fp);
		fread( &pEffect->m_fTraceMoveSpeed, 4, 1, fp);
		fread( &pEffect->m_fTargetMoveSpeed, 4, 1, fp);
		fread( &pEffect->m_bTargetMove, 4, 1, fp);

		pEffect->m_TraceList.clear();
		fread( &count, 4, 1, fp );
		for(int i=0; i<count; i++)
		{
			VECTOR vV;
			fread( &vV.x, 4, 1, fp );
			fread( &vV.y, 4, 1, fp );
			fread( &vV.z, 4, 1, fp );

			pEffect->m_TraceList.push_back( vV );
		}

		XiahGameEngine::Load( pEffect->m_Opacity, fp);

		fread( &pEffect->m_bRepeatTraceMove, 4, 1, fp);
		pEffect->m_bPositionFixToParentObject = FALSE;
	}
	else
	if( ver == VERSION_20030904 )
	{
		fread( &pEffect->m_bRepeat, 4, 1, fp);
		fread( &pEffect->m_bTraceMove, 4, 1, fp);
		fread( &pEffect->m_fTraceMoveSpeed, 4, 1, fp);
		fread( &pEffect->m_fTargetMoveSpeed, 4, 1, fp);
		fread( &pEffect->m_bTargetMove, 4, 1, fp);

		pEffect->m_TraceList.clear();
		fread( &count, 4, 1, fp );
		for(int i=0; i<count; i++)
		{
			VECTOR vV;
			fread( &vV.x, 4, 1, fp );
			fread( &vV.y, 4, 1, fp );
			fread( &vV.z, 4, 1, fp );

			pEffect->m_TraceList.push_back( vV );
		}

		XiahGameEngine::Load( pEffect->m_Opacity, fp);

		fread( &pEffect->m_bRepeatTraceMove, 4, 1, fp);
		fread( &pEffect->m_bPositionFixToParentObject, 4, 1, fp );
	}

	return true;
}

void CEffect::SaveEffect(FILE *fp)
{
	if( NULL != m_pCurEffect )
		SaveEffect( m_pCurEffect, fp );
}

void CEffect::SaveEffect(_EFFECT* pEffect,FILE *fp)
{
	// 저장할때에는 현재 버전으로 해준다.
	BYTE ver = VERSION_20030904;
	fwrite( &ver, 1, 1, fp );

	fwrite( &pEffect->m_EffectID, 4, 1 ,fp);
	XiahGameEngine::Load( pEffect->m_EffectName, fp);
	fwrite( &pEffect->m_LifeTime, 4, 1, fp);

	XiahGameEngine::Load( pEffect->m_Position[ 0], fp);
	XiahGameEngine::Load( pEffect->m_Position[ 1], fp);
	XiahGameEngine::Load( pEffect->m_Position[ 2], fp);

	XiahGameEngine::Load( pEffect->m_Rotation[ 0], fp);
	XiahGameEngine::Load( pEffect->m_Rotation[ 1], fp);
	XiahGameEngine::Load( pEffect->m_Rotation[ 2], fp);

	int count = pEffect->m_ParticleList.size();
	fwrite( &count, 4, 1, fp );

	PARTICLELIST::iterator it;
	for(it=pEffect->m_ParticleList.begin(); it!=pEffect->m_ParticleList.end(); it++)
	{
		_PARTICLE *pParticleData = *it;
		SaveParticle(pParticleData, fp);
	}

	// save others... waterfall, thunder, light
//	count = pEffect->m_WaterfallList.size();
	count = 0;
	fwrite( &count, 4, 1, fp );
/*  지우면 안되
	WATERFALLLIST::iterator wit;
	for(wit=pEffect->m_WaterfallList.begin(); wit!=pEffect->m_WaterfallList.end(); wit++)
	{
		CWaterfall* pWaterfall = *wit;
		pWaterfall->Save(fp);
	}
*/

//	count = pEffect->m_ThunderList.size();
	fwrite( &count, 4, 1, fp );
/*  지우면 안되
	THUNDERLIST::iterator tit;
	for(tit=pEffect->m_ThunderList.begin(); tit!=pEffect->m_ThunderList.end(); tit++)
	{
		CThunder* pThunder = *tit;
		pThunder->Save(fp);
	}
*/

	LIGHTLIST::iterator lit;
	count = pEffect->m_LightList.size();
	fwrite( &count, 4, 1, fp );
	for(lit=pEffect->m_LightList.begin(); lit!=pEffect->m_LightList.end(); lit++)
	{
		CEffectLight* pLight = *lit;
		pLight->Save(fp);
	}

	// More Option
	fwrite( &pEffect->m_bRepeat, 4, 1, fp );
	fwrite( &pEffect->m_bTraceMove, 4, 1, fp );
	fwrite( &pEffect->m_fTraceMoveSpeed, 4, 1, fp );
	fwrite( &pEffect->m_fTargetMoveSpeed, 4, 1, fp );
	fwrite( &pEffect->m_bTargetMove, 4, 1, fp );

	count = pEffect->m_TraceList.size();
	fwrite( &count, 4, 1, fp );
	VECTORLIST::iterator vit;
	for(vit=pEffect->m_TraceList.begin(); vit!=pEffect->m_TraceList.end(); vit++)
	{
		VECTOR vV = *vit;

		fwrite( &vV.x, 4, 1, fp );
		fwrite( &vV.y, 4, 1, fp );
		fwrite( &vV.z, 4, 1, fp );
	}// for(pEffect->m_TraceList)

	// effect opacity
	XiahGameEngine::Load( pEffect->m_Opacity, fp);

	fwrite( &pEffect->m_bRepeatTraceMove, 4, 1, fp );
	fwrite( &pEffect->m_bPositionFixToParentObject, 4, 1, fp );

}

void CEffect::LoadParticle(_PARTICLE *pParticleData, FILE *fp, bool bLoadRes)
{
	fread( &pParticleData->m_Position, sizeof( VECTOR ), 1, fp);
	XiahGameEngine::Load( pParticleData->m_Rotation[ 0], fp);
	XiahGameEngine::Load( pParticleData->m_Rotation[ 1], fp);
	XiahGameEngine::Load( pParticleData->m_Rotation[ 2], fp);

	fread( &pParticleData->m_LifeTimeType, 4, 1, fp);
	fread( &pParticleData->m_LifeTime, 4, 1, fp);
	fread( &pParticleData->m_DelayTime, 4, 1, fp);
	fread( &pParticleData->m_bForceFirst, 4, 1, fp);

	fread( &pParticleData->m_ElementSpawnType, 4, 1, fp);
	fread( &pParticleData->m_ElementSpawnCount, 4, 1, fp);
	fread( &pParticleData->m_ElementSpawnCountBy, 4, 1, fp);
	fread( &pParticleData->m_ElementSpawnDelay, 4, 1, fp);

	XiahGameEngine::Load( pParticleData->m_SpawnSpeed, fp );
	fread( &pParticleData->m_SpawnDirection, sizeof( VECTOR), 1, fp);
	
	fread( &pParticleData->m_SpawnVolumeType, 4, 1, fp);
	XiahGameEngine::Load( pParticleData->m_SpawnVolumeSize[0], fp );
	XiahGameEngine::Load( pParticleData->m_SpawnVolumeSize[1], fp );
	XiahGameEngine::Load( pParticleData->m_SpawnVolumeSize[2], fp );
	
	fread( &pParticleData->m_SpawnPositionAtVolumeType, 4, 1, fp);
	fread( &pParticleData->m_SpawnDirectionType, 4, 1, fp);

	XiahGameEngine::Load( pParticleData->m_Gravity[0], fp );
	XiahGameEngine::Load( pParticleData->m_Gravity[1], fp );
	XiahGameEngine::Load( pParticleData->m_Gravity[2], fp );

	int count;
	fread( &count, 4, 1, fp);
	for(int i=0; i<count; i++)
	{
		_ELEMENT* pElement = new _ELEMENT;
		LoadElement( pElement, fp, bLoadRes );

		pParticleData->m_ElementList.push_back(pElement);
	}// for
}

void CEffect::SaveParticle(_PARTICLE *pParticleData,FILE *fp)
{
	fwrite( &pParticleData->m_Position, sizeof( VECTOR ), 1, fp);
	XiahGameEngine::Load( pParticleData->m_Rotation[ 0], fp);
	XiahGameEngine::Load( pParticleData->m_Rotation[ 1], fp);
	XiahGameEngine::Load( pParticleData->m_Rotation[ 2], fp);

	fwrite( &pParticleData->m_LifeTimeType, 4, 1, fp);
	fwrite( &pParticleData->m_LifeTime, 4, 1, fp);
	fwrite( &pParticleData->m_DelayTime, 4, 1, fp);
	fwrite( &pParticleData->m_bForceFirst, 4, 1, fp);

	fwrite( &pParticleData->m_ElementSpawnType, 4, 1, fp);
	fwrite( &pParticleData->m_ElementSpawnCount, 4, 1, fp);
	fwrite( &pParticleData->m_ElementSpawnCountBy, 4, 1, fp);
	fwrite( &pParticleData->m_ElementSpawnDelay, 4, 1, fp);

	XiahGameEngine::Load( pParticleData->m_SpawnSpeed, fp );
	fwrite( &pParticleData->m_SpawnDirection, sizeof( VECTOR ), 1, fp);
	
	fwrite( &pParticleData->m_SpawnVolumeType, 4, 1, fp);
	XiahGameEngine::Load( pParticleData->m_SpawnVolumeSize[0], fp );
	XiahGameEngine::Load( pParticleData->m_SpawnVolumeSize[1], fp );
	XiahGameEngine::Load( pParticleData->m_SpawnVolumeSize[2], fp );
	
	fwrite( &pParticleData->m_SpawnPositionAtVolumeType, 4, 1, fp);
	fwrite( &pParticleData->m_SpawnDirectionType, 4, 1, fp);

	XiahGameEngine::Load( pParticleData->m_Gravity[0], fp );
	XiahGameEngine::Load( pParticleData->m_Gravity[1], fp );
	XiahGameEngine::Load( pParticleData->m_Gravity[2], fp );

	int count = pParticleData->m_ElementList.size();
	fwrite( &count, 4, 1, fp);

	ELEMENTLIST::iterator it;
	for(it = pParticleData->m_ElementList.begin(); it != pParticleData->m_ElementList.end(); it++)
	{
		_ELEMENT *pElement = *it;
		SaveElement(pElement, fp);
	}
}

void CEffect::LoadElement(_ELEMENT *pElementData, FILE *fp, bool bLoadRes)
{
	fread( &pElementData->m_Type, 4, 1, fp);
	XiahGameEngine::Load( pElementData->m_MeshPath, fp);
	XiahGameEngine::Load( pElementData->m_TexturePath, fp);

	fread( &pElementData->m_WrapCount_X, 4, 1, fp);
	fread( &pElementData->m_WrapCount_Y, 4, 1, fp);

	XiahGameEngine::Load( pElementData->m_TexIndex, fp);

	fread( &pElementData->m_RenderType, 4, 1,fp);
	XiahGameEngine::Load( pElementData->m_Opacity, fp);

	XiahGameEngine::Load( pElementData->m_Rotation[ 0], fp);
	XiahGameEngine::Load( pElementData->m_Rotation[ 1], fp);
	XiahGameEngine::Load( pElementData->m_Rotation[ 2], fp);

	XiahGameEngine::Load( pElementData->m_Size[ 0], fp);
	XiahGameEngine::Load( pElementData->m_Size[ 1], fp);
	XiahGameEngine::Load( pElementData->m_Size[ 2], fp);

	fread( &pElementData->m_LifeTime, 4, 1, fp);
	fread( &pElementData->m_LifeTimeType, 4, 1, fp);

	fread( &pElementData->m_bCullMode, 4, 1, fp);
	fread( &pElementData->m_bCenterMove, 4, 1, fp);

	// Element Render Blending Option
	if( m_nCurEffectVer < VERSION_UPGRADE0303 )
	{
		pElementData->m_nRenderSrc  = RB_SRCCOLOR;
		pElementData->m_nRenderDest = RB_ONE;
		pElementData->m_nRenderOp	= RBO_ADD;
		pElementData->m_nTextureCOP = TB_MODULATE;
		pElementData->m_nTextureCA1 = TBA_TEXTURE;
		pElementData->m_nTextureCA2 = TBA_CURRENT;
		pElementData->m_nTextureAOP = TB_MODULATE;
		pElementData->m_nTextureAA1 = TBA_TEXTURE;
		pElementData->m_nTextureAA2 = TBA_CURRENT;

		pElementData->m_bEmitter = FALSE;
	}
	else
	if( m_nCurEffectVer == VERSION_UPGRADE0303 )
	{
		fread( &pElementData->m_nRenderSrc,  1, 1, fp );
		fread( &pElementData->m_nRenderDest, 1, 1, fp );
		fread( &pElementData->m_nRenderOp,   1, 1, fp );
		fread( &pElementData->m_nTextureCOP, 1, 1, fp );
		fread( &pElementData->m_nTextureCA1, 1, 1, fp );
		fread( &pElementData->m_nTextureCA2, 1, 1, fp );
		fread( &pElementData->m_nTextureAOP, 1, 1, fp );
		fread( &pElementData->m_nTextureAA1, 1, 1, fp );
		fread( &pElementData->m_nTextureAA2, 1, 1, fp );

		pElementData->m_bEmitter = FALSE;
	}
	else
	if( m_nCurEffectVer == VERSION_20030904 )
	{
		fread( &pElementData->m_nRenderSrc,  1, 1, fp );
		fread( &pElementData->m_nRenderDest, 1, 1, fp );
		fread( &pElementData->m_nRenderOp,   1, 1, fp );
		fread( &pElementData->m_nTextureCOP, 1, 1, fp );
		fread( &pElementData->m_nTextureCA1, 1, 1, fp );
		fread( &pElementData->m_nTextureCA2, 1, 1, fp );
		fread( &pElementData->m_nTextureAOP, 1, 1, fp );
		fread( &pElementData->m_nTextureAA1, 1, 1, fp );
		fread( &pElementData->m_nTextureAA2, 1, 1, fp );

		fread( &pElementData->m_bEmitter, 4, 1, fp );
	}

	// register texture and mesh
	if( bLoadRes )
	{
		m_pEffectResPool->GetTexture( pElementData->m_TexturePath.data() );
		m_pEffectResPool->GetMesh( pElementData->m_MeshPath.data() );
	}

	// 이것은 클라이언트에만 있는 것으로, 다른 툴에 넣어도 상관없음.
	// 저장할때 데이터를 줄이기 위해 이렇게 했음.
	pElementData->m_nRenderSrc	= GetRenderBlend( (__int8)pElementData->m_nRenderSrc );
	pElementData->m_nRenderDest	= GetRenderBlend( (__int8)pElementData->m_nRenderDest );
	pElementData->m_nRenderOp	= GetRenderBlendOP( (__int8)pElementData->m_nRenderOp );

	pElementData->m_nTextureCA1 = GetTextureBlendArg( (__int8)pElementData->m_nTextureCA1 );
	pElementData->m_nTextureCA2 = GetTextureBlendArg( (__int8)pElementData->m_nTextureCA2 );
	pElementData->m_nTextureCOP = GetTextureBlendOP( (__int8)pElementData->m_nTextureCOP );

	pElementData->m_nTextureAA1 = GetTextureBlendArg( (__int8)pElementData->m_nTextureAA1 );
	pElementData->m_nTextureAA2 = GetTextureBlendArg( (__int8)pElementData->m_nTextureAA2 );
	pElementData->m_nTextureAOP = GetTextureBlendOP( (__int8)pElementData->m_nTextureAOP );
}

void CEffect::SaveElement(_ELEMENT *pElementData, FILE *fp)
{
	fwrite( &pElementData->m_Type, 4, 1, fp);
	XiahGameEngine::Load( pElementData->m_MeshPath, fp);
	XiahGameEngine::Load( pElementData->m_TexturePath, fp);

	fwrite( &pElementData->m_WrapCount_X, 4, 1, fp);
	fwrite( &pElementData->m_WrapCount_Y, 4, 1, fp);

	XiahGameEngine::Load( pElementData->m_TexIndex, fp);

	fwrite( &pElementData->m_RenderType, 4, 1,fp);
	XiahGameEngine::Load( pElementData->m_Opacity, fp);

	XiahGameEngine::Load( pElementData->m_Rotation[ 0], fp);
	XiahGameEngine::Load( pElementData->m_Rotation[ 1], fp);
	XiahGameEngine::Load( pElementData->m_Rotation[ 2], fp);

	XiahGameEngine::Load( pElementData->m_Size[ 0], fp);
	XiahGameEngine::Load( pElementData->m_Size[ 1], fp);
	XiahGameEngine::Load( pElementData->m_Size[ 2], fp);

	fwrite( &pElementData->m_LifeTime, 4, 1, fp);
	fwrite( &pElementData->m_LifeTimeType, 4, 1, fp);

	fwrite( &pElementData->m_bCullMode, 4, 1, fp);
	fwrite( &pElementData->m_bCenterMove, 4, 1, fp);

	fwrite( &pElementData->m_nRenderSrc,  1, 1, fp );
	fwrite( &pElementData->m_nRenderDest, 1, 1, fp );
	fwrite( &pElementData->m_nRenderOp,   1, 1, fp );
	fwrite( &pElementData->m_nTextureCOP, 1, 1, fp );
	fwrite( &pElementData->m_nTextureCA1, 1, 1, fp );
	fwrite( &pElementData->m_nTextureCA2, 1, 1, fp );
	fwrite( &pElementData->m_nTextureAOP, 1, 1, fp );
	fwrite( &pElementData->m_nTextureAA1, 1, 1, fp );
	fwrite( &pElementData->m_nTextureAA2, 1, 1, fp );

	fwrite( &pElementData->m_bEmitter, 4, 1, fp );
}

void CEffect::CreateEffect()
{
	_EFFECT* pNewEffect = new _EFFECT;
	if(pNewEffect == NULL)
	{
		DBG_LogFile( _T("CEffect::CreateEffect() fail"));
	}
	m_pCurEffect = pNewEffect;

	pNewEffect->m_EffectID = GetFreeEffectID();
	pNewEffect->m_EffectName = MyString(_T("New Effect"));
	pNewEffect->m_LifeTime = 4000;

	pNewEffect->m_bRepeat = false;
	pNewEffect->m_bTraceMove = false;
	pNewEffect->m_fTraceMoveSpeed = 0.0f;
	pNewEffect->m_fTargetMoveSpeed = 0.0f;
	pNewEffect->m_bTargetMove = false;
	pNewEffect->m_TraceList.clear();
	pNewEffect->m_bRepeatTraceMove = false;
	pNewEffect->m_bPositionFixToParentObject = false;

	pNewEffect->m_Position[ 0].push_back( EFFECTDATA( 0, 0));
	pNewEffect->m_Position[ 0].push_back( EFFECTDATA( pNewEffect->m_LifeTime, 0));
	pNewEffect->m_Position[ 1].push_back( EFFECTDATA( 0, 0));
	pNewEffect->m_Position[ 1].push_back( EFFECTDATA( pNewEffect->m_LifeTime, 0));
	pNewEffect->m_Position[ 2].push_back( EFFECTDATA( 0, 0));
	pNewEffect->m_Position[ 2].push_back( EFFECTDATA( pNewEffect->m_LifeTime, 0));

	pNewEffect->m_Rotation[ 0].push_back( EFFECTDATA( 0, 0));
	pNewEffect->m_Rotation[ 0].push_back( EFFECTDATA( pNewEffect->m_LifeTime, 0));
	pNewEffect->m_Rotation[ 1].push_back( EFFECTDATA( 0, 0));
	pNewEffect->m_Rotation[ 1].push_back( EFFECTDATA( pNewEffect->m_LifeTime, 0));
	pNewEffect->m_Rotation[ 2].push_back( EFFECTDATA( 0, 0));
	pNewEffect->m_Rotation[ 2].push_back( EFFECTDATA( pNewEffect->m_LifeTime, 0));

	pNewEffect->m_Opacity.push_back( EFFECTDATA( 0, 1 ) );
	pNewEffect->m_Opacity.push_back( EFFECTDATA( pNewEffect->m_LifeTime, 1 ) );

//	m_EffectList.insert( EFFECTLIST::value_type( pNewEffect->m_EffectID, pNewEffect ) );
	m_EffectList.insert( EFFECTLIST::value_type( pNewEffect->m_EffectManageID, pNewEffect ) );

	CreateParticle(pNewEffect);
}

_PARTICLE* CEffect::CreateParticle(_EFFECT *pEffect)
{
	_PARTICLE* pParticle = new _PARTICLE;
	if(pParticle == NULL)
	{
		DBG_LogFile( _T("CEffect::CreateParticle fail"));
	}

	float TimeLength = 1000.0f;

	pParticle->m_Position = VECTOR(0,0,0);

	pParticle->m_Rotation[ 0].push_back( EFFECTDATA( 0, 0));
	pParticle->m_Rotation[ 0].push_back( EFFECTDATA( TimeLength, 0));
	pParticle->m_Rotation[ 1].push_back( EFFECTDATA( 0, 0));
	pParticle->m_Rotation[ 1].push_back( EFFECTDATA( TimeLength, 0));
	pParticle->m_Rotation[ 2].push_back( EFFECTDATA( 0, 0));
	pParticle->m_Rotation[ 2].push_back( EFFECTDATA( TimeLength, 0));

	pParticle->m_LifeTimeType = PLTT_Permanent;
	pParticle->m_LifeTime = TimeLength;
	pParticle->m_DelayTime = 0;
	pParticle->m_bForceFirst = FALSE;

	pParticle->m_ElementSpawnType = EST_Multi;
	pParticle->m_ElementSpawnCount = 0;
	pParticle->m_ElementSpawnCountBy = 1;
	pParticle->m_ElementSpawnDelay = 10;

	pParticle->m_SpawnDirection = VECTOR( 0, 0, 0);
	pParticle->m_SpawnVolumeType = PSVT_Sphere;
	pParticle->m_SpawnDirectionType = PSDT_ToOut;	// 무시
	pParticle->m_SpawnPositionAtVolumeType = PSPAVT_OnlyOutSide;

	pParticle->m_SpawnSpeed.push_back( EFFECTDATA( 0, 1 ) );
	pParticle->m_SpawnSpeed.push_back( EFFECTDATA( TimeLength, 1 ) );

	pParticle->m_SpawnVolumeSize[0].push_back( EFFECTDATA( 0, 1));
	pParticle->m_SpawnVolumeSize[0].push_back( EFFECTDATA( TimeLength, 1));
	pParticle->m_SpawnVolumeSize[1].push_back( EFFECTDATA( 0, 1));
	pParticle->m_SpawnVolumeSize[1].push_back( EFFECTDATA( TimeLength, 1));
	pParticle->m_SpawnVolumeSize[2].push_back( EFFECTDATA( 0, 1));
	pParticle->m_SpawnVolumeSize[2].push_back( EFFECTDATA( TimeLength, 1));

	pParticle->m_Gravity[0].push_back( EFFECTDATA( 0, 0));
	pParticle->m_Gravity[0].push_back( EFFECTDATA( TimeLength, 0));
	pParticle->m_Gravity[1].push_back( EFFECTDATA( 0, -2));
	pParticle->m_Gravity[1].push_back( EFFECTDATA( TimeLength, -2));
	pParticle->m_Gravity[2].push_back( EFFECTDATA( 0, 0));
	pParticle->m_Gravity[2].push_back( EFFECTDATA( TimeLength, 0));

	pEffect->m_ParticleList.push_back(pParticle);
	CreateElement(pParticle);

	return pParticle;
}

_ELEMENT* CEffect::CreateElement(_PARTICLE *pParticle)
{
	_ELEMENT* pElement = new _ELEMENT;
	if(pElement == NULL)
	{
		DBG_LogFile( _T("CEffect::CreateElement fail"));
	}


	float TimeLength = 1000.0f;

	pElement->m_Type = PET_Billboard;
	pElement->m_RenderType = PERT_Black;
	pElement->m_WrapCount_X = 1;
	pElement->m_WrapCount_Y = 1;

	pElement->m_TexIndex.push_back( EFFECTDATA( 0, 0));
	pElement->m_TexIndex.push_back( EFFECTDATA( TimeLength, 0));
	pElement->m_Opacity.push_back( EFFECTDATA( 0, 1));
	pElement->m_Opacity.push_back( EFFECTDATA( TimeLength, 1));

	pElement->m_LifeTime = TimeLength;
	pElement->m_LifeTimeType = PELTT_LifeTime;

	pElement->m_Rotation[ 0].push_back( EFFECTDATA( 0, 0));
	pElement->m_Rotation[ 0].push_back( EFFECTDATA( TimeLength, 0));
	pElement->m_Rotation[ 1].push_back( EFFECTDATA( 0, 0));
	pElement->m_Rotation[ 1].push_back( EFFECTDATA( TimeLength, 0));
	pElement->m_Rotation[ 2].push_back( EFFECTDATA( 0, 0));
	pElement->m_Rotation[ 2].push_back( EFFECTDATA( TimeLength, 0));

	pElement->m_Size[ 0].push_back( EFFECTDATA( 0, ELEMENT_INIT_SIZE ));
	pElement->m_Size[ 0].push_back( EFFECTDATA( TimeLength, ELEMENT_INIT_SIZE ));
	pElement->m_Size[ 1].push_back( EFFECTDATA( 0, ELEMENT_INIT_SIZE ));
	pElement->m_Size[ 1].push_back( EFFECTDATA( TimeLength, ELEMENT_INIT_SIZE ));
	pElement->m_Size[ 2].push_back( EFFECTDATA( 0, ELEMENT_INIT_SIZE ));
	pElement->m_Size[ 2].push_back( EFFECTDATA( TimeLength, ELEMENT_INIT_SIZE ));
	
	pElement->m_bCullMode = FALSE;
	pElement->m_bCenterMove = FALSE;

	pElement->m_nRenderSrc  = RB_SRCCOLOR;
	pElement->m_nRenderDest = RB_ONE;
	pElement->m_nRenderOp	= RBO_ADD;
	pElement->m_nTextureCOP = TB_MODULATE;
	pElement->m_nTextureCA1 = TBA_TEXTURE;
	pElement->m_nTextureCA2 = TBA_CURRENT;
	pElement->m_nTextureAOP = TB_MODULATE;
	pElement->m_nTextureAA1 = TBA_TEXTURE;
	pElement->m_nTextureAA2 = TBA_CURRENT;
	pElement->m_bEmitter    = FALSE;

	pParticle->m_ElementList.push_back(pElement);

	// register texture and mesh
	m_pEffectResPool->GetTexture( pElement->m_TexturePath.data() );
	m_pEffectResPool->GetMesh( pElement->m_MeshPath.data() );

	return pElement;
}

int CEffect::GetFreeEffectID()
{
#ifdef	_EFFECTEDITOR
	// DB에서 가장 큰 아이디를 찾는다.
	int nMax = -1;

	CR_EffectInfo EffectInfo(&g_DB);
	EffectInfo.m_strFilter.Format(_T("nType = %d"), 4 );// eEffect
	EffectInfo.Open();

	while( !EffectInfo.IsEOF() )
	{
		if( nMax < EffectInfo.m_nEffectID )
			nMax = EffectInfo.m_nEffectID;

		EffectInfo.MoveNext();
	}// while
	EffectInfo.Close();

	m_nLastAddedEffectID = nMax;
#endif

	return ++m_nLastAddedEffectID;
}

XIAHGE_API _EFFECT * CEffect::GetEffect(DWORD id)
{
	EFFECTLIST::iterator it;
	it = m_EffectList.find(id);
	if( it == m_EffectList.end() )
		return NULL;

	return it->second;
}

XIAHGE_API _EFFECT * CEffect::GetCurEffect()
{
	return m_pCurEffect;
}

XIAHGE_API _EFFECTPACKAGEPAIR * CEffect::GetCurEffectPackagePair()
{
	return m_pCurEffectPackagepair;
}

XIAHGE_API void CEffect::Initialize(LPDIRECT3DDEVICE9 pDevice, int nMaxTextureSize, int nMaxMeshSize)
{
	m_pDevice = pDevice;

	// Init Rendering Pool
	for(int i=0; i<PACKAGEPOOL_MAX; i++)
		m_PackagePool.push_back( &m_PrePackagePool[i] );

	for(i=0; i<PACKAGEPAIRPOOL_MAX; i++)
		m_PackagePairPool.push_back( &m_PrePackagePairPool[i] );

	for(i=0; i<EFFECTRENDERPOOL_MAX; i++)
		m_EffectRenderPool.push_back( &m_PreEffectRenderPool[i] );

	for(i=0; i<PARTICLERENDERPOOL_MAX; i++)
		m_ParticleRenderPool.push_back( &m_PreParticleRenderPool[i] );

	for(i=0; i<ELEMENTRENDERPOOL_MAX; i++)
		m_ElementRenderPool.push_back( &m_PreElementRenderPool[i] );

	for(i=0; i<LIGHTRENDERPOOL_MAX; i++)
		m_LightRenderPool.push_back( &m_PreLightRenderPool[i] );

	// vertex buffer and index buffer pool
	HRESULT hr;
	for(i=0; i<VERTEXRENDERPOOL_MAX; i++)
	{
/*
		hr = m_pDevice->CreateVertexBuffer( ELEMENT_RENDER_MAX*4*sizeof(_ELEMENTVERTEX2), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
					    D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &m_PreVertexRenderPool[i].VB, NULL );
		hr = m_pDevice->CreateIndexBuffer( ELEMENT_RENDER_MAX*4*3*sizeof(WORD), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
						D3DPOOL_DEFAULT, &m_PreVertexRenderPool[i].IB, NULL );
*/
		hr = m_pDevice->CreateVertexBuffer( ELEMENT_RENDER_MAX*4*sizeof(_ELEMENTVERTEX2), 0,
					    D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_PreVertexRenderPool[i].VB, NULL );
		hr = m_pDevice->CreateIndexBuffer( ELEMENT_RENDER_MAX*4*3*sizeof(WORD), 0, D3DFMT_INDEX16,
						D3DPOOL_MANAGED, &m_PreVertexRenderPool[i].IB, NULL );

		m_VertexRenderPool.push_back( &m_PreVertexRenderPool[i] );
	}

	// CEffectResPool => Mesh and Texture Manager
	m_pEffectResPool = new CEffectResPool;
	if(m_pEffectResPool == NULL)
	{
		DBG_LogFile( _T(" CEffect::Initialize fail"));
	}

	m_pEffectResPool->Create( pDevice, nMaxTextureSize, nMaxMeshSize );

	// make empty texture
	m_pEffectResPool->GetTexture(_T(""));
}

XIAHGE_API void CEffect::Release()
{
	if( !m_bReleaseCalled )
	{
#ifdef _DEBUG
		LPCTSTR str = _T("CEffect Release \n");
		::OutputDebugString( str );
#endif

		DeleteAllEffect();
		DeleteAllPackageData();

		//
		for(int i=0; i < VERTEXRENDERPOOL_MAX; ++i)
		{
			if( m_PreVertexRenderPool[i].VB )
			{
				m_PreVertexRenderPool[i].VB->Release();
				m_PreVertexRenderPool[i].VB = NULL;
			}

			if( m_PreVertexRenderPool[i].IB )
			{
				m_PreVertexRenderPool[i].IB->Release();
				m_PreVertexRenderPool[i].IB  = NULL;
			}
		}

		ReleaseAllMeshTexture();

		m_bReleaseCalled = true;

		if( m_pCharFileHandle != NULL )
            fclose( m_pCharFileHandle );
		m_pCharFileHandle = NULL;

		// [12/10/2004] DB 이펙트가 날라가서 함수를 변형이 필요하다.
		if(m_pCharFileHandle2 != NULL)
			fclose(m_pCharFileHandle2);
		m_pCharFileHandle2 = NULL;

		// DB 이펙트가 날라가서 함수를 변형이 필요하다.
		if(m_pCharFileHandle3 != NULL)
			fclose(m_pCharFileHandle3);
		m_pCharFileHandle3 = NULL;

		if( m_pTileFileHandle != NULL )
			fclose( m_pTileFileHandle );
		m_pTileFileHandle = NULL;
	}
}

XIAHGE_API void CEffect::ReleaseAllMeshTexture()
{
	if( m_pEffectResPool )
	{
		delete m_pEffectResPool;
		m_pEffectResPool = NULL;
	}
}

void CEffect::DeleteAllEffect()
{
	// delete effect list
	EFFECTLIST::iterator it;
	for(it=m_EffectList.begin(); it!=m_EffectList.end(); it++)
	{
		_EFFECT *pEffect = it->second;
		DeleteEffect( pEffect );
	}
	m_EffectList.clear();
}

void CEffect::DeleteParticle(_PARTICLE *pParticleData, bool bDeletePointer)
{
	ELEMENTLIST::iterator eit;
	for(eit=pParticleData->m_ElementList.begin(); eit!=pParticleData->m_ElementList.end(); eit++)
	{
		_ELEMENT *pElement = *eit;

		// before delete, scan element texture and decrease RefCount
		if( !pElement->m_TexturePath.empty() )
		{
			_EFFECTTEXTURE* pEffectTexture = m_pEffectResPool->FindTexture(pElement->m_TexturePath);
			if( NULL != pEffectTexture )
				pEffectTexture->m_nRefCount--;
		}
		else
			m_pEffectResPool->DecTextureRefCount(_T(""));

		// scan element mesh
		if( !pElement->m_MeshPath.empty() )
		{
			_EFFECTMESH* pEffectMesh = m_pEffectResPool->FindMesh(pElement->m_MeshPath);
			if( NULL != pEffectMesh )
				pEffectMesh->m_nRefCount--;
		}

		if(pElement)
		{
			delete pElement;
			pElement = NULL;
		}
	}
	pParticleData->m_ElementList.clear();

	if( bDeletePointer )
	{
		if(pParticleData)
		{
			delete pParticleData;
			pParticleData = NULL;
		}
	}
}

void CEffect::DeleteParticle(_EFFECT *pEffect, _PARTICLE *pParticle, bool bDeletePointer)
{
	pEffect->m_ParticleList.remove(pParticle);
	DeleteParticle( pParticle, bDeletePointer );
}

void CEffect::DeleteElement(_PARTICLE *pParticle, _ELEMENT *pElement, bool bDeletePointer)
{
	// before delete, scan element texture and decrease RefCount
	if( !pElement->m_TexturePath.empty() )
	{
		_EFFECTTEXTURE* pEffectTexture = m_pEffectResPool->FindTexture(pElement->m_TexturePath);
		if( NULL != pEffectTexture )
			pEffectTexture->m_nRefCount--;
	}
	else
		m_pEffectResPool->DecTextureRefCount(_T(""));

	// scan element mesh
	if( !pElement->m_MeshPath.empty() )
	{
		_EFFECTMESH* pEffectMesh = m_pEffectResPool->FindMesh(pElement->m_MeshPath);
		if( NULL != pEffectMesh )
			pEffectMesh->m_nRefCount--;
	}

	pParticle->m_ElementList.remove(pElement);

	if( bDeletePointer )
	{
		if(pElement)
		{
			delete pElement;
			pElement = NULL;
		}
	}
}

void CEffect::DeleteEffect(_EFFECT* pEffect, bool bDeletePointer)
{
	if( !pEffect ) return;

	// delete particles and element
	PARTICLELIST::iterator pit;
	for(pit=pEffect->m_ParticleList.begin(); pit!=pEffect->m_ParticleList.end(); pit++)
	{
		_PARTICLE *pParticle = *pit;
		DeleteParticle( pParticle );
	}
	pEffect->m_ParticleList.clear();

/*  지우면 안되
	// delete others... waterfall, thunder, light
	WATERFALLLIST::iterator wit;
	for(wit=pEffect->m_WaterfallList.begin(); wit!=pEffect->m_WaterfallList.end(); wit++)
	{
		CWaterfall* pWaterfall = *wit;
		delete pWaterfall;
	}
	pEffect->m_WaterfallList.clear();

	THUNDERLIST::iterator tit;
	for(tit=pEffect->m_ThunderList.begin(); tit!=pEffect->m_ThunderList.end(); tit++)
	{
		CThunder* pThunder = *tit;
		delete pThunder;
	}
	pEffect->m_ThunderList.clear();
*/

	LIGHTLIST::iterator lit;
	for(lit=pEffect->m_LightList.begin(); lit!=pEffect->m_LightList.end(); lit++)
	{
		CEffectLight* pLight = *lit;
		if(pLight)
		{
			delete pLight;
			pLight = NULL;
		}
	}
	pEffect->m_LightList.clear();

	if( bDeletePointer )
	{
		if(pEffect)
		{
			delete pEffect;
			pEffect = NULL;
		}
	}
}

void CEffect::DeleteEffect(DWORD id)
{
	EFFECTLIST::iterator it;
	it = m_EffectList.find(id);
	if( it == m_EffectList.end() ) return;

	_EFFECT* pEffect = it->second;
	DeleteEffect( pEffect );

	m_EffectList.erase( it );
}

void CEffect::DeleteAllPackageData()
{
	// package
	EFFECTPACKAGELIST::iterator epit;
	for(epit=m_EffectPackageList.begin(); epit!=m_EffectPackageList.end(); epit++)
	{
		_EFFECTPACKAGE* pPackage = *epit;
		if(pPackage)
		{
			delete pPackage;
			pPackage = NULL;
		}
	}// for(m_EffectPackageList)
	m_EffectPackageList.clear();

	// Tile effect package
	EFFECTPACKAGELIST::iterator tepit;
	for(tepit=m_TileEffectPackageList.begin(); tepit!=m_TileEffectPackageList.end(); tepit++)
	{
		_EFFECTPACKAGE* pPackage = *tepit;
		if(pPackage)
		{
			delete pPackage;
			pPackage = NULL;

		}
	}// for(m_TileEffectPackageList)
	m_TileEffectPackageList.clear();
}

_EFFECTRENDER* CEffect::EnqEffectToRender(DWORD id)
{
	_EFFECT* pEffect = GetEffect(id);
	if( NULL == pEffect ) return NULL;
	
	return EnqEffectToRender(pEffect);
}

_EFFECTRENDER* CEffect::EnqEffectToRender(_EFFECT* pEffect)
{
	// effect and particle will enqueue effect render list
	// element will enqueue by SpawnElement() call from particle render

	// 1. EFFECT 
	// Get one first memory in pool and Delete one first memory in pool
	if( m_EffectRenderPool.size() == 0 ) return NULL;

	_EFFECTRENDER* pEffectRender = m_EffectRenderPool.front();
	m_EffectRenderPool.pop_front();

	pEffectRender->m_ParticleList.clear();
	pEffectRender->m_LightList.clear();

	pEffectRender->pEffect = pEffect;
	m_EffectRenderList.push_back( pEffectRender );

	// 2. PARTICLE
	// Get one first memory in pool and Delete one first memory in pool
	PARTICLELIST::iterator pit;
	for(pit=pEffect->m_ParticleList.begin(); pit!=pEffect->m_ParticleList.end(); pit++)
	{
		if( m_ParticleRenderPool.size() == 0 ) break;

		_PARTICLERENDER* pParticleRender = m_ParticleRenderPool.front();
		m_ParticleRenderPool.pop_front();

		pParticleRender->m_VertexRenderList.clear();
		pParticleRender->m_ElementList.clear();

		_PARTICLE* pParticle = *pit;
		pParticleRender->pParticle = pParticle;

		pEffectRender->m_ParticleList.push_back( pParticleRender );

		// get first memory in pool.  optimized vertex and index render data
		ELEMENTLIST::iterator emit;
		for(emit=pParticleRender->pParticle->m_ElementList.begin(); emit!=pParticleRender->pParticle->m_ElementList.end(); emit++)
		{
			_ELEMENT* pElement = *emit;
			if( pElement->m_Type == PET_Mesh ) continue;

			if( m_VertexRenderPool.size() == 0 ) break;

			_VERTEXRENDER* pVertexRender = m_VertexRenderPool.front();
			m_VertexRenderPool.pop_front();

			pVertexRender->pElement = pElement;
			pVertexRender->nVerticesNum = 0;
			pVertexRender->nPrimitiveCount = 0;

			pParticleRender->m_VertexRenderList.push_back( pVertexRender );
		}// for(emit)

	}// for(pit)

	// Effect Light
	pEffectRender->m_LightList.clear();
	if( pEffect->m_LightList.size() > 0 )
	{
		LIGHTLIST::iterator lit;
		for(lit=pEffect->m_LightList.begin(); lit!=pEffect->m_LightList.end(); lit++)
		{
			CEffectLight* pLight = *lit;

			if( m_LightRenderPool.size() == 0 ) break;

			CEffectLight* pNewLight = m_LightRenderPool.front();
			m_LightRenderPool.pop_front();

			// copy
			*pNewLight = *pLight;

			pEffectRender->m_LightList.push_back( pNewLight );
		}// for
	}// if

	return pEffectRender;
}

void CEffect::DeqEffectFromRender(_EFFECTRENDER* pEffectRender)
{
	_EFFECTPACKAGEPAIR* pPackagePair = pEffectRender->pPackagePair;

	EFFECTPACKAGELIST::iterator epit;
	for(epit=pPackagePair->PackageList.begin(); epit!=pPackagePair->PackageList.end(); epit++)
	{
		_EFFECTPACKAGE* pPackage = *epit;

		if( pPackage->pEffectRender == pEffectRender )
		{
			m_PackagePool.push_back( pPackage );
			pPackagePair->PackageList.remove( pPackage );
			break;
		}
	}// for pPackagePair->PackageList

	if( pPackagePair->PackageList.size() == 0 )
	{
		pPackagePair->bNowUsing = false;
		// delete list
		m_PackagePairPool.push_back( pPackagePair );
		m_CurPackagePairList.remove( pPackagePair );
	}

	DeleteEffectRender( pEffectRender );
	m_EffectRenderList.remove( pEffectRender );
	m_CurEffectRenderList.remove( pEffectRender );
}

void CEffect::StartEffectToRender(_EFFECTRENDER* pEffectRender)
{
	// data init in EffectRender and make first init data in ParticleRender
	pEffectRender->m_bPlay = true;
	pEffectRender->m_ElapsedTime = 0;
	pEffectRender->m_Position = VECTOR(0,0,0);
	pEffectRender->m_Rotation = VECTOR(0,0,0);
	pEffectRender->m_CustomPosition = pEffectRender->m_vMoveStartPos;

	if( pEffectRender->m_nStartTime > 0 )
		pEffectRender->m_bOKStart = false;
	else
		pEffectRender->m_bOKStart = true;

	PARTICLERENDERLIST::iterator pit;
	for(pit=pEffectRender->m_ParticleList.begin(); pit!=pEffectRender->m_ParticleList.end(); pit++)
	{
		_PARTICLERENDER* pParticleRender = *pit;

		if(pParticleRender)
		{
			pParticleRender->m_bPlay = true;
			pParticleRender->m_ElapsedTime = 0;
			pParticleRender->m_ElementCreatedCount = 0;
			pParticleRender->m_ElementRenderCount = 0;
			pParticleRender->m_LastSpawnTime = 0;
			pParticleRender->m_bPassDelay = FALSE;
			pParticleRender->m_bPassFirst = FALSE;
			pParticleRender->m_Position = pEffectRender->m_Position;
			pParticleRender->m_Rotation = pEffectRender->m_Rotation;
			pParticleRender->m_ElementList.clear();
		}
		else
		{
			DBG_LogFile( _T("CEffect::StartEffectToRender fail"));
		}

	}// for(pit)


/*	지우면 안되
	// waterfall, thunder, light
	_EFFECT* pEffect = pEffectRender->pEffect;
	WATERFALLLIST::iterator wit;
	for(wit=pEffect->m_WaterfallList.begin(); wit!=pEffect->m_WaterfallList.end(); wit++)
	{
		CWaterfall* pWaterfall = *wit;
		pWaterfall->Start(true);
	}

	THUNDERLIST::iterator tit;
	for(tit=pEffect->m_ThunderList.begin(); tit!=pEffect->m_ThunderList.end(); tit++)
	{
		CThunder* pThunder = *tit;
		pThunder->m_bPlay = true;
	}
*/

#if 0
	LIGHTLIST::iterator lit;
	for(lit=pEffectRender->m_LightList.begin(); lit!=pEffectRender->m_LightList.end(); lit++)
	{
		CEffectLight* pLight = *lit;
		pLight->Start(true);
	}
#endif
}

XIAHGE_API void CEffect::UpdateEffect(DWORD dTime)
{

	// test
//	dTime = 60;

	// 현재 지워질 이펙트 리스트
	DeleteEffectPackagePairList();


	DWORD dwOriginTime = dTime;
	m_CurEffectRenderList.clear();
	//
	EFFECTRENDERLIST DeleteEffectRenderList;
	float fEffectOpacity;

	EFFECTPACKAGEPAIRLIST::iterator eppit;
	for(eppit=m_CurPackagePairList.begin(); eppit!=m_CurPackagePairList.end(); eppit++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *eppit;

		EFFECTPACKAGELIST::iterator epit;
		for(epit=pPackagePair->PackageList.begin(); epit!=pPackagePair->PackageList.end(); epit++)
		{
			_EFFECTPACKAGE* pPackage = *epit;

			_EFFECTRENDER* pEffectRender = pPackage->pEffectRender;
			m_pCurEffectRender = pEffectRender;

			// 화면에 안보이고 반복되면 Update를 하지 않는다.
			if( !pPackagePair->bIsVisible && pEffectRender->pEffect->m_bRepeat )
				break;

			// Repeat
			if( pEffectRender->pEffect->m_bRepeat && !pEffectRender->m_bPlay )
			{
				// 진행시간을 0으로, 글구, Particle만 새로 더 생성해 준다.
				pEffectRender->m_bPlay = true;
				pEffectRender->m_ElapsedTime = 0;
				_EFFECT* pEffect = pEffectRender->pEffect;
				
				PARTICLERENDERLIST NewParticleRenderList;
				
				// PARTICLE
				// Get one first memory in pool and Delete one first memory in pool
				PARTICLELIST::iterator pit;
				for(pit=pEffect->m_ParticleList.begin(); pit!=pEffect->m_ParticleList.end(); pit++)
				{
					_PARTICLE* pOriParticle = *pit;
					
					if( m_ParticleRenderPool.size() == 0 ) break;
					
					_PARTICLERENDER* pParticleRender = m_ParticleRenderPool.front();
					m_ParticleRenderPool.pop_front();
					
					pParticleRender->pParticle = pOriParticle;
					pParticleRender->m_ElementList.clear();
					pParticleRender->m_VertexRenderList.clear();
					
					pEffectRender->m_ParticleList.push_back( pParticleRender );
					NewParticleRenderList.push_back( pParticleRender );
					
					// ELEMENT
					// get first memory in pool.  optimized vertex and index render data
					ELEMENTLIST::iterator emit;
					for(emit=pParticleRender->pParticle->m_ElementList.begin(); emit!=pParticleRender->pParticle->m_ElementList.end(); emit++)
					{
						_ELEMENT* pElement = *emit;
						if( pElement->m_Type == PET_Mesh ) continue;
						
						if( m_VertexRenderPool.size() == 0 ) break;
						
						_VERTEXRENDER* pVertexRender = m_VertexRenderPool.front();
						m_VertexRenderPool.pop_front();
						
						pVertexRender->pElement = pElement;
						pVertexRender->nVerticesNum = 0;
						pVertexRender->nPrimitiveCount = 0;
						
						pParticleRender->m_VertexRenderList.push_back( pVertexRender );
					}// for(emit)
				}// for(pit)
				
				// particle render
				PARTICLERENDERLIST::iterator prit;
				for(prit=NewParticleRenderList.begin(); prit!=NewParticleRenderList.end(); prit++)
				{
					_PARTICLERENDER* pParticleRender = *prit;
					if(pParticleRender)
					{
						pParticleRender->m_bPlay = true;
						pParticleRender->m_ElapsedTime = 0;
						pParticleRender->m_ElementCreatedCount = 0;
						pParticleRender->m_ElementRenderCount = 0;
						pParticleRender->m_LastSpawnTime = 0;
						pParticleRender->m_bPassDelay = FALSE;
						pParticleRender->m_bPassFirst = FALSE;
						pParticleRender->m_Position = pEffectRender->m_Position;
						pParticleRender->m_Rotation = pEffectRender->m_Rotation;
					}
					else
					{
						DBG_LogFile( _T("CEffect::UpdateEffect fail"));
					}
				}// for(NewParticleRenderList)
				
				// Effect Light Render
				LIGHTLIST::iterator lit;
				for(lit=pEffectRender->m_LightList.begin(); lit!=pEffectRender->m_LightList.end(); lit++)
				{
					CEffectLight* pLight = *lit;
					
					pLight->m_ElapsedTime = 0;
				}// for( Effect light )
				
				// 반복되는 이펙트의 사운드
				// 지금은 반복되는 이펙트의 사운드는 첨에만 한번 플레이.
				// m_EffectSound.CreateEffectSoundInstance(pPackagePair->nCharUniqID, pPackagePair->nCharID, pPackagePair->nAniType);
			}// if( m_bRepeat )

			// Ok, normal process.
			// Play?
			if( !pEffectRender->m_bPlay )
			{
				DeleteEffectRenderList.push_back( pEffectRender );
				continue;
			}

			// make quick list
			m_CurEffectRenderList.push_back( pEffectRender );
			
			/*
			// --- UPDATE effect data ---
			*/
			// 해당 캐릭터의 애니메이션 시간이 변경되어 이펙트 시간을 늘리거나 줄려준다.
			dTime = dwOriginTime;
			if( pPackage->m_LifeTimeChange )
			{
				float fGap = (float)pPackage->m_LifeTimeChange / (float)pEffectRender->pEffect->m_LifeTime;
				dTime /= fGap;
			}
			float fTIME = (float)dTime / 1000.0f;

			pEffectRender->m_ElapsedTime += dTime;
			
			// start time
			if( !pEffectRender->m_bOKStart && pEffectRender->m_nStartTime <= pEffectRender->m_ElapsedTime )
			{
				pEffectRender->m_bOKStart = true;
				pEffectRender->m_ElapsedTime -= pEffectRender->m_nStartTime;
			}
			
			if( !pEffectRender->m_bOKStart ) continue;

			// only character studio2 code
#ifdef		_CHARACTERSTUDIO2
			if( pEffectRender->m_nAttachedBoneIndex != -1 )
			{
				if( g_pObjectDoc->m_nWhatDraw == eBCF )
				{
					MATRIX matBone;
					matBone = g_pObjectDoc->m_pMainMesh->GetBoneMatrix( pEffectRender->m_nAttachedBoneIndex );

					pEffectRender->m_vMoveStartPos.x = matBone._41;
					pEffectRender->m_vMoveStartPos.y = matBone._42;
					pEffectRender->m_vMoveStartPos.z = matBone._43;
				}
				else
				if( g_pObjectDoc->m_nWhatDraw == eVUE )
				{
					MATRIX matBone;
					matBone = g_pObjectDoc->GetView()->m_pSoulTreeCharacter->GetBoneMatrix( pEffectRender->m_nAttachedBoneIndex );

					pEffectRender->m_vMoveStartPos.x = matBone._41;
					pEffectRender->m_vMoveStartPos.y = matBone._42;
					pEffectRender->m_vMoveStartPos.z = matBone._43;
				}
			}
#endif

			// 매트릭스에 고정되지 않는 이펙트는 시작 위치를 매트릭스를 곱해서 월드좌표로 시작한다.
			// 고정되는 이펙트는 시작위치를 원점으로 두고 빌보드를 만들때 매트릭스를 곱한다.
			// Attached Bone Position
			if( pPackage->nBoneIndex == -1 )	// Fixed Pos
			{
				if( !pEffectRender->pEffect->m_bPositionFixToParentObject )
				{
					if( pPackagePair->pWorldMatrix == NULL )
                        pEffectRender->m_CustomPosition = pEffectRender->m_vMoveStartPos * pPackagePair->WorldMatrix;
					else
						pEffectRender->m_CustomPosition = pEffectRender->m_vMoveStartPos * (*pPackagePair->pWorldMatrix);
				}
				else
					pEffectRender->m_CustomPosition = pEffectRender->m_vMoveStartPos;
			}
			else	// Bone Pos
			{
				VECTOR vV = VECTOR( 0,0,0 );

				if( !pEffectRender->pEffect->m_bPositionFixToParentObject )
                    pEffectRender->m_CustomPosition = vV * pPackage->BoneMatrix;
				else
					pEffectRender->m_CustomPosition = vV;
			}

			// position
			pEffectRender->m_Position = VECTOR( GetEffectData( pEffectRender->pEffect->m_Position[0], pEffectRender->m_ElapsedTime ),
												GetEffectData( pEffectRender->pEffect->m_Position[1], pEffectRender->m_ElapsedTime ),
												GetEffectData( pEffectRender->pEffect->m_Position[2], pEffectRender->m_ElapsedTime )) + pEffectRender->m_CustomPosition;
			pEffectRender->m_Rotation = VECTOR( GetEffectData( pEffectRender->pEffect->m_Rotation[0], pEffectRender->m_ElapsedTime ),
												GetEffectData( pEffectRender->pEffect->m_Rotation[1], pEffectRender->m_ElapsedTime ),
												GetEffectData( pEffectRender->pEffect->m_Rotation[2], pEffectRender->m_ElapsedTime ));

			// Move Function
			// 이펙트가 전체적으로 목표를 향해 움직이려면 pEffectRender->m_Position를 변화시켜줘야한다.
			if( !pEffectRender->pEffect->m_bTraceMove && pEffectRender->pEffect->m_bTargetMove )	// Target Move
			{
				// m_vTargetMovePos를 클라이언트에서 필요할때마다 변경시켜줘도 됨.
				// world matrix point를 구해서 목표 위치랑 비교해서 position을 변경한다.
				VECTOR vCurV = pEffectRender->m_Position + pEffectRender->m_vTargetMoveDelta;
				vCurV = vCurV * pEffectRender->pPackagePair->WorldMatrix;

				VECTOR vDir = pEffectRender->m_vTargetMovePos - vCurV;
				float fLength = D3DXVec3Length( &vDir );
				D3DXVec3Normalize( &vDir, &vDir );
//				if( fLength > 4.0f )		// 숫치는 알아서 조정하셔~!
//					vDir *= fLength * 1.02f;
//				else
					vDir *= pEffectRender->pEffect->m_fTargetMoveSpeed;

				pEffectRender->m_vTargetMoveDelta += (vDir * fTIME );
				pEffectRender->m_Position += pEffectRender->m_vTargetMoveDelta;
			}// if(m_bTargetMove)
			else
			if( !pEffectRender->pEffect->m_bTargetMove && pEffectRender->pEffect->m_bTraceMove && pEffectRender->pEffect->m_TraceList.size() != 0 )// 궤적 이동.
			{
				if( !pEffectRender->m_bTraceMoveTurn ) // Set Position
				{
					pEffectRender->m_bTraceMoveTurn = true;

					// 반복 설정.
					if( pEffectRender->pEffect->m_bRepeatTraceMove )
					{
						pEffectRender->TraceListIT++;
						if( pEffectRender->TraceListIT == pEffectRender->pEffect->m_TraceList.end() )
							pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
						else
							pEffectRender->TraceListIT--;
					}

					// 리스트의 첫번째 위치가 처음 위치이다.
					if( pEffectRender->TraceListIT == pEffectRender->pEffect->m_TraceList.begin() )
						pEffectRender->m_vTargetMoveDelta = *(pEffectRender->TraceListIT);

					// 다음 목표 위치 이동.
					if( pEffectRender->TraceListIT != pEffectRender->pEffect->m_TraceList.end() )
						pEffectRender->TraceListIT++;
					// end point
					if( pEffectRender->TraceListIT == pEffectRender->pEffect->m_TraceList.end() )
						pEffectRender->TraceListIT--;
				}// if(m_bTraceMoveTurn)

				// Move to Target Position
				pEffectRender->m_vTargetMovePos = *(pEffectRender->TraceListIT);
				VECTOR vCurV = pEffectRender->m_Position + pEffectRender->m_vTargetMoveDelta;

				VECTOR vDir = pEffectRender->m_vTargetMovePos - vCurV;
				float fLength = D3DXVec3Length( &vDir );
				D3DXVec3Normalize( &vDir, &vDir );
				if( fLength < 0.1f ) // 위치 변경
					pEffectRender->m_bTraceMoveTurn = false;

				vDir *= pEffectRender->pEffect->m_fTraceMoveSpeed;

				pEffectRender->m_vTargetMoveDelta += (vDir * fTIME );
				pEffectRender->m_Position += pEffectRender->m_vTargetMoveDelta;
			}// if(m_bTraceMove)
			else
			if( pEffectRender->pEffect->m_bTargetMove && pEffectRender->pEffect->m_bTraceMove && pEffectRender->pEffect->m_TraceList.size() != 0 )// 궤적 이동 + Target Move
			{
				if( !pEffectRender->m_bTraceMoveTurn ) // Set Position
				{
					pEffectRender->m_bTraceMoveTurn = true;

					// 리스트의 첫번째 위치가 처음 위치이다.
					if( pEffectRender->TraceListIT == pEffectRender->pEffect->m_TraceList.begin() )
					{
						pEffectRender->m_vTargetMoveDelta = *(pEffectRender->TraceListIT);
						pEffectRender->m_nTraceListIndex = 1;
					}

					// 다음 목표 위치 이동.
					if( pEffectRender->TraceListIT != pEffectRender->pEffect->m_TraceList.end() )
					{
						pEffectRender->TraceListIT++;
						pEffectRender->m_nTraceListIndex++;
					}
					// end point
					if( pEffectRender->TraceListIT == pEffectRender->pEffect->m_TraceList.end() )
					{
						pEffectRender->TraceListIT--;
						pEffectRender->m_nTraceListIndex--;
					}
				}// if(m_bTraceMoveTurn)

				// Make TargetMovePos, that is, combine trace pos and target pos
				VECTOR vTracePos = *(pEffectRender->TraceListIT);
				VECTOR vCurV = pEffectRender->m_Position + pEffectRender->m_vTargetMoveDelta;

				int nListSize = pEffectRender->pEffect->m_TraceList.size();
				float fWeight = pEffectRender->m_nTraceListIndex * 1.0f / nListSize;

				VECTOR vCombineV = ((1.0f-fWeight*fWeight) * vTracePos) + (fWeight*fWeight * pEffectRender->m_vTargetMovePos);

				VECTOR vDir = vCombineV - vCurV;
				float fLength = D3DXVec3Length( &vDir );
				D3DXVec3Normalize( &vDir, &vDir );
				if( fLength < 0.1f ) // 위치 변경
					pEffectRender->m_bTraceMoveTurn = false;

				vDir *= pEffectRender->pEffect->m_fTraceMoveSpeed;

				pEffectRender->m_vTargetMoveDelta += (vDir * fTIME );
				pEffectRender->m_Position += pEffectRender->m_vTargetMoveDelta;
			}// if(m_bTargetMove && m_bTraceMove)

			//
			if( pEffectRender->m_ElapsedTime > pEffectRender->pEffect->m_LifeTime )
				pEffectRender->m_bPlay = false;

			// 이걸 material, diffuse에 곱해서 최종 투명도를 계산한다.
			fEffectOpacity = GetEffectData( pEffectRender->pEffect->m_Opacity, pEffectRender->m_ElapsedTime );

			// 외공 지속 이펙트가 거의 끝날때 희미하게 사라지도록 한다.
			if( pPackagePair->dwTotalTime > 0 && 
				pPackagePair->dwElapsedTime >= pPackagePair->dwTotalTime-5000 )
			{
				if( pPackagePair->dwElapsedTime > pPackagePair->dwTotalTime )
					pPackagePair->dwElapsedTime = pPackagePair->dwTotalTime;

				fEffectOpacity = ( (pPackagePair->dwTotalTime - pPackagePair->dwElapsedTime)/5000.0f );

				// 정책이 바뀌었음. 최소한 희미한 상태를 유지하게
				if( fEffectOpacity <= 0.2f )
					fEffectOpacity = 0.2f;

				//DBG_Put("TotalTime=%d, ElapsedTime=%d, fOpacity=%6.3f", pPackagePair->dwTotalTime, pPackagePair->dwElapsedTime, fEffectOpacity );
			}// if


			//
			PARTICLERENDERLIST::iterator pit;
			for(pit=pEffectRender->m_ParticleList.begin(); pit!=pEffectRender->m_ParticleList.end(); pit++)
			{
				_PARTICLERENDER* pParticleRender = *pit;
				
				if( !pParticleRender->m_bPlay )
				{
					VERTEXRENDERLIST::iterator vrit;
					for(vrit=pParticleRender->m_VertexRenderList.begin(); vrit!=pParticleRender->m_VertexRenderList.end(); vrit++)
					{
						_VERTEXRENDER* pVertexRender = *vrit;
						m_VertexRenderPool.push_back(pVertexRender);
					}
					pParticleRender->m_VertexRenderList.clear();

					// 2004-02-09, 추가됨. 이런 이런걸 빼먹다니.
					ELEMENTRENDERLIST::iterator erit;
					for(erit=pParticleRender->m_ElementList.begin(); erit!=pParticleRender->m_ElementList.end(); erit++)
					{
						_ELEMENTRENDER* pElementRender = *erit;

						// 솔직히 이젠 이거 필요 없는데.
						pElementRender->m_pTexture->m_nRefCount--;
						if( NULL != pElementRender->m_pMesh )
							pElementRender->m_pMesh->m_nRefCount--;

						m_ElementRenderPool.push_back(pElementRender);
					}
					pParticleRender->m_ElementList.clear();

					pit = pEffectRender->m_ParticleList.erase(pit);
					m_ParticleRenderPool.push_back(pParticleRender);
					if (pit == pEffectRender->m_ParticleList.end()) break;
					continue;
				}
				
				/*
				// --- UPDATE particle data ---
				*/
				pParticleRender->m_ElapsedTime += dTime;
				
				// this is Partice Delay Time
				if( pParticleRender->pParticle->m_DelayTime != 0 )
				{
					if( pParticleRender->m_ElapsedTime < pParticleRender->pParticle->m_DelayTime &&
						!pParticleRender->m_bPassDelay ) continue;
					else
						if( pParticleRender->m_ElapsedTime >= pParticleRender->pParticle->m_DelayTime &&
							!pParticleRender->m_bPassDelay )
						{
							pParticleRender->m_bPassDelay = TRUE;
							pParticleRender->m_ElapsedTime = 0;
						}
				}
				
				// this is Particle Life Time
				if( pParticleRender->pParticle->m_LifeTimeType == PLTT_LifeTime )
				{
					if( pParticleRender->m_ElapsedTime > pParticleRender->pParticle->m_LifeTime )
						//if( pParticleRender->m_ElementCreatedCount == 0 )
						if( pParticleRender->m_ElementRenderCount == 0 )
						{	// no more element, this time is to remove particle
							pParticleRender->m_bPlay = false;
							continue;
						}
				}
				else
				if( pParticleRender->pParticle->m_LifeTimeType == PLTT_Permanent )
				{
					while( pParticleRender->m_ElapsedTime > pParticleRender->pParticle->m_LifeTime )
						pParticleRender->m_ElapsedTime -= pParticleRender->pParticle->m_LifeTime;
				}

				// Matrix calculation
				MATRIX rm, prm;
				D3DXMatrixIdentity( &rm );
				D3DXMatrixIdentity( &prm );

				float fRotXRadian = GetEffectData( pParticleRender->pParticle->m_Rotation[0], pParticleRender->m_ElapsedTime );
				float fRotYRadian = GetEffectData( pParticleRender->pParticle->m_Rotation[1], pParticleRender->m_ElapsedTime );
				float fRotZRadian = GetEffectData( pParticleRender->pParticle->m_Rotation[2], pParticleRender->m_ElapsedTime );

				SetRotationEuler( rm, VECTOR(fRotXRadian, fRotYRadian, fRotZRadian) );

				// effect rotation and position apply
				SetRotationEuler( prm, VECTOR(pEffectRender->m_Rotation.x, pEffectRender->m_Rotation.y, pEffectRender->m_Rotation.z) );

				prm._41 = pEffectRender->m_Position.x;
				prm._42 = pEffectRender->m_Position.y;
				prm._43 = pEffectRender->m_Position.z;

				pParticleRender->m_Position = pParticleRender->pParticle->m_Position * (rm * prm);

				// Spawn Elements by ElementSpawnType
				switch( pParticleRender->pParticle->m_ElementSpawnType )
				{
				case EST_One:
					{
						if( pParticleRender->m_ElementCreatedCount == 0 )
						{
							pParticleRender->m_LastSpawnTime = pParticleRender->m_ElapsedTime;
							SpawnElement( pParticleRender );
						}
					}
					break;
				case EST_Multi:
				case EST_Count:
					{
						if( pParticleRender->m_ElapsedTime - pParticleRender->m_LastSpawnTime >
							pParticleRender->pParticle->m_ElementSpawnDelay )
						{
							pParticleRender->m_LastSpawnTime = pParticleRender->m_ElapsedTime;
							for(int i=0; i<pParticleRender->pParticle->m_ElementSpawnCountBy; i++)
								SpawnElement( pParticleRender );
						}
						else
						if( pParticleRender->pParticle->m_bForceFirst && !pParticleRender->m_bPassFirst )
						{
							for(int i=0; i<pParticleRender->pParticle->m_ElementSpawnCountBy; i++)
								SpawnElement( pParticleRender );
								
							pParticleRender->m_bPassFirst = TRUE;
						}
					}
					break;
				}// switch( pParticleRender->pParticle->m_ElementSpawnType )

				ELEMENTRENDERLIST::iterator erit;
				for(erit=pParticleRender->m_ElementList.begin(); erit!=pParticleRender->m_ElementList.end(); erit++)
				{
					_ELEMENTRENDER* pElementRender = *erit;
					
					if( !pElementRender->m_bPlay )
					{
						pElementRender->m_pTexture->m_nRefCount--;
						if( NULL != pElementRender->m_pMesh )
							pElementRender->m_pMesh->m_nRefCount--;
						
						erit = pParticleRender->m_ElementList.erase(erit);
						m_ElementRenderPool.push_back(pElementRender);

						pParticleRender->m_ElementRenderCount--;

						if( pParticleRender->pParticle->m_ElementSpawnType == EST_One )
						{
							pParticleRender->m_bPlay = false;
							break;
						}
						if (erit == pParticleRender->m_ElementList.end()) break;
						continue;
					}
					
					/*
					// --- Update element data ---
					*/
					pElementRender->m_ElapsedTime += dTime;
					
					if( pElementRender->m_pElement->m_LifeTimeType == PELTT_LifeTime )
					{
						if( pElementRender->m_ElapsedTime > pElementRender->m_pElement->m_LifeTime )
						{
							pElementRender->m_bPlay = false;
							continue;
						}
					}
					
					// render type
					float fOpacity = GetEffectData( pElementRender->m_pElement->m_Opacity, pElementRender->m_ElapsedTime );
					switch( pElementRender->m_pElement->m_RenderType )
					{
					case PERT_Black:
						{
							pElementRender->m_Material.Ambient.r = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Ambient.g = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Ambient.b = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Diffuse.r = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Diffuse.g = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Diffuse.b = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Specular.r = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Specular.g = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Specular.b = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Ambient.a = 1 * fEffectOpacity;
							pElementRender->m_Material.Diffuse.a = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.a = 1 * fEffectOpacity;
						}
						break;
					case PERT_Normal:
					case PERT_AlphaChannel:
						{
							pElementRender->m_Material.Ambient.r = 1 * fEffectOpacity;
							pElementRender->m_Material.Ambient.g = 1 * fEffectOpacity;
							pElementRender->m_Material.Ambient.b = 1 * fEffectOpacity;
							pElementRender->m_Material.Diffuse.r = 1 * fEffectOpacity;
							pElementRender->m_Material.Diffuse.g = 1 * fEffectOpacity;
							pElementRender->m_Material.Diffuse.b = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.r = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.g = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.b = 1 * fEffectOpacity;
							pElementRender->m_Material.Ambient.a = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Diffuse.a = fOpacity * fEffectOpacity;
							pElementRender->m_Material.Specular.a = fOpacity * fEffectOpacity;
						}
						break;
					case PERT_White:
						{
							float fInverse = 1 - fOpacity;
							pElementRender->m_Material.Ambient.r = fInverse * fEffectOpacity;
							pElementRender->m_Material.Ambient.g = fInverse * fEffectOpacity;
							pElementRender->m_Material.Ambient.b = fInverse * fEffectOpacity;
							pElementRender->m_Material.Diffuse.r = 1 * fEffectOpacity;
							pElementRender->m_Material.Diffuse.g = 1 * fEffectOpacity;
							pElementRender->m_Material.Diffuse.b = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.r = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.g = 1 * fEffectOpacity;
							pElementRender->m_Material.Specular.b = 1 * fEffectOpacity;
						}
						break;
					}// switch( pElementRender->m_pElement->m_RenderType )
					
					// other data
					pElementRender->m_TexIndex = (int)GetEffectData( pElementRender->m_pElement->m_TexIndex, pElementRender->m_ElapsedTime );
					pElementRender->m_Size.x = GetEffectData( pElementRender->m_pElement->m_Size[0], pElementRender->m_ElapsedTime );
					pElementRender->m_Size.y = GetEffectData( pElementRender->m_pElement->m_Size[1], pElementRender->m_ElapsedTime );
					pElementRender->m_Size.z = GetEffectData( pElementRender->m_pElement->m_Size[2], pElementRender->m_ElapsedTime );

					// 경험치 이펙트일때는 특별 대우.
					if( pEffectRender->pEffect->m_nEffectType == eExpAcquireEffect )
					{
						// important data
						pElementRender->m_Velocity += pElementRender->m_vGravity * fTIME;
						pElementRender->m_ExtraPosition += pElementRender->m_Velocity * fTIME;	// 원래 위치

//						VECTOR vTargetPos = pEffectRender->m_vTargetMovePos;	// 캐릭터 위치.
						VECTOR vTargetPos = m_vTargetMovePos; // 이펙트 에디터에서 사용하는 변수지만 여기서 사용하자.

						VECTOR vToTargetPos = vTargetPos - pElementRender->m_Position;
						float fDistance = D3DXVec3Length( &vToTargetPos );

						float fWeight = pElementRender->m_ElapsedTime * 1.0f / pElementRender->m_pElement->m_LifeTime;

//						VECTOR vCombineV = ((1.0f-fWeight) * pElementRender->m_ExtraPosition) + (fWeight * vTargetPos);
						VECTOR vCombineV = ((1.0f-fWeight*fWeight) * pElementRender->m_ExtraPosition) + (fWeight*fWeight * vTargetPos);

						VECTOR vDir = (vCombineV - pElementRender->m_Position) * fTIME;
						if( fDistance > 2.0f )
                            vDir *= (fDistance+ (8.5f+(fWeight*5)) );
						else
							vDir *= (10.0f+(fWeight*5));

						pElementRender->m_Position += vDir;		// 블랜딩 위치.
					}
					else
					{
						// important data
						pElementRender->m_Velocity += pElementRender->m_vGravity * fTIME;
						pElementRender->m_Position += pElementRender->m_Velocity * fTIME;
					}
					
					// if element is mesh
					if( pElementRender->m_pElement->m_Type == PET_Mesh )
					{
						MATRIX scale;
						D3DXMatrixIdentity( &scale );
						D3DXMatrixScaling( &scale, pElementRender->m_Size.x, pElementRender->m_Size.y, pElementRender->m_Size.z );

						if( pElementRender->m_pElement->m_bCenterMove )
						{
							MATRIX view, xm;
							D3DXMatrixIdentity( &view );
							D3DXMatrixIdentity( &xm );

							// 원점을 중심으로 뻗어나가는 메트릭스를 만든다.
							D3DXMatrixRotationAxis( &xm, &VECTOR(1,0,0), (3.14f/2.0f) );
							D3DXMatrixLookAtLH( &view, &VECTOR(0,0,0), &pElementRender->m_Velocity, &m_vViewUp );
							D3DXMatrixInverse( &view, NULL, &view );

							pElementRender->m_MeshTM = scale * xm * view;
						}
						else	// normal process
						{
							MATRIX rotate;
							D3DXMatrixIdentity( &rotate );

							float fRotXRadian = GetEffectData( pElementRender->m_pElement->m_Rotation[0], pElementRender->m_ElapsedTime);
							float fRotYRadian = GetEffectData( pElementRender->m_pElement->m_Rotation[1], pElementRender->m_ElapsedTime);
							float fRotZRadian = GetEffectData( pElementRender->m_pElement->m_Rotation[2], pElementRender->m_ElapsedTime);

							SetRotationEuler( rotate, VECTOR(fRotXRadian, fRotYRadian, fRotZRadian) );

							pElementRender->m_MeshTM = scale * rotate;
						}

						// 현재는 빌보드와 메시의 위치를 따로 계산해줘야한다. 원래 그러함
						if( pPackage->nBoneIndex == -1 ) // Fixed Pos
						{
							// World 좌표로 바꿔준다.
							pElementRender->m_MeshTM._41 = pElementRender->m_Position.x - pEffectRender->m_CustomPosition.x + pEffectRender->m_vMoveStartPos.x;
							pElementRender->m_MeshTM._42 = pElementRender->m_Position.y - pEffectRender->m_CustomPosition.y + pEffectRender->m_vMoveStartPos.y;
							pElementRender->m_MeshTM._43 = pElementRender->m_Position.z - pEffectRender->m_CustomPosition.z + pEffectRender->m_vMoveStartPos.z;

							if( pPackagePair->pWorldMatrix == NULL )
                                pElementRender->m_MeshTM *= pPackagePair->WorldMatrix;
							else
								pElementRender->m_MeshTM *= (*pPackagePair->pWorldMatrix);
						}
						else	// Bone Pos
						{
							if( !pEffectRender->pEffect->m_bPositionFixToParentObject )
							{
								pElementRender->m_MeshTM._41 = pElementRender->m_Position.x;
								pElementRender->m_MeshTM._42 = pElementRender->m_Position.y;
								pElementRender->m_MeshTM._43 = pElementRender->m_Position.z;
							}
							else
							{
								// Bone 좌표로 바꿔준다.
								VECTOR vV = pElementRender->m_Position * pPackage->BoneMatrix;
								pElementRender->m_MeshTM._41 = vV.x;
								pElementRender->m_MeshTM._42 = vV.y;
								pElementRender->m_MeshTM._43 = vV.z;
							}

//							pElementRender->m_MeshTM *= pPackage->BoneMatrix;
						}

						// mesh animation, HCH 애니메이션을 이펙트 총 진행시간동안 1번 플레이시킨다. 
						float fTemp = pElementRender->m_ElapsedTime / pElementRender->m_pElement->m_LifeTime;
						pElementRender->m_nCurMeshFrame = (int)(fTemp * pElementRender->m_pMesh->m_nFrameCount );
						
						if( pElementRender->m_nCurMeshFrame > pElementRender->m_pMesh->m_nFrameCount-1 )
							pElementRender->m_nCurMeshFrame = pElementRender->m_pMesh->m_nFrameCount-1;

//						단순히 애니메이션 시키는 코드. 이 코드는 걍 두자.
//						pElementRender->m_nCurMeshFrame =(++pElementRender->m_nCurMeshFrame) % m_pMesh->m_nFrameCount;
					}// if( pElementRender->m_pElement->m_Type == PET_Mesh )
					
				}// for(pParticleRender->m_ElementList[i])

			}// for(pEffectRender->m_ParticleList)

		}// for( pPackagePair->PackageList )

	}// for( m_CurPackagePairList )

	// delete not play effect render list
	EFFECTRENDERLIST::iterator erlist;
	for(erlist=DeleteEffectRenderList.begin(); erlist!=DeleteEffectRenderList.end(); erlist++)
	{
		_EFFECTRENDER* pERender = *erlist;

		DeqEffectFromRender(pERender);
	}// for(DeleteEffectRenderList)
	DeleteEffectRenderList.clear();

	// Original Code
	// Update 부분에서 Vertex Buffer를 만든다.
//	MakeElementVertexToRender(dTime);

	// 이제는 UpdateEffect는 매프레임마다 하고, MakeElementVertexToRender()를 시간단위로 호출하자.
	m_dwVertexBufferMakingElapsedTime += dTime;
	if( m_dwVertexBufferMakingElapsedTime >= m_dwVertexBufferMakingTimeVal )
	{
		MakeElementVertexToRender( m_dwVertexBufferMakingElapsedTime );

		m_dwVertexBufferMakingElapsedTime -= m_dwVertexBufferMakingTimeVal;
	}


	// debug test
	//texture
/*	Debug
	EFFECTTEXTUREMAP texturemap;
	m_pEffectResPool->GetAllTexture(texturemap);
	bool bPass = false;

	EFFECTTEXTUREMAP::iterator etit;
	for(etit=texturemap.begin(); etit!=texturemap.end(); etit++)
	{
		_EFFECTTEXTURE* pTexture = etit->second;
		TRACE2("(Texture) Name : %s, RefCount = %d \n", etit->first, pTexture->m_nRefCount);
		bPass = true;
	}
	if(!bPass) TRACE0("No Texture \n");
	TRACE1("m_nCurTextureAllocated = %d \n", m_pEffectResPool->m_nCurTextureAllocated);

	//mesh
	EFFECTMESHMAP meshmap;
	m_pEffectResPool->GetAllMesh(meshmap);
	bool bMPass =false;

	EFFECTMESHMAP::iterator emit;
	for(emit=meshmap.begin(); emit!=meshmap.end(); emit++)
	{
		_EFFECTMESH* pMesh = emit->second;
		TRACE2("(Mesh) Name : %s, RefCount = %d \n", emit->first, pMesh->m_nRefCount);
		bMPass = true;
	}
	if(!bMPass) TRACE0("No Mesh \n");
	TRACE1("m_nCurMeshAllocated = %d \n", m_pEffectResPool->m_nCurMeshAllocated);
*/

}

void CEffect::DeleteEffectRender(_EFFECTRENDER *pEffectRender)
{
	// render list
	PARTICLERENDERLIST::iterator prit;
	for(prit=pEffectRender->m_ParticleList.begin(); prit!=pEffectRender->m_ParticleList.end(); prit++)
	{
		_PARTICLERENDER* pParticleRender = *prit;
		
		ELEMENTRENDERLIST::iterator erit;
		for(erit=pParticleRender->m_ElementList.begin(); erit!=pParticleRender->m_ElementList.end(); erit++)
		{
			_ELEMENTRENDER* pElementRender = *erit;
			
			pElementRender->m_pTexture->m_nRefCount--;
			if( NULL != pElementRender->m_pMesh )
				pElementRender->m_pMesh->m_nRefCount--;

			m_ElementRenderPool.push_back( pElementRender );
		}// for( pParticleRender->m_ElementList )
		pParticleRender->m_ElementList.clear();
		
		VERTEXRENDERLIST::iterator vrit;
		for(vrit=pParticleRender->m_VertexRenderList.begin(); vrit!=pParticleRender->m_VertexRenderList.end(); vrit++)
		{
			_VERTEXRENDER* pVertexRender = *vrit;
			m_VertexRenderPool.push_back( pVertexRender );
		}// for( pParticleRender->m_VertexRenderList )
		pParticleRender->m_VertexRenderList.clear();
		
		m_ParticleRenderPool.push_back( pParticleRender );
	}// for( pEffectRender->m_ParticleList )
	pEffectRender->m_ParticleList.clear();

	// 이건 모야? 굳이 여기 있을 필요가 없잖아.
//	pEffectRender->pPackagePair = NULL;

	// Effect Light
	LIGHTLIST::iterator lit;
	for(lit=pEffectRender->m_LightList.begin(); lit!=pEffectRender->m_LightList.end(); lit++)
	{
		CEffectLight* pLight = *lit;

		m_LightRenderPool.push_back( pLight );
	}// for
	pEffectRender->m_LightList.clear();

	//
	m_EffectRenderPool.push_back( pEffectRender );
}

void CEffect::DeletePackagePair(int nCharUniqID)
{
/*
	EFFECTRENDERLIST DeleteEffectRenerList;

	//
	EFFECTPACKAGEPAIRLIST::iterator eppit;
	for(eppit=m_CurPackagePairList.begin(); eppit!=m_CurPackagePairList.end(); eppit++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *eppit;

		if( pPackagePair->nCharUniqID != nCharUniqID ) continue;

		EFFECTPACKAGELIST::iterator epit;
		for(epit=pPackagePair->PackageList.begin(); epit!=pPackagePair->PackageList.end(); epit++)
		{
			_EFFECTPACKAGE* pPackage = *epit;

			DeleteEffectRenerList.push_back( pPackage->pEffectRender );
			m_PackagePool.push_back( pPackage );
		}// for pPackagePair->PackageList
		pPackagePair->PackageList.clear();

		m_PackagePairPool.push_back( pPackagePair );
		eppit = m_CurPackagePairList.erase( eppit );
	}// for m_CurPackagePairList

	//
	EFFECTRENDERLIST::iterator erit;
	for(erit=DeleteEffectRenerList.begin(); erit!=DeleteEffectRenerList.end(); erit++)
	{
		_EFFECTRENDER* pEffectRender = *erit;

		DeleteEffectRender( pEffectRender );
		m_EffectRenderList.remove( pEffectRender );

		m_CurEffectRenderList.remove( pEffectRender );

		erit = DeleteEffectRenerList.erase( erit );
	}// for( DeleteEffectRenerList )
*/

}

void CEffect::SpawnElement(_PARTICLERENDER *pParticleRender)
{
	// this is time out
	if( pParticleRender->pParticle->m_LifeTimeType == PLTT_LifeTime )
		if( pParticleRender->m_ElapsedTime > pParticleRender->pParticle->m_LifeTime )
			return;

	// this is count out
	if( pParticleRender->pParticle->m_ElementSpawnType == EST_Count )
		if( pParticleRender->m_ElementCreatedCount > pParticleRender->pParticle->m_ElementSpawnCount )
			return;

	// Memory Full
	if( m_ElementRenderPool.size() == 0 ) return;

	// OK, normal process
	VECTOR vDeltaPos(0,0,0), SpawnDirection(0,0,0);
	D3DXVec3Normalize( &SpawnDirection, &pParticleRender->pParticle->m_SpawnDirection );

	// this is Spawn Volume Type
	if( pParticleRender->pParticle->m_SpawnVolumeType != PSVT_Point )
	{
		float fX = GetEffectData( pParticleRender->pParticle->m_SpawnVolumeSize[0], pParticleRender->m_ElapsedTime );
		float fY = GetEffectData( pParticleRender->pParticle->m_SpawnVolumeSize[1], pParticleRender->m_ElapsedTime );
		float fZ = GetEffectData( pParticleRender->pParticle->m_SpawnVolumeSize[2], pParticleRender->m_ElapsedTime );
		VECTOR vSpawnVolume = VECTOR( fX, fY, fZ );
		VECTOR vHalf_size = vSpawnVolume / 2;

		float fsp_size = D3DXVec3Length( &vSpawnVolume );
		fsp_size /= 2;

		VECTOR vDir( rand() % 360 - 180, rand() % 360 - 180, rand() % 360 - 180);
		D3DXVec3Normalize( &vDir, &vDir );

		switch( pParticleRender->pParticle->m_SpawnPositionAtVolumeType )
		{
		case PSPAVT_OnlyOutSide:
			vDeltaPos = vDir * fsp_size;
			break;
		case PSPAVT_InVolume:
			{
				float frandom_factor;
				frandom_factor = (float)( rand() % 100);
				frandom_factor = 1.0f / frandom_factor;
				vDeltaPos = vDir * fsp_size * frandom_factor;
			}
			break;
		}// switch( pParticleRender->pParticle->m_SpawnPositionAtVolumeType )

		if( pParticleRender->pParticle->m_SpawnVolumeType == PSVT_Box )
		{
			if( vDeltaPos.x < -vHalf_size.x) vDeltaPos.x = vHalf_size.x;
			if( vDeltaPos.y < -vHalf_size.y) vDeltaPos.y = vHalf_size.y;
			if( vDeltaPos.z < -vHalf_size.z) vDeltaPos.z = vHalf_size.z;

			if( vDeltaPos.x > vHalf_size.x) vDeltaPos.x = vHalf_size.x;
			if( vDeltaPos.y > vHalf_size.y) vDeltaPos.y = vHalf_size.y;
			if( vDeltaPos.z > vHalf_size.z) vDeltaPos.z = vHalf_size.z;
		}// 아래의 코드에서 변수 caption이 나오지 않으면 현재 괄호 부분을 주석처리 한다. 그럼 나온다. ^^

		if( pParticleRender->pParticle->m_SpawnDirectionType != PSDT_Custom )
			SpawnDirection = pParticleRender->pParticle->m_SpawnDirectionType == PSDT_ToOut ? vDir : -vDir;

	}// if( pParticleRender->pParticle->m_ElementSpawnType != PSVT_Point )

	// << Animated Data set >>
	// element가 생성될때 속도는 새롭게 계산되서 주어지고, Gravity만 계속 계산해서 주면 
	// 현재 element의 속도를 계산할 수 있다.
	float fX = GetEffectData( pParticleRender->pParticle->m_Gravity[0], pParticleRender->m_ElapsedTime );
	float fY = GetEffectData( pParticleRender->pParticle->m_Gravity[1], pParticleRender->m_ElapsedTime );
	float fZ = GetEffectData( pParticleRender->pParticle->m_Gravity[2], pParticleRender->m_ElapsedTime );
	VECTOR vGravity = VECTOR( fX, fY, fZ );

	// element가 생성될때 변화되는 속도값을 준다.
	float fSpeed = GetEffectData( pParticleRender->pParticle->m_SpawnSpeed, pParticleRender->m_ElapsedTime );
	SpawnDirection *= fSpeed;

	VECTOR vElementPosition = pParticleRender->m_Position + vDeltaPos;

	// Result value is vDeltaPos and SpawnSpeed
	// with this result, create element render list. all element of particle apply this result
	bool bCreatedElement = false;
	ELEMENTLIST::iterator elit;
	for(elit=pParticleRender->pParticle->m_ElementList.begin(); elit!=pParticleRender->pParticle->m_ElementList.end(); elit++)
	{
		if( m_ElementRenderPool.size() == 0 ) break;

		bCreatedElement = true;

		_ELEMENTRENDER* pElementRender = m_ElementRenderPool.front();
		m_ElementRenderPool.pop_front();

		pElementRender->m_pElement = *elit;
		pElementRender->m_Position = vElementPosition;
		pElementRender->m_ExtraPosition = vElementPosition;

		// 빌보드 엘리먼트가 매트릭스에 고정되어 있을때 시작점을 매트릭스에 적용된 위치로 한다.
		// One은 매트릭스에 고정시킬꺼니깐 One이 아닐때.
		if( pElementRender->m_pElement->m_Type == PET_Billboard &&
			pParticleRender->pParticle->m_ElementSpawnType != EST_One &&
			m_pCurEffectRender->pEffect->m_bPositionFixToParentObject &&
			!pElementRender->m_pElement->m_bEmitter )
		{
			// effect matrix
			MATRIX matEffect;
			if( m_pCurEffectRender->pPackage->nBoneIndex == -1 )
			{
				if( m_pCurEffectRender->pPackagePair->pWorldMatrix == NULL )
                    matEffect = m_pCurEffectRender->pPackagePair->WorldMatrix;
				else
					matEffect = *m_pCurEffectRender->pPackagePair->pWorldMatrix;
			}
			else
				matEffect = m_pCurEffectRender->pPackage->BoneMatrix;

			VECTOR vNewPosition = vElementPosition * matEffect;

			pElementRender->m_Position = vNewPosition;
			pElementRender->m_ExtraPosition = vNewPosition;
		}

		pElementRender->m_Velocity = SpawnDirection;
		pElementRender->m_vGravity = vGravity;

		pElementRender->m_bPlay = true;
		pElementRender->m_ElapsedTime = 0;
		pElementRender->m_nCurMeshFrame = 0;

		// set texture and mesh
		pElementRender->m_pTexture = m_pEffectResPool->GetTexture( pElementRender->m_pElement->m_TexturePath.data() );
		pElementRender->m_pMesh = m_pEffectResPool->GetMesh( pElementRender->m_pElement->m_MeshPath.data() );

		pParticleRender->m_ElementList.push_back( pElementRender );

		pParticleRender->m_ElementRenderCount++;
	}// pParticleRender->pParticle->m_ElementList

	if( bCreatedElement )
        pParticleRender->m_ElementCreatedCount++;
}

void CEffect::MakeElementVertexToRender(DWORD dTime)
{
	if( m_dwVertexBufferMakingTimeVal > 520 ) return;

	HRESULT hr;
	MATRIX view;
	m_pDevice->GetTransform( D3DTS_VIEW, (D3DMATRIX *)&view );
	m_vView			= VECTOR( view._13, view._23, view._33 );
	m_vViewRight	= VECTOR( view._11, view._21, view._31 );
	m_vViewUp		= VECTOR( view._12, view._22, view._32 );

	_ELEMENTVERTEX2		TotalBillboardVertex[ ELEMENT_RENDER_MAX*4 ];
	WORD				TotalBillboardIndex[ ELEMENT_RENDER_MAX*6 ];

	// quick list에 있는것만 update
	EFFECTRENDERLIST::iterator erit;
	for(erit=m_CurEffectRenderList.begin(); erit!=m_CurEffectRenderList.end(); erit++)
	{
		_EFFECTRENDER* pEffectRender = *erit;

		// 이펙트는 있는데 화면에 안보이면 안그린다.
		if( !pEffectRender->pPackagePair->bIsVisible ) 
		{
			continue;
		}

		float fEffectOpacity = GetEffectData( pEffectRender->pEffect->m_Opacity, pEffectRender->m_ElapsedTime );

		PARTICLERENDERLIST::iterator prit;
		for(prit=pEffectRender->m_ParticleList.begin(); prit!=pEffectRender->m_ParticleList.end(); prit++)
		{
			_PARTICLERENDER* pParticleRender = *prit;

			// make vertex render data => vertex and index
			VERTEXRENDERLIST::iterator vrit;
			for(vrit=pParticleRender->m_VertexRenderList.begin(); vrit!=pParticleRender->m_VertexRenderList.end(); vrit++)
			{
				_VERTEXRENDER* pVertexRender = *vrit;

				// 바로 Lock을 하지 말고, 우선 데이타부터 만들고 복사하자. 그래야 조금이라도 빨리지지.
				int nVertexIndex = 0;
				// scan current vertex render's element
				ELEMENTRENDERLIST::iterator elrit;
				for(elrit=pParticleRender->m_ElementList.begin(); elrit!=pParticleRender->m_ElementList.end(); elrit++)
				{
					_ELEMENTRENDER* pElementRender = *elrit;

					// different element or same element ?
					if( pVertexRender->pElement != pElementRender->m_pElement ) continue;

					if( pElementRender->m_pElement->m_Type != PET_Billboard ) continue;

					// opacity
					float fOpacity = GetEffectData( pElementRender->m_pElement->m_Opacity, pElementRender->m_ElapsedTime );
					fOpacity *= fEffectOpacity;

					// Build Element Vertex
					VECTOR HalfSize = pElementRender->m_Size / 2.0f;
					MATRIX xm,ym,zm;
					D3DXMatrixIdentity( &xm );
					D3DXMatrixIdentity( &ym );
					D3DXMatrixIdentity( &zm );

					D3DXMatrixRotationAxis( &xm, &m_vViewRight,	GetEffectData( pElementRender->m_pElement->m_Rotation[ 0], pElementRender->m_ElapsedTime) );
					D3DXMatrixRotationAxis( &ym, &m_vViewUp,	GetEffectData( pElementRender->m_pElement->m_Rotation[ 1], pElementRender->m_ElapsedTime) );
					D3DXMatrixRotationAxis( &zm, &m_vView,		GetEffectData( pElementRender->m_pElement->m_Rotation[ 2], pElementRender->m_ElapsedTime) );

					if( !pEffectRender->pEffect->m_bPositionFixToParentObject && !pElementRender->m_pElement->m_bEmitter )
					{	// Effect Editor에서 설정된 방향이 World Matrix에서 그대로.
						pElementRender->m_BillboardVertex[ 0].Position =  - m_vViewUp * HalfSize.y - m_vViewRight * HalfSize.x;
						pElementRender->m_BillboardVertex[ 1].Position =  + m_vViewUp * HalfSize.y - m_vViewRight * HalfSize.x;
						pElementRender->m_BillboardVertex[ 2].Position =  - m_vViewUp * HalfSize.y + m_vViewRight * HalfSize.x;
						pElementRender->m_BillboardVertex[ 3].Position =  + m_vViewUp * HalfSize.y + m_vViewRight * HalfSize.x;

						MATRIX matrix1;
						D3DXMatrixIdentity( &matrix1 );
						matrix1._41 = pElementRender->m_Position.x;
						matrix1._42 = pElementRender->m_Position.y;
						matrix1._43 = pElementRender->m_Position.z;

						MATRIX rm = xm * ym * zm * matrix1;

						for(int i = 0; i < 4; i++)
						{
							pElementRender->m_BillboardVertex[i].Position = pElementRender->m_BillboardVertex[ i].Position * rm;

							pElementRender->m_BillboardVertex[i].diffuse = D3DRGBA(fOpacity,fOpacity,fOpacity,fOpacity);

							pElementRender->m_BillboardVertex[i].Normal = -m_vView;
						}
					}
					else	// Emitter, Effect Editor에서 설정된 방향이 Object Matrix에서 그대로.
					if( pEffectRender->pEffect->m_bPositionFixToParentObject && pElementRender->m_pElement->m_bEmitter )
					{
						// effect matrix
						MATRIX matEffect;
						if( pEffectRender->pPackage->nBoneIndex == -1 )
						{
							if( pEffectRender->pPackagePair->pWorldMatrix == NULL )
                                matEffect = pEffectRender->pPackagePair->WorldMatrix;
							else
								matEffect = *pEffectRender->pPackagePair->pWorldMatrix;
						}
						else
							matEffect = pEffectRender->pPackage->BoneMatrix;

						// new Element Position
						VECTOR vPosition = pElementRender->m_Position * matEffect;

						// new billboard matrix
						MATRIX rm = xm * ym * zm;
						VECTOR vViewUp    = m_vViewUp * rm;
						VECTOR vViewRight = m_vViewRight * rm;

						for(int i = 0; i < 4; i++)
						{
							pElementRender->m_BillboardVertex[i].Position = vPosition;

							pElementRender->m_BillboardVertex[i].diffuse = D3DRGBA(fOpacity,fOpacity,fOpacity,fOpacity);

							pElementRender->m_BillboardVertex[i].Normal = -m_vView;
						}

						pElementRender->m_BillboardVertex[ 0].Position +=  (- vViewUp * HalfSize.y - vViewRight * HalfSize.x);
						pElementRender->m_BillboardVertex[ 1].Position +=  (+ vViewUp * HalfSize.y - vViewRight * HalfSize.x);
						pElementRender->m_BillboardVertex[ 2].Position +=  (- vViewUp * HalfSize.y + vViewRight * HalfSize.x);
						pElementRender->m_BillboardVertex[ 3].Position +=  (+ vViewUp * HalfSize.y + vViewRight * HalfSize.x);
					}
					else	// Object Matrix에는 고정되나 월드상에서 중력이 고대로 적용.
					if( pEffectRender->pEffect->m_bPositionFixToParentObject && !pElementRender->m_pElement->m_bEmitter )
					{
						// One 일때와 아닐때를 구분
						if( pParticleRender->pParticle->m_ElementSpawnType == EST_One )
						{	// 현재 엘리먼트 위치는 원점에서의 위치이므로 매트릭스를 곱해서 계산.
							// effect matrix
							MATRIX matEffect;
							if( pEffectRender->pPackage->nBoneIndex == -1 )
								matEffect = pEffectRender->pPackagePair->WorldMatrix;
							else
								matEffect = pEffectRender->pPackage->BoneMatrix;

							// new Element Position
							VECTOR vPosition = pElementRender->m_Position * matEffect;

							// new billboard matrix
							MATRIX rm = xm * ym * zm;
							VECTOR vViewUp    = m_vViewUp * rm;
							VECTOR vViewRight = m_vViewRight * rm;

							for(int i = 0; i < 4; i++)
							{
								pElementRender->m_BillboardVertex[i].Position = vPosition;

								pElementRender->m_BillboardVertex[i].diffuse = D3DRGBA(fOpacity,fOpacity,fOpacity,fOpacity);

								pElementRender->m_BillboardVertex[i].Normal = -m_vView;
							}

							pElementRender->m_BillboardVertex[ 0].Position +=  (- vViewUp * HalfSize.y - vViewRight * HalfSize.x);
							pElementRender->m_BillboardVertex[ 1].Position +=  (+ vViewUp * HalfSize.y - vViewRight * HalfSize.x);
							pElementRender->m_BillboardVertex[ 2].Position +=  (- vViewUp * HalfSize.y + vViewRight * HalfSize.x);
							pElementRender->m_BillboardVertex[ 3].Position +=  (+ vViewUp * HalfSize.y + vViewRight * HalfSize.x);
						}
						else// 현재 엘리먼트 위치는 월드 좌표이다.
						{
							pElementRender->m_BillboardVertex[ 0].Position =  - m_vViewUp * HalfSize.y - m_vViewRight * HalfSize.x;
							pElementRender->m_BillboardVertex[ 1].Position =  + m_vViewUp * HalfSize.y - m_vViewRight * HalfSize.x;
							pElementRender->m_BillboardVertex[ 2].Position =  - m_vViewUp * HalfSize.y + m_vViewRight * HalfSize.x;
							pElementRender->m_BillboardVertex[ 3].Position =  + m_vViewUp * HalfSize.y + m_vViewRight * HalfSize.x;

							MATRIX matrix1;
							D3DXMatrixIdentity( &matrix1 );
							matrix1._41 = pElementRender->m_Position.x;
							matrix1._42 = pElementRender->m_Position.y;
							matrix1._43 = pElementRender->m_Position.z;

							MATRIX rm = xm * ym * zm * matrix1;

							for(int i = 0; i < 4; i++)
							{
								pElementRender->m_BillboardVertex[i].Position = pElementRender->m_BillboardVertex[ i].Position * rm;

								pElementRender->m_BillboardVertex[i].diffuse = D3DRGBA(fOpacity,fOpacity,fOpacity,fOpacity);

								pElementRender->m_BillboardVertex[i].Normal = -m_vView;
							}
						}
					}

					//
					int x,y;
					float xsize = 1.0f / (float)pElementRender->m_pElement->m_WrapCount_X;
					float ysize = 1.0f / (float)pElementRender->m_pElement->m_WrapCount_Y;

					x = pElementRender->m_TexIndex % pElementRender->m_pElement->m_WrapCount_X;
					y = pElementRender->m_TexIndex / pElementRender->m_pElement->m_WrapCount_X;

					pElementRender->m_BillboardVertex[ 0].tu = x * xsize;
					pElementRender->m_BillboardVertex[ 0].tv = y * ysize + ysize;
					pElementRender->m_BillboardVertex[ 1].tu = x * xsize;
					pElementRender->m_BillboardVertex[ 1].tv = y * ysize;
					pElementRender->m_BillboardVertex[ 2].tu = x * xsize + xsize;
					pElementRender->m_BillboardVertex[ 2].tv = y * ysize + ysize;
					pElementRender->m_BillboardVertex[ 3].tu = x * xsize + xsize;
					pElementRender->m_BillboardVertex[ 3].tv = y * ysize;

					// vertex buffer
					memcpy( &TotalBillboardVertex[nVertexIndex*4], pElementRender->m_BillboardVertex, sizeof(_ELEMENTVERTEX2)*4 );

					// index buffer
					WORD* pIndex = &TotalBillboardIndex[nVertexIndex*6];
					pIndex[0] = nVertexIndex*4 +0;
					pIndex[1] = nVertexIndex*4 +1;
					pIndex[2] = nVertexIndex*4 +2;
					pIndex[3] = nVertexIndex*4 +1;
					pIndex[4] = nVertexIndex*4 +3;
					pIndex[5] = nVertexIndex*4 +2;

					nVertexIndex++;

					// Vertex Buffer에서 Vertex개수가 1000을 넘지 않게 한다.
					// 빌보드니깐, 직사각형이 4개의 Vertex를 이룬다.
					if( nVertexIndex == ELEMENT_RENDER_MAX )
						break;
				}// for( pParticleRender->m_ElementList )

				if( nVertexIndex > 0 )
				{
					// Get Vertex Buffer and Index Buffer
					_ELEMENTVERTEX2* pDestVertices;
					hr = pVertexRender->VB->Lock( 0, 0, (void**)&pDestVertices, 0 );

					WORD* pDestIndex;
					hr = pVertexRender->IB->Lock( 0, 0, (void**)&pDestIndex, 0 );

					memcpy( pDestVertices, TotalBillboardVertex, sizeof(_ELEMENTVERTEX2)*4*nVertexIndex );

					memcpy( pDestIndex, TotalBillboardIndex, sizeof(WORD)*6*nVertexIndex );

					pVertexRender->nVerticesNum = nVertexIndex * 4;
					pVertexRender->nPrimitiveCount = nVertexIndex * 2;
					pVertexRender->VB->Unlock();
					pVertexRender->IB->Unlock();
				}
				else
				{
					pVertexRender->nVerticesNum		= 0;
					pVertexRender->nPrimitiveCount	= 0;
				}

			}// for( pParticleRender->m_VertexRenderList )

		}// for(pEffectRender->m_ParticleList)

	}// for(m_EffectRenderList)

}

XIAHGE_API void CEffect::RenderEffect(DWORD dTime)
{
	// test
//	dTime = 60;

//	MakeElementVertexToRender(dTime);

	if( m_dwVertexBufferMakingTimeVal > 520 ) return;

	MATRIX iTM;
	D3DXMatrixIdentity( &iTM );
	HRESULT hr;

	D3DMATERIAL9 BillboardMaterial;
	D3DMATERIAL9 BackupMaterial;

	m_pDevice->GetMaterial(&BackupMaterial);
	memset( &BillboardMaterial, 1, sizeof(D3DMATERIAL9) );

	DWORD dwLight;
	m_pDevice->GetRenderState( D3DRS_LIGHTING, &dwLight );
	m_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	m_pDevice->GetRenderState( D3DRS_FOGENABLE, &m_dwFogEnable );
	m_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	m_pDevice->GetRenderState( D3DRS_AMBIENT, &m_dwAmbient);
	m_pDevice->SetRenderState( D3DRS_AMBIENT, RGBA_MAKE( 190, 190, 190, 100) );

	m_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
    m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);

	DWORD dwSpecular;
	m_pDevice->GetRenderState( D3DRS_SPECULARENABLE, &dwSpecular );
	m_pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	m_nTotalRenderVertexCount	= 0;
	m_nTotalRenderFaceCount		= 0;

	/*
	// --- Element render ---
	*/
	EFFECTRENDERLIST::iterator erit;
	for(erit=m_CurEffectRenderList.begin(); erit!=m_CurEffectRenderList.end(); erit++)
	{
		_EFFECTRENDER* pEffectRender = *erit;

		// 이펙트는 있는데 화면에 안보이면 안그린다.
		if( !pEffectRender->pPackagePair->bIsVisible ) 
		{
			continue;
		}

		PARTICLERENDERLIST::iterator prit;
		for(prit=pEffectRender->m_ParticleList.begin(); prit!=pEffectRender->m_ParticleList.end(); prit++)
		{
			_PARTICLERENDER* pParticleRender = *prit;

			m_pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_COLOR1);// D3DMCS_COLOR1
			m_pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,	D3DMCS_COLOR1);// D3DMCS_MATERIAL
			m_pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE,		D3DMCS_COLOR1);

			// first, render billboard
			VERTEXRENDERLIST::iterator vrit;
			for(vrit=pParticleRender->m_VertexRenderList.begin(); vrit!=pParticleRender->m_VertexRenderList.end(); vrit++)
			{
				_VERTEXRENDER* pVertexRender = *vrit;

				if( pParticleRender->m_ElementList.size() == 0 ) continue;
				if( pVertexRender->nVerticesNum == 0 ) continue;

				// Find matching element render
				_ELEMENTRENDER* pElementRender = NULL;
				ELEMENTRENDERLIST::iterator elrit;
				for(elrit=pParticleRender->m_ElementList.begin(); elrit!=pParticleRender->m_ElementList.end(); elrit++)
				{
					pElementRender = *elrit;
					if( pElementRender->m_pElement == pVertexRender->pElement ) break;
				}

				// render
				// 원래 이건데 잠시 바꾼다.
//				m_pDevice->SetMaterial( &pElementRender->m_Material);

				// 빌보드를 VB로 합쳐서 그리는 바람에 매터리얼로 투명도를 조절해버리면 
				// 빌보드 통째로 적용된다. 그래서 엘리먼트 따로따로 적용 시키기 위해서
				// 빌보드는 Diffuse를 vertex 구조에 추가시켰다. 이것으로 알파를 적용.
				m_pDevice->SetMaterial( &BillboardMaterial );
				g_Device.SetTexture(0, pElementRender->m_pTexture->m_pTexture);
				//m_pDevice->SetTexture( 0, pElementRender->m_pTexture->m_pTexture );
				m_pDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&iTM );// billboard world

				// set render and texture state
				m_pDevice->SetRenderState( D3DRS_SRCBLEND,  pElementRender->m_pElement->m_nRenderSrc  );
				m_pDevice->SetRenderState( D3DRS_DESTBLEND, pElementRender->m_pElement->m_nRenderDest );
				m_pDevice->SetRenderState( D3DRS_BLENDOP,	pElementRender->m_pElement->m_nRenderOp   );

				m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, pElementRender->m_pElement->m_nTextureCA1 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, pElementRender->m_pElement->m_nTextureCA2 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   pElementRender->m_pElement->m_nTextureCOP );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, pElementRender->m_pElement->m_nTextureAA1 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, pElementRender->m_pElement->m_nTextureAA2 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   pElementRender->m_pElement->m_nTextureAOP );

				// Normal Process
				if( pVertexRender->nVerticesNum )
				{
					g_Device.SetStreamSource( pVertexRender->VB, sizeof(_ELEMENTVERTEX2) );
					g_Device.SetIndices( pVertexRender->IB );
					m_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
					g_Device.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
					//m_pDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
					hr = m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pVertexRender->nVerticesNum, 0, pVertexRender->nPrimitiveCount );

					m_nTotalRenderVertexCount	+= pVertexRender->nVerticesNum;
					m_nTotalRenderFaceCount		+= pVertexRender->nPrimitiveCount;
				}// if

			}// for( pParticleRender->m_VertexRenderList )

			m_pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_MATERIAL);// D3DMCS_COLOR1
			m_pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,	D3DMCS_MATERIAL);// D3DMCS_MATERIAL
			m_pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE,		D3DMCS_MATERIAL);

			DWORD dwBackup;
			m_pDevice->GetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, &dwBackup );
			MATRIX BackupTextureTS;
			m_pDevice->GetTransform( D3DTS_TEXTURE0, (D3DMATRIX *)&BackupTextureTS);

			// second, render mesh
			ELEMENTRENDERLIST::iterator elrit;
			for(elrit=pParticleRender->m_ElementList.begin(); elrit!=pParticleRender->m_ElementList.end(); elrit++)
			{
				_ELEMENTRENDER* pElementRender = *elrit;

				if( pElementRender->m_pElement->m_Type != PET_Mesh ) continue;

				if( !pElementRender->m_pElement->m_bCullMode )
					m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
					
				m_pDevice->SetMaterial( &pElementRender->m_Material );
				g_Device.SetTexture(0, pElementRender->m_pTexture->m_pTexture);
				//m_pDevice->SetTexture( 0, pElementRender->m_pTexture->m_pTexture );

				// set render and texture state
				m_pDevice->SetRenderState( D3DRS_SRCBLEND,  pElementRender->m_pElement->m_nRenderSrc  );
				m_pDevice->SetRenderState( D3DRS_DESTBLEND, pElementRender->m_pElement->m_nRenderDest );
				m_pDevice->SetRenderState( D3DRS_BLENDOP,	pElementRender->m_pElement->m_nRenderOp   );

				m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, pElementRender->m_pElement->m_nTextureCA1 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, pElementRender->m_pElement->m_nTextureCA2 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   pElementRender->m_pElement->m_nTextureCOP );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, pElementRender->m_pElement->m_nTextureAA1 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, pElementRender->m_pElement->m_nTextureAA2 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   pElementRender->m_pElement->m_nTextureAOP );

				// calculate matrix
//				MATRIX WorldMatrix = pElementRender->m_MeshTM;// * pEffectRender->pPackagePair->WorldMatrix;
//				m_pDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&WorldMatrix ); // (D3DMATRIX *)&pElementRender->m_MeshTM );
				m_pDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&pElementRender->m_MeshTM );

				MATRIX tm;
				D3DXMatrixIdentity( &tm );
				float txsize = 1.0f / (float)pElementRender->m_pElement->m_WrapCount_X;
				float tysize = 1.0f / (float)pElementRender->m_pElement->m_WrapCount_Y;
					
				tm._11 = txsize;
				tm._22 = tysize;
				tm._31 = txsize * ( pElementRender->m_TexIndex % pElementRender->m_pElement->m_WrapCount_X);
				tm._32 = tysize * ( pElementRender->m_TexIndex / pElementRender->m_pElement->m_WrapCount_X);

				g_Device.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
				//m_pDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
				m_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
				m_pDevice->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX *)&tm);

				for(int i=0; i<pElementRender->m_pMesh->m_nMeshCount; i++)
				{
					_MESHBUFFER* pBuffer = pElementRender->m_pMesh->m_pMeshBuffer + i;

					if( pBuffer && pBuffer->pVB && pBuffer->pIB )
					{
						g_Device.SetStreamSource( pBuffer->pVB, sizeof(_ELEMENTVERTEX) );
						g_Device.SetIndices( pBuffer->pIB );
						m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, pElementRender->m_nCurMeshFrame*pBuffer->nVertexCount, 0, pBuffer->nVertexCount, 0, pBuffer->nFaceCount );
					}

/*
					hr = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, pBuffer->nVertexCount, pBuffer->nFaceCount, pBuffer->pFace,
                         D3DFMT_INDEX16, pBuffer->pVertex + pElementRender->m_nCurMeshFrame * pBuffer->nVertexCount, sizeof(_ELEMENTVERTEX) );
*/

					m_nTotalRenderVertexCount	+= pBuffer->nVertexCount;
					m_nTotalRenderFaceCount		+= pBuffer->nFaceCount;
				}// for( pElementRender->m_pMesh->m_nMeshCount )

				if( !pElementRender->m_pElement->m_bCullMode )
					m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

			}// for( pParticleRender->m_ElementList )

			// restore
			m_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, dwBackup );
			m_pDevice->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX *)&BackupTextureTS);

		}// for( pEffectRender->m_ParticleList )

	}// for( m_EffectRenderList )

//	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

	if( !dwLight )
		m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// restore
	m_pDevice->SetMaterial(&BackupMaterial);
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	m_pDevice->SetRenderState( D3DRS_SPECULARENABLE, dwSpecular );
}

XIAHGE_API void CEffect::RenderOthers(DWORD dTime)
{
	//	Billow(2002.01.15) : Light Index 초기화
	for( int i=2; i < EFFECTLIGHT_INDEX_MAX; i++)
		m_pDevice->LightEnable( i, FALSE );

	// 이펙트 라이팅은 전체를 통털어 인덱싱한다. => 2, 3, 4, 5, 6, 7, 8
	int nEffectLightIndex = 1;

	EFFECTRENDERLIST::iterator eit;
	for(eit=m_CurEffectRenderList.begin(); eit!=m_CurEffectRenderList.end(); eit++)
	{
		_EFFECTRENDER* pEffectRender = *eit;

		// 이펙트는 있는데 화면에 안보이면 안그린다.
		if( !pEffectRender->pPackagePair->bIsVisible ) 
		{
			continue;
		}

		// Effect Light
		LIGHTLIST::iterator lit;
		for(lit=pEffectRender->m_LightList.begin(); lit!=pEffectRender->m_LightList.end(); lit++)
		{
			CEffectLight* pLight = *lit;

			if( !pLight->m_bIsSetMatrix )
			{
				if( pEffectRender->pPackagePair->pWorldMatrix == NULL )
                    pLight->SetMatrix( &pEffectRender->pPackagePair->WorldMatrix );
				else
					pLight->SetMatrix( pEffectRender->pPackagePair->pWorldMatrix );
			}

			pLight->Update( dTime );

			if( pLight->m_bTurnOn && pLight->m_bPlay )
			{
				// 이펙트 라이트가 많으면 과감히 생략한다.
				if( ++nEffectLightIndex >= EFFECTLIGHT_INDEX_MAX )
					continue;

				pLight->m_nLightID = nEffectLightIndex;

				m_pDevice->SetLight( pLight->m_nLightID, &pLight->m_Light );
				m_pDevice->LightEnable( pLight->m_nLightID, TRUE );
			}// if
//			else
//			m_pDevice->LightEnable( pLight->m_nLightID, FALSE );
		}// for( Light )


		/*	아직 사용은 하지 말기를.
		// waterfall
		nCount = pEffect->m_WaterfallList.size();
		if( nCount > 0 )
		{
			WATERFALLLIST::iterator wit;
			for(wit=pEffect->m_WaterfallList.begin(); wit!=pEffect->m_WaterfallList.end(); wit++)
			{
				CWaterfall *pWaterfall = *wit;

				if( pWaterfall->m_bPlay )
					pWaterfall->Flow();
			}
		}

		// thunder
		nCount = pEffect->m_ThunderList.size();
		if( nCount > 0 )
		{
			THUNDERLIST::iterator tit;
			for(tit=pEffect->m_ThunderList.begin(); tit!=pEffect->m_ThunderList.end(); tit++)
			{
				CThunder *pThunder = *tit;

				if( pThunder->m_bPlay )
					pThunder->Render();
			}
		}
		*/
 

	}// for( m_EffectRenderList )

	if( m_dwFogEnable )
		m_pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );

	m_pDevice->SetRenderState( D3DRS_AMBIENT, m_dwAmbient);
}

_EFFECTTEXTURE* CEffect::GetTexture(LPCTSTR filename)
{
	_EFFECTTEXTURE* pTexture = m_pEffectResPool->GetTexture(filename);

	return pTexture;
}

void CEffect::DeleteTexture(MyString strFilename)
{
	m_pEffectResPool->DeleteTexture( strFilename );
}

void CEffect::GetAllTexture(EFFECTTEXTUREMAP &texturemap)
{
	m_pEffectResPool->GetAllTexture( texturemap );
}

_EFFECTMESH* CEffect::GetMesh(LPCTSTR filename)
{
	_EFFECTMESH* pMesh = m_pEffectResPool->GetMesh(filename);

	return pMesh;
}

void CEffect::DeleteMesh(MyString strFilename)
{
	m_pEffectResPool->DeleteMesh( strFilename );
}

void CEffect::GetAllMesh(EFFECTMESHMAP &meshmap)
{
	m_pEffectResPool->GetAllMesh( meshmap );
}

void CEffect::IncTextureRefCount(MyString strFilename)
{
	m_pEffectResPool->IncTextureRefCount( strFilename );
}

void CEffect::DecTextureRefCount(MyString strFilename)
{
	m_pEffectResPool->DecTextureRefCount( strFilename );
}

void CEffect::IncMeshRefCount(MyString strFilename)
{
	m_pEffectResPool->IncMeshRefCount( strFilename );
}

void CEffect::DecMeshRefCount(MyString strFilename)
{
	m_pEffectResPool->DecMeshRefCount( strFilename );
}

/*	지우면 안되
CWaterfall* CEffect::AddWaterfall(_EFFECT *pEffect)
{
	CWaterfall* pWaterfall = new CWaterfall;

	pWaterfall->Init( 0, 0, 5, 0, 4, 25, 5, 1, 1 );

	pEffect->m_WaterfallList.push_back(pWaterfall);

	return pWaterfall;
}

CThunder * CEffect::AddThunder(_EFFECT *pEffect)
{
	CThunder* pThunder = new CThunder;

	pThunder->Init( 0, 7, 0, 5, 6, 2, 2, 900, 6 );

	pEffect->m_ThunderList.push_back(pThunder);

	return pThunder;
}
*/

CEffectLight* CEffect::AddLight(_EFFECT *pEffect)
{
	CEffectLight* pLight = new CEffectLight;
	if(pLight == NULL)
	{
		DBG_LogFile( _T("CEffect::AddLight fail"));
	}

	pLight->Init(0,0,0,0,0,0,0,0,0,0,0,0);
	pLight->m_Range.push_back(EFFECTDATA(0,1));
	pLight->m_Range.push_back(EFFECTDATA(pEffect->m_LifeTime, 1));
	pLight->m_LifeTime = pEffect->m_LifeTime;
	pLight->m_nLightID = pEffect->m_nLightIndex++;

	pEffect->m_LightList.push_back(pLight);

	return pLight;
}

/*지우면 안되
void CEffect::DeleteWaterfall(_EFFECT *pEffect, CWaterfall *pWaterfall)
{
	WATERFALLLIST::iterator wit;
	wit = pEffect->m_WaterfallList.find(pWaterfall);

	if( wit != pEffect->m_WaterfallList.end() )	// Find
	{
		pEffect->m_WaterfallList.erase( wit );
		delete pWaterfall;
	}
}

void CEffect::DeleteThunder(_EFFECT *pEffect, CThunder *pThunder)
{
	THUNDERLIST::iterator tit;
	tit = pEffect->m_ThunderList.find(pThunder);

	if( tit != pEffect->m_ThunderList.end() ) // Find
	{
		pEffect->m_ThunderList.erase( tit );
		delete pThunder;
	}
}
*/

void CEffect::DeleteLight(_EFFECT *pEffect, CEffectLight *pLight)
{
	LIGHTLIST::iterator lit;
	for(lit=pEffect->m_LightList.begin(); lit!=pEffect->m_LightList.end(); lit++)
	{
		CEffectLight* pELight = *lit;

		if( pELight == pLight )	// Find
		{
			pEffect->m_nLightIndex--;
			pEffect->m_LightList.erase( lit );
			if(pLight)
			{
				delete pLight;
				pLight = NULL;
			}
			break;
		}
	}
}

XIAHGE_API void CEffect::LoadEffectFromPackage()
{
	// file name
	MyString strBasePath	= NEW_EFFECT_PATH;
	MyString strEffectch	= strBasePath + _T("Effect_ch.dat");
	MyString strEffecteff1	= strBasePath + _T("Effect_eff.dat");
	MyString strEffecteff2	= strBasePath + _T("Effect_eff.idx");	
	MyString strEffectres	= strBasePath + _T("Effect_res.dat");
	MyString strEffectmsh2	= strBasePath + _T("Effect_msh.idx");

	// 굳이 이렇게 안해도 되지만, 난 완벽을 좋아해~~  *^^*;
	// open file and error check
	FILE *Effect_ch = _tfopen( strEffectch.data(), _T("rb") );
	if( !Effect_ch )
	{
		LPCTSTR str=_T("Effect_ch.dat file can't open \n");
		::OutputDebugString(str);
		return;
	}

	FILE *Effect_eff1 = _tfopen( strEffecteff1.data(), _T("rb") );
	if( !Effect_eff1 )
	{
		LPCTSTR str=_T("Effect_eff.dat file can't open \n");
		::OutputDebugString(str);

		fclose(Effect_ch);
		return;
	}

	FILE *Effect_eff2 = _tfopen( strEffecteff2.data(), _T("rb") );
	if( !Effect_eff2 )
	{
		LPCTSTR str=_T("Effect_eff.idx file can't open \n");
		::OutputDebugString(str);

		fclose(Effect_ch);
		fclose(Effect_eff1);
		return;
	}

	FILE *Effect_res = _tfopen( strEffectres.data(), _T("rb") );
	if( !Effect_res )
	{
		LPCTSTR str=_T("Effect_res.idx file can't open \n");
		::OutputDebugString(str);

		fclose(Effect_ch);
		fclose(Effect_eff1);
		fclose(Effect_eff2);
		return;
	}

	FILE *Effect_msh2 = _tfopen( strEffectmsh2.data(), _T("rb") );
	if( !Effect_msh2 )
	{
		LPCTSTR str=_T("Effect_msh.idx file can't open \n");
		::OutputDebugString(str);

		fclose(Effect_ch);
		fclose(Effect_eff1);
		fclose(Effect_eff2);
		fclose(Effect_res);
		return;
	}

	// OK Go!

	// Effect_ch.dat file size check
	long start, end;
	start = ftell( Effect_ch );
	fseek( Effect_ch, 0, SEEK_END );
	end = ftell( Effect_ch );

	int nEffectchSize = end - start;
	fseek( Effect_ch, 0, SEEK_SET );

	// Load Effect package data and make package list
	fpos_t Effect_chPos;
	fgetpos(Effect_ch, &Effect_chPos);		// file start
	while( Effect_chPos < nEffectchSize )
	{
		m_pCurEffectPackage = NULL;
		_EFFECTPACKAGE* pPackage = new _EFFECTPACKAGE;

		// load Effect_ch.dat
		fread( &pPackage->nCharID,		4, 1, Effect_ch );
		fread( &pPackage->nAniType,		4, 1, Effect_ch );
		fread( &pPackage->nEffectID,	4, 1, Effect_ch );
		fread( &pPackage->nPosX,		4, 1, Effect_ch );
		fread( &pPackage->nPosY,		4, 1, Effect_ch );
		fread( &pPackage->nPosZ,		4, 1, Effect_ch );
		fread( &pPackage->nBoneIndex,	4, 1, Effect_ch );
		fread( &pPackage->nStartTime,	4, 1, Effect_ch );

		// load Effect_eff.idx
		DWORD dwEffOffset;
		int nEffLen;
		fread( &dwEffOffset,	4, 1, Effect_eff2 );
		fread( &nEffLen,		4, 1, Effect_eff2 );

		// load Effect_eff.dat and save temp eff so load effect data from temp eff
		BYTE* pBuffer = new BYTE[ nEffLen ];
		fseek( Effect_eff1, dwEffOffset, SEEK_SET );
		fread( pBuffer, sizeof(BYTE), nEffLen, Effect_eff1 );

		MyString strFileTempEff = strBasePath + _T("tmep01.eff");
		FILE *fileTempEff = _tfopen( strFileTempEff.data(), _T("wb") );
		fwrite( pBuffer, sizeof(BYTE), nEffLen, fileTempEff );
		delete []pBuffer;
		fclose(fileTempEff);

		m_pCurEffectPackage = pPackage;

		// load effect
		FILE *fp;
		fp = _tfopen( strFileTempEff.data(), _T("rb") );
		bool bLoaded = LoadEffect(fp, false);				// don't load texture and mesh
		fclose(fp);
		_tunlink( strFileTempEff.data() );

		if( bLoaded )
		{
			pPackage->strEffectName = m_pCurEffect->m_EffectName;
			m_EffectPackageList.push_back( pPackage );
		}
		else
			delete pPackage;

/*		일단은 이걸 주석처리하자 절대 지우면 안됨~~!!
		//
		// 아래 코드는 필요한 하드 코딩이다.
		//

		// 1. 캐릭터의 264 Ani는 이펙트를 로딩하기 위한 더미 캐릭터이다.
		if( pPackage->nAniType == 264 )
		{
			if( pPackage->strEffectName == _T("Level UP") )	// 캐릭터의 갑자 상승 Level up 이펙트.
				m_pLevelUpGapJaPackage = pPackage;
			else
			if( pPackage->strEffectName == _T("Level UP2") )// 캐릭터의 수련치 상승 Level up 이펙트.
				m_pLevelUpTPPackage = pPackage;
			else
			if( pPackage->strEffectName == _T("수련외공") )	// 캐릭터의 외공 수련 Level up 이펙트.
				m_pLevelUpOutGongPackage = pPackage;
			else
			if( pPackage->strEffectName == _T("수련내공") )	// 캐릭터의 내공 수련 Level up 이펙트.
				m_pLevelUpInGongPackage = pPackage;
			else
			if( pPackage->strEffectName == _T("살") )		// 크리티컬 타격 '살'
				m_pHitSalPackage = pPackage;
			else
			if( pPackage->strEffectName == _T("경험치") )	// 경험치 획득 이펙트.
			{
				pPackage->bRepeat = true;
				m_pExpAcquirePackage = pPackage;
			}
			else
			if( pPackage->strEffectName.GetLength() == 1 )	// 데미지 숫자 1, 2, ...
			{
				CString strName;
				for(int i=0; i<10; i++)
				{
					strName.Format("%d",i);
					if( pPackage->strEffectName == strName )
					{
						m_pHitNumberPackage[i] = pPackage;
						break;
					}
				}// for
			}

		}// if( nAniType == 264 )

		// 2. Effect Editor에서 반복 정보가 없어서 하드 코딩으로 반복 정보를 넣어두자.
		// 캐릭터 수영할때
		if( pPackage->nAniType == 263 )
			pPackage->bRepeat = true;
		else
		// NPC 정지, 걷기 동작 이펙트 반복.
		if( pPackage->nCharID == 541 ||		//  사갈  
			pPackage->nCharID == 546 ||		//  요마 
			pPackage->nCharID == 593    )	//  광견
		{
			if( pPackage->nAniType == 1 ||	//  정지 
				pPackage->nAniType == 2   )	//  걷기
				pPackage->bRepeat = true;
		}
		else
		// 유탄 이동할때 이펙트.
		if( pPackage->nCharID == 590 && pPackage->nAniType == 1 )
			pPackage->bRepeat = true;

		//
		// 하드 코딩 끝.
		//
*/

		// get current file position
		fgetpos(Effect_ch, &Effect_chPos);
	}// while


	// Effect_res.dat file size check
	start = ftell( Effect_res );
    fseek( Effect_res, 0, SEEK_END );
	end = ftell( Effect_res );

	int nEffectresSize = end - start;
	fseek( Effect_res, 0, SEEK_SET );

	// load Effect_res.dat
	fpos_t Effect_resPos;
	fgetpos( Effect_res, &Effect_resPos );	// file start
	while( Effect_resPos < nEffectresSize )
	{
		int nTexLen, nResID;
		MyString strTexPath;
		TCHAR szTexPath[255];
		memset( szTexPath, 0, sizeof(szTexPath) );

		fread( &nTexLen, 4, 1, Effect_res );
		fread( szTexPath, sizeof(TCHAR), nTexLen, Effect_res );
		fread( &nResID, 4, 1, Effect_res );

		strTexPath.assign( szTexPath );

		// 중복 검사 
		bool bSameRes = false;
		EPACKAGERESMAP::iterator eprit;
		for(eprit=m_EffectPackageResMap.begin(); eprit!=m_EffectPackageResMap.end(); eprit++)
		{
			int nRID = eprit->second;
			if( nRID == nResID )
			{
				bSameRes = true;
				break;
			}
		}// for(m_EffectPackageResMap)

		if( !bSameRes )
			m_EffectPackageResMap.insert( EPACKAGERESMAP::value_type( strTexPath, nResID ) );

		// get current file position
		fgetpos( Effect_res, &Effect_resPos );
	}// while


	// load Effect_msh.idx file size check
	start = ftell( Effect_msh2 );
    fseek( Effect_msh2, 0, SEEK_END );
	end = ftell( Effect_msh2 );

	int nEffectmsh2Size = end - start;
	fseek( Effect_msh2, 0, SEEK_SET );

	// load Effect_msh.idx
	fpos_t Effect_msh2Pos;
	fgetpos( Effect_msh2, &Effect_msh2Pos );	// file start
	while( Effect_msh2Pos < nEffectmsh2Size )
	{
		_EFFECTPACKAGEMESH* pEPMesh = new _EFFECTPACKAGEMESH;

		int nMeshPathLen;
		TCHAR szMeshPath[255];
		memset( szMeshPath, 0, sizeof(szMeshPath) );

		fread( &nMeshPathLen,		4, 1, Effect_msh2 );
		fread( szMeshPath, sizeof(TCHAR), nMeshPathLen, Effect_msh2 );
		fread( &pEPMesh->dwOffset,	4, 1, Effect_msh2 );
		fread( &pEPMesh->nLen,		4, 1, Effect_msh2 );

		pEPMesh->strMeshPath.assign( szMeshPath );

		// 중복 검사
		bool bSameMesh = false;
		EPACKAGEMESHLIST::iterator epmit;
		for(epmit=m_EffectPackageMeshList.begin(); epmit!=m_EffectPackageMeshList.end(); epmit++)
		{
			_EFFECTPACKAGEMESH* pMesh = *epmit;
			if( pMesh->strMeshPath == pEPMesh->strMeshPath )
			{
				bSameMesh = true;
				break;
			}
		}// for( m_EffectPackageMeshList )

		if( bSameMesh )
			delete pEPMesh;
		else
			m_EffectPackageMeshList.push_back( pEPMesh );

		// get current file position
		fgetpos( Effect_msh2, &Effect_msh2Pos );
	}// while

	fclose( Effect_ch );
	fclose( Effect_eff1 );
	fclose( Effect_eff2 );
	fclose( Effect_res );
	fclose( Effect_msh2 );
}

XIAHGE_API void CEffect::LoadEffectMeshFromPackage()
{
	MyString strBasePath	= NEW_EFFECT_PATH;
	MyString strEffectmsh1	= strBasePath + _T("Effect_msh.dat");

	FILE *Effect_msh1 = _tfopen( strEffectmsh1.data(), _T("rb") );
	if( !Effect_msh1 )
	{
		LPCTSTR str=_T("Effect_msh.dat file can't open \n");
		::OutputDebugString(str);
		return;
	}

	int nCount = 0;
	TCHAR cTempFile[15];
	MyString strTempFile;

	EPACKAGEMESHLIST::iterator epmit;
	for(epmit=m_EffectPackageMeshList.begin(); epmit!=m_EffectPackageMeshList.end(); epmit++)
	{
		_EFFECTPACKAGEMESH* pEPMesh = *epmit;

		// make temp file name
		memset( cTempFile, 0, sizeof(cTempFile) );
		_stprintf( cTempFile, _T("ttmsh%d"), nCount++ );
		strTempFile = strBasePath;
		strTempFile.append( cTempFile );

		fseek( Effect_msh1, pEPMesh->dwOffset, SEEK_SET );

		// read mesh data
		BYTE* pMBuffer = new BYTE [ pEPMesh->nLen ];
		if(pMBuffer == NULL)
		{
			DBG_LogFile( _T("CEffect::LoadEffectMeshFromPackage fail"));
		}
		fread( pMBuffer, sizeof(BYTE), pEPMesh->nLen, Effect_msh1 );

		// make temp mesh file
		FILE *fileMesh = _tfopen( strTempFile.data(), _T("wb") );
		fwrite( pMBuffer, sizeof(BYTE), pEPMesh->nLen, fileMesh );
		
		delete []pMBuffer;
		pMBuffer = NULL;

		fclose( fileMesh );

		// register this mesh
		m_pEffectResPool->RegisterMeshFromFile(pEPMesh->strMeshPath, strTempFile );
	}// for( m_EffectPackageMeshList )

	fclose( Effect_msh1 );

	// delete temp mesh file
	memset( cTempFile, 0, sizeof(cTempFile) );
	for(int i=0; i<nCount; i++)
	{
		_stprintf( cTempFile, _T("ttmsh%d"), i );
		strTempFile = strBasePath;
		strTempFile.append( cTempFile );

		_tunlink( strTempFile.data() );
	}

	// clear mesh list
	EPACKAGEMESHLIST::iterator epmlit;
	for(epmlit=m_EffectPackageMeshList.begin(); epmlit!=m_EffectPackageMeshList.end(); epmlit++)
	{
		_EFFECTPACKAGEMESH* pPMesh = *epmlit;
		delete pPMesh;
		pPMesh = NULL;

	}// for(m_EffectPackageMeshList)
	m_EffectPackageMeshList.clear();

}

XIAHGE_API void CEffect::LoadEffectTextureFromPackage(/*CResIndex* pResIndex*/)
{

/*
	DWORD dwWidth, dwHeight;
	EPACKAGERESMAP::iterator eprit;
	for(eprit=m_EffectPackageResMap.begin(); eprit!=m_EffectPackageResMap.end(); eprit++)
	{
        MyString strTexPath = eprit->first;
		int nResID = eprit->second;

		CDDSurface* pSurface = pResIndex->GetResource( nResID, dwWidth, dwHeight);
		if( pSurface )
			m_pEffectResPool->RegisterTextureFromFile( strTexPath, pSurface->GetDDSurface() );
		else
			m_pEffectResPool->RegisterTextureFromFile( strTexPath, NULL );
	}// for( m_EffectPackageResMap )
*/

}

void CEffect::LoadTileEffectFromPackage()
{
	// file name
	MyString strBasePath	= NEW_EFFECT_PATH;
	MyString strTEffti		= strBasePath + _T("teff_ti.dat");
	MyString strTEffeff1	= strBasePath + _T("teff_eff.dat");
	MyString strTEffeff2	= strBasePath + _T("teff_eff.idx");
	MyString strTEffres		= strBasePath + _T("teff_res.dat");
	MyString strTEffmsh2	= strBasePath + _T("teff_msh.idx");

	// open file and error check
	FILE *TEff_ti = _tfopen( strTEffti.data(), _T("rb") );
	if( !TEff_ti )
	{
		LPCTSTR str=_T("teff_ti.dat file can't open \n");
		::OutputDebugString(str);
		return;
	}

	FILE *TEff_eff1 = _tfopen( strTEffeff1.data(), _T("rb") );
	if( !TEff_eff1 )
	{
		LPCTSTR str=_T("teff_eff.dat file can't open \n");
		::OutputDebugString(str);

		fclose(TEff_ti);
		return;
	}

	FILE *TEff_eff2 = _tfopen( strTEffeff2.data(), _T("rb") );
	if( !TEff_eff2 )
	{
		LPCTSTR str=_T("teff_eff.idx file can't open \n");
		::OutputDebugString(str);

		fclose(TEff_ti);
		fclose(TEff_eff1);
		return;
	}

	FILE *TEff_res = _tfopen( strTEffres.data(), _T("rb") );
	if( !TEff_res )
	{
		LPCTSTR str=_T("teff_res.dat file can't open \n");
		::OutputDebugString(str);

		fclose(TEff_ti);
		fclose(TEff_eff1);
		fclose(TEff_eff2);
		return;
	}

	FILE *TEff_msh2 = _tfopen( strTEffmsh2.data(), _T("rb") );
	if( !TEff_msh2 )
	{
		LPCTSTR str=_T("teff_msh.idx file can't open \n");
		::OutputDebugString(str);

		fclose(TEff_ti);
		fclose(TEff_eff1);
		fclose(TEff_eff2);
		fclose(TEff_res);
		return;
	}

	// Ok go!

	// TEff_ti.dat file size check
	long start, end;
	start = ftell( TEff_ti );
	fseek( TEff_ti, 0, SEEK_END );
	end = ftell( TEff_ti );

	int nTEfftiSize = end - start;
	fseek( TEff_ti, 0, SEEK_SET );

	// Load Effect package data and make package list
	fpos_t TEff_tiPos;
	fgetpos( TEff_ti, &TEff_tiPos );	// file start
	while( TEff_tiPos < nTEfftiSize )
	{
		m_pCurEffectPackage = NULL;
		_EFFECTPACKAGE* pPackage = new _EFFECTPACKAGE;

		// 1. load TEff_ti.dat
		pPackage->nCharID = TYPE_TILE_EFFECT;// if this is tile effect, nCharID is TYPE_TILE_EFFECT
		pPackage->nBoneIndex = -1;

		fread( &pPackage->nAniType,		4, 1, TEff_ti );
		fread( &pPackage->nEffectID,	4, 1, TEff_ti );
		fread( &pPackage->nPosX,		4, 1, TEff_ti );
		fread( &pPackage->nPosY,		4, 1, TEff_ti );
		fread( &pPackage->nPosZ,		4, 1, TEff_ti );
		fread( &pPackage->nStartTime,	4, 1, TEff_ti );

		// 2. load TEff_eff.idx
		DWORD dwEffOffset;
		int nEffLen;
		fread( &dwEffOffset,	4, 1, TEff_ti );
		fread( &nEffLen,		4, 1, TEff_ti );

		// 3. load TEff_eff.dat and save tmep eff so load effect data from temp eff
		BYTE* pBuffer = new BYTE[ nEffLen ];
		if(pBuffer == NULL)
		{
			DBG_LogFile( _T("CEffect::LoadTileEffectFromPackage fail"));
		}

		fseek( TEff_ti, dwEffOffset, SEEK_SET );
		fread( pBuffer, sizeof(BYTE), nEffLen, TEff_eff1 );

		MyString strFileTempEff = strBasePath + _T("tmep01.eff");
		FILE *fileTempEff = _tfopen( strFileTempEff.data(), _T("wb") );
		fwrite( pBuffer, sizeof(BYTE), nEffLen, fileTempEff );
		delete []pBuffer;
		pBuffer = NULL;

		fclose(fileTempEff);

		m_pCurEffectPackage = pPackage;

		// load effect
		FILE *fp;
		fp = _tfopen( strFileTempEff.data(), _T("rb") );
		bool bLoaded = LoadEffect(fp, false);				// don't load texture and mesh
		fclose(fp);
		_tunlink( strFileTempEff.data() );

		if( bLoaded )
		{
			pPackage->strEffectName = m_pCurEffect->m_EffectName;
			m_TileEffectPackageList.push_back( pPackage );
		}
		else
			delete pPackage;

		// get current file position
		fgetpos( TEff_ti, &TEff_tiPos );
	}//

	// 4. load TEff_res.dat
	// TEff_res.dat file size check
	start = ftell( TEff_res );
	fseek( TEff_res, 0, SEEK_END );
	end = ftell( TEff_res );

	int nTEffresSize = end - start;
	fseek( TEff_res, 0, SEEK_SET );

	fpos_t TEff_resPos;
	fgetpos( TEff_res, &TEff_resPos );	// file start
	while( TEff_resPos < nTEffresSize )
	{
		int nTexLen, nResID;
		MyString strTexPath;
		TCHAR szTexPath[255];
		memset( szTexPath, 0, sizeof(szTexPath) );

		fread( &nTexLen, 4, 1, TEff_res );
		fread( szTexPath, sizeof(TCHAR), nTexLen, TEff_res );
		fread( &nResID, 4, 1, TEff_res );

		strTexPath.assign( szTexPath );

		// 중복 검사 
		bool bSameRes = false;
		EPACKAGERESMAP::iterator eprit;
		for(eprit=m_EffectPackageResMap.begin(); eprit!=m_EffectPackageResMap.end(); eprit++)
		{
			int nRID = eprit->second;
			if( nRID == nResID )
			{
				bSameRes = true;
				break;
			}
		}// for(m_EffectPackageResMap)

		if( !bSameRes )
			m_EffectPackageResMap.insert( EPACKAGERESMAP::value_type( strTexPath, nResID ) );

		// get current file position
		fgetpos( TEff_res, &TEff_resPos );
	}// while

	// 5. load TEff_msh.idx
	// TEff_res.dat file size check
	start = ftell( TEff_msh2 );
	fseek( TEff_msh2, 0, SEEK_END );
	end = ftell( TEff_msh2 );

	int nTEffmsh2Size = end - start;
	fseek( TEff_msh2, 0, SEEK_SET );

	fpos_t TEff_msh2Pos;
	fgetpos( TEff_msh2, &TEff_msh2Pos );	// file start
	while( TEff_msh2Pos < nTEffmsh2Size )
	{
		_EFFECTPACKAGEMESH* pEPMesh = new _EFFECTPACKAGEMESH;
		if(pEPMesh == NULL)
		{
			DBG_LogFile( _T("CEffect::LoadTileEffectFromPackage2 fail"));
		}

		int nMeshPathLen;
		TCHAR szMeshPath[255];
		memset( szMeshPath, 0, sizeof(szMeshPath) );

		fread( &nMeshPathLen,		4, 1, TEff_msh2 );
		fread( szMeshPath, sizeof(TCHAR), nMeshPathLen, TEff_msh2 );
		fread( &pEPMesh->dwOffset,	4, 1, TEff_msh2 );
		fread( &pEPMesh->nLen,		4, 1, TEff_msh2 );

		pEPMesh->strMeshPath.assign( szMeshPath );

		// 중복 검사
		bool bSameMesh = false;
		EPACKAGEMESHLIST::iterator epmit;
		for(epmit=m_TileEffectPackageMeshList.begin(); epmit!=m_TileEffectPackageMeshList.end(); epmit++)
		{
			_EFFECTPACKAGEMESH* pMesh = *epmit;
			if( pMesh->strMeshPath == pEPMesh->strMeshPath )
			{
				bSameMesh = true;
				break;
			}
		}// for( m_TileEffectPackageMeshList )

		if( bSameMesh )
		{
			delete pEPMesh;
			pEPMesh = NULL;
		}
		else
			m_TileEffectPackageMeshList.push_back( pEPMesh );

		// get current file position
		fgetpos( TEff_msh2, &TEff_msh2Pos );
	}// while

	fclose( TEff_ti );
	fclose( TEff_eff1 );
	fclose( TEff_eff2 );
	fclose( TEff_res );
	fclose( TEff_msh2 );
}

void CEffect::LoadTileEffectMeshFromPackage()
{
	MyString strBasePath = NEW_EFFECT_PATH;
	MyString strTEffmsh1 = strBasePath + _T("teff_msh.dat");

	FILE *TEff_msh1 = _tfopen( strTEffmsh1.data(), _T("wb") );
	if( !TEff_msh1 )
	{
		LPCTSTR str=_T("teff_msh.dat file can't open \n");
		::OutputDebugString(str);
		return;
	}

	int nCount = 0;
	MyString strTempFile = strBasePath;
	TCHAR cTempFile[15];
	memset( cTempFile, 0, sizeof(cTempFile) );

	EPACKAGEMESHLIST::iterator epmit;
	for(epmit=m_TileEffectPackageMeshList.begin(); epmit!=m_TileEffectPackageMeshList.end(); epmit++)
	{
		_EFFECTPACKAGEMESH* pEPMesh = *epmit;

		// make temp file name
		_stprintf( cTempFile, _T("ttmsh%d"), nCount++ );
		strTempFile.append( cTempFile );

		fseek( TEff_msh1, pEPMesh->dwOffset, SEEK_SET );

		// read mesh data
		BYTE* pMBuffer = new BYTE [ pEPMesh->nLen ];
		if(pMBuffer == NULL)
		{
			DBG_LogFile( _T("CEffect::LoadTileEffectMeshFromPackage fail"));
		}
		fread( pMBuffer, sizeof(BYTE), pEPMesh->nLen, TEff_msh1 );

		// make temp mesh file
		FILE *fileMesh = _tfopen( strTempFile.data(), _T("wb") );
		fwrite( pMBuffer, sizeof(BYTE), pEPMesh->nLen, fileMesh );
		delete []pMBuffer;
		pMBuffer = NULL;
		fclose( fileMesh );

		// register this mesh
		m_pEffectResPool->RegisterMeshFromFile(pEPMesh->strMeshPath, strTempFile );
	}// for(m_TileEffectPackageMeshList)

	fclose( TEff_msh1 );

	// delete temp mesh file
	strTempFile = strBasePath;
	memset( cTempFile, 0, sizeof(cTempFile) );
	for(int i=0; i<nCount; i++)
	{
		_stprintf( cTempFile, _T("ttmsh%d"), i );
		strTempFile.append( cTempFile );

		_tunlink( strTempFile.data() );
	}

	// clear mesh list
	EPACKAGEMESHLIST::iterator epmlit;
	for(epmlit=m_TileEffectPackageMeshList.begin(); epmlit!=m_TileEffectPackageMeshList.end(); epmlit++)
	{
		_EFFECTPACKAGEMESH* pPMesh = *epmlit;
		if(pPMesh)
		{
			delete pPMesh;
			pPMesh = NULL;
		}
	}// for(m_TileEffectPackageMeshList)
	m_TileEffectPackageMeshList.clear();
}

bool CEffect::IsCharUniqIDinPackage(int nCharUniqID)
{
/*
	EFFECTPACKAGEPAIRLIST::iterator epplist;
	for(epplist=m_CurPackagePairList.begin(); epplist!=m_CurPackagePairList.end(); epplist++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *epplist;

		if( pPackagePair->nCharUniqID == nCharUniqID )
			return true;
	}// for(m_CurPackagePairList)
*/

	return false;
}

bool CEffect::IsEffectInRendering(int nEffectManageID)
{
	EFFECTPACKAGEPAIRLIST::iterator epplist;
	for(epplist=m_CurPackagePairList.begin(); epplist!=m_CurPackagePairList.end(); epplist++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *epplist;

		EFFECTPACKAGELIST::iterator eplist;
		for(eplist=pPackagePair->PackageList.begin(); eplist!=pPackagePair->PackageList.end(); eplist++)
		{
			_EFFECTPACKAGE* pPackage = *eplist;

			if( pPackage->nEffectManageID == nEffectManageID )
				return true;
		}// for( pPackagePair->PackageList )

	}// for(m_CurPackagePairList)

	return false;
}

XIAHGE_API bool CEffect::MakeSharedPackagePair(int nCharUniqID, int nCharID, int nAniType)
{
	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return false;

	_EFFECTPACKAGEPAIR* pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = nCharUniqID;
//	pPackagePair->nCharID = nCharID;
//	pPackagePair->nAniType = nAniType;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	pPackagePair->PackageList.clear();
	D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
	pPackagePair->pWorldMatrix = NULL;
	pPackagePair->bNowUsing = true;
	pPackagePair->bIsVisible = true;

	m_CurPackagePairList.push_back( pPackagePair );

	m_pSharedPackagePair   = pPackagePair;
	m_pCurEffectPackagepair = pPackagePair;

	return true;
}

XIAHGE_API void CEffect::OffSharedPackagePair()
{
	m_pSharedPackagePair = NULL;
}

void CEffect::SetCurWorldMatrix(MATRIX WorldMatrix)
{
	m_WorldMatrix = WorldMatrix;
}

void CEffect::SetPackagePairMatrix(int nCharUniqID, MATRIX *CharMatrix)
{
/*
	EFFECTPACKAGEPAIRLIST::iterator epplist;
	for(epplist=m_CurPackagePairList.begin(); epplist!=m_CurPackagePairList.end(); epplist++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *epplist;

		if( pPackagePair->nCharUniqID != nCharUniqID ) continue;

//		pPackagePair->WorldMatrix = *CharMatrix;
	}// for(m_CurPackagePairList)
*/
}

void CEffect::EnqLevelupEffect(BYTE nType, int nCharUniqID, int nCharID, int nAniType, D3DMATRIX* CharMatrix)
{
	// 이 함수도 더이상 안쓴다. 
	_EFFECTPACKAGE* pLevelUpPackage = NULL;
	switch( nType )
	{
	case LEVELUP_GAPJA:		// 갑자 상승.
		pLevelUpPackage = m_pLevelUpGapJaPackage;
		break;
	case LEVELUP_TP:		// 수련치 상승.
		pLevelUpPackage = m_pLevelUpTPPackage;
		break;
	case LEVELUP_OUTGONG:	// 외공 수련.
		pLevelUpPackage = m_pLevelUpOutGongPackage;
		break;
	case LEVELUP_INGONG:	// 내공 수련.
		pLevelUpPackage = m_pLevelUpInGongPackage;
		break;
	}// switch

	if( !pLevelUpPackage ) return;

	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return;

	_EFFECTPACKAGEPAIR* pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = nCharUniqID;
//	pPackagePair->nCharID = nCharID;
//	pPackagePair->nAniType = nAniType;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	pPackagePair->PackageList.clear();
	pPackagePair->WorldMatrix = *CharMatrix;

	// make package
	if( m_PackagePool.size() == 0 ) return;

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	*pNewPackage = *pLevelUpPackage;

	pNewPackage->nCharID = nCharID;
	pPackagePair->PackageList.push_back( pNewPackage );

	m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	_EFFECT* pEffect = GetEffect( pNewPackage->nEffectManageID );
	pNewPackage->pEffect = pEffect;
	if(pEffect)
	{
		_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
		if( !pEffectRender ) return;

		pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);
		pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
		pEffectRender->m_bTraceMoveTurn = false;
		pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
		pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
		pEffectRender->m_nStartTime = pNewPackage->nStartTime;

//		pEffectRender->m_nCharUniqID = nCharUniqID;
//		pEffectRender->m_nAniType = pNewPackage->nAniType;
		pEffectRender->pPackagePair = pPackagePair;
		
		pNewPackage->pEffectRender = pEffectRender;
		
		StartEffectToRender( pEffectRender );
		
		// create effect sound
		// 현재 Level up 이펙트에 연결된 사운드가 없다.
		//m_EffectSound.CreateEffectSoundInstance(pPackagePair->nCharUniqID, pPackagePair->nCharID, pPackagePair->nAniType);
	}//if(pEffect)
}

void CEffect::EnqExpAcquireEffect(VECTOR vStartPos, VECTOR vDestPos)
{
	// 이 함수도 더이상 안쓴다.
	if( !m_pExpAcquirePackage ) return;

	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return;

	_EFFECTPACKAGEPAIR* pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = TYPE_EXPACQUIRE_EFFECT;
//	pPackagePair->nCharID = TYPE_EXPACQUIRE_EFFECT;
//	pPackagePair->nAniType = 0;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
	pPackagePair->PackageList.clear();

	// make package
	if( m_PackagePool.size() == 0 ) return;

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	*pNewPackage = *m_pExpAcquirePackage;

	pNewPackage->nCharID = TYPE_EXPACQUIRE_EFFECT;
	pNewPackage->nAniType = 0;
	pPackagePair->PackageList.push_back( pNewPackage );

	m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	_EFFECT* pEffect = GetEffect( pNewPackage->nEffectManageID );
	pNewPackage->pEffect = pEffect;
	if(pEffect)
	{
		_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
		if( !pEffectRender ) return;

		pEffectRender->m_vCurPos = vStartPos;
		pEffectRender->m_vDestPos = vDestPos;
		pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
		pEffectRender->m_vMoveStartPos = VECTOR(0,0,0);
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);
		pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
		pEffectRender->m_bTraceMoveTurn = false;
		pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();

		// 경험치 획득 이펙트를 기본적으로 반복시킨다.
		pNewPackage->nStartTime = pEffect->m_LifeTime;
		pEffectRender->m_nStartTime = 0; //pNewPackage->nStartTime;

//		pEffectRender->m_nCharUniqID = TYPE_EXPACQUIRE_EFFECT;
//		pEffectRender->m_nAniType = pNewPackage->nAniType;
		pEffectRender->pPackagePair = pPackagePair;

		pNewPackage->pEffectRender = pEffectRender;

		StartEffectToRender( pEffectRender );
	}//if(pEffect)
}

XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)
{
	if (pEffect == NULL) return NULL;

	// 현재의 이펙트를 곧바로 렌더링 리스트에 넣어서 렌더링을 시작할 수 있게 한다.
	_EFFECTPACKAGEPAIR* pPackagePair;
	if( !m_pSharedPackagePair )
	{
		// make pack pair
		if( m_PackagePairPool.size() == 0 ) return NULL;
		
		pPackagePair = m_PackagePairPool.front();
		m_PackagePairPool.pop_front();
		
//		pPackagePair->nCharUniqID = 0;
//		pPackagePair->nCharID = 0;
//		pPackagePair->nAniType = 0;
		pPackagePair->dwTotalTime = 0;
		pPackagePair->dwElapsedTime = 0;
		pPackagePair->PackageList.clear();
		D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
		pPackagePair->pWorldMatrix = NULL;
		pPackagePair->bNowUsing = true;
		pPackagePair->bIsVisible = true;
	}
	else	// In Character Studio2, Effect Play All
	{		// 즉, 하나의 PP에 여러 P를 연결할 때 사용.
		pPackagePair = m_pSharedPackagePair;
	}

	// 원본 데이터가 잘못 되었을 때를 수정한다.
	if( nBoneIndex != -1 )
		nX = nY = nZ = 0;

	// make package
	if( m_PackagePool.size() == 0 )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	pNewPackage->nCharID = 0;
	pNewPackage->nAniType = 0;
	pNewPackage->nBoneIndex = nBoneIndex;
	pNewPackage->nEffectID = pEffect->m_EffectID;
	pNewPackage->nEffectManageID = pEffect->m_EffectManageID;
	pNewPackage->nPosX = nX;
	pNewPackage->nPosY = nY;
	pNewPackage->nPosZ = nZ;
	pNewPackage->nStartTime = nSTime;
	pNewPackage->strEffectName = pEffect->m_EffectName;
	pNewPackage->m_LifeTimeChange = 0;
	pNewPackage->pEffectRender = NULL;

	pPackagePair->PackageList.push_back( pNewPackage );

	if( !m_pSharedPackagePair)
		m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	pNewPackage->pEffect = pEffect;

	// load resource => texture and mesh
	RealizeEffectResouce( pEffect );

	//
	_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
	if( !pEffectRender )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
	pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
	pEffectRender->m_nStartTime = pNewPackage->nStartTime;
	pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
	pEffectRender->m_bTraceMoveTurn = false;
	pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
	if( pEffectRender->pEffect->m_bTargetMove )
		pEffectRender->m_vTargetMovePos = m_vTargetMovePos;
	else
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);

//	pEffectRender->m_nCharUniqID = 0;
//	pEffectRender->m_nAniType = pNewPackage->nAniType;
	pEffectRender->pPackagePair = pPackagePair;
	pEffectRender->pPackage = pNewPackage;

	pNewPackage->pEffectRender = pEffectRender;

	StartEffectToRender( pEffectRender );

	return pNewPackage;
}

XIAHGE_API _EFFECTPACKAGE* CEffect::EnqHitEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)
{
	if (nType < 0 || nType >= eHitEnumMax) return NULL;
	_EFFECT* pEffect = m_pHitEffect[nType];
	if( pEffect == NULL || (DWORD_PTR)pEffect < 0x10000 || IsBadReadPtr(pEffect, sizeof(_EFFECT)) )
	{
		DBG_LogFile( _T("CEffect::EnqHitEffectImmediately fail"));
		return NULL;
	}

	// 현재의 이펙트를 곧바로 렌더링 리스트에 넣어서 렌더링을 시작할 수 있게 한다.
	_EFFECTPACKAGEPAIR* pPackagePair;

	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return NULL;

	pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = 0;
//	pPackagePair->nCharID = 0;
//	pPackagePair->nAniType = 0;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	pPackagePair->PackageList.clear();
	D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
	pPackagePair->pWorldMatrix = NULL;
	pPackagePair->bNowUsing = true;
	pPackagePair->bIsVisible = true;

	// 원본 데이터가 잘못 되었을 때를 수정한다.
	if( nBoneIndex != -1 )
		nX = nY = nZ = 0;

	// make package
	if( m_PackagePool.size() == 0 )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	pNewPackage->nCharID = 0;
	pNewPackage->nAniType = 0;
	pNewPackage->nBoneIndex = nBoneIndex;
	pNewPackage->nEffectID = pEffect->m_EffectID;
	pNewPackage->nEffectManageID = pEffect->m_EffectManageID;
	pNewPackage->nPosX = nX;
	pNewPackage->nPosY = nY;
	pNewPackage->nPosZ = nZ;
	pNewPackage->nStartTime = nSTime;
	pNewPackage->strEffectName = pEffect->m_EffectName;
	pNewPackage->m_LifeTimeChange = 0;
	pNewPackage->pEffectRender = NULL;

	pPackagePair->PackageList.push_back( pNewPackage );

	if( !m_pSharedPackagePair)
		m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	pNewPackage->pEffect = pEffect;

	// load resource => texture and mesh
	RealizeEffectResouce( pEffect );

	//
	_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
	if( !pEffectRender )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
	pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
	pEffectRender->m_nStartTime = pNewPackage->nStartTime;
	pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
	pEffectRender->m_bTraceMoveTurn = false;
	pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
	if( pEffectRender->pEffect->m_bTargetMove )
		pEffectRender->m_vTargetMovePos = m_vTargetMovePos;
	else
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);

//	pEffectRender->m_nCharUniqID = 0;
//	pEffectRender->m_nAniType = pNewPackage->nAniType;
	pEffectRender->pPackagePair = pPackagePair;
	pEffectRender->pPackage = pNewPackage;

	pNewPackage->pEffectRender = pEffectRender;

	StartEffectToRender( pEffectRender );

	return pNewPackage;
}

XIAHGE_API _EFFECTPACKAGE* CEffect::EnqLevelUpEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)
{
	_EFFECT* pEffect = m_pLevelUpEffect[nType];
	if( pEffect == NULL ) return NULL;

	// 현재의 이펙트를 곧바로 렌더링 리스트에 넣어서 렌더링을 시작할 수 있게 한다.
	_EFFECTPACKAGEPAIR* pPackagePair;

	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return NULL;

	pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = 0;
//	pPackagePair->nCharID = 0;
//	pPackagePair->nAniType = 0;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	pPackagePair->PackageList.clear();
	D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
	pPackagePair->pWorldMatrix = NULL;
	pPackagePair->bNowUsing = true;
	pPackagePair->bIsVisible = true;

	// 원본 데이터가 잘못 되었을 때를 수정한다.
	if( nBoneIndex != -1 )
		nX = nY = nZ = 0;

	// make package
	if( m_PackagePool.size() == 0 )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	pNewPackage->nCharID = 0;
	pNewPackage->nAniType = 0;
	pNewPackage->nBoneIndex = nBoneIndex;
	pNewPackage->nEffectID = pEffect->m_EffectID;
	pNewPackage->nEffectManageID = pEffect->m_EffectManageID;
	pNewPackage->nPosX = nX;
	pNewPackage->nPosY = nY;
	pNewPackage->nPosZ = nZ;
	pNewPackage->nStartTime = nSTime;
	pNewPackage->strEffectName = pEffect->m_EffectName;
	pNewPackage->m_LifeTimeChange = 0;
	pNewPackage->pEffectRender = NULL;

	pPackagePair->PackageList.push_back( pNewPackage );

	if( !m_pSharedPackagePair)
		m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	pNewPackage->pEffect = pEffect;

	// load resource => texture and mesh
	RealizeEffectResouce( pEffect );

	//
	_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
	if( !pEffectRender )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
	pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
	pEffectRender->m_nStartTime = pNewPackage->nStartTime;
	pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
	pEffectRender->m_bTraceMoveTurn = false;
	pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
	if( pEffectRender->pEffect->m_bTargetMove )
		pEffectRender->m_vTargetMovePos = m_vTargetMovePos;
	else
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);

//	pEffectRender->m_nCharUniqID = 0;
//	pEffectRender->m_nAniType = pNewPackage->nAniType;
	pEffectRender->pPackagePair = pPackagePair;
	pEffectRender->pPackage = pNewPackage;

	pNewPackage->pEffectRender = pEffectRender;

	StartEffectToRender( pEffectRender );

	return pNewPackage;
}

XIAHGE_API _EFFECTPACKAGE* CEffect::EnqAppearEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)
{
	if(nType < 0 || nType >= eAppearEnumMax) return NULL;
	_EFFECT* pEffect = m_pAppearEffect[nType];
	if( pEffect == NULL || (DWORD_PTR)pEffect < 0x10000 || IsBadReadPtr(pEffect, sizeof(_EFFECT)) ) return NULL;

	// 현재의 이펙트를 곧바로 렌더링 리스트에 넣어서 렌더링을 시작할 수 있게 한다.
	_EFFECTPACKAGEPAIR* pPackagePair;

	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return NULL;

	pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = 0;
//	pPackagePair->nCharID = 0;
//	pPackagePair->nAniType = 0;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	pPackagePair->PackageList.clear();
	D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
	pPackagePair->pWorldMatrix = NULL;
	pPackagePair->bNowUsing = true;
	pPackagePair->bIsVisible = true;

	// 원본 데이터가 잘못 되었을 때를 수정한다.
	if( nBoneIndex != -1 )
		nX = nY = nZ = 0;

	// make package
	if( m_PackagePool.size() == 0 )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	pNewPackage->nCharID = 0;
	pNewPackage->nAniType = 0;
	pNewPackage->nBoneIndex = nBoneIndex;
	pNewPackage->nEffectID = pEffect->m_EffectID;
	pNewPackage->nEffectManageID = pEffect->m_EffectManageID;
	pNewPackage->nPosX = nX;
	pNewPackage->nPosY = nY;
	pNewPackage->nPosZ = nZ;
	pNewPackage->nStartTime = nSTime;
	pNewPackage->strEffectName = pEffect->m_EffectName;
	pNewPackage->m_LifeTimeChange = 0;
	pNewPackage->pEffectRender = NULL;

	pPackagePair->PackageList.push_back( pNewPackage );

	if( !m_pSharedPackagePair)
		m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	pNewPackage->pEffect = pEffect;

	// load resource => texture and mesh
	RealizeEffectResouce( pEffect );

	//
	_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
	if( !pEffectRender )
	{
        EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
	pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
	pEffectRender->m_nStartTime = pNewPackage->nStartTime;
	pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
	pEffectRender->m_bTraceMoveTurn = false;
	pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
	if( pEffectRender->pEffect->m_bTargetMove )
		pEffectRender->m_vTargetMovePos = m_vTargetMovePos;
	else
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);

//	pEffectRender->m_nCharUniqID = 0;
//	pEffectRender->m_nAniType = pNewPackage->nAniType;
	pEffectRender->pPackagePair = pPackagePair;
	pEffectRender->pPackage = pNewPackage;

	pNewPackage->pEffectRender = pEffectRender;

	StartEffectToRender( pEffectRender );

	return pNewPackage;
}

XIAHGE_API _EFFECTPACKAGE* CEffect::EnqExpAcquireEffectImmediately(VECTOR vTargetPos, int nSTime, int nX, int nY, int nZ, int nBoneIndex)
{
	_EFFECT* pEffect = m_pExpAcquireEffect;
	if( pEffect == NULL ) return NULL;

	// 현재의 이펙트를 곧바로 렌더링 리스트에 넣어서 렌더링을 시작할 수 있게 한다.
	_EFFECTPACKAGEPAIR* pPackagePair;

	// make pack pair
	if( m_PackagePairPool.size() == 0 ) return NULL;

	pPackagePair = m_PackagePairPool.front();
	m_PackagePairPool.pop_front();

//	pPackagePair->nCharUniqID = 0;
//	pPackagePair->nCharID = 0;
//	pPackagePair->nAniType = 0;
	pPackagePair->dwTotalTime = 0;
	pPackagePair->dwElapsedTime = 0;
	pPackagePair->PackageList.clear();
	D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
	pPackagePair->pWorldMatrix = NULL;
	pPackagePair->bNowUsing = true;
	pPackagePair->bIsVisible = true;

	// 원본 데이터가 잘못 되었을 때를 수정한다.
	if( nBoneIndex != -1 )
		nX = nY = nZ = 0;

	// make package
	if( m_PackagePool.size() == 0 )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	pNewPackage->nCharID = 0;
	pNewPackage->nAniType = 0;
	pNewPackage->nBoneIndex = nBoneIndex;
	pNewPackage->nEffectID = pEffect->m_EffectID;
	pNewPackage->nEffectManageID = pEffect->m_EffectManageID;
	pNewPackage->nPosX = nX;
	pNewPackage->nPosY = nY;
	pNewPackage->nPosZ = nZ;
	pNewPackage->nStartTime = nSTime;
	pNewPackage->strEffectName = pEffect->m_EffectName;
	pNewPackage->m_LifeTimeChange = 0;
	pNewPackage->pEffectRender = NULL;

	pPackagePair->PackageList.push_back( pNewPackage );

	if( !m_pSharedPackagePair)
		m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	pNewPackage->pEffect = pEffect;

	// load resource => texture and mesh
	RealizeEffectResouce( pEffect );

	//
	_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
	if( !pEffectRender )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
	pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
	pEffectRender->m_nStartTime = pNewPackage->nStartTime;
	pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
	pEffectRender->m_bTraceMoveTurn = false;
	pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
	if( pEffectRender->pEffect->m_bTargetMove )
		pEffectRender->m_vTargetMovePos = m_vTargetMovePos;
	else
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);

//	pEffectRender->m_nCharUniqID = 0;
//	pEffectRender->m_nAniType = pNewPackage->nAniType;
	pEffectRender->pPackagePair = pPackagePair;
	pEffectRender->pPackage = pNewPackage;

	// element의 최종 목표 위치.
	pEffectRender->m_vTargetMovePos = vTargetPos;

	pNewPackage->pEffectRender = pEffectRender;

	StartEffectToRender( pEffectRender );

	return pNewPackage;
}

XIAHGE_API _EFFECTPACKAGE* CEffect::EnqOutGongPersistEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)
{
	_EFFECT* pEffect = m_pOutGongPersistEffect[nType];
	nX = m_nOutGongPersistEffectXArray[nType];
	nY = m_nOutGongPersistEffectYArray[nType];
	nZ = m_nOutGongPersistEffectZArray[nType];

	if( pEffect == NULL ) return NULL;

	// 현재의 이펙트를 곧바로 렌더링 리스트에 넣어서 렌더링을 시작할 수 있게 한다.
	_EFFECTPACKAGEPAIR* pPackagePair;
	if( !m_pSharedPackagePair )
	{
		// make pack pair
		if( m_PackagePairPool.size() == 0 ) return NULL;
		
		pPackagePair = m_PackagePairPool.front();
		m_PackagePairPool.pop_front();
		
//		pPackagePair->nCharUniqID = 0;
//		pPackagePair->nCharID = 0;
//		pPackagePair->nAniType = 0;
		pPackagePair->dwTotalTime = 0;
		pPackagePair->dwElapsedTime = 0;
		pPackagePair->PackageList.clear();
		D3DXMatrixIdentity( &pPackagePair->WorldMatrix );
		pPackagePair->pWorldMatrix = NULL;
		pPackagePair->bNowUsing = true;
		pPackagePair->bIsVisible = true;
	}
	else	// In Character Studio2, Effect Play All
	{		// 즉, 하나의 PP에 여러 P를 연결할 때 사용.
		pPackagePair = m_pSharedPackagePair;
	}

	// 원본 데이터가 잘못 되었을 때를 수정한다.
	if( nBoneIndex != -1 )
		nX = nY = nZ = 0;

	// make package
	if( m_PackagePool.size() == 0 )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	_EFFECTPACKAGE* pNewPackage = m_PackagePool.front();
	m_PackagePool.pop_front();

	pNewPackage->nCharID = 0;
	pNewPackage->nAniType = 0;
	pNewPackage->nBoneIndex = nBoneIndex;
	pNewPackage->nEffectID = pEffect->m_EffectID;
	pNewPackage->nEffectManageID = pEffect->m_EffectManageID;
	pNewPackage->nPosX = nX;
	pNewPackage->nPosY = nY;
	pNewPackage->nPosZ = nZ;
	pNewPackage->nStartTime = nSTime;
	pNewPackage->strEffectName = pEffect->m_EffectName;
	pNewPackage->m_LifeTimeChange = 0;
	pNewPackage->pEffectRender = NULL;

	pPackagePair->PackageList.push_back( pNewPackage );

	if( !m_pSharedPackagePair)
		m_CurPackagePairList.push_back( pPackagePair );

	// set effect render
	pNewPackage->pEffect = pEffect;

	// load resource => texture and mesh
	RealizeEffectResouce( pEffect );

	//
	_EFFECTRENDER* pEffectRender = EnqEffectToRender( pEffect );
	if( !pEffectRender )
	{
		EffectPackagePairMemoryReturn( pPackagePair );
		return NULL;
	}

	pEffectRender->m_nAttachedBoneIndex = pNewPackage->nBoneIndex;
	pEffectRender->m_vMoveStartPos = VECTOR(pNewPackage->nPosX, pNewPackage->nPosY, pNewPackage->nPosZ);
	pEffectRender->m_nStartTime = pNewPackage->nStartTime;
	pEffectRender->m_vTargetMoveDelta = VECTOR(0,0,0);
	pEffectRender->m_bTraceMoveTurn = false;
	pEffectRender->TraceListIT = pEffectRender->pEffect->m_TraceList.begin();
	if( pEffectRender->pEffect->m_bTargetMove )
		pEffectRender->m_vTargetMovePos = m_vTargetMovePos;
	else
		pEffectRender->m_vTargetMovePos = VECTOR(0,0,0);

//	pEffectRender->m_nCharUniqID = 0;
//	pEffectRender->m_nAniType = pNewPackage->nAniType;
	pEffectRender->pPackagePair = pPackagePair;
	pEffectRender->pPackage = pNewPackage;

	pNewPackage->pEffectRender = pEffectRender;

	StartEffectToRender( pEffectRender );

	return pNewPackage;
}

XIAHGE_API void CEffect::SetHitEffect(int nType,_EFFECT* pEffect)
{
	m_pHitEffect[nType] = pEffect;
}

XIAHGE_API void CEffect::SetLevelUpEffect(int nType,_EFFECT* pEffect)
{
	m_pLevelUpEffect[nType] = pEffect;
}

XIAHGE_API void CEffect::SetAppearEffect(int nType,_EFFECT* pEffect)
{
	m_pAppearEffect[nType] = pEffect;
}

XIAHGE_API void CEffect::SetExpAcquireEffect(_EFFECT* pEffect)
{
	m_pExpAcquireEffect = pEffect;
	m_pExpAcquireEffect->m_nEffectType = eExpAcquireEffect;
}

XIAHGE_API void CEffect::SetOutGongPersistEffect(int nType, _EFFECT* pEffect, int nX, int nY, int nZ)
{
	m_pOutGongPersistEffect[ nType ] = pEffect;
	m_nOutGongPersistEffectXArray[ nType ] = nX;
	m_nOutGongPersistEffectYArray[ nType ] = nY;
	m_nOutGongPersistEffectZArray[ nType ] = nZ;
}

void CEffect::DeqEffectImmediately(_EFFECT *pEffect)
{	// 에디터에서 사용하는 함수로, 현재의 이펙트를 바로 멈추게 하는 함수이다.
	EFFECTPACKAGEPAIRLIST::iterator eppit;
	for(eppit=m_CurPackagePairList.begin(); eppit!=m_CurPackagePairList.end(); eppit++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *eppit;

		EFFECTPACKAGELIST::iterator epit;
		for(epit=pPackagePair->PackageList.begin(); epit!=pPackagePair->PackageList.end(); epit++)
		{
			_EFFECTPACKAGE* pPackage = *epit;

			if( pPackage->pEffect == pEffect )
			{
				DeqEffectFromRender( pPackage->pEffectRender );

				// OK, finish
				return;
			}// if

		}// for( pPackagePair->PackageList )

	}// for( m_CurPackagePairList )
}

XIAHGE_API void CEffect::DeqEffectPackagePair(_EFFECTPACKAGEPAIR* pPackagePair)
{
	// 여기서 바로 지우지 말고, 지워질 리스트에 넣어둔다.
	m_DeleteEffectPackagePairList.push_back( pPackagePair );
}

void CEffect::DeleteEffectPackagePairList()
{
	// 지워질 이펙트들은 여기서 몽땅 일괄처리해서 싱크를 맞춰준다.
	EFFECTPACKAGEPAIRLIST::iterator eppit;
	for(eppit=m_DeleteEffectPackagePairList.begin(); eppit!=m_DeleteEffectPackagePairList.end(); eppit++)
	{
		_EFFECTPACKAGEPAIR* pPackagePair = *eppit;

		EFFECTPACKAGELIST Deletelist;
		// make list
		EFFECTPACKAGELIST::iterator epit;
		for(epit=pPackagePair->PackageList.begin(); epit!=pPackagePair->PackageList.end(); epit++)
		{
			_EFFECTPACKAGE* pPackage = *epit;

			Deletelist.push_back( pPackage );
		}// for( pPackagePair->PackageList )

		// delete
		EFFECTPACKAGELIST::iterator epit2;
		for(epit2=Deletelist.begin(); epit2!=Deletelist.end(); epit2++)
		{
			_EFFECTPACKAGE* pPackage = *epit2;

			// DeqEffectFromRender호출하지 말고, 여기서 바로 하자.
			//		DeqEffectFromRender( pPackage->pEffectRender );

			m_PackagePool.push_back( pPackage );
			pPackagePair->PackageList.remove( pPackage );

			if( pPackagePair->PackageList.size() == 0 )
			{
				pPackagePair->bNowUsing = false;
				// delete list
				m_PackagePairPool.push_back( pPackagePair );
				m_CurPackagePairList.remove( pPackagePair );
			}

			_EFFECTRENDER* pER = pPackage->pEffectRender;
			if( pER != NULL && !IsBadReadPtr(pER, sizeof(_EFFECTRENDER)) )
			{
				DeleteEffectRender( pER );
				m_EffectRenderList.remove( pER );
				m_CurEffectRenderList.remove( pER );
			}
			else
			{
				DBG_LogFile( _T("DeleteEffectPackagePairList: SKIPPED invalid pEffectRender!") );
			}
		}// for

	}// for( m_DeleteEffectPackagePairList )

	m_DeleteEffectPackagePairList.clear();
}

void CEffect::EffectPackagePairMemoryReturn(_EFFECTPACKAGEPAIR* pPackagePair)
{
	pPackagePair->bNowUsing = false;
	pPackagePair->bIsVisible = false;
	m_PackagePairPool.push_back( pPackagePair );

	m_CurPackagePairList.remove( pPackagePair );

	if( m_pSharedPackagePair )
		m_pSharedPackagePair = NULL;

	// 이펙트 메모리를 모두 해제해야 하므로 패키지 리스트를 검색한다.
	EFFECTPACKAGELIST::iterator eppit;
	for(eppit=pPackagePair->PackageList.begin(); eppit!=pPackagePair->PackageList.end(); eppit++)
	{
		_EFFECTPACKAGE* pPack = *eppit;

		if( pPack->pEffectRender && !IsBadReadPtr(pPack->pEffectRender, sizeof(_EFFECTRENDER)) )
		{
			DeleteEffectRender( pPack->pEffectRender );

			m_EffectRenderList.remove( pPack->pEffectRender );
			m_CurEffectRenderList.remove( pPack->pEffectRender );
		}

		m_PackagePool.push_back( pPack );
	}

	pPackagePair->PackageList.clear();
}

void CEffect::CopyEffect(_EFFECT* pSrcEffect, _EFFECT* pDestEffect)
{
	// 필요한 데이타를 미리 복사하고, 기존의 이펙트 데이타들의 메모리 해제.
	DWORD dwOriDBID = pDestEffect->m_DBID;
	DWORD dwOriEffectID = pDestEffect->m_EffectID;					// new effect info table id
	DWORD dwOriEffectManagerID = pDestEffect->m_EffectManageID;		//m_EffectList에 들어가는 관리 ID 
	MyString strOriEffectName = pDestEffect->m_EffectName;

	// 이걸 실행해도 m_EffectManageID는 아직 살아있다.
	DeleteEffect( pDestEffect, false );// 포인터는 지우지 않는다. 

	// copy effect data
	*pDestEffect = *pSrcEffect;

	// 데이타 복귀.
	pDestEffect->m_DBID = dwOriDBID;
	pDestEffect->m_EffectID = dwOriEffectID;
	pDestEffect->m_EffectManageID = dwOriEffectManagerID;

	// particle
	pDestEffect->m_ParticleList.clear();
	PARTICLELIST::iterator pit;
	for(pit=pSrcEffect->m_ParticleList.begin(); pit!=pSrcEffect->m_ParticleList.end(); pit++)
	{
		_PARTICLE* pSrcParticle = *pit;
		_PARTICLE* pDestParticle = new _PARTICLE;

		if(pDestParticle == NULL)
		{
			DBG_LogFile( _T("CEffect::CopyEffect fail"));
		}

		// data copy
		*pDestParticle = *pSrcParticle;

		// element
		pDestParticle->m_ElementList.clear();
		ELEMENTLIST::iterator peit;
		for(peit=pSrcParticle->m_ElementList.begin(); peit!=pSrcParticle->m_ElementList.end(); peit++)
		{
			_ELEMENT* pSrcElement = *peit;
			_ELEMENT* pDestElement = new _ELEMENT;
			if(pDestElement == NULL)
			{
				DBG_LogFile( _T("CEffect::CopyEffect2 fail"));
				//continue;
			}
			// data copy
			*pDestElement = *pSrcElement;

			pDestParticle->m_ElementList.push_back( pDestElement );
		}// for(pSrcParticle->m_ElementList)

		pDestEffect->m_ParticleList.push_back( pDestParticle );
	}// for(pSrcEffect->m_ParticleList)


/*			이 부분은 추후에 추가해야됨. 절대 지우지 마시요.

			// Waterfall
			pEffect->m_WaterfallList.clear();
			for(wit=m_pCopyEffect->m_WaterfallList.begin(); wit!=m_pCopyEffect->m_WaterfallList.end(); wit++)
			{
				CWaterfall* pWaterfall = *wit;

				CWaterfall* pNewWaterfall = new CWaterfall;

				int dir, x, y, z, power, number, size, width, sizeB;
				pWaterfall->GetSettingData(dir, x, y, z, power, number, size, width, sizeB);
				pNewWaterfall->SetOnlyData(dir, x, y, z, power, number, size, width, sizeB, pWaterfall->m_TexturePath);

				pEffect->m_WaterfallList.insert( pNewWaterfall );
			}//

			// tbunder
			pEffect->m_ThunderList.clear();
			for(tit=m_pCopyEffect->m_ThunderList.begin(); tit!=m_pCopyEffect->m_ThunderList.end(); tit++)
			{
				CThunder* pThunder = *tit;

				CThunder* pNewThunder = new CThunder;

				int x, y, z, Ratio, BodyDivision, SubBody, SubBodyDivision, DrawGap, Width;
				pThunder->GetSettingData(x, y, z, Ratio, BodyDivision, SubBody, SubBodyDivision, DrawGap, Width);
				pNewThunder->SetOnlyData(x, y, z, Ratio, BodyDivision, SubBody, SubBodyDivision, DrawGap, Width, pThunder->m_TexturePath);

				pEffect->m_ThunderList.insert( pNewThunder );
			}//

			// light
			pEffect->m_EffectLightList.clear();
			// 에러가 발생함.
/*
			for(elit=m_pCopyEffect->m_EffectLightList.begin(); elit!=m_pCopyEffect->m_EffectLightList.end(); elit++)
			{
				CEffectLight* pLight = *elit;

				CEffectLight* pNewLight = new CEffectLight;

				int x, y, z, dR, dG, dB, sR, sG, sB, aR, aG, aB;

				pLight->GetSettingData(x, y, z, dR, dG, dB, sR, sG, sB, aR, aG, aB );
				pNewLight->SetOnlyData(x, y, z, dR, dG, dB, sR, sG, sB, aR, aG, aB );
				pNewLight->m_bPlay = false;
				pNewLight->m_bTurnOn = false;
				pNewLight->m_ElapsedTime = 0;

				pEffect->m_EffectLightList.insert( pNewLight );
			}//
*/

}

void CEffect::CopyParticle(_EFFECT *pEffect, _PARTICLE *pSrcParticle, _PARTICLE *pDestParticle)
{
	// check if particle is valid
	bool bIsValid = false;
	PARTICLELIST::iterator pit;
	for(pit=pEffect->m_ParticleList.begin(); pit!=pEffect->m_ParticleList.end(); pit++)
	{
		_PARTICLE* pParticle = *pit;

		if( pParticle == pDestParticle )
		{
			bIsValid = true;
			break;
		}
	}// for(pEffect->m_ParticleList)

	// 파티클과 이펙트가 서로 맞지 않는다.
	if( !bIsValid ) return;

	// OK, 기존의 파티클 정보를 지우고 새로운 데이타를 복사한다.
	DeleteParticle( pEffect, pDestParticle, false );// 포인터는 지우지 않는다. 

	// data copy
	*pDestParticle = *pSrcParticle;

	// element
	pDestParticle->m_ElementList.clear();
	ELEMENTLIST::iterator eit;
	for(eit=pSrcParticle->m_ElementList.begin(); eit!=pSrcParticle->m_ElementList.end(); eit++)
	{
		_ELEMENT* pSrcElement = *eit;
		_ELEMENT* pDestElement = new _ELEMENT;
		if(pDestElement == NULL)
		{
			DBG_LogFile( _T("CEffect::CopyParticle fail"));
		}

		// data copy
		*pDestElement = *pSrcElement;

		pDestParticle->m_ElementList.push_back( pDestElement );
	}// for(pSrcParticle->m_ElementList)

	pEffect->m_ParticleList.push_back( pDestParticle );
}

void CEffect::CopyElement(_PARTICLE *pParticle, _ELEMENT *pSrcElement, _ELEMENT *pDestElement)
{
	// check if element is valid
	bool bIsValid = false;
	ELEMENTLIST::iterator eit;
	for(eit=pParticle->m_ElementList.begin(); eit!=pParticle->m_ElementList.end(); eit++)
	{
		_ELEMENT* pElement = *eit;

		if( pElement == pDestElement )
		{
			bIsValid = true;
			break;
		}
	}// for(pParticle->m_ElementList)

	if( !bIsValid ) return;

	// OK, 기존 데이타를 지우고 새로운 데이타 복사.
	DeleteElement( pParticle, pDestElement, false );// 포인터는 지우지 않는다. 

	// data copy
	*pDestElement = *pSrcElement;

	pParticle->m_ElementList.push_back( pDestElement );
}

void CEffect::SetBooleanValue(bool bValue)
{
	m_bBoolean = bValue;
}

XIAHGE_API void CEffect::SetTargetMovePosition(VECTOR vTargetPos)
{
	m_vTargetMovePos = vTargetPos;
}

DWORD CEffect::GetRenderBlend(__int8 nType)
{
	DWORD dwValue = 0;
	switch( nType )
	{
	case RB_ZERO:			dwValue = D3DBLEND_ZERO;
		break;
	case RB_ONE:			dwValue = D3DBLEND_ONE;
		break;
	case RB_SRCCOLOR:		dwValue = D3DBLEND_SRCCOLOR;
		break;
	case RB_INVSRCCOLOR:	dwValue = D3DBLEND_INVSRCCOLOR;
		break;
	case RB_SRCALPHA:		dwValue = D3DBLEND_SRCALPHA;
		break;
	case RB_INVSRCALPHA:	dwValue = D3DBLEND_INVSRCALPHA;
		break;
	case RB_DESTALPHA:		dwValue = D3DBLEND_DESTALPHA;
		break;
	case RB_INVDESTALPHA:	dwValue = D3DBLEND_INVDESTALPHA;
		break;
	case RB_DESTCOLOR:		dwValue = D3DBLEND_DESTCOLOR;
		break;
	case RB_INVDESTCOLOR:	dwValue = D3DBLEND_INVDESTCOLOR;
		break;
	case RB_SRCALPHASAT:	dwValue = D3DBLEND_SRCALPHASAT;
		break;
	};// switch

	return dwValue;
}

DWORD CEffect::GetRenderBlendOP(__int8 nType)
{
	DWORD dwValue = 0;
	switch( nType )
	{
	case RBO_ADD:			dwValue = D3DBLENDOP_ADD;
		break;
	case RBO_SUBTRACT:		dwValue = D3DBLENDOP_SUBTRACT;
		break;
	case RBO_REVSUBTRACT:	dwValue = D3DBLENDOP_REVSUBTRACT;
		break;
	case RBO_MIN:			dwValue = D3DBLENDOP_MIN;
		break;
	case RBO_MAX:			dwValue = D3DBLENDOP_MAX;
		break;
	};// switch

	return dwValue;
}

DWORD CEffect::GetTextureBlendOP(__int8 nType)
{
	DWORD dwValue = 0;
	switch( nType )
	{
	case TB_DISABLE:			dwValue = D3DTOP_DISABLE;
		break;
	case TB_SELECTARG1:			dwValue = D3DTOP_SELECTARG1;
		break;
	case TB_SELECTARG2:			dwValue = D3DTOP_SELECTARG2;
		break;
	case TB_MODULATE:			dwValue = D3DTOP_MODULATE;
		break;
	case TB_MODULATE2X:			dwValue = D3DTOP_MODULATE2X;
		break;
	case TB_MODULATE4X:			dwValue = D3DTOP_MODULATE4X;
		break;
	case TB_ADD:				dwValue = D3DTOP_ADD;
		break;
	case TB_ADDSIGNED:			dwValue = D3DTOP_ADDSIGNED;
		break;
	case TB_ADDSIGNED2X:		dwValue = D3DTOP_ADDSIGNED2X;
		break;
	case TB_SUBTRACT:			dwValue = D3DTOP_SUBTRACT;
		break;
	case TB_ADDSMOOTH:			dwValue = D3DTOP_ADDSMOOTH;
		break;
	case TB_BLENDDIFFUSEALPHA:	dwValue = D3DTOP_BLENDDIFFUSEALPHA;
		break;
	case TB_BLENDTEXTUREALPHA:	dwValue = D3DTOP_BLENDTEXTUREALPHA;
		break;
	case TB_BLENDTEXTUREALPHAPM:dwValue = D3DTOP_BLENDTEXTUREALPHAPM;
		break;
	case TB_BLENDCURRENTALPHA:	dwValue = D3DTOP_BLENDCURRENTALPHA;
		break;
	case TB_DOTPRODUCT3:		dwValue = D3DTOP_DOTPRODUCT3;
		break;
	case TB_MULTIPLYADD:		dwValue = D3DTOP_MULTIPLYADD;
		break;
	case TB_LERP:				dwValue = D3DTOP_LERP;
		break;
	};// switch

	return dwValue;
}

DWORD CEffect::GetTextureBlendArg(__int8 nType)
{
	DWORD dwValue = 0;
	switch( nType )
	{
	case TBA_CURRENT:	dwValue = D3DTA_CURRENT;
		break;
	case TBA_DIFFUSE:	dwValue = D3DTA_DIFFUSE;
		break;
	case TBA_TEXTURE:	dwValue = D3DTA_TEXTURE;
		break;
	};// swtich

	return dwValue;
}

// [12/10/2004] DB 이펙트가 날라가서 함수를 변형이 필요하다.

// 로딩 방식 문제가 많구만.
XIAHGE_API BOOL XiahGameEngine::CEffect::LoadXiahEffectPackage(LPCTSTR szFilename, int nFileType)
{
//	MyString strBasePath	= NEW_EFFECT_PATH;
//	MyString strXiahEffect  = strBasePath + "CharEffect.xpe";

	// open file and error check
	FILE *CharEffect = _tfopen( szFilename, _T("rb") );
	if( !CharEffect )
	{
		LPCTSTR str=_T("CharEffect.xpe file can't open \n");
		::OutputDebugString(str);
		return FALSE;
	}

	//
#ifdef _DEBUG
	LPCTSTR str = _T("Loading CharEffect.xpe file \n");
	::OutputDebugString( str );
#endif

	if(1 == nFileType )
	{
		m_pCharFileHandle = CharEffect;

		// set Current effect type
		m_nCurEffectType = eCharacterEffect;
	}else if(2 == nFileType)
	{
		m_pCharFileHandle2 = CharEffect;

		// set Current effect type
		m_nCurEffectType = eCharacterEffect2;
	}
	else if(3 == nFileType)
	{
		m_pCharFileHandle3 = CharEffect;

		// set Current effect type
		m_nCurEffectType = eCharacterEffect3;
	}

	// load Header
	int nXiahEffectPackageCount;
	fread( &nXiahEffectPackageCount, 4, 1, CharEffect );

	m_pCurEffectPackage = NULL;
	struct _PACKAGEDATA
	{
		int		nEffectID;
		DWORD	dwEffOffset;
	};

	_PACKAGEDATA* pPackageData;
    pPackageData = new _PACKAGEDATA [ nXiahEffectPackageCount ];

	if(pPackageData == NULL)
	{
		DBG_LogFile( _T("XiahGameEngine::CEffect::LoadXiahEffectPackage1 fail"));
	}

	for(int i=0; i < nXiahEffectPackageCount; ++i)
	{
		fread( &pPackageData[i].nEffectID, 4, 1, CharEffect );
		fread( &pPackageData[i].dwEffOffset, 4, 1, CharEffect );
	}// for

	// Realize all package
	_EFFECTPACKAGE* pPackage = new _EFFECTPACKAGE;
	if(pPackage == NULL)
	{
		DBG_LogFile( _T("XiahGameEngine::CEffect::LoadXiahEffectPackage2 fail"));
	}

	for(i=0; i < nXiahEffectPackageCount; ++i)
	{
		pPackage->nEffectID = pPackageData[i].nEffectID;
		m_pCurEffectPackage = pPackage;

		RealizeEffectInEffectPackage( pPackageData[i].dwEffOffset );
	}// for

	if(pPackage)
	{
		delete pPackage;
		pPackage = NULL;
	}

	delete [] pPackageData;
	pPackageData = NULL;

	return TRUE;
}

XIAHGE_API BOOL XiahGameEngine::CEffect::LoadXiahTileEffectPackage(LPCTSTR szFilename)
{
	// open file and error check
	FILE *TileEffect = _tfopen( szFilename, _T("rb") );
	if( !TileEffect )
	{
		LPCTSTR str=_T("TileEffect.xpe file can't open \n");
		::OutputDebugString(str);
		return FALSE;
	}

	//
#ifdef _DEBUG
	LPCTSTR str = _T("Loading TileEffect.xpe file \n");
	::OutputDebugString( str );
#endif

	m_pTileFileHandle = TileEffect;

	// load Header
	int nXiahEffectPackageCount;
	fread( &nXiahEffectPackageCount, 4, 1, TileEffect );

	m_pCurEffectPackage = NULL;
	struct _PACKAGEDATA
	{
		int		nEffectID;
		DWORD	dwEffOffset;
	};

	_PACKAGEDATA* pPackageData = new _PACKAGEDATA [ nXiahEffectPackageCount ];

	if(pPackageData == NULL)
	{
		DBG_LogFile( _T("XiahGameEngine::CEffect::LoadXiahTileEffectPackage fail"));
	}

	for(int i=0; i<nXiahEffectPackageCount; i++)
	{
		fread( &pPackageData[i].nEffectID, 4, 1, TileEffect );
		fread( &pPackageData[i].dwEffOffset, 4, 1, TileEffect );
	}// for

	// set Current effect type
	m_nCurEffectType = eTileEffect;

	// Realize all package
	_EFFECTPACKAGE* pPackage = new _EFFECTPACKAGE;
	if(pPackage == NULL)
	{
		DBG_LogFile( _T("XiahGameEngine::CEffect::LoadXiahTileEffectPackage2 fail"));
	}

	for(i=0; i<nXiahEffectPackageCount; i++)
	{
		pPackage->nEffectID = pPackageData[i].nEffectID;
		m_pCurEffectPackage =  pPackage;

		RealizeEffectInEffectPackage( pPackageData[i].dwEffOffset );
	}// for

	if(pPackage)
	{
		delete pPackage;
		pPackage = NULL;
	}
	delete []pPackageData;
	pPackageData = NULL;

	return TRUE;
}

void XiahGameEngine::CEffect::RealizeEffectInEffectPackage(DWORD dwEffOffset)
{
	FILE* fp = NULL;
	switch( m_nCurEffectType )
	{
		case eCharacterEffect:		fp = m_pCharFileHandle;		break;
		case eTileEffect:			fp = m_pTileFileHandle;		break;
		case eExpAcquireEffect:		fp = m_pCharFileHandle;		break;
		case eCharacterEffect2:		fp = m_pCharFileHandle2;	break;
		case eCharacterEffect3:		fp = m_pCharFileHandle3;	break;
	};

	// move effect file data position in Package File
	fseek( fp, dwEffOffset, SEEK_SET );

	DWORD dwEffLen;
	fread( &dwEffLen, 4, 1, fp );

	// load effect file
	LoadEffect( fp, false );

	//
	int nTextureCount;
	fread( &nTextureCount, 4, 1, fp );
	for(int i=0; i<nTextureCount; i++)
	{
		MyString strTexPath;
		XiahGameEngine::Load( strTexPath, fp );

		int nTextureResID;
		fread( &nTextureResID, 4, 1, fp );

		m_pCurEffect->m_TextureResIDList.insert( STRINGINTLIST::value_type( strTexPath, nTextureResID ) );
	}// for

	int nMeshCount;
	fread( &nMeshCount, 4, 1, fp );
	for(i=0; i<nMeshCount; i++)
	{
		MyString strMeshPath;
		XiahGameEngine::Load( strMeshPath, fp );

		DWORD dwMeshOffset;
		fread( &dwMeshOffset, 4, 1, fp );

		m_pCurEffect->m_MeshList.insert( STRINGDWORDLIST::value_type( strMeshPath, dwMeshOffset ) );
	}// for
}

void XiahGameEngine::CEffect::RealizeEffectInEffectPackage(_EFFECTPACKAGE* pPackage)
{
/*	안쓴다.
	m_pCurEffectPackage = pPackage;
	if( pPackage->pEffect == NULL )
	{
		// 우선 m_EffectList.에 존재하는지 검사.
		_EFFECT* pEffect = GetEffect( pPackage->nEffectID );
		if( pEffect )
		{
			pPackage->pEffect = pEffect;
			return;
		}

		// 메모리에 없으니깐 파일에서 읽는다.
		// move effect file data position in Package File
		fseek( m_pFileHandle, pPackage->dwEffOffset, SEEK_SET );

		DWORD dwEffLen;
		fread( &dwEffLen, 4, 1, m_pFileHandle );

		// load effect file
		LoadEffect( m_pFileHandle, false );
		pPackage->pEffect = m_pCurEffect;

		//
		int nTextureCount;
		fread( &nTextureCount, 4, 1, m_pFileHandle );
		for(int i=0; i<nTextureCount; i++)
		{
			MyString strTexPath;
			XiahGameEngine::Load( strTexPath, m_pFileHandle );

			int nTextureResID;
			fread( &nTextureResID, 4, 1, m_pFileHandle );

			pPackage->pEffect->m_TextureResIDList.insert( STRINGINTLIST::value_type( strTexPath, nTextureResID ) );
		}// for

		int nMeshCount;
		fread( &nMeshCount, 4, 1, m_pFileHandle );
		for(i=0; i<nMeshCount; i++)
		{
			MyString strMeshPath;
			XiahGameEngine::Load( strMeshPath, m_pFileHandle );

			DWORD dwMeshOffset;
			fread( &dwMeshOffset, 4, 1, m_pFileHandle );

			pPackage->pEffect->m_MeshList.insert( STRINGDWORDLIST::value_type( strMeshPath, dwMeshOffset ) );
		}// for

	}// if
*/
}

XIAHGE_API void XiahGameEngine::CEffect::RealizeEffectResouce(_EFFECT* pEffect)
{
	if( !pEffect->m_bLoadResource )
	{
		STRINGINTLIST::iterator tit;
		for(tit=pEffect->m_TextureResIDList.begin(); tit!=pEffect->m_TextureResIDList.end(); tit++)
		{
			MyString strTexPath = tit->first;
			int nResID = tit->second;

			// register in effect texture pool
			LPDIRECT3DTEXTURE9 pTexture = XiahPak::GetTexture( nResID );
			m_pEffectResPool->RegisterTextureFromFile( strTexPath, pTexture );

		}// for(pEffect->m_TextureResIDList)

		// select current file pointer
		FILE* fp = NULL;
		switch( pEffect->m_nEffectType )
		{
		case eCharacterEffect:		fp = m_pCharFileHandle;		break;
		case eTileEffect:			fp = m_pTileFileHandle;		break;
		case eExpAcquireEffect:		fp = m_pCharFileHandle;		break;
		case eCharacterEffect2:		fp = m_pCharFileHandle2;	break;
		case eCharacterEffect3:		fp = m_pCharFileHandle3;	break;
		};

		STRINGDWORDLIST::iterator mit;
		for(mit=pEffect->m_MeshList.begin(); mit!=pEffect->m_MeshList.end(); mit++)
		{
			MyString strMeshPath = mit->first;
			DWORD dwMeshOffset = mit->second;

			// move HCH mesh data position in file
			fseek( fp, dwMeshOffset, SEEK_SET );

			// register in effect mesh pool
			m_pEffectResPool->RegisterHCHMeshFromFile( strMeshPath, fp );

		}// for(pEffect->m_MeshList)

		pEffect->m_bLoadResource = true;
	}// if
}

XIAHGE_API void CEffect::SetVertexBufferRenewTime(DWORD dwRenewTime)
{
	m_dwVertexBufferMakingTimeVal = dwRenewTime;
}



//
// --- CEffectResPool ----------------------------
//

CEffectResPool::CEffectResPool()
{
	m_nCurTextureAllocated = 0;
	m_nCurMeshAllocated = 0;
}

CEffectResPool::~CEffectResPool()
{
	Release();
}

void CEffectResPool::Release()
{
#ifdef _DEBUG
	LPCTSTR str = "CEffectResPool Release \n";
	::OutputDebugString( str );
#endif

	EFFECTTEXTUREMAP::iterator etit;
	for(etit=m_TextureMap.begin(); etit!=m_TextureMap.end(); etit++)
	{
		_EFFECTTEXTURE* pTexture = etit->second;
		delete pTexture;
		pTexture = NULL;
	}
	m_TextureMap.clear();

	EFFECTMESHMAP::iterator emit;
	for(emit=m_MeshMap.begin(); emit!=m_MeshMap.end(); emit++)
	{
		_EFFECTMESH* pMesh = emit->second;
		delete pMesh;
		pMesh = NULL;
	}
	m_MeshMap.clear();
}

void CEffectResPool::Create(LPDIRECT3DDEVICE9 pDevice, int nMaxTextureSize, int nMaxMeshSize)
{
	m_pDevice = pDevice;
	m_nMaxTextureAllocated = nMaxTextureSize;
	m_nMaxMeshAllocated = nMaxMeshSize;
}

int CEffectResPool::CreateTextureFromFile(_EFFECTTEXTURE* pEffectTexture, LPCTSTR filename )
{	// 파일을 읽어서 텍스쳐를 생성한다.  => .bmp, .dds, .dib, .jpg, .png, .tga
	pEffectTexture->m_strFileName = filename;
	if( pEffectTexture->m_strFileName.empty() ) return 0;

	// calculate file size
	FILE *fp;
	fp = _tfopen( filename, _T("rb") );
	if( !fp ) return 0;

	long start, end;
	start = ftell( fp);
	fseek( fp, 0, SEEK_END);
	end = ftell( fp);

	pEffectTexture->m_nAllocSize = end - start;
	fclose( fp);

	// release old texture and create new texture
	if( pEffectTexture->m_pTexture )
		pEffectTexture->m_pTexture->Release();
	pEffectTexture->m_pTexture = NULL;

	HRESULT hr = D3DXCreateTextureFromFile( m_pDevice, filename, &pEffectTexture->m_pTexture );

	if( !pEffectTexture->m_pTexture) return 0;

    return pEffectTexture->m_nAllocSize;
}

_EFFECTMESH * CEffectResPool::GetMesh(LPCTSTR filename)
{
    _EFFECTMESH* pEffectMesh = NULL;
	MyString strFilename = filename;
	MyString str;

	if( strFilename.empty() ) return pEffectMesh;

	// Find this mesh in meshmap, if not found, use FILE I/O to load file mesh
	EFFECTMESHMAP::iterator it;
	it = m_MeshMap.find(filename);

	if( it != m_MeshMap.end() )	// Find!
	{
		pEffectMesh = it->second;
		pEffectMesh->m_nRefCount++;
	}
	else	// Not Found!
	{
		pEffectMesh = new _EFFECTMESH;
		MyString strFile = filename;

		pEffectMesh->m_pMeshBuffer = NULL;

/*
		// 어차피 파일 포멧은 BCF, HCH이니깐, B or H 로 판단하자.
		// 아 그런데, 이펙트 메쉬는 HCH이다. ^^;
		LPCTSTR pstrFile = strFile.data();

		char cType = pstrFile[ strFile.size()-1 - 2 ];
		int nSize=0;
		if( cType == 'b' || cType == 'B' )
			nSize = LoadBCFFromFile( pEffectMesh, filename );
		else
		if( cType == 'h' || cType == 'H' )
			nSize = LoadHCHFromFile( pEffectMesh, filename );

		// Check whether file load or not, In DEBUG Mode
#ifdef _DEBUG
		if(nSize==0)
		{
			str = "Can't load mesh : ";
			str += filename;
			str += " \n";
			::OutputDebugString(str.data());
		}
		else
		{
			str = "Load mesh : ";
			str += filename;
			str += " \n";
			::OutputDebugString(str.data());
		}
#endif
*/
		// insert this mesh In meshmap
		m_nCurMeshAllocated += 0;
		pEffectMesh->m_nRefCount = 1;
		m_MeshMap.insert( EFFECTMESHMAP::value_type( strFilename, pEffectMesh ) );

/*		이 부분을 사용할지 안할지는 모르겠다. 그래서 주석이다.
		// Check current mesh allocated memory, so management unusage mesh memory
		if( m_nCurMeshAllocated > m_nMaxMeshAllocated )
		{
#ifdef _DEBUG
			str.Format("Mesh Memory Full : %d > %d \n", m_nCurMeshAllocated, m_nMaxMeshAllocated);
			::OutputDebugString(str);
#endif

			// delete if m_nRefCount = 0; so that, no more usage
			EFFECTMESHMAP::iterator mit;
			for(mit=m_MeshMap.begin(); mit!=m_MeshMap.end(); mit++)
			{
				_EFFECTMESH* pEMesh = mit->second;
				CString strMesh = mit->first;

				if( strMesh == strFilename  ) continue; // same file
				if( pEMesh->m_nRefCount > 1 ) continue;

				// delete this mesh
				m_nCurMeshAllocated -= pEMesh->m_nAllocSize;

				mit = m_MeshMap.erase(mit);
				delete pEMesh;
				if (mit == m_MeshMap.end()) break;

				// enough memory?
				if( m_nCurMeshAllocated < m_nMaxMeshAllocated ) break;

			}// for(m_MeshMap)

#ifdef _DEBUG
			if( m_nCurMeshAllocated > m_nMaxMeshAllocated )
			{
				str.Format("All mesh is using so Mesh Memory overloaded. \n");
				::OutputDebugString(str);
			}
#endif
		}// if( m_nCurMeshAllocated > m_nMaxMeshAllocated )
*/

	}// if() and else

	return pEffectMesh;
}

void CEffectResPool::RegisterMeshFromFile(MyString strName, MyString strFilename)
{
	_EFFECTMESH* pEffectMesh = NULL;

	// Find this mesh in meshmap, if not found, use FILE I/O to load file mesh
	EFFECTMESHMAP::iterator it;
	it = m_MeshMap.find(strName.data());

	if( it != m_MeshMap.end() )	// Find!
	{
		pEffectMesh = it->second;
		pEffectMesh->m_nRefCount++;
	}
	else	// not Found
	{
		pEffectMesh = new _EFFECTMESH;
		if(pEffectMesh == NULL)
		{
			DBG_LogFile( _T("CEffectResPool::RegisterMeshFromFile fail"));
		}

		// 어차피 파일 포멧은 BCF, HCH이니깐, B or H 로 판단하자.
		LPCTSTR pstrFile = strName.data();

		TCHAR cType = pstrFile[ strName.size()-1 - 2 ];
		int nSize=0;
		if( cType == 'b' || cType == 'B' )
			nSize = LoadBCFFromFile( pEffectMesh, strFilename.data() );
		else
		if( cType == 'h' || cType == 'H' )
			nSize = LoadHCHFromFile( pEffectMesh, strFilename.data() );

		// set file name
		int nNameSize = strName.size() - strName.rfind('\\');

		LPCTSTR pstrName1 = strName.data();
		LPCTSTR pstrName2 = &pstrName1[nNameSize];

		pEffectMesh->m_strFileName = pstrName2;

		// Check whether file load or not, In DEBUG Mode
#ifdef _DEBUG
		MyString str;
		if(nSize==0)
		{
			str = "Can't load mesh : ";
			str += strFilename;
			str += " \n";
			::OutputDebugString(str.data());
		}
		else
		{
			str = "Load mesh : ";
			str += strFilename;
			str += " \n";
			::OutputDebugString(str.data());
		}
#endif

		// insert this mesh In meshmap
		m_nCurMeshAllocated += nSize;
		pEffectMesh->m_nRefCount = 1;
		m_MeshMap.insert( EFFECTMESHMAP::value_type( strName, pEffectMesh ) );

		// this function do not check memory-full 
	}

}

void CEffectResPool::RegisterHCHMeshFromFile(MyString strName, FILE* fp)
{
	_EFFECTMESH* pEffectMesh = NULL;

	// Find this mesh in meshmap, if not found, use FILE I/O to load file mesh
	EFFECTMESHMAP::iterator it;
	it = m_MeshMap.find( strName );

	if( it != m_MeshMap.end() )	// Find!
	{
		pEffectMesh = it->second;
		pEffectMesh->m_nRefCount++;
	}
	else	// not Found
	{
		pEffectMesh = new _EFFECTMESH;
		if(pEffectMesh == NULL)
		{
			DBG_LogFile( _T("CEffectResPool::RegisterHCHMeshFromFile fail"));
		}

		DWORD dwMeshLen;
		fread( &dwMeshLen, 4, 1, fp );

		pEffectMesh->m_nAllocSize = dwMeshLen;

		// load HCH mesh data from memory
		BYTE* pBuffer = new BYTE [ dwMeshLen ];
		if(pBuffer == NULL)
		{
			DBG_LogFile( _T("CEffectResPool::RegisterHCHMeshFromFile fail"));
		}

		fread( pBuffer, dwMeshLen, 1, fp );
		LoadHCHFromMemory( pEffectMesh, pBuffer );
		delete []pBuffer;
		pBuffer = NULL;

//		Or load HCH mesh data from file
//		LoadHCHFromFile( pEffectMesh, fp );

		// Check whether file load or not, In DEBUG Mode
#ifdef _DEBUG
		MyString str;
		str = "Load mesh : ";
		str += strName.data();
		str += " \n";
		::OutputDebugString(str.data());
#endif

		// insert this mesh In meshmap
		m_nCurMeshAllocated += dwMeshLen;
		pEffectMesh->m_nRefCount = 1;
		m_MeshMap.insert( EFFECTMESHMAP::value_type( strName, pEffectMesh ) );
	}

}

void CEffectResPool::RegisterTextureFromFile(MyString strName, LPDIRECT3DTEXTURE9 pTexture)
{
	_EFFECTTEXTURE* pEffectTexture = NULL;

	// Find this texture in texturemap
	EFFECTTEXTUREMAP::iterator it;
	it = m_TextureMap.find( strName );

	if( it != m_TextureMap.end() )	// Find!
	{
		pEffectTexture = it->second;
		pEffectTexture->m_nRefCount++;
	}
	else	// Not Found! => register
	{
		pEffectTexture = new _EFFECTTEXTURE;
		if(pEffectTexture == NULL)
		{
			DBG_LogFile( _T("CEffectResPool::RegisterTextureFromFile fail"));
		}

		// insert this texture In texture map 
//		m_nCurTextureAllocated += nSize;
		pEffectTexture->m_pTexture = pTexture;
		pEffectTexture->m_nRefCount = 1;
		pEffectTexture->m_strFileName = strName;
		pEffectTexture->m_nAllocSize = 100;
		m_TextureMap.insert( EFFECTTEXTUREMAP::value_type( strName, pEffectTexture ) );

#ifdef _DEBUG
		MyString str;
		str = "Load texture : ";
		str += strName.data();
		str += " \n";
		::OutputDebugString(str.data());
#endif
	}// if()

}

int CEffectResPool::LoadHCHFromFile(_EFFECTMESH *pEffectMesh, LPCTSTR filename)
{
	pEffectMesh->m_strFileName = filename;
	if( pEffectMesh->m_strFileName.empty() ) return 0;

	// calculate file size
	FILE *fp;
	fp = _tfopen( filename, _T("rb") );
	if( !fp ) return 0;

	long start, end;
	start = ftell( fp);
	fseek( fp, 0, SEEK_END);
	end = ftell( fp);

	pEffectMesh->m_nAllocSize = end - start;

	fclose( fp);

	// releaes old mesh and create new mesh
	if( pEffectMesh->m_pMeshBuffer )
	{
		delete []pEffectMesh->m_pMeshBuffer;
		pEffectMesh->m_pMeshBuffer = NULL;
	}

	CHCHFile2 hchFile;
	hchFile.LoadFile(filename);

	pEffectMesh->m_nMeshCount = hchFile.m_HCHFile.nMeshCount;
	pEffectMesh->m_nFrameCount= hchFile.m_HCHFile.nFrameCount;

	// texture => no texture 나중에 필요하면 넣는다. 

	// Mesh Buffer
	pEffectMesh->m_pMeshBuffer = new _MESHBUFFER[ pEffectMesh->m_nMeshCount ];

	for(int i=0; i<pEffectMesh->m_nMeshCount; i++)
	{
		_MESHBUFFER *pBuffer = pEffectMesh->m_pMeshBuffer + i;
		pBuffer->nTexID = 0;
		pBuffer->nVertexCount = hchFile.m_HCHFile.pMesh[i].nVertexCount;
		pBuffer->nFaceCount = hchFile.m_HCHFile.pMesh[i].nFaceCount;

		pBuffer->pVertex = new _ELEMENTVERTEX[ hchFile.m_HCHFile.pMesh[i].nVertexCount * hchFile.m_HCHFile.nFrameCount ];
		pBuffer->pFace = new _FACEDATA[ hchFile.m_HCHFile.pMesh[i].nFaceCount ];

		memcpy( pBuffer->pVertex, hchFile.m_HCHFile.pMesh[i].pVertex, sizeof(_ELEMENTVERTEX) * hchFile.m_HCHFile.pMesh[i].nVertexCount * hchFile.m_HCHFile.nFrameCount );
		memcpy( pBuffer->pFace, hchFile.m_HCHFile.pMesh[i].pFace, sizeof( _FACEDATA ) * hchFile.m_HCHFile.pMesh[i].nFaceCount );
	}// for(pEffectMesh->m_nMeshCount)

	return pEffectMesh->m_nAllocSize;
}

int CEffectResPool::LoadHCHFromFile(_EFFECTMESH* pEffectMesh, FILE* fp)
{
	// releaes old mesh and create new mesh
	if( pEffectMesh->m_pMeshBuffer )
	{
		delete []pEffectMesh->m_pMeshBuffer;
		pEffectMesh->m_pMeshBuffer = NULL;
	}

	// read data from file
	fread( &pEffectMesh->m_nMeshCount,  4, 1, fp );
	fread( &pEffectMesh->m_nFrameCount, 4, 1, fp );

	int nBoneCount;
	fread( &nBoneCount, 4, 1, fp );

	DWORD dwMaterialSize = sizeof(MATERIALm) * pEffectMesh->m_nMeshCount;
	DWORD dwBoneSize = sizeof(D3DMATRIX) * nBoneCount * pEffectMesh->m_nFrameCount;
	fseek( fp, dwMaterialSize + dwBoneSize, SEEK_CUR );

	// Mesh Buffer
	pEffectMesh->m_pMeshBuffer = new _MESHBUFFER[ pEffectMesh->m_nMeshCount ];
	for(int i=0; i<pEffectMesh->m_nMeshCount; i++)
	{
		_MESHBUFFER *pBuffer = pEffectMesh->m_pMeshBuffer + i;
		pBuffer->nTexID = 0;

		fread( &pBuffer->nVertexCount, 4, 1, fp );
		fread( &pBuffer->nFaceCount,   4, 1, fp );

		pBuffer->pVertex = new _ELEMENTVERTEX[ pBuffer->nVertexCount * pEffectMesh->m_nFrameCount ];
		pBuffer->pFace = new _FACEDATA[ pBuffer->nFaceCount ];

		fread( pBuffer->pFace, sizeof(_FACEDATA)*pBuffer->nFaceCount, 1, fp );
		fread( pBuffer->pVertex, sizeof(_ELEMENTVERTEX)*pBuffer->nVertexCount*pEffectMesh->m_nFrameCount, 1, fp );
	}// for(pEffectMesh->m_nMeshCount)

	return pEffectMesh->m_nAllocSize;
}

int CEffectResPool::LoadHCHFromMemory(_EFFECTMESH* pEffectMesh, BYTE* pBuffer)
{
	// releaes old mesh and create new mesh
	if( pEffectMesh->m_pMeshBuffer )
	{
		delete []pEffectMesh->m_pMeshBuffer;
		pEffectMesh->m_pMeshBuffer = NULL;
	}

	// Read data from memory
	DWORD dwBufPos = 0;

	memcpy( &pEffectMesh->m_nMeshCount, &pBuffer[dwBufPos], sizeof(pEffectMesh->m_nMeshCount) );
	dwBufPos += sizeof( pEffectMesh->m_nMeshCount );

	memcpy( &pEffectMesh->m_nFrameCount, &pBuffer[dwBufPos], sizeof(pEffectMesh->m_nFrameCount) );
	dwBufPos += sizeof( pEffectMesh->m_nFrameCount );

	int nBoneCount;
	memcpy( &nBoneCount, &pBuffer[dwBufPos], sizeof(nBoneCount) );
	dwBufPos += sizeof( nBoneCount );

	// 이 데이터들은 필요 없다.
	DWORD dwMaterialSize = sizeof(MATERIALm) * pEffectMesh->m_nMeshCount;
	DWORD dwBoneSize = sizeof(D3DMATRIX) * nBoneCount * pEffectMesh->m_nFrameCount;
	dwBufPos += ( dwMaterialSize + dwBoneSize );

	// Mesh Buffer
	pEffectMesh->m_pMeshBuffer = new _MESHBUFFER[ pEffectMesh->m_nMeshCount ];
	for(int i=0; i<pEffectMesh->m_nMeshCount; i++)
	{
		_MESHBUFFER *pMeshBuffer = pEffectMesh->m_pMeshBuffer + i;
		pMeshBuffer->nTexID = 0;

		memcpy( &pMeshBuffer->nVertexCount, &pBuffer[dwBufPos], sizeof(pMeshBuffer->nVertexCount) );
		dwBufPos += sizeof(pMeshBuffer->nVertexCount);

		memcpy( &pMeshBuffer->nFaceCount, &pBuffer[dwBufPos], sizeof(pMeshBuffer->nFaceCount) );
		dwBufPos += sizeof(pMeshBuffer->nFaceCount);

		// new 
		pMeshBuffer->pVertex = new _ELEMENTVERTEX[ pMeshBuffer->nVertexCount * pEffectMesh->m_nFrameCount ];
		pMeshBuffer->pFace = new _FACEDATA[ pMeshBuffer->nFaceCount ];

		memcpy( pMeshBuffer->pFace, &pBuffer[dwBufPos], sizeof(_FACEDATA)*pMeshBuffer->nFaceCount );
		dwBufPos += ( sizeof(_FACEDATA)*pMeshBuffer->nFaceCount );

		memcpy( pMeshBuffer->pVertex, &pBuffer[dwBufPos], sizeof(_ELEMENTVERTEX)*pMeshBuffer->nVertexCount*pEffectMesh->m_nFrameCount );
		dwBufPos += ( sizeof(_ELEMENTVERTEX)*pMeshBuffer->nVertexCount*pEffectMesh->m_nFrameCount );

		// vertex buffer
//		m_pDevice->CreateVertexBuffer( pMeshBuffer->nVertexCount*pEffectMesh->m_nFrameCount*sizeof(_ELEMENTVERTEX),
//			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, &pMeshBuffer->pVB, NULL );
		m_pDevice->CreateVertexBuffer( pMeshBuffer->nVertexCount*pEffectMesh->m_nFrameCount*sizeof(_ELEMENTVERTEX),
			0, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &pMeshBuffer->pVB, NULL );

		_ELEMENTVERTEX* pVer;

		pMeshBuffer->pVB->Lock( 0, 0, (void**)&pVer, 0 );

		memcpy( pVer, pMeshBuffer->pVertex, sizeof(_ELEMENTVERTEX)*pMeshBuffer->nVertexCount*pEffectMesh->m_nFrameCount );

		pMeshBuffer->pVB->Unlock();

		delete []pMeshBuffer->pVertex;
		pMeshBuffer->pVertex = NULL;

		// index buffer
//		m_pDevice->CreateIndexBuffer( pMeshBuffer->nFaceCount*3*sizeof(WORD),
//					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pMeshBuffer->pIB, NULL );
		m_pDevice->CreateIndexBuffer( pMeshBuffer->nFaceCount*3*sizeof(WORD),
					0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pMeshBuffer->pIB, NULL );

		WORD* pInd;
		pMeshBuffer->pIB->Lock( 0, 0, (void**)&pInd, 0 );

		int nIndIndex = -1;
		for(int j=0; j<pMeshBuffer->nFaceCount; j++)
		{
			for(int h=0; h<3; h++)
				pInd[ ++nIndIndex ] = pMeshBuffer->pFace[j].v[h];
		}// for

		pMeshBuffer->pIB->Unlock();

		delete []pMeshBuffer->pFace;
		pMeshBuffer->pFace = NULL;

	}// for(pEffectMesh->m_nMeshCount)

	return pEffectMesh->m_nAllocSize;
}

int CEffectResPool::LoadBCFFromFile(_EFFECTMESH *pEffectMesh, LPCTSTR filename)
{
	pEffectMesh->m_strFileName = filename;
	if( pEffectMesh->m_strFileName.empty() ) return 0;

	// calculate file size
	FILE *fp;
	fp = _tfopen( filename, _T("rb") );
	if( !fp ) return 0;

	long start, end;
	start = ftell( fp);
	fseek( fp, 0, SEEK_END);
	end = ftell( fp);

	pEffectMesh->m_nAllocSize = end - start;

	// read file at first position
	fseek( fp, 0, SEEK_SET);

	fread( &pEffectMesh->m_nMeshCount, 4, 1, fp );

	// make texture list of this MESH
	for(int i=0; i<pEffectMesh->m_nMeshCount; i++)
	{
		TCHAR len;
		TCHAR buff[255];
		fread( &len, 1, 1, fp);

		if( len > 0 )
		{
			fread( buff, len, 1, fp );
			buff[ len ] = '\0';
		}

		// insert texture path to list and other data JUMPS
		MyString texturePath = buff;
		m_MeshTextureList.push_back( texturePath );

		int a,b;
		fread( &a, 4, 1, fp );
		fread( &b, 4, 1, fp );

		typedef struct tagCOLORVALUE
		{
			float	fRed;
			float	fGreen;
			float	fBlue;
		} COLORVALUE;

		fseek( fp, (long)72, SEEK_CUR );
	}// for(pEffectMesh->m_nMeshCount)

	int temp;
	fread( &pEffectMesh->m_nFrameCount, 4, 1, fp);
	fread( &temp, 4, 1, fp );

	if( temp > 0 )
		fseek( fp, (long)( sizeof(D3DMATRIX) * pEffectMesh->m_nFrameCount * temp ), SEEK_CUR );

	// Mesh buffer
	pEffectMesh->m_pMeshBuffer = new _MESHBUFFER[ pEffectMesh->m_nMeshCount ];
	for( i=0; i<pEffectMesh->m_nMeshCount; i++ )
	{
		_MESHBUFFER *pBuffer = pEffectMesh->m_pMeshBuffer + i;

		fread( &pBuffer->nTexID, 4, 1, fp );

		fread( &pBuffer->nVertexCount, 4, 1, fp );
		fread( &pBuffer->nFaceCount, 4, 1, fp );

		pBuffer->pVertex = new _ELEMENTVERTEX[ pBuffer->nVertexCount * pEffectMesh->m_nFrameCount ];
		pBuffer->pFace = new _FACEDATA[ pBuffer->nFaceCount ];

		fread( pBuffer->pFace, sizeof(_FACEDATA), pBuffer->nFaceCount, fp );
		fread( pBuffer->pVertex, sizeof(_ELEMENTVERTEX), pBuffer->nVertexCount * pEffectMesh->m_nFrameCount, fp );
	}// for(pEffectMesh->m_nMeshCount)

	fclose(fp);

	return pEffectMesh->m_nAllocSize;
}

void CEffectResPool::DeleteTexture(MyString strFilename)
{
	EFFECTTEXTUREMAP::iterator emit;
	emit = m_TextureMap.find(strFilename.data());

	if( emit == m_TextureMap.end() ) return;

	_EFFECTTEXTURE* pETexture = emit->second;

	m_nCurTextureAllocated -= pETexture->m_nAllocSize;

	m_TextureMap.erase(emit);
	delete pETexture;
}

void CEffectResPool::GetAllTexture(EFFECTTEXTUREMAP &texturemap)
{
	EFFECTTEXTUREMAP::iterator it;

	for(it=m_TextureMap.begin(); it!=m_TextureMap.end(); it++)
		texturemap.insert( EFFECTTEXTUREMAP::value_type( it->first, it->second ) );
}

void CEffectResPool::GetAllMesh(EFFECTMESHMAP &meshmap)
{
	EFFECTMESHMAP::iterator it;

	for(it=m_MeshMap.begin(); it!=m_MeshMap.end(); it++)
		meshmap.insert( EFFECTMESHMAP::value_type( it->first, it->second ) );
}

_EFFECTTEXTURE* CEffectResPool::FindTexture(MyString strTexture)
{
	_EFFECTTEXTURE* pEffectTexture = NULL;

	EFFECTTEXTUREMAP::iterator it;
	it = m_TextureMap.find(strTexture.data());

	if( it != m_TextureMap.end() )	// Find
		pEffectTexture = it->second;

	return pEffectTexture;
}

_EFFECTMESH* CEffectResPool::FindMesh(MyString strMesh)
{
	_EFFECTMESH* pEffectMesh = NULL;

	EFFECTMESHMAP::iterator it;
	it = m_MeshMap.find(strMesh.data());

	if( it != m_MeshMap.end() ) // Find
		pEffectMesh = it->second;

	return pEffectMesh;
}

_EFFECTTEXTURE* CEffectResPool::GetTexture(LPCTSTR filename)
{
	_EFFECTTEXTURE* pEffectTexture = NULL;
	MyString strFile = filename;
	MyString str;

	// Find this texture in texturemap. if not found, use FILE I/O to load file texture
	EFFECTTEXTUREMAP::iterator it;
	it = m_TextureMap.find(strFile.data());

	if( it != m_TextureMap.end() )	// Find!
	{
		pEffectTexture = it->second;

#ifdef	_EDITOR
		// 데이타가 없으면 직접 파일에서 읽어서 만든다.
		// 이 코드는 클라이언트에서는 필요 없고, 에디터에서는 필요하다.
		if( NULL == pEffectTexture->m_pTexture )
			CreateTextureFromFile( pEffectTexture, filename );
#endif

		pEffectTexture->m_nRefCount++;
	}
	else	// No Found => register 
	{
		// 텍스쳐가 없으면 NULL로 만든다. 첨에 만든 "" 텍스쳐를 돌려준다.
		if( filename != _T("") )
		{
			MyString strFile = _T("");
			it = m_TextureMap.find(strFile.data());

			if (it != m_TextureMap.end()) {
				pEffectTexture = it->second;
				pEffectTexture->m_nRefCount++;
			} else {
				pEffectTexture = NULL;
			}
		}
		else
		{
			pEffectTexture = new _EFFECTTEXTURE;

			// insert this texture In texture map 
			m_nCurTextureAllocated += 0;
			pEffectTexture->m_nRefCount = 1;
			m_TextureMap.insert( EFFECTTEXTUREMAP::value_type( strFile, pEffectTexture ) );
		}

#ifdef _DEBUG
		MyString str;
		str = "Load texture : ";
		str += filename;
		str += " \n";
		::OutputDebugString(str.data());
#endif

/*
		// file loading
		pEffectTexture = new _EFFECTTEXTURE;
		int nSize = CreateTextureFromFile( pEffectTexture, filename );

		// Check whether file load or not, In DEBUG Mode
#ifdef _DEBUG
		if(nSize==0)
		{
			str = "Can't load texture : ";
            str += filename;
			str += " \n";
			::OutputDebugString(str.data());
		}
		else
		{
			str = "Load texture : ";
			str += filename;
			str += " \n";
			::OutputDebugString(str.data());
		}
#endif

		// insert this texture In texture map 
		m_nCurTextureAllocated += nSize;
		pEffectTexture->m_nRefCount = 1;
		m_TextureMap.insert( EFFECTTEXTUREMAP::value_type( strFile, pEffectTexture ) );
*/

/*		당분간은 사용하지 말자.
		// Check current texture allocated memory, so management unusage texture memory
		if( m_nCurTextureAllocated > m_nMaxTextureAllocated )
		{
#ifdef _DEBUG
			str.Format("Texture Memory Full : %d > %d \n", m_nCurTextureAllocated, m_nMaxTextureAllocated);
			::OutputDebugString(str);
#endif

			// delete no more using texture
			EFFECTTEXTUREMAP::iterator tit;
			for(tit=m_TextureMap.begin(); tit!=m_TextureMap.end(); tit++)
			{
				_EFFECTTEXTURE* pETexture = tit->second;
				CString strTexture = tit->first;

				if( strTexture == strFile ) continue;			// same file so this texture
				if( pETexture->m_nRefCount > 1 ) continue;		// now using

				// delete this texture
				m_nCurTextureAllocated -= pETexture->m_nAllocSize;

				tit = m_TextureMap.erase(tit);
				delete pETexture;

				// enough memory?
				if( m_nCurTextureAllocated < m_nMaxTextureAllocated ) break;

			} // for( m_TextureMap )

#ifdef _DEBUG
			if( m_nCurTextureAllocated > m_nMaxTextureAllocated )
			{
				str.Format("All texture is using so Texture Memory overloaded. \n");
				::OutputDebugString(str);
			}
#endif
		}// if( m_nCurTextureAllocated > m_nMaxTextureAllocated )
*/

	}// if and else

	return pEffectTexture;
}

void CEffectResPool::DeleteMesh(MyString strFilename)
{
	EFFECTMESHMAP::iterator emit;
	emit = m_MeshMap.find(strFilename.data());

	if( emit == m_MeshMap.end() ) return;

	_EFFECTMESH* pMesh = emit->second;

	m_nCurMeshAllocated -= pMesh->m_nAllocSize;

	m_MeshMap.erase(emit);
	delete pMesh;
	pMesh = NULL;
}

void CEffectResPool::IncTextureRefCount(MyString strFilename)
{
	_EFFECTTEXTURE* pEffectTexture = FindTexture( strFilename );

	if( NULL != pEffectTexture )
		pEffectTexture->m_nRefCount++;
}

void CEffectResPool::DecTextureRefCount(MyString strFilename)
{
	_EFFECTTEXTURE* pEffectTexture = FindTexture( strFilename );

	if( NULL != pEffectTexture )
		pEffectTexture->m_nRefCount--;
}

void CEffectResPool::IncMeshRefCount(MyString strFilename)
{
	_EFFECTMESH* pEffectMesh = FindMesh( strFilename );

	if( NULL != pEffectMesh )
		pEffectMesh->m_nRefCount++;
}

void CEffectResPool::DecMeshRefCount(MyString strFilename)
{
	_EFFECTMESH* pEffectMesh = FindMesh( strFilename );

	if( NULL != pEffectMesh )
		pEffectMesh->m_nRefCount--;
}



//
// --- CEffectLight ----------------------------
//


CEffectLight::CEffectLight()
{

}

CEffectLight::~CEffectLight()
{

}

void CEffectLight::Init(int x, int y, int z, int dR, int dG, int dB, int sR, int sG, int sB, int aR, int aG, int aB)
{
	// manipulate as you wish
	m_bTurnOn = true;
	m_bPlay = false;
	m_ElapsedTime = 0;
	m_bIsSetMatrix = false;

	Setting(x, y, z, dR, dG, dB, sR, sG, sB, aR, aG, aB);
}

void CEffectLight::Save(FILE *fp)
{
	fwrite( &m_LifeTime,	4, 1, fp );
	fwrite( &m_nPosX,		4, 1, fp );
	fwrite( &m_nPosY,		4, 1, fp );
	fwrite( &m_nPosZ,		4, 1, fp );
	fwrite( &m_nDiffuseR,	4, 1, fp );
	fwrite( &m_nDiffuseG,	4, 1, fp );
	fwrite( &m_nDiffuseB,	4, 1, fp );
	fwrite( &m_nSpecularR,	4, 1, fp );
	fwrite( &m_nSpecularG,	4, 1, fp );
	fwrite( &m_nSpecularB,	4, 1, fp );
	fwrite( &m_nAmbientR,	4, 1, fp );
	fwrite( &m_nAmbientG,	4, 1, fp );
	fwrite( &m_nAmbientB,	4, 1, fp );

	XiahGameEngine::Load( m_Range, fp );
}

void CEffectLight::Load(FILE *fp, LPDIRECT3DDEVICE9 pDevice)
{
	DWORD time;
	int x, y, z, dR, dG, dB, sR, sG, sB, aR, aG, aB;

	fread( &time,	4, 1, fp );
	fread( &x,		4, 1, fp );
	fread( &y,		4, 1, fp );
	fread( &z,		4, 1, fp );
	fread( &dR,		4, 1, fp );
	fread( &dG,		4, 1, fp );
	fread( &dB,		4, 1, fp );
	fread( &sR,		4, 1, fp );
	fread( &sG,		4, 1, fp );
	fread( &sB,		4, 1, fp );
	fread( &aR,		4, 1, fp );
	fread( &aG,		4, 1, fp );
	fread( &aB,		4, 1, fp );

	XiahGameEngine::Load( m_Range, fp );

	m_LifeTime = time;
	Init(x, y, z, dR, dG, dB, sR, sG, sB, aR, aG, aB );
	SetDevice( pDevice );
}

void CEffectLight::GetSettingData(int &x, int &y, int &z, int &dR, int &dG, int &dB, int &sR, int &sG, int &sB, int &aR, int &aG, int &aB)
{
	x = m_nPosX;
	y = m_nPosY;
	z = m_nPosZ;
	dR = m_nDiffuseR;
	dG = m_nDiffuseG;
	dB = m_nDiffuseB;
	sR = m_nSpecularR;
	sG = m_nSpecularG;
	sB = m_nSpecularB;
	aR = m_nAmbientR;
	aG = m_nAmbientG;
	aB = m_nAmbientB;
}

void CEffectLight::Setting(int x, int y, int z, int dR, int dG, int dB, int sR, int sG, int sB, int aR, int aG, int aB)
{
	m_nPosX = x;
	m_nPosY = y;
	m_nPosZ = z;
	m_nDiffuseR = dR;
	m_nDiffuseG = dG;
	m_nDiffuseB = dB;
	m_nSpecularR = sR;
	m_nSpecularG = sG;
	m_nSpecularB = sB;
	m_nAmbientR = aR;
	m_nAmbientG = aG;
	m_nAmbientB = aB;

	ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
	m_Light.Type = D3DLIGHT_POINT; // D3DLIGHT_DIRECTIONAL
	m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 0.7f;
	m_Light.Position.x = m_nPosX;
	m_Light.Position.y = m_nPosY;
	m_Light.Position.z = m_nPosZ;

	m_Light.Direction.x = 1.0f;
	m_Light.Direction.y = 1.0f;
	m_Light.Direction.z = 1.0f;

	m_Light.Diffuse.r = m_nDiffuseR / 255.0f;
	m_Light.Diffuse.g = m_nDiffuseG / 255.0f;
	m_Light.Diffuse.b = m_nDiffuseB / 255.0f;
	m_Light.Specular.r = m_nSpecularR / 255.0f;
	m_Light.Specular.g = m_nSpecularG / 255.0f;
	m_Light.Specular.b = m_nSpecularB / 255.0f;
	m_Light.Ambient.r = m_nAmbientR / 255.0f;
	m_Light.Ambient.g = m_nAmbientG / 255.0f;
	m_Light.Ambient.b = m_nAmbientB / 255.0f;

	m_Light.Range = 100.0f;	//FLT_MAX;
}

void CEffectLight::Update(DWORD dwTime)
{
	m_ElapsedTime += dwTime;
	m_Light.Range = GetEffectData( m_Range, m_ElapsedTime );

	VECTOR vPos( m_nPosX, m_nPosY, m_nPosZ );
	VECTOR vV = vPos * (*m_pMatrix);

	m_Light.Position.x = vV.x;
	m_Light.Position.y = vV.y;
	m_Light.Position.z = vV.z;

	if( m_ElapsedTime >= m_LifeTime )
		m_pd3dDevice->LightEnable( m_nLightID, FALSE );
}

void CEffectLight::Start(bool bStart)
{
	m_ElapsedTime = 0;
	if( bStart )
		m_bPlay = true;
	else
	{
		m_bPlay = false;
		m_bIsSetMatrix = false;
		m_pd3dDevice->LightEnable( m_nLightID, FALSE );
	}
}


void CEffectLight::SetDevice(LPDIRECT3DDEVICE9 pDevice)
{
	m_pd3dDevice = pDevice;
}

void CEffectLight::SetMatrix(MATRIX* pMatrix)
{
	m_bIsSetMatrix = true;
	m_pMatrix = pMatrix;
}


};// namespace
