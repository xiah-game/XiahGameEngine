#include "stdafx.h"
#include "RenderObject.h"
#include "Light.h"

namespace XiahGameEngine
{
	namespace Light
	{
		XIAHGE_API CLight::CLight()
		{
		}
		
		XIAHGE_API CLight::~CLight()
		{
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
		XIAHGE_API CLightManager g_LightManager;

		XIAHGE_API CLightManager::CLightManager()
		{
			CLight *pLight = CreateLight();

			pLight->m_bEnable = TRUE;
			
			pLight->m_Light.Type = D3DLIGHT_DIRECTIONAL;
			pLight->m_Light.Diffuse.r = 1;
			pLight->m_Light.Diffuse.g = 1;
			pLight->m_Light.Diffuse.b = 1;
			pLight->m_Light.Diffuse.a = 1;
			pLight->m_Light.Ambient.r = 0;
			pLight->m_Light.Ambient.g = 0;
			pLight->m_Light.Ambient.b = 0;
			pLight->m_Light.Ambient.a = 0;
			
			Vector3 vDir( 1, -1, 1);
			vDir.Normalize();
			
			pLight->m_Light.Direction.x = vDir.x;
			pLight->m_Light.Direction.y = vDir.y;
			pLight->m_Light.Direction.z = vDir.z;

			//pLight->m_Light.Range = SQRT( FLT_MAX);
			pLight->m_Light.Range = nSQRT( FLT_MAX);
		}

		XIAHGE_API CLightManager::~CLightManager()
		{
			Release();
		}

		XIAHGE_API BOOL CLightManager::Release()
		{
			LIGHTLIST::iterator it;

			for(it = m_LightList.begin(); it != m_LightList.end(); it++)
			{
				CLight* pLight = *it;
				delete pLight;
				pLight = NULL;
			}
			
			m_LightList.clear();
			m_VisibleLightList.clear();
		
			return TRUE;
		}

		XIAHGE_API CLight* CLightManager::CreateLight()
		{
			CLight *pLight = new CLight;

			pLight->m_nIndex = m_LightList.size();
			pLight->m_bEnable = FALSE;
			ZeroMemory( &pLight->m_Light, sizeof( D3DLIGHT9));

			m_LightList.push_back( pLight);

			return pLight;
		}
		
		XIAHGE_API CLight* CLightManager::GetGlobalLight()
		{
			return m_LightList[ 0];
		}

		XIAHGE_API BOOL CLightManager::PrepareRender()
		{
			m_VisibleLightList.clear();

			if( g_pCurrentCamera == NULL)
				return TRUE;

			LIGHTLIST::iterator it;

			for(it = m_LightList.begin(); it != m_LightList.end(); it++)
			{
				CLight *pLight = *it;

				g_pDirect3DDevice->LightEnable( pLight->m_nIndex, FALSE);

				if( pLight->m_bEnable == FALSE)
					continue;


				if( pLight->m_Light.Type == D3DLIGHT_POINT)
				{
					Vector3 vPos;

					vPos.x = pLight->m_Light.Position.x;
					vPos.y = pLight->m_Light.Position.y;
					vPos.z = pLight->m_Light.Position.z;

					if( g_pCurrentCamera->Visible( vPos))
					{
						m_VisibleLightList.push_back( pLight);
					}
				}
				else if( pLight->m_Light.Type == D3DLIGHT_DIRECTIONAL)
				{
					m_VisibleLightList.push_back( pLight);
				}
			}

			return TRUE;
		}
		
		XIAHGE_API BOOL CLightManager::Render()
		{
			LIGHTLIST::iterator it;

			for(it = m_VisibleLightList.begin(); it != m_VisibleLightList.end(); it++)
			{
				CLight *pLight = *it;

				g_pDirect3DDevice->SetLight( pLight->m_nIndex, &pLight->m_Light);
				g_pDirect3DDevice->LightEnable( pLight->m_nIndex, TRUE);
			}

			return TRUE;
		}
	};

};