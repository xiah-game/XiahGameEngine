#pragma once

namespace XiahGameEngine
{

	//---------------------------------------------------------------------------------------
	enum eRenderType 
	{
		eRT_NormalSort,
		eRT_NormalNoSort,
		eRT_AlphaSort,
		eRT_AlphaNoSort,
		eRT_Light,

		eRT_Count
	};

#define DECLARE_RENDERTYPE( type) \
	private:\
	XIAHGE_API virtual void SetRenderType()\
	{\
		m_RenderType = type;\
	}

	//---------------------------------------------------------------------------------------
	class CRenderObject
	{
	private:
		XIAHGE_API virtual void SetRenderType(){};
	public:
		XIAHGE_API CRenderObject();
		XIAHGE_API virtual ~CRenderObject();

		XIAHGE_API virtual BOOL Render();
		XIAHGE_API virtual BOOL PrepareRender();

		//XIAHGE_API virtual BOOL SetDetailLevel(float fDetailLevel);	// LOD Level 0 ~ 1사이의 값이다

		XIAHGE_API virtual BOOL IsValid(){ return TRUE;}
		XIAHGE_API virtual BOOL IsVisible(){ return m_bVisible;}

	protected:
		BOOL			m_bVisible;
		eRenderType		m_RenderType;

		BOOL			m_bNeedUpdate;
		//float			m_fDetailLevel;
	};

};