#pragma once

namespace XiahGameEngine
{
	namespace Light
	{
		class CLight
		{
		public:
			XIAHGE_API CLight();
			XIAHGE_API ~CLight();

		public:
			int		  m_nIndex;
			BOOL	  m_bEnable;
			D3DLIGHT9 m_Light;
		};

		typedef std::vector<CLight *> LIGHTLIST;

		class CLightManager : public CRenderObject
		{
			DECLARE_RENDERTYPE(eRT_Light)
		public:
			XIAHGE_API CLightManager();
			XIAHGE_API ~CLightManager();

			XIAHGE_API BOOL Release();

			XIAHGE_API CLight* CreateLight();
			XIAHGE_API CLight* GetGlobalLight();

			XIAHGE_API BOOL PrepareRender();
			XIAHGE_API BOOL Render();

		protected:
			LIGHTLIST		m_LightList;
			LIGHTLIST		m_VisibleLightList;
		};

		XIAHGE_API extern CLightManager g_LightManager;
	};
};