/********************************************************************
	created:	2004/03/01
	created:	1:3:2004   21:09
	filename: 	c:\documents and settings\administrator\å®‹ä½“ È­¸é\tempui\tempui\cuistaticlist.h
	file path:	c:\documents and settings\administrator\å®‹ä½“ È­¸é\tempui\tempui
	file base:	cuistaticlist
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
// Xiah UI

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUISTATICLIST_404326C500AB_INCLUDED
#define _INC_CUISTATICLIST_404326C500AB_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 * Á¤Àû ¸®½ºÆ®
	 * \date 2004-02-27
	 */
	class CUIStaticList 
		: public CUIControl
	{
	public:
		CUIStaticList(CUIBasisDialogMediator* pMeditatorRef, int nID, int nParentID);
		virtual ~CUIStaticList();

		virtual void ReCreate();

		virtual void Create(sCtrlData& data);
		virtual void MouseCheck(sMouseEvent& mouse);
		virtual void Draw();

		virtual void DataChange(sChangeData& data);
		virtual void GetData(sGetData& data);

		virtual void Destroy();
	protected:
		int m_nCount;
		int m_nCurrent;

		std::vector<CUIControl*> m_vStaticList;

		virtual void CreateVB(); // VB & Tex
		virtual void SetVB();
	};
};

#endif /* _INC_CUISTATICLIST_404326C500AB_INCLUDED */
