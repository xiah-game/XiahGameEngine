/****************************************************************************************************
	파 일 명:   CUIBasisDialog.h
	만든날자:	2004/02/27  13:51
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIBASISDIALOG_401D2A4C01A5_INCLUDED
#define _INC_CUIBASISDIALOG_401D2A4C01A5_INCLUDED

#include <map>
#include <vector>

#include "CUIBasisDialogMediator.h"

#include "CUICheckBox.h"
#include "CUITabCtrl.h"
#include "CUIEditCtrl.h"
#include "CUIProgressCtrl.h"
#include "CUIComboBox.h"
#include "CUISpinCtrl.h"
#include "CUISliderCtrl.h"
#include "CUITreeCtrl.h"
#include "CUIToolTip.h"
#include "CUIFrame.h"

#include "CUIStaticList.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 *
	 * \date 2004-02-27
	 */
	class CUIBasisDialog 
		: public CUIBasisDialogMediator
	{
	public:
		CUIBasisDialog();
		virtual ~CUIBasisDialog();

		virtual void ReCreate();
		virtual void Destroy(void);

		bool Create(sFrameData & pFrameData);
		bool AddControl(sCtrlData & pCtrlData);

		virtual void ControlChanged(CUIControl* pControl, int nID = -1);
		virtual void Draw();
		virtual void Show(const int nSubID = -1);
		virtual void Hide(const int nSubID = -1);

		virtual void UpDate(sMouseEvent& mouse, int nBefore = 0);

		// 데이터 갱신
		virtual void DataChange(sChangeData& data);

		virtual void GetData(sGetData& data);

	protected:
		//컨트롤 맵
		std::map<int, CUIControl*> m_mCtrlList;

		//사용자가 제어 불가능한 컨트롤
		std::vector<CUIControl*> m_vStaticControl;

		//사용자가 제어 가능한 컨트롤
		std::vector<CUIControl*> m_vUseControl;

		CUIControl *m_pCallCtrl;	// 호출된 컨트롤
		int m_nCallCtrlID;			// 호출된 컨트롤 ID

		int m_nControlCount;	// 컨트롤 수
		int m_nBeforeEvent;		// 마지막 이벤트 발생 컨트롤

		int m_nFrameID;			// 프레임 ID

		CUIFrame* m_pFrame;		// 프레임

		inline bool CtrlCall(const int nCtrlID);
	};

	/**
	* 컨트롤 호출
	* \param nCtrlID 컨트롤 ID
	*/
	inline bool CUIBasisDialog::CtrlCall(const int nCtrlID)
	{
		if(m_nCallCtrlID != nCtrlID)
		{
			std::map<int, CUIControl*>::iterator iter = m_mCtrlList.find(nCtrlID);

			if(iter == m_mCtrlList.end())
				return false;

			m_pCallCtrl	  = iter->second;
			m_nCallCtrlID = nCtrlID;
		}

		return true;
	}
};

#endif /* _INC_CUIBASISDIALOG_401D2A4C01A5_INCLUDED */
