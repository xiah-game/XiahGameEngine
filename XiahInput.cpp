#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION		0x0800
#endif

#include "stdafx.h"
#include "XiahInput.h"
#include "cjoystic.h"


namespace XiahGameEngine
{
	namespace XiahInput
	{
		XIAHGE_API sPoint  g_ptMouse;
		XIAHGE_API BOOL	   g_bMouseMove = FALSE;
		XIAHGE_API BOOL    g_bLButtonDown = FALSE;
		XIAHGE_API BOOL	   g_bRButtonDown = FALSE;

		XIAHGE_API BOOL	  g_bLButtonUp = FALSE;
		XIAHGE_API BOOL	  g_bRButtonUp = FALSE;

		XIAHGE_API BOOL	  g_bLButtonOn = FALSE;		//버튼을 눌려져 있으면 On(누르고 있어도 계속 TRUE)
		XIAHGE_API BOOL	  g_bRButtonOn = FALSE;

		//HT_CHEAT : 게임 패드 삭제
		XIAHGE_API  BOOL		g_Lock_Button_On= FALSE;
		XIAHGE_API  BOOL		g_Lock_Button_Up= FALSE;

		XIAHGE_API  BOOL		g_Sel_Left_Down= FALSE;
		XIAHGE_API  BOOL		g_Sel_Right_Down= FALSE;
		XIAHGE_API  BOOL		g_Sel_Left_On= FALSE;
		XIAHGE_API  BOOL		g_Sel_Right_On= FALSE;

		XIAHGE_API  BOOL		g_Zoomin_Down= FALSE;
		XIAHGE_API  BOOL		g_Zoomout_Down= FALSE;
		XIAHGE_API  BOOL		g_Pan_Left_Down= FALSE;
		XIAHGE_API  BOOL		g_Pan_Right_Down= FALSE;

		XIAHGE_API  BOOL		g_Attack_Button_On= FALSE;

		XIAHGE_API sPoint		g_Axis1;		// Control Axis1


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		// GAME PAD용 입니다.

		//XIAHGE_API DWORD		g_Pad_Mode = NONE_PAD_MODE;					// PAD 설정모드

		//XIAHGE_API BOOL			g_HP_Button_On = FALSE;
		//XIAHGE_API BOOL			g_MP_Button_On = FALSE;
		//
		//XIAHGE_API BOOL			g_Lock_Button_On = FALSE;
		//XIAHGE_API BOOL			g_Lock_Button_Up = FALSE;
		//
		//XIAHGE_API BOOL			g_Menu_Button_On = FALSE;
		//XIAHGE_API BOOL			g_Mugong_Button_On = FALSE;
		//XIAHGE_API BOOL			g_Attack_Button_On = FALSE;

		//XIAHGE_API BOOL			g_PadCamera_Button_On = FALSE;

		//XIAHGE_API BOOL			g_Zoomin_Down = FALSE;
		//XIAHGE_API BOOL			g_Zoomout_Down = FALSE;
		//XIAHGE_API BOOL			g_Pan_Left_Down = FALSE;
		//XIAHGE_API BOOL			g_Pan_Right_Down = FALSE;

		//XIAHGE_API BOOL			g_Sel_Left_Down = FALSE;
		//XIAHGE_API BOOL			g_Sel_Right_Down = FALSE;
		//XIAHGE_API BOOL			g_Sel_Left_On = FALSE;
		//XIAHGE_API BOOL			g_Sel_Right_On = FALSE;

		//// # 0
		//XIAHGE_API BOOL		g_Button0_On = FALSE;
		//XIAHGE_API BOOL		g_Button0_Down = FALSE;
		//XIAHGE_API BOOL		g_Button0_Up = FALSE;

		//// # 1
		//XIAHGE_API BOOL		g_Button1_On = FALSE;
		//XIAHGE_API BOOL		g_Button1_Down = FALSE;
		//XIAHGE_API BOOL		g_Button1_Up = FALSE;

		//// # 2
		//XIAHGE_API BOOL		g_Button2_On = FALSE;
		//XIAHGE_API BOOL		g_Button2_Down = FALSE;
		//XIAHGE_API BOOL		g_Button2_Up = FALSE;

