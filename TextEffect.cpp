#include "stdafx.h"
#include "D3DDevice.h"
#include "TextEffect.h"


namespace XiahGameEngine
{
	//XIAHGE_API CTextEffect g_TextEffect;

	//--------------------------------------------------------
	XIAHGE_API CTextEffect::CTextEffect()
	{
		m_List.clear();
		m_pFont = NULL;
		m_dwLifeTime = 1000;
	}

	XIAHGE_API CTextEffect::~CTextEffect()
	{
		Release();
	}

	XIAHGE_API void CTextEffect::Release()
	{
		m_pFont = NULL;

		TEXTEFFECTLIST::iterator tit;
		for(tit=m_List.begin(); tit!=m_List.end(); tit++)
		{
			sTEXTEFFECTDATA* pData = *tit;
			delete pData;
			pData = NULL;
		}
		m_List.clear();
	}

	XIAHGE_API void CTextEffect::SetFont(sFont *pFont)
	{
		m_pFont = pFont;
	}

	XIAHGE_API void CTextEffect::CreateTextEffect(sRect rcRect, LPCTSTR str, D3DCOLOR Color, BYTE BackColor, sFont* pFont, BYTE byType)
	{
		sTEXTEFFECTDATA* pData = new sTEXTEFFECTDATA;

		pData->rcRect	= rcRect;
		_tcscpy( pData->szChar, str );
		pData->Color	= Color;
		pData->BackColor = BackColor;
		pData->dwElapsedTime = 0;
		pData->fYPos	= pData->rcRect.top;
		pData->byType	= byType;

		if( pFont )
			pData->pFont = pFont;
		else
			pData->pFont = m_pFont;

		//
		pData->Text2D.SetParentRect( &pData->rcRect );
		pData->Text2D.SetText( 0, 0, pData->szChar, pData->pFont, pData->Color, pData->BackColor );

		m_List.push_back( pData );
	}

	XIAHGE_API void CTextEffect::Update(DWORD dwDeltaTime)
	{
		TEXTEFFECTLIST DeleteList;

		float fTime = (float)dwDeltaTime / 1000;
		float fDecrease = fTime * 80;

		TEXTEFFECTLIST::iterator tit;
		for(tit=m_List.begin(); tit!=m_List.end(); tit++)
		{
			sTEXTEFFECTDATA* pData = *tit;
			if(pData == NULL)
			{
				DBG_LogFile( _T("CTextEffect::Update fail"));
			}

			pData->dwElapsedTime += dwDeltaTime;

			switch( pData->byType)
			{
			case 0:
				{
					// 위로 올라간다.
					pData->fYPos -= fDecrease;
					pData->rcRect.top = pData->fYPos;
					//pData->rcRect.top--;

					if( pData->dwElapsedTime >= m_dwLifeTime )
					{
						DeleteList.push_back( pData );
					}
				}
				break;
			case 1:
				{
					// 위로 올라간다.
					pData->fYPos -= fDecrease;

					if( pData->fYPos > 600)
						pData->rcRect.top = pData->fYPos;
					//pData->rcRect.top--;

					if( pData->dwElapsedTime >= 3000 )
					{
						DeleteList.push_back( pData );
					}
				}
				break;
			}
		}// for(m_List)

		TEXTEFFECTLIST::iterator it;
		for(it=DeleteList.begin(); it!=DeleteList.end(); it++)
		{
			sTEXTEFFECTDATA* pData = *it;
			if(pData == NULL)
			{
				DBG_LogFile( _T("CTextEffect::Update fail"));
			}
			m_List.remove( pData );
			delete pData;
			pData = NULL;
		}// for(DeleteList)
	}

