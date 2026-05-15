/****************************************************************************************************
	파 일 명:   CUIFileLoad.h
	만든날자:	2004/02/27  13:52
    코 딩 자:	
	설    명:   
****************************************************************************************************/
// Xiah New UI Engine

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUIFILELOAD_40244F700263_INCLUDED
#define _INC_CUIFILELOAD_40244F700263_INCLUDED

#include <map>

using namespace std;

#define CTRL_BAR			0
#define CTRL_BUTTON			1
#define CTRL_COMBOBOX		2
#define CTRL_EDITBOX		3
#define CTRL_LISTBOX		4
#define CTRL_SCROLLBAR		5
#define CTRL_STATIC			6
#define CTRL_TOOLTIP		7
#define CTRL_FRAME			8
#define CTRL_STATICDUMMY	9

namespace XiahGameEngine
{
	/**
	 * \ingroup XiahGameEngine
	 * 인터페이스 파일 로딩
	 * \date 2004-02-27
	 */
	class CUIFileLoad 
	{
	public:
		enum Type
		{
			PROGRESS = 0,
			BUTTON,
			COMBOBOX,
			EDITBOX,
			STATICLIST,// LISTBOX,
			SCROLLBAR,
			STATIC,
			TOOLTIP,
			FRAME,
			STATICDUMMY
		};

		CUIFileLoad();
		virtual ~CUIFileLoad();

		bool CtrlLoad(const TCHAR* strCmrFileName);
		int FrameLoad(const TCHAR* strFmrFileName);

		void FrameRead(sFrameData &data);
		void CtrlRead(sCtrlData &data);

		void FrameEnd();
		void CtrlRelease();

	private:
		FILE* m_pFile;
		std::map<int, sCtrlData*> m_mCtrlList;

		inline void DefaultRead(sCtrlData *pData, char **buffer, bool bS = false);
	};
};

#endif /* _INC_CUIFILELOAD_40244F700263_INCLUDED */