		//// # 3
		//XIAHGE_API BOOL		g_Button3_On = FALSE;
		//XIAHGE_API BOOL		g_Button3_Down = FALSE;
		//XIAHGE_API BOOL		g_Button3_Up = FALSE;

		//// # 4
		//XIAHGE_API BOOL		g_Button4_On = FALSE;
		//XIAHGE_API BOOL		g_Button4_Down = FALSE;
		//XIAHGE_API BOOL		g_Button4_Up = FALSE;

		//// # 5
		//XIAHGE_API BOOL	  g_Button5_On = FALSE;
		//XIAHGE_API BOOL	  g_Button5_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button5_Up = FALSE;

		//// # 6
		//XIAHGE_API BOOL	  g_Button6_On = FALSE;
		//XIAHGE_API BOOL	  g_Button6_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button6_Up = FALSE;

		//// # 7
		//XIAHGE_API BOOL	  g_Button7_On = FALSE;
		//XIAHGE_API BOOL	  g_Button7_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button7_Up = FALSE;

		//// # 8
		//XIAHGE_API BOOL	  g_Button8_On = FALSE;
		//XIAHGE_API BOOL	  g_Button8_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button8_Up = FALSE;
		//
		//// # 9
		//XIAHGE_API BOOL	  g_Button9_On = FALSE;
		//XIAHGE_API BOOL	  g_Button9_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button9_Up = FALSE;

		//// # 10
		//XIAHGE_API BOOL	  g_Button10_On = FALSE;
		//XIAHGE_API BOOL	  g_Button10_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button10_Up = FALSE;

		//// # 11
		//XIAHGE_API BOOL	  g_Button11_On = FALSE;
		//XIAHGE_API BOOL	  g_Button11_Down = FALSE;
		//XIAHGE_API BOOL	  g_Button11_Up = FALSE;

		//// # Z-1축
		//XIAHGE_API BOOL	  g_ButtonZ1_On = FALSE;
		//XIAHGE_API BOOL	  g_ButtonZ1_Down = FALSE;
		//XIAHGE_API BOOL	  g_ButtonZ1_Up = FALSE;
		//// # Z-2축
		//XIAHGE_API BOOL	  g_ButtonZ2_On = FALSE;
		//XIAHGE_API BOOL	  g_ButtonZ2_Down = FALSE;
		//XIAHGE_API BOOL	  g_ButtonZ2_Up = FALSE;

		//XIAHGE_API sPoint		g_Axis1;		// Control Axis1

		//// POV 버튼
		//XIAHGE_API BOOL		g_Pov_L_On = FALSE;
		//XIAHGE_API BOOL		g_Pov_L_Down = FALSE;
		//XIAHGE_API BOOL		g_Pov_L_Up = FALSE;

		//XIAHGE_API BOOL		g_Pov_R_On = FALSE;
		//XIAHGE_API BOOL		g_Pov_R_Down = FALSE;
		//XIAHGE_API BOOL		g_Pov_R_Up = FALSE;

		////////////////////////////////////////////////////////////////////////////////////////////

		//XIAHGE_API BOOL	  UpdateInput_Pad()
		//{
		//	static BOOL		h_Menu = FALSE;
		//	static long		t_Z1 = 0;
		//	static long		t_Z2 = 0;

		//	static long		t_D1 = 0;
		//	static long		t_D2 = 0;

			// GAME PAD의 검사
		//	XiahGameEngine::g_cj->UpdateInputState();

		//	// PAD의 기본 버튼
		//	// # 0
		//	if( (g_cj->g_js.rgbButtons[0] && !g_Button0_On))
		//	{
		//		g_Button0_Down = TRUE;
		//		g_Button0_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[0] && g_Button0_On)
		//	{
		//		g_Button0_Down = FALSE;
		//		g_Button0_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button0_Down = FALSE;
		//		g_Button0_Up = FALSE;
		//	}
		//	g_Button0_On = g_cj->g_js.rgbButtons[0]?1:0;
	
