/****************************************************************************************************
	파 일 명:   CUIBasisDialog.cpp
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#include "stdafx.h"
#include "CUIBasisDialog.h"

using namespace std;

namespace XiahGameEngine
{
	extern CUIEditCtrl	*g_pActiveEditCtrl;

	CUIBasisDialog::CUIBasisDialog() : m_nControlCount(0), m_nBeforeEvent(-1), m_nFrameID(0), m_pFrame(NULL), m_pCallCtrl(NULL), m_nCallCtrlID(-1)
	{
	}

	CUIBasisDialog::~CUIBasisDialog()
	{
		delete m_pFrame, m_pFrame = NULL;

		//Destroy();

		std::map<int, CUIControl*>::iterator iter = m_mCtrlList.begin();

		for(; iter != m_mCtrlList.end(); ++iter)
		{
			CUIControl* pCtrl = iter->second;

			delete pCtrl;
			pCtrl = NULL;
		}
	}

	/**
	*
	* \param pFrameData 
	* \return 
	*/
	bool CUIBasisDialog::Create(sFrameData & pFrameData)
	{
		m_nFrameID = pFrameData.nID;

		sCtrlData tempData;

		tempData.nID		= pFrameData.nID;
		tempData.nWidth		= pFrameData.nWidth;
		tempData.nHeight	= pFrameData.nHeight;
		tempData.nResID		= pFrameData.nResID;
		tempData.nAlpha		= pFrameData.nAlpha;
		tempData.nType		= FRAME;

		m_pFrame = new CUIFrame(this, tempData.nID, -1);

		m_pFrame->Create(tempData);

		return true;
	}

	/**
	*
	* \param pCtrlData 
	* \return 
	*/
	bool CUIBasisDialog::AddControl(sCtrlData & pCtrlData)
	{
		CUIControl *pAddCtrl = NULL;
		bool bUseCtrl = false;

		switch(pCtrlData.nType)
		{
		case PROGRESS:
			pAddCtrl = new CUIProgressCtrl(this, pCtrlData.nID, m_nFrameID);
			break;
		case BUTTON:
			bUseCtrl = true;
			pAddCtrl = new CUIButton(this, pCtrlData.nID, m_nFrameID);
			break;
		case COMBOBOX:
			bUseCtrl = true;
			pAddCtrl = new CUIComboBox(this, pCtrlData.nID, m_nFrameID);
			break;
		case EDITBOX:
			bUseCtrl = true;
			pAddCtrl = new CUIEditCtrl(this, pCtrlData.nID, m_nFrameID);
			break;
		case STATICLIST:  // 즐스럽게 사용된 리스트 박스 _ 임시 정적리스트에 연결하자. suck
			pAddCtrl = new CUIStaticList(this, pCtrlData.nID, m_nFrameID);
			bUseCtrl = true; // temp	툴도 미지원이고 이전 작업자가 여기저기서 정적 컨트롤를 이용해서 어쩔수 없다.
			break;
		case SCROLLBAR:
			bUseCtrl = true;
			pAddCtrl = new CUIScrollBar(this, pCtrlData.nID, m_nFrameID);
			break;
		case STATIC:
			bUseCtrl = true; // temp	툴도 미지원이고 이전 작업자가 여기저기서 정적 컨트롤를 이용해서 어쩔수 없다.
			pAddCtrl = new CUIStaticText(this, pCtrlData.nID, m_nFrameID);
			break;
		case STATICDUMMY:
			bUseCtrl = true; // temp	툴도 미지원이고 이전 작업자가 여기저기서 정적 컨트롤를 이용해서 어쩔수 없다.
			pAddCtrl = new CUIStaticText(this, pCtrlData.nID, m_nFrameID);
			break;
		default:
			DBG_LogFile( _T("CUIBasisDialog AddControl fail - [ CtrlID %d type %d]"), pCtrlData.nID, pCtrlData.nType);
			break;
		} // switch(pCtrlData.nType)

		// 컨트롤 데이터 생성
		pAddCtrl->Create(pCtrlData);

		m_mCtrlList.insert(std::map<int, CUIControl*>::value_type(pCtrlData.nID, pAddCtrl));

		// 사용자 제어 컨트롤 / 미제어 컨트롤 별도로 리스트 관리
		if(bUseCtrl)
			m_vUseControl.push_back(pAddCtrl);
		else
			m_vStaticControl.push_back(pAddCtrl);

		++m_nControlCount;

		return true;
	}

	/**
	* 컨트롤 변경
	* \param pControl 
	* \param nID 
	*/
	void CUIBasisDialog::ControlChanged(CUIControl* pControl, int nID)
	{
		if(nID != -1)
		{
			m_nBeforeEvent = nID;
		}
		else
		{
			m_nBeforeEvent = pControl->GetID();
		}
	}

	/**
	* Draw
	*/
	void CUIBasisDialog::Draw()
	{
		if(m_pFrame->IsShow())
		{
			// 프레임 출력
			m_pFrame->Draw();

			// 하위 컨트롤 출력
			std::map<int, CUIControl*>::iterator iter;
			CUIControl *pCtrl = NULL;

			for(iter = m_mCtrlList.begin(); iter != m_mCtrlList.end(); ++iter)
			{
				pCtrl = iter->second;

				//g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
				//g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);

				g_pDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE);
				g_pDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);

				g_pDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

				if(pCtrl)
					pCtrl->Draw();
			}
		}
	}

	/**
	* Set Show
	* \param nSubID 컨트롤 ID, 미입력시 전체
	*/
	void CUIBasisDialog::Show(const int nSubID)
	{
		if(nSubID == -1)	// 전체
		{
			m_pFrame->Show();
		}
		else	// 해당 컨트롤만
		{
			if(CtrlCall(nSubID))
			{
				if(m_pCallCtrl)
					m_pCallCtrl->Show();
			}			
		}
	}

	/**
	* Set Hide
	* \param nSubID 컨트롤 ID, 미입력시 전체
	*/
	void CUIBasisDialog::Hide(const int nSubID)
	{
		if(nSubID == -1)	// 전체
		{
			m_pFrame->Hide();
		}
		else	// 해당 컨트롤만
		{
			if(CtrlCall(nSubID))
			{
				if(m_pCallCtrl)
					m_pCallCtrl->Hide();
			}			
		}
	}

	/**
	 * 갱신
	 * \param mouse 마우스 이벤트 정보
	 * \param nBefore 갱신 유형
	 */
	void CUIBasisDialog::UpDate(sMouseEvent& mouse, int nBefore)
	{
		m_pFrame->MouseCheck(mouse);

		if(mouse.nEventType == 1 || nBefore) // Dialog IN
		{
			mouse.nEventType = 0;

			if(!g_nCurrentFrameID && !g_pActiveEditCtrl)  // [3/16/2004] 이전 xiah때문에 -_- 바꾸자 바꾸자.
				g_nCurrentFrameID = m_nFrameID;

			std::vector<CUIControl*>::iterator iter = m_vUseControl.begin();
			CUIControl *pCtrl = NULL;

			// TODO: 사용자 컨트롤만 갱신
			if(nBefore)
			{
				if(nBefore == 2)
					mouse.ptMousePos.x = -99;

				for(; iter != m_vUseControl.end(); ++iter)
				{
					pCtrl = (*iter);
					pCtrl->MouseCheck(mouse);

					mouse.nEventType = 0;
				} // for(; iter != m_vUseControl.end(); ++iter)
			}
			else
			{
				int nTemp = 1;

				for(; iter != m_vUseControl.end(); ++iter)
				{
					pCtrl = (*iter);
					pCtrl->MouseCheck(mouse);

					if(mouse.nEventType != 0 && mouse.nEventType != 1)
						nTemp = mouse.nEventType;

					//if(mouse.nEventType != 0)  // 
					{
						mouse.nEventType = 0;
						//return;
					} // if(mouse.nEventType != 0 )
				} // for(; iter != m_vUseControl.end(); ++iter)
				//mouse.nEventType = 1;
				mouse.nEventType = nTemp;
			}			
		}
		else
		{
			if(!g_nCurrentFrameID && !g_pActiveEditCtrl)
				g_nCurrentFrameID = 0;
		}
		//else
		//	g_nCurrentFrameID = 0;
	}


	void CUIBasisDialog::ReCreate()
	{
		std::map<int, CUIControl*>::iterator iter = m_mCtrlList.begin();

		for(; iter != m_mCtrlList.end(); ++iter)
		{
			CUIControl* pCtrl = iter->second;

			pCtrl->ReCreate();
		} // for(; iter != m_mCtrlList.end(); ++iter, ++nDestroyCount)
	}

	/**
	* 해제
	*/
	void CUIBasisDialog::Destroy(void)
	{
		std::map<int, CUIControl*>::iterator iter = m_mCtrlList.begin();
		CUIControl *pCtrl = NULL;
		int nDestroyCount = 0;

		for(; iter != m_mCtrlList.end(); ++iter, ++nDestroyCount)
		{
			pCtrl = iter->second;

			pCtrl->Destroy();

			//delete pCtrl;
			//pCtrl = NULL;
		} // for(; iter != m_mCtrlList.end(); ++iter, ++nDestroyCount)

		if(nDestroyCount != m_nControlCount)
			DBG_LogFile(_T("Destroy fail [ DlgID %d ]"), m_nFrameID);

		// list clear
		//m_mCtrlList.clear();
		//m_vUseControl.clear();
		//m_vStaticControl.clear();
	}

	/**
	* 정보 변경
	* \param data 변경 정보
	*/
	void CUIBasisDialog::DataChange(sChangeData& data)
	{
		if(data.nSubID != -1)
		{// 일반 메시지
			if(CtrlCall(data.nSubID))
			{
				if(m_pCallCtrl)
					m_pCallCtrl->DataChange(data);
				else
					DBG_LogFile( _T("CUIBasisDialog DataChange fail - [ SubID %d ]"), data.nSubID);
			}			
		}
		else
		{// 위치 이동
			m_pFrame->DataChange(data); // 프레임 이동후 타입은 51로 변경 위치값은 변경된 수치

			std::map<int, CUIControl*>::iterator iter = m_mCtrlList.begin();

			for(; iter != m_mCtrlList.end(); ++iter)
			{
				CUIControl *pCtrl = iter->second;

				if(pCtrl)
					pCtrl->DataChange(data);
			}
		}
	}

	/**
	 *
	 * \param data 
	 */
	void CUIBasisDialog::GetData(sGetData& data)
	{
		if(data.nSubID != -1)
		{
			if(CtrlCall(data.nSubID))
			{
				if(m_pCallCtrl)
					m_pCallCtrl->GetData(data);
			}			
		}
		else
		{
			m_pFrame->GetData(data);
		}
	}

};