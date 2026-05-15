/****************************************************************************************************
	파 일 명:   CUIFrame.cpp
	만든날자:	2004/02/24  17:36
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIFrame.h"

using namespace std;

namespace XiahGameEngine
{
	CUIFrame::CUIFrame(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef,nID,nParentID)
	{
	}

	CUIFrame::~CUIFrame()
	{
	}


	/*
	void CUIFrame::Create(sCtrlData& data)
	{
		CUIControl::Create(data);
	}
	void CUIFrame::MouseCheck(sMouseEvent& mouse)
	{
		CUIControl::MouseCheck(mouse);
	}
	*/


	/**
	 * Draw
	 */
	void CUIFrame::Draw()
	{
		g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);

		//control에서 상속받은 자기 자신의 texture를 그린다
		g_Device.SetTexture(0, NULL);
		//g_pDirect3DDevice->SetTexture( 0, NULL);
		g_Device.SetStreamSource( m_pVB, sizeof(VT_TLVertex));
		g_Device.SetFVF(D3DFVF_TLVERTEX);
		//g_pDirect3DDevice->SetFVF( D3DFVF_TLVERTEX);
		g_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	}
};