		//	// # 1 Pad 무공 사용 
		//	if( (g_cj->g_js.rgbButtons[1] && !g_Button1_On))
		//	{
		//		g_Button1_Down = TRUE;
		//		g_Button1_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[1] && g_Button1_On)
		//	{
		//		g_Button1_Down = FALSE;
		//		g_Button1_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button1_Down = FALSE;
		//		g_Button1_Up = FALSE;
		//	}
		//	g_Button1_On = g_cj->g_js.rgbButtons[1]?1:0;


		//	// # 2
		//	if( (g_cj->g_js.rgbButtons[2] && !g_Button2_On))
		//	{
		//		g_Button2_Down = TRUE;
		//		g_Button2_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[2] && g_Button2_On)
		//	{
		//		g_Button2_Down = FALSE;
		//		g_Button2_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button2_Down = FALSE;
		//		g_Button2_Up = FALSE;
		//	}
		//	g_Button2_On = g_cj->g_js.rgbButtons[2]?1:0;


		//	// # 3
		//	if( (g_cj->g_js.rgbButtons[3] && !g_Button3_On))
		//	{
		//		g_Button3_Down = TRUE;
		//		g_Button3_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[3] && g_Button3_On)
		//	{
		//		g_Button3_Down = FALSE;
		//		g_Button3_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button3_Down = FALSE;
		//		g_Button3_Up = FALSE;
		//	}
		//	g_Button3_On = g_cj->g_js.rgbButtons[3]?1:0;

		//	// # 4
		//	if( (g_cj->g_js.rgbButtons[4] && !g_Button4_On))
		//	{
		//		g_Button4_Down = TRUE;
		//		g_Button4_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[4] && g_Button4_On)
		//	{
		//		g_Button4_Down = FALSE;
		//		g_Button4_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button4_Down = FALSE;
		//		g_Button4_Up = FALSE;
		//	}
		//	g_Button4_On = g_cj->g_js.rgbButtons[4]?1:0;

		//	// # 5
		//	if( (g_cj->g_js.rgbButtons[5] && !g_Button5_On))
		//	{
		//		g_Button5_Down = TRUE;
		//		g_Button5_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[5] && g_Button5_On)
		//	{
		//		g_Button5_Down = FALSE;
		//		g_Button5_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button5_Down = FALSE;
		//		g_Button5_Up = FALSE;
		//	}
		//	g_Button5_On = g_cj->g_js.rgbButtons[5]?1:0;


		//	// # 6
		//	if( (g_cj->g_js.rgbButtons[6] && !g_Button6_On))
		//	{
		//		g_Button6_Down = TRUE;
		//		g_Button6_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[6] && g_Button6_On)
		//	{
		//		g_Button6_Down = FALSE;
		//		g_Button6_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button6_Down = FALSE;
		//		g_Button6_Up = FALSE;
		//	}
		//	g_Button6_On = g_cj->g_js.rgbButtons[6]?1:0;


		//	// # 7
		//	if( (g_cj->g_js.rgbButtons[7] && !g_Button7_On))
		//	{
		//		g_Button7_Down = TRUE;
		//		g_Button7_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[7] && g_Button7_On)
		//	{
		//		g_Button7_Down = FALSE;
		//		g_Button7_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button7_Down = FALSE;
		//		g_Button7_Up = FALSE;
		//	}
		//	g_Button7_On = g_cj->g_js.rgbButtons[7]?1:0;

		//	// # 8
		//	if( (g_cj->g_js.rgbButtons[8] && !g_Button8_On))
		//	{
		//		g_Button8_Down = TRUE;
		//		g_Button8_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[8] && g_Button8_On)
		//	{
		//		g_Button8_Down = FALSE;
		//		g_Button8_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button8_Down = FALSE;
		//		g_Button8_Up = FALSE;
		//	}
		//	g_Button8_On = g_cj->g_js.rgbButtons[8]?1:0;


		//	// # 9
		//	if( (g_cj->g_js.rgbButtons[9] && !g_Button9_On))
		//	{
		//		g_Button9_Down = TRUE;
		//		g_Button9_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[9] && g_Button9_On)
		//	{
		//		g_Button9_Down = FALSE;
		//		g_Button9_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button9_Down = FALSE;
		//		g_Button9_Up = FALSE;
		//	}
		//	g_Button9_On = g_cj->g_js.rgbButtons[9]?1:0;

