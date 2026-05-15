#pragma once

// SUPPORT FMOD SOUND LIBRARY
#include "fmod.h"
#include "fmod_errors.h"

namespace XiahGameEngine
{
	#define CUF_View		0x1
	#define CUF_Projection  0x2
	
	class XIAHGE_API CCamera : public Frustum
	{
	public:
		CCamera();
		virtual ~CCamera();

		void SetView(Vector3 vFrom,Vector3 vAt,Vector3 vUp);
		void SetProjection(float fFov,float fAspect,float fNear,float fFar);

		virtual int UpdateCamera(unsigned long update_field);
		virtual BOOL IsUpdate(){ return m_bUpdate;}

		Vector3 WorldToScreen(Vector3 vPos);
		Vector3 ScreenToWorld(Vector3 vPos);

		Vector3 GetCursorWorld(float fDepth);

		BOOL	GetSoundEffectVolume(Vector3 vPos,LONG *pVolume,LONG *pPan); 

		// FMOD
		BOOL	RegSoundEffect(Vector3 vPos,long pPan,FSOUND_SAMPLE *ds);

	public:
		Matrix4x4 m_matView;
		Matrix4x4 m_matProjection;
		
		Vector3	  m_vFrom;
		Vector3   m_vAt;
		Vector3   m_vUp;

		float	  m_fNear;
		float	  m_fFar;
		float	  m_fFov;
		float	  m_fAspect;

		BOOL	  m_bUpdate;
	};

};