	XIAHGE_API void CTextEffect::Render()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		TEXTEFFECTLIST::iterator tit;
		for(tit=m_List.begin(); tit!=m_List.end(); tit++)
		{
			sTEXTEFFECTDATA* pData = *tit;
			if(pData == NULL)
			{
				DBG_LogFile( _T("CTextEffect::Render fail"));
			}
			pData->Text2D.Render();
		}// for
	}

	//--------------------------------------------------------
	XIAHGE_API void InitEnergyGauge()
	{
		HRESULT hr;

		// vertex buffer
/*
		hr = g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
						D3DFVF_TLVERTEX, D3DPOOL_DEFAULT, &g_EnergyGaugeBackVB, NULL );

		hr = g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
						D3DFVF_TLVERTEX, D3DPOOL_DEFAULT, &g_EnergyGaugeFrontVB, NULL );
*/

		hr = g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex), D3DUSAGE_WRITEONLY,
						D3DFVF_TLVERTEX, D3DPOOL_MANAGED, &g_EnergyGaugeBackVB, NULL );

		hr = g_pDirect3DDevice->CreateVertexBuffer( 4*sizeof(VT_TLVertex), D3DUSAGE_WRITEONLY,
						D3DFVF_TLVERTEX, D3DPOOL_MANAGED, &g_EnergyGaugeFrontVB, NULL );

	}

	XIAHGE_API void RenderEnergyGauge(int nX, int nY, int nSize, DWORD nCur, DWORD nMax, D3DCOLOR Color, D3DCOLOR BackColor, int nHeight)
	{
		if( !g_EnergyGaugeBackVB || !g_EnergyGaugeFrontVB ) return;

		//int nHeight = 10;

		// Back
		VT_TLVertex* pBackVertex;

		// 다이내믹으로 만든것은 데이터가 AGP 메모리로 올라가는데, 쓸것을 바로 만들어서 없애는 형식을 쓴다. 
		// AGP 메모리가 한정되어 있어서 데이터를 저장하지 않고 현재 필요한 데이타를
		// Lock, Unlock 해서 만들어서 쓰고 D3DLOCK_DISCARD 해주면 쓴 다음에 지운다.
		// 만약 첨에 한번만 Lock해서 만들고 바꾸지 않고 계속 쓴다면 D3DLOCK_NOOVERWRITE해준다.

//		g_EnergyGaugeBackVB->Lock( 0, 0, (void**)&pBackVertex, D3DLOCK_DISCARD );
		g_EnergyGaugeBackVB->Lock( 0, 0, (void**)&pBackVertex, 0 );

		pBackVertex[0].pos.x = nX;
		pBackVertex[0].pos.y = nY;

		pBackVertex[1].pos.x = nX;
		pBackVertex[1].pos.y = nY - nHeight;

		pBackVertex[2].pos.x = nX + nSize;
		pBackVertex[2].pos.y = nY;

		pBackVertex[3].pos.x = nX + nSize;
		pBackVertex[3].pos.y = nY - nHeight;

		for(int i=0; i<4; i++)
		{
			pBackVertex[i].pos.w = 1;
            pBackVertex[i].diffuse = BackColor;
		}

		g_EnergyGaugeBackVB->Unlock();

		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

		g_Device.SetTexture(0, NULL);
		//g_pDirect3DDevice->SetTexture( 0, NULL );
		g_Device.SetStreamSource(g_EnergyGaugeBackVB, sizeof(VT_TLVertex) );
		g_Device.SetIndices( NULL );
		g_Device.SetFVF(D3DFVF_TLVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX );
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		// Front
		VT_TLVertex* pFrontVertex;
//		g_EnergyGaugeFrontVB->Lock( 0, 0, (void**)&pFrontVertex, D3DLOCK_DISCARD );
		g_EnergyGaugeFrontVB->Lock( 0, 0, (void**)&pFrontVertex, 0 );

		pFrontVertex[0].pos.x = nX + 1;
		pFrontVertex[0].pos.y = nY - 1;

		pFrontVertex[1].pos.x = nX + 1;
		pFrontVertex[1].pos.y = nY - nHeight + 1;

		// error check
		if( nMax == 0 ) nMax = 1;

		int nSizeX = 80;

		if(nMax > 10000000)
		{
			int index = (nCur*0.5) / (nMax*0.1);

			switch(index)
			{
			case 0:
			//	nCur = nCur - (nMax * 0.8);
			//	Color = D3DCOLOR_XRGB( 0, 204, 255);
				break;
			case 1:
				nCur = nCur - (nMax * 0.2);
				Color = D3DCOLOR_XRGB( 255, 153, 0);
				break;
			case 2:
				nCur = nCur - (nMax * 0.4);
				Color = D3DCOLOR_XRGB( 255, 255, 0);
				break;
			case 3:
				nCur = nCur - (nMax * 0.6);
				Color = D3DCOLOR_XRGB( 0, 255, 128);
				break;
			case 4:
				nCur = nCur - (nMax * 0.8);
				Color = D3DCOLOR_XRGB( 0, 204, 255);
				break;
			case 5:
				nCur = nCur - (nMax * 0.8);
				Color = D3DCOLOR_XRGB( 0, 204, 255);
				break;
			}
			
			nMax = nMax - (nMax * 0.8);
			nSizeX = ((nCur/1000) * (nSize-1)) / (nMax/1000);
		}
		else
		{
			//if(nMax < 10000000)
			nSizeX = (nCur * (nSize-1)) / nMax;
			//else 
			//	nSizeX = ((nCur/10000) * (nSize-1)) / (nMax/10000);
		}
		pFrontVertex[2].pos.x = nX + nSizeX;
		pFrontVertex[2].pos.y = nY - 1;

		pFrontVertex[3].pos.x = nX + nSizeX;
		pFrontVertex[3].pos.y = nY - nHeight + 1;

		for(i=0; i<4; i++)
		{
			pFrontVertex[i].pos.w = 1;
            pFrontVertex[i].diffuse = Color;
		}

		g_EnergyGaugeFrontVB->Unlock();

		g_Device.SetStreamSource( g_EnergyGaugeFrontVB, sizeof(VT_TLVertex) );
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE);

	}

    XIAHGE_API void ReleaseEnergyGauge()
	{
		if( g_EnergyGaugeBackVB ) g_EnergyGaugeBackVB->Release();
		if( g_EnergyGaugeFrontVB ) g_EnergyGaugeFrontVB->Release();
		g_EnergyGaugeBackVB = NULL;
		g_EnergyGaugeFrontVB = NULL;
	}

	//--------------------------------------------------------


};