		//	// # 10
		//	if( (g_cj->g_js.rgbButtons[10] && !g_Button10_On))
		//	{
		//		g_Button10_Down = TRUE;
		//		g_Button10_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[10] && g_Button10_On)
		//	{
		//		g_Button10_Down = FALSE;
		//		g_Button10_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button10_Down = FALSE;
		//		g_Button10_Up = FALSE;
		//	}
		//	g_Button10_On = g_cj->g_js.rgbButtons[10]?1:0;

		//	// # 11
		//	if( (g_cj->g_js.rgbButtons[11] && !g_Button11_On))
		//	{
		//		g_Button11_Down = TRUE;
		//		g_Button11_Up = FALSE;
		//	}
		//	else if( !g_cj->g_js.rgbButtons[11] && g_Button11_On)
		//	{
		//		g_Button11_Down = FALSE;
		//		g_Button11_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Button11_Down = FALSE;
		//		g_Button11_Up = FALSE;
		//	}
		//	g_Button11_On = g_cj->g_js.rgbButtons[11]?1:0;


		//	// # Z-1축
		//	if( (g_cj->g_js.lZ > 500 && !g_ButtonZ1_On))
		//	{
		//		g_ButtonZ1_Down = TRUE;
		//		g_ButtonZ1_Up = FALSE;
		//	}
		//	else if( g_cj->g_js.lZ < 500 && g_ButtonZ1_On)
		//	{
		//		g_ButtonZ1_Down = FALSE;
		//		g_ButtonZ1_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_ButtonZ1_Down = FALSE;
		//		g_ButtonZ1_Up = FALSE;
		//	}
		//	g_ButtonZ1_On = g_cj->g_js.lZ > 500 ? 1:0;

		//	// # Z-2축
		//	if( (g_cj->g_js.lRz > 500 && !g_ButtonZ2_On))
		//	{
		//		g_ButtonZ2_Down = TRUE;
		//		g_ButtonZ2_Up = FALSE;
		//	}
		//	else if( g_cj->g_js.lRz < 500 && g_ButtonZ2_On)
		//	{
		//		g_ButtonZ2_Down = FALSE;
		//		g_ButtonZ2_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_ButtonZ2_Down = FALSE;
		//		g_ButtonZ2_Up = FALSE;
		//	}
		//	g_ButtonZ2_On = g_cj->g_js.lRz > 500 ? 1:0;


		//	// # POV LEFT
		//	if( g_cj->g_js.rgdwPOV[0] == 27000 && !g_Pov_L_On )
		//	{
		//		g_Pov_L_Down = TRUE;
		//		g_Pov_L_Up = FALSE;
		//		DBG_Put("L");
		//	}
		//	else if( !(g_cj->g_js.rgdwPOV[0] == 27000) && g_Pov_L_On)
		//	{
		//		g_Pov_L_Down = FALSE;
		//		g_Pov_L_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Pov_L_Down = FALSE;
		//		g_Pov_L_Up = FALSE;
		//	}
		//	g_Pov_L_On = g_cj->g_js.rgdwPOV[0] == 27000 ? 1:0;

		//	// # POV RIGHT
		//	if( g_cj->g_js.rgdwPOV[0] == 9000 && !g_Pov_R_On )
		//	{
		//		g_Pov_R_Down = TRUE;
		//		g_Pov_R_Up = FALSE;
		//		DBG_Put("R");
		//	}
		//	else if( !(g_cj->g_js.rgdwPOV[0] == 9000) && g_Pov_R_On)
		//	{
		//		g_Pov_R_Down = FALSE;
		//		g_Pov_R_Up = TRUE;
		//	}
		//	else
		//	{
		//		g_Pov_R_Down = FALSE;
		//		g_Pov_R_Up = FALSE;
		//	}
		//	g_Pov_R_On = g_cj->g_js.rgdwPOV[0] == 9000 ? 1:0;


		//	/////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	/////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//	// X AXIS
		//	// PAD의 메뉴모드는 커서가 움직이면 안된다
		//	if(h_Menu == TRUE || g_PadCamera_Button_On == TRUE)
		//	{
		//		// Nothing

		//	}
		//	else
		//	{
		//		if(g_cj->g_js.lX > 300 || g_cj->g_js.lX < -300)
		//		{
		//			if(g_cj->g_js.lX > 0)
		//			{
		//				// 오른쪽
		//				g_ptMouse.x += (g_cj->g_js.lX * 28 / 1000);

		//				if(g_ptMouse.x > 1024) g_ptMouse.x = 1024;
		//			}
		//			else
		//			{
		//				// 왼쪽
		//				g_ptMouse.x += (g_cj->g_js.lX * 28 / 1000);
		//				if(g_ptMouse.x < 0) g_ptMouse.x = 0;
		//			}
		//		}

		//		// Y AXIS
		//		if(g_cj->g_js.lY > 300 || g_cj->g_js.lY < -300)
		//		{
		//			if(g_cj->g_js.lY > 0)
		//			{
		//				// 아래
		//				g_ptMouse.y += (g_cj->g_js.lY * 23 / 1000);
		//				if(g_ptMouse.y > 768) g_ptMouse.y = 768;
		//			}
		//			else
		//			{
		//				// 위
		//				g_ptMouse.y += (g_cj->g_js.lY * 23 / 1000);
		//				if(g_ptMouse.y < 0) g_ptMouse.y = 0;
		//			}
		//		}
		//	}
		//	g_Sel_Left_Down = g_Pov_L_Down;
		//	g_Sel_Right_Down = g_Pov_R_Down;

		//	// 아무런 변화가 없으면 FALSE 리턴 (사용중이 아니다.) 그렇다면 마우스를 사용한다
		//	if( g_ptMouse == g_Axis1 && 
		//		t_Z1 == g_cj->g_js.lRx && t_Z2 == g_cj->g_js.lRy && 
		//		t_D1 == g_cj->g_js.lZ && t_D2 == g_cj->g_js.lRz &&
		//		g_Button0_On == 0 && g_Button1_On == 0 && 
		//		g_Button2_On == 0 && g_Button3_On == 0 && 
		//		g_Button4_On == 0 && g_Button5_On == 0 && 
		//		g_Button6_On == 0 && g_Button7_On == 0 && 
		//		g_Button8_On == 0 && g_Button9_On == 0 && 
		//		g_Button10_On == 0 && g_Button11_On == 0 &&
		//		g_Attack_Button_On == 0 &&	g_Mugong_Button_On == 0 && g_Menu_Button_On == 0 &&
		//		g_Lock_Button_On == 0 && g_Lock_Button_Up == 0 &&
		//		g_HP_Button_On == 0&& g_MP_Button_On == 0 &&
		//		g_Pan_Left_Down == 0 && g_Pan_Right_Down == 0 &&
		//		g_Zoomin_Down == 0 && g_Zoomout_Down == 0 &&
		//		g_Pov_L_On == 0 && g_Pov_R_On == 0 &&
		//		h_Menu == FALSE) return FALSE;


		//	// AXIS 1처리
		//	g_Axis1 = g_ptMouse;
		//	if(g_cj) SetCursorPos(g_Axis1.x,g_Axis1.y);

		//	// 패드 클리어
		//	Clear_Pad();
		//	// 
		//	switch(g_Pad_Mode)
		//	{
		//		// 아무것두 없다
		//		case NONE_PAD_MODE :
		//		break;

		//		// PS2 MODE
		//		case PS2_PAD_MODE :
		//		{
		//			// Attack
		//			g_Attack_Button_On = g_Button2_Down;
		//			// 무공
		//			g_Mugong_Button_On = g_Button1_Down;
		//			// 메뉴
		//			g_Menu_Button_On = g_Button0_Down;
		//			// 락온
		//			g_Lock_Button_On = g_Button10_On;
		//			g_Lock_Button_Up = g_Button10_Up;

		//			// 내력/체력 물약사용
		//			g_HP_Button_On = g_Button5_Down;
		//			g_MP_Button_On = g_Button4_Down;

