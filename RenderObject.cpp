#include "stdafx.h"
#include "RenderObject.h"

namespace XiahGameEngine
{
	XIAHGE_API CRenderObject::CRenderObject()
	{
		//m_fDetailLevel = 1.0f;
		m_bVisible = FALSE;
		m_bNeedUpdate = FALSE;
		SetRenderType();
	}
	
	XIAHGE_API CRenderObject::~CRenderObject()
	{
	
	}

	XIAHGE_API BOOL CRenderObject::Render()
	{
		return TRUE;
	}

	XIAHGE_API BOOL CRenderObject::PrepareRender()
	{
		return TRUE;
	}

	/*
	XIAHGE_API BOOL CRenderObject::SetDetailLevel(float fDetailLevel)
	{
		m_fDetailLevel = fDetailLevel;
		return TRUE;
	}
	*/

};