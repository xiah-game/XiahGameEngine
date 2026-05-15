#pragma once

#include "../text2d.h"

namespace XiahGameEngine
{
	#define MAXIMUM_LINE_COUNT 30

	#define VERTICAL_DISTANCE 20

	/**
	 * \ingroup XiahGameEngine
	 * 팝업 박스
	 *
	 * \date 2004-10-19
	 * 
	 * \todo 
	 * 리펙토링이 꼭 필요하다
	 * \bug 
	 * dll에서 미상속 클래스를 virtual로 정의해서 동적 할당 후 해제에 주의하라.
	 */
	class CIPopUp
	{
	public:
		XIAHGE_API CIPopUp();
		XIAHGE_API ~CIPopUp();		// virtual로 삭제시 문제됨

		XIAHGE_API void Draw();
		XIAHGE_API void SetToolTip(BYTE byType, sRect* rtRect, BYTE byCount, ...);
		XIAHGE_API void AddToolTip(LPCTSTR szTip, D3DCOLOR color = D3DCOLOR_XRGB( 255, 255, 255));
		XIAHGE_API void AddToolTip(LPCTSTR szTip, BYTE byFontSize, D3DCOLOR color = D3DCOLOR_XRGB( 255, 255, 255));

		XIAHGE_API void SetGapLine(BYTE byGapLine);
		XIAHGE_API void SetFrameColor(D3DCOLOR FrameColor);

	private:

		void SetVB(D3DCOLOR FrameColor);

	private:	

		sRect					m_rtFrameRect;
		BYTE					m_byLineCount;
		CText2D					m_Text2D[ MAXIMUM_LINE_COUNT];
		sRect					m_rtTextRect[ MAXIMUM_LINE_COUNT];
		LPDIRECT3DVERTEXBUFFER9	m_pVB;
		VT_TLVertex				m_Vertex[4];

		// 툴팁에 사용되는 변수, 대부분 툴팁은 이름이 크게 나오고 약간의 간격을 둬서 하부 항목들이
		// 나오는데, 기연 아이템의 경우 이름 밑에 기연 아이템이라고 나오고 약간의 간격을 둬서 하부
		// 항목들이 나와야 한다. 그래서 좀더 확장성 있게, 약간의 간격을 둘 라인 넘버를 가지게 한다.
		BYTE			m_byGapLine;		// 대부분 2로 설정된다.

		D3DCOLOR m_FrameColor;
	};
};