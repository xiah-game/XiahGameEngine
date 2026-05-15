#include "stdafx.h"
#include "Camera.h"

#include "XiahInput.h"
#include "XiahBGM.h"	// for char FX

// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"


namespace XiahGameEngine
{
	//---------------------------------------------------------------------------------------
	CCamera::CCamera()
	{
	}

	//---------------------------------------------------------------------------------------
	CCamera::~CCamera()
	{
	}

	//---------------------------------------------------------------------------------------
	void CCamera::SetView(Vector3 vFrom,Vector3 vAt,Vector3 vUp)
	{
		m_vFrom = vFrom;
		m_vAt   = vAt;
		m_vUp	= vUp;

		UpdateCamera(CUF_View);
	}

	//---------------------------------------------------------------------------------------
	void CCamera::SetProjection(float fFov,float fAspect,float fNear,float fFar)
	{
		m_fFov = fFov;
		m_fAspect = fAspect;
		m_fNear = fNear;
		m_fFar  = fFar;

		UpdateCamera(CUF_Projection);
	}

	//---------------------------------------------------------------------------------------
	int CCamera::UpdateCamera(unsigned long update_field)
	{
		if( update_field & CUF_View)
		{
			m_matView.SetViewMatrix( m_vFrom, m_vAt, m_vUp);
		
			m_bUpdate = TRUE;
		}

		if( update_field & CUF_Projection)
		{
			m_matProjection.SetProjectionMatrix( m_fFov, m_fAspect, m_fNear, m_fFar);

			m_bUpdate = TRUE;
		}

		Matrix4x4 matTrans = m_matView * m_matProjection;
//		matTrans = matTrans.GetInverse();
		Frustum::Set( matTrans);

		return 0;
	}

	Vector3 CCamera::WorldToScreen(Vector3 vPos)
	{
		return XiahGameEngine::WorldToScreen( vPos, g_EngineInfo.m_nWidth, g_EngineInfo.m_nHeight, m_fNear, m_fFar, m_matProjection, m_matView);
	}

	//HT_CHEAT : 윈도우창모드
	// 픽킹시 보정
	Vector3 CCamera::ScreenToWorld(Vector3 vPos)
	{
		float windowWidth = WindowRect.right - WindowRect.left;
		float windowHeight = WindowRect.bottom - WindowRect.top;

		//return XiahGameEngine::ScreenToWorld( vPos, g_EngineInfo.m_nWidth, g_EngineInfo.m_nHeight, m_fNear, m_fFar, m_matProjection, m_matView);
		return XiahGameEngine::ScreenToWorld( vPos, windowWidth, windowHeight, m_fNear, m_fFar, m_matProjection, m_matView);
	}

	Vector3 CCamera::GetCursorWorld(float fDepth)
	{
		return ScreenToWorld( Vector3( XiahInput::g_ptMouse.x, XiahInput::g_ptMouse.y, fDepth));
	}

	// object들의 FX를 위하여 등록한다.
	BOOL CCamera::RegSoundEffect(Vector3 vPos,long pan,FSOUND_SAMPLE *ds)
	{
		if(XiahFX::Add_FX(vPos,ds,pan) == FALSE) return FALSE;
		return TRUE;
	}

	// 이것은 PAN을 위하여만 사용한다
	BOOL CCamera::GetSoundEffectVolume(Vector3 vPos,LONG *pVolume,LONG *pPan)
	{
		Vector3 scPos = WorldToScreen( vPos);
		if( scPos.x < 0)
			scPos.x = 0;

		if( scPos.x > 1024)
			scPos.x = 1023;
		
		*pPan = (scPos.x / 4);

		return TRUE;
	}
};