		//			// Left panning
		//			g_Pan_Left_Down = g_cj->g_js.lZ < -800 ? 1:0;

		//			// Right panning
		//			g_Pan_Right_Down = g_cj->g_js.lZ > 800 ? 1:0;

		//			// ZOOM IN
		//			g_Zoomin_Down = g_cj->g_js.lRz < -800 ? 1:0;

		//			// ZOOM OUT
		//			g_Zoomout_Down = g_cj->g_js.lRz > 800 ? 1:0;

		//			t_Z1 = g_cj->g_js.lRx;
		//			t_Z2 = g_cj->g_js.lRy;
		//			t_D1 = g_cj->g_js.lZ;
		//			t_D2 = g_cj->g_js.lRz;

		//			// POV PAD
		//			g_Sel_Left_Down = g_Pov_L_Down;
		//			g_Sel_Right_Down = g_Pov_R_Down;
		//		}
		//		break;

		//		// XBOX MODE
		//		case XBOX_PAD_MODE :
		//		{
		//			// Attack
		//			g_Attack_Button_On = g_Button0_Down;
		//			// 무공
		//			g_Mugong_Button_On = g_Button1_Down;
		//			// 메뉴
		//			g_Menu_Button_On = g_Button3_Down;
		//			// 락온!
		//			g_Lock_Button_On = g_Button6_On;
		//			g_Lock_Button_Up = g_Button6_Up;					

		//			// 내력/체력 물약사용
		//			g_HP_Button_On = g_ButtonZ2_Down;
		//			g_MP_Button_On = g_ButtonZ1_Down;

		//			// Left panning
		//			g_Pan_Left_Down = g_cj->g_js.lRx < -800 ? 1:0;

		//			// Right panning
		//			g_Pan_Right_Down = g_cj->g_js.lRx > 800 ? 1:0;

		//			// ZOOM IN
		//			g_Zoomin_Down = g_cj->g_js.lRy < -800 ? 1:0;

		//			// ZOOM OUT
		//			g_Zoomout_Down = g_cj->g_js.lRy > 800 ? 1:0;

		//			t_Z1 = g_cj->g_js.lRx;
		//			t_Z2 = g_cj->g_js.lRy;
		//			t_D1 = g_cj->g_js.lZ;
		//			t_D2 = g_cj->g_js.lRz;

		//			// POV PAD
		//			g_Sel_Left_Down = g_Pov_L_Down;
		//			g_Sel_Right_Down = g_Pov_R_Down;
		//		}
		//		break;

		//		// PC PAD MODE
		//		case PC_PAD_MODE :
		//		{
		//			// Attack
		//			g_Attack_Button_On = g_Button0_Down;
		//			if(g_Attack_Button_On && h_Menu == TRUE) h_Menu = FALSE;

		//			// 무공
		//			g_Mugong_Button_On = g_Button1_Down;
		//			// 메뉴
		//			g_Menu_Button_On = g_Button3_Down;
		//			// PAD용 카메라
		//			g_PadCamera_Button_On = g_Button2_On;

		//			// 내력/체력 물약사용
		//			g_HP_Button_On = g_Button5_Down;
		//			g_MP_Button_On = g_Button4_Down;
		//		
		//			// camara
		//			if(g_PadCamera_Button_On)
		//			{
		//				g_Pan_Left_Down = g_cj->g_js.lX == -1000 ? 1 : 0;
		//				g_Pan_Right_Down = g_cj->g_js.lX == 1000 ? 1 : 0;
		//				g_Zoomin_Down = g_cj->g_js.lY == -1000 ? 1 : 0;
		//				g_Zoomout_Down = g_cj->g_js.lY == 1000 ? 1 : 0;

		//				g_cj->g_js.lX = 0;
		//				g_cj->g_js.lY = 0;
		//			}
		//			
		//			// PAD 용 메뉴버튼에대한 방향 조작
		//			if(g_Menu_Button_On) h_Menu = !h_Menu;
		//			if(h_Menu)
		//			{
		//				if(g_cj->g_js.lX == 1000 && !g_Sel_Right_On)
		//				{
		//					g_Sel_Right_Down = TRUE;
		//				}
		//				else
		//				if(g_cj->g_js.lX == 1000 && g_Sel_Right_On)
		//				{
		//					g_Sel_Right_Down = FALSE;
		//				}
		//				else
		//				{
		//					g_Sel_Right_Down = FALSE;
		//				}
		//				g_Sel_Right_On  = g_cj->g_js.lX == 1000 ? TRUE : FALSE;

