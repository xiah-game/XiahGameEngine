/********************************************************************
	created:	2004/03/01
	created:	1:3:2004   21:09
	filename: 	c:\documents and settings\administrator\若뗤퐪 화면\tempui\tempui\cuistaticlist.cpp
	file path:	c:\documents and settings\administrator\若뗤퐪 화면\tempui\tempui
	file base:	cuistaticlist
	file ext:	cpp
	author:		
	
	purpose:	
*********************************************************************/
// Xiah UI

#include "stdafx.h"
#include "CUIStaticList.h"
#include "CUIStaticText.h"

class CUIStaticText;

using namespace std;

namespace XiahGameEngine
{
	CUIStaticList::CUIStaticList(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID) : CUIControl(pMeditatorRef, nID, nParentID),
		m_nCount(0), m_nCurrent(-1)
	{
	}

	CUIStaticList::~CUIStaticList()
	{
		//Destroy(); // ?

		std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();
		CUIControl *pCtrl = NULL;		

		for(; iter != m_vStaticList.end(); ++iter)
		{
			pCtrl = (*iter);

			pCtrl->Destroy();

			delete pCtrl, pCtrl = NULL;
		} // for(; iter != m_vStaticList.end(); ++iter, ++nListCount)
	}

	/**
	 * Create
	 * \param data 컨트롤 정보
	 */
	void CUIStaticList::Create(sCtrlData& data)
	{
		CUIControl::Create(data);	// 기본 정보

		m_nCount = data.nCount;	// 정적 리스트 수

		register std::vector<sCtrlData*>::iterator iterlist = data.vCtrlList.begin();

		for(; iterlist != data.vCtrlList.end(); ++iterlist)
		{
			sCtrlData *pCtrlTemp = *iterlist;

			CUIControl *pAddCtrl = new CUIStaticText(NULL, pCtrlTemp->nID, data.nID);

			pAddCtrl->Create(*pCtrlTemp);	// 서브 컨트롤 생성(리스트)

			m_vStaticList.push_back(pAddCtrl);
		}
	}

	void CUIStaticList::ReCreate()
	{
		std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();		

		for(; iter != m_vStaticList.end(); ++iter)
		{
			CUIControl* pCtrl = (*iter);

			pCtrl->ReCreate();
		} // for(; iter != m_vStaticList.end(); ++iter, ++nListCount)

	}

	void CUIStaticList::CreateVB()
	{
	}

	void CUIStaticList::SetVB()
	{
	}

	/**
	 * 해제
	 */
	void CUIStaticList::Destroy()
	{		
		std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();
		CUIControl *pCtrl = NULL;
		register int nListCount = 0;

		for(; iter != m_vStaticList.end(); ++iter, ++nListCount)
		{
			pCtrl = (*iter);

			pCtrl->Destroy();

			//delete pCtrl, pCtrl = NULL;
		} // for(; iter != m_vStaticList.end(); ++iter, ++nListCount)

		if(nListCount != m_nCount)
			DBG_LogFile( _T("Destroy StaticLis fail [ DlgID %d - %d ]"), m_nParentID, m_nID);

		//m_vStaticList.clear();
	}

	/**
	 * 마우스 검사
	 * \param mouse 마우스 이벤트 정보
	 */
	void CUIStaticList::MouseCheck(sMouseEvent& mouse)
	{
		CUIControl::MouseCheck(mouse);

		register std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();
		CUIControl *pCtrl = NULL;

		for(; iter != m_vStaticList.end(); ++iter)	// 리스트 검사
		{
			pCtrl = *iter;

			pCtrl->MouseCheck(mouse);

			if(mouse.nEventType == 1)
			{
				mouse.nEventFrameID = m_nParentID;
				mouse.nEventCtrlID = m_nID;
			}

			mouse.nEventType = 0;
		} // for(; iter != m_vStaticList.end(); ++iter)
	}

	/**
	 * Draw
	 */
	void CUIStaticList::Draw()
	{
		if(!m_bShow)
			return;
		else
		{
			if(m_nAlpha != 0)
			{
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			}

			register std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();

			if(m_nCurrent != -1)
			{
				for(; iter != m_vStaticList.end(); ++iter)
				{
					CUIControl *pCtrl = (*iter);

					if(pCtrl->GetID() == m_nCurrent)
						pCtrl->Draw();
				}
			}
			else
			{
				for(; iter != m_vStaticList.end(); ++iter)
				{
					(*iter)->Draw();
				}
			}
		}
	}

	/**
	* 정보 변경
	* \param data 변경 내용
	*/
	void CUIStaticList::DataChange(sChangeData& data)
	{
		switch(data.nType)
		{
		case STATICLIST_SHOW:
			{
				/*
				register std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();
				CUIControl *pCtrl = NULL;

				for(; iter != m_vStaticList.end(); ++iter)
				{
					pCtrl = *iter;

					if(pCtrl->GetID() == data.nValue1)
						pCtrl->Show();
					else
						pCtrl->Hide();
				}
				*/
				m_nCurrent = data.nValue1;
			}
			break;
		case 51:
			{
				SetMovePos(data.nValue1, data.nValue2);

				register std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();
				CUIControl *pCtrl = NULL;

				for(; iter != m_vStaticList.end(); ++iter)
				{
					pCtrl = *iter;

					pCtrl->SetMovePos(data.nValue1, data.nValue2);
				}
			}
			break;
		case CURRENT_INDEX: // Current
			{
				m_nCurrent = data.nValue1;
			}
			break;
		case TEXTURE: // Set Texture
			{
				register std::vector<CUIControl*>::iterator iter = m_vStaticList.begin();
				CUIControl *pCtrl = NULL;

				for(; iter != m_vStaticList.end(); ++iter)
				{
					pCtrl = *iter;

					pCtrl->DataChange(data);
				}
			}
			break;
		default:
			CUIControl::DataChange(data);
			break;
		}
	}

	/**
	 * Get Data
	 * \param data 가져올 정보
	 */
	void CUIStaticList::GetData(sGetData& data)
	{
		switch(data.nType)
		{
		case GET_CURRENT_INDEX: // Current Index
			data.nValue1 = m_nCurrent;
			break;
		default:
			CUIControl::GetData(data);
			break;
		}
	}
};

