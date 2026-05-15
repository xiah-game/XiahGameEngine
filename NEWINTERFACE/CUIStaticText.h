/****************************************************************************************************
	파 일 명:   CUIStaticText.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUISTATICTEXT_401D2C280203_INCLUDED
#define _INC_CUISTATICTEXT_401D2C280203_INCLUDED

#include "CUIControl.h"

using namespace std;

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 * 정적 텍스트
	 * \date 2004-02-27
	 */
	class CUIStaticText 
		: public CUIControl
	{
	public:
		CUIStaticText(CUIBasisDialogMediator* pMeditatorRef, int nID,int nParentID);
		virtual ~CUIStaticText();

		virtual void ReCreate();

		virtual void Create(sCtrlData& data);
		virtual void Draw();

		virtual void MouseCheck(sMouseEvent& mouse);

		virtual void DataChange(sChangeData& data);

		virtual void Destroy();

	private:
		bool m_bDummyType;
		int m_nTextureType;
		LPDIRECT3DTEXTURE9 m_pOutTexture;
	};
};

#endif /* _INC_CUISTATICTEXT_401D2C280203_INCLUDED */