		//				//
		//				if(g_cj->g_js.lX == -1000 && !g_Sel_Left_On)
		//				{
		//					g_Sel_Left_Down = TRUE;
		//				}
		//				else
		//				if(g_cj->g_js.lX == -1000 && g_Sel_Left_On)
		//				{
		//					g_Sel_Left_Down = FALSE;
		//				}
		//				else
		//				{
		//					g_Sel_Left_Down = FALSE;
		//				}
		//				g_Sel_Left_Down  = g_cj->g_js.lX == -1000 ? TRUE : FALSE;

		//				g_cj->g_js.lX = 0;
		//			}
		//		}
		//		break;
		//	}
		//	return TRUE;
		//}

		//// 사용하는 PAD관련 FLAG CLEAR
		//XIAHGE_API void	Clear_Pad()
		//{
		//	g_HP_Button_On = FALSE;
		//	g_MP_Button_On = FALSE;
		//	
		//	g_Lock_Button_On = FALSE;
		//	g_Lock_Button_Up = FALSE;
		//	
		//	g_Menu_Button_On = FALSE;
		//	g_Mugong_Button_On = FALSE;
		//	g_Attack_Button_On = FALSE;

		//	g_Zoomin_Down = FALSE;
		//	g_Zoomout_Down = FALSE;
		//	g_Pan_Left_Down = FALSE;
		//	g_Pan_Right_Down = FALSE;
		//	g_Sel_Left_Down = FALSE;
		//	g_Sel_Right_Down = FALSE;
		//}
		//
		//// PAD MODE를 설정한다
		//XIAHGE_API void Setup_Pad(int mode)
		//{
		//	g_Pad_Mode = mode;
		//}

		XIAHGE_API BOOL   UpdateInput()
		{
			sPoint pos;
			GetCursorPos( (LPPOINT)&pos);
			//ScreenToClient( g_EngineInfo.m_hWnd, (LPPOINT)&pos);

			//HT_CHEAT : WINDOWSIZE
			ScreenToClient( g_EngineInfo.m_hWnd, (LPPOINT)&pos);

			// PAD가 사용중이면 검사한다
			//if(g_cj)
			//{
			//	if(UpdateInput_Pad() == TRUE) return TRUE;
			//}

			if( pos != g_ptMouse)
				g_bMouseMove = TRUE;
			else
				g_bMouseMove = FALSE;

			// Mouse Pos
			g_ptMouse = pos;
			g_Axis1 = pos;

			//if(g_cj) SetCursorPos(g_ptMouse.x,g_ptMouse.y);

			BOOL bLButton = GetAsyncKeyState( VK_LBUTTON) < 0;
			BOOL bRButton = GetAsyncKeyState( VK_RBUTTON) < 0;


			// MOUSE 버튼 LEFT
			if( (bLButton && !g_bLButtonOn))
			{
				g_bLButtonDown = TRUE;
				g_bLButtonUp = FALSE;
			}
			else if( !bLButton && g_bLButtonOn)
			{
				g_bLButtonDown = FALSE;
				g_bLButtonUp = TRUE;
			}
			else
			{
				g_bLButtonDown = FALSE;
				g_bLButtonUp = FALSE;
			}

			// MOUSE 버튼 RIGHT
			if( (bRButton && !g_bRButtonOn))
			{
				g_bRButtonDown = TRUE;
				g_bRButtonUp = FALSE;
			}
			else if( !bRButton && g_bRButtonOn)
			{
				g_bRButtonDown = FALSE;
				g_bRButtonUp = TRUE;
			}
			else
			{
				g_bRButtonDown = FALSE;
				g_bRButtonUp = FALSE;
			}

			g_bLButtonOn = bLButton;
			g_bRButtonOn = bRButton;

			return TRUE;
		}
	};

};
