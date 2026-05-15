#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION		0x0800
#endif

namespace XiahGameEngine
{
	namespace XiahInput
	{

		XIAHGE_API extern sPoint  g_ptMouse;
		XIAHGE_API extern BOOL    g_bLButtonDown;	//LButton이 처음 눌릴때 만 TRUE (누르고 있어도 한번만 TRUE)
		XIAHGE_API extern BOOL	  g_bRButtonDown;

		XIAHGE_API extern BOOL	  g_bLButtonUp;		//LButton이 띄어 질때 한번 TRUE
		XIAHGE_API extern BOOL	  g_bRButtonUp;

		XIAHGE_API extern BOOL	  g_bLButtonOn;		//버튼을 눌려져 있으면 On(누르고 있어도 계속 TRUE)
		XIAHGE_API extern BOOL	  g_bRButtonOn;

		XIAHGE_API extern BOOL	  UpdateInput();
		
		//HT_CHEAT : 게임 패드 삭제
		XIAHGE_API extern BOOL		g_Lock_Button_On;
		XIAHGE_API extern BOOL		g_Lock_Button_Up;

		XIAHGE_API extern BOOL		g_Sel_Left_Down;
		XIAHGE_API extern BOOL		g_Sel_Right_Down;
		XIAHGE_API extern BOOL		g_Sel_Left_On;
		XIAHGE_API extern BOOL		g_Sel_Right_On;

		XIAHGE_API extern BOOL		g_Zoomin_Down;
		XIAHGE_API extern BOOL		g_Zoomout_Down;
		XIAHGE_API extern BOOL		g_Pan_Left_Down;
		XIAHGE_API extern BOOL		g_Pan_Right_Down;

		XIAHGE_API extern BOOL		g_Attack_Button_On;

		XIAHGE_API extern sPoint	g_Axis1;		// Control Axis1



		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//#define NONE_PAD_MODE		0xff
		//#define	PS2_PAD_MODE		0x00
		//#define	XBOX_PAD_MODE		0x01
		//#define	PC_PAD_MODE			0x02

		//// GAME PAD
		//XIAHGE_API extern DWORD		g_Pad_Mode;			// PAD 설정모드 (PC,PS2,XBOX)

		//XIAHGE_API extern BOOL		g_HP_Button_On;
		//XIAHGE_API extern BOOL		g_MP_Button_On;
		//
		//XIAHGE_API extern BOOL		g_Lock_Button_On;
		//XIAHGE_API extern BOOL		g_Lock_Button_Up;
		//
		//XIAHGE_API extern BOOL		g_Menu_Button_On;
		//XIAHGE_API extern BOOL		g_Mugong_Button_On;
		//XIAHGE_API extern BOOL		g_Attack_Button_On;
		//XIAHGE_API extern BOOL		g_PadCamera_Button_On;	// PC PAD 카메라 제어용

		//XIAHGE_API extern BOOL		g_Zoomin_Down;
		//XIAHGE_API extern BOOL		g_Zoomout_Down;
		//XIAHGE_API extern BOOL		g_Pan_Left_Down;
		//XIAHGE_API extern BOOL		g_Pan_Right_Down;

		//XIAHGE_API extern BOOL		g_Sel_Left_Down;
		//XIAHGE_API extern BOOL		g_Sel_Right_Down;
		//XIAHGE_API extern BOOL		g_Sel_Left_On;
		//XIAHGE_API extern BOOL		g_Sel_Right_On;

		//XIAHGE_API extern sPoint	g_Axis1;		// Control Axis1

		//XIAHGE_API extern BOOL		g_Pov_L_On;		// 디지틀 패드 LEFT
		//XIAHGE_API extern BOOL		g_Pov_L_Down;	// 디지틀 패드 LEFT
		//XIAHGE_API extern BOOL		g_Pov_L_Up;		// 디지틀 패드 LEFT

		//XIAHGE_API extern BOOL		g_Pov_R_On;		// 디지틀 패드 RIGHT
		//XIAHGE_API extern BOOL		g_Pov_R_Down;	// 디지틀 패드 RIGHT
		//XIAHGE_API extern BOOL		g_Pov_R_Up;		// 디지틀 패드 RIGHT

		//// # 0
		//XIAHGE_API extern BOOL	  g_Button0_On;		// 0
		//XIAHGE_API extern BOOL	  g_Button0_Down;	// 0
		//XIAHGE_API extern BOOL	  g_Button0_Up;		// 0

		//// # 1
		//XIAHGE_API extern BOOL	  g_Button1_On;		// 1
		//XIAHGE_API extern BOOL	  g_Button1_Down;	// 1
		//XIAHGE_API extern BOOL	  g_Button1_Up;		// 1

		//// # 2
		//XIAHGE_API extern BOOL	  g_Button2_On;		// 2
		//XIAHGE_API extern BOOL	  g_Button2_Down;	// 2
		//XIAHGE_API extern BOOL	  g_Button2_Up;		// 2

		//// # 3
		//XIAHGE_API extern BOOL	  g_Button3_On;		// 3
		//XIAHGE_API extern BOOL	  g_Button3_Down;	// 3
		//XIAHGE_API extern BOOL	  g_Button3_Up;		// 3

		//// # 4
		//XIAHGE_API extern BOOL	  g_Button4_On;		// 4
		//XIAHGE_API extern BOOL	  g_Button4_Down;	// 4
		//XIAHGE_API extern BOOL	  g_Button4_Up;		// 4

		//// # 5
		//XIAHGE_API extern BOOL	  g_Button5_On;		// 5
		//XIAHGE_API extern BOOL	  g_Button5_Down;	// 5
		//XIAHGE_API extern BOOL	  g_Button5_Up;		// 5

		//// # 6
		//XIAHGE_API extern BOOL	  g_Button6_On;		// 6
		//XIAHGE_API extern BOOL	  g_Button6_Down;	// 6
		//XIAHGE_API extern BOOL	  g_Button6_Up;		// 6

		//// # 7
		//XIAHGE_API extern BOOL	  g_Button7_On;		// 7
		//XIAHGE_API extern BOOL	  g_Button7_Down;	// 7
		//XIAHGE_API extern BOOL	  g_Button7_Up;		// 7

		//// # 8
		//XIAHGE_API extern BOOL	  g_Button8_On;		// 8
		//XIAHGE_API extern BOOL	  g_Button8_Down;	// 8
		//XIAHGE_API extern BOOL	  g_Button8_Up;		// 8
		//
		//// # 9
		//XIAHGE_API extern BOOL	  g_Button9_On;		// 9
		//XIAHGE_API extern BOOL	  g_Button9_Down;	// 9
		//XIAHGE_API extern BOOL	  g_Button9_Up;		// 9

		//// PS2용 아날로그 PUSH는 10,11 이다.
		//// # 10
		//XIAHGE_API extern BOOL	  g_Button10_On;	// 10
		//XIAHGE_API extern BOOL	  g_Button10_Down;	// 10
		//XIAHGE_API extern BOOL	  g_Button10_Up;	// 10

		//// # 11
		//XIAHGE_API extern BOOL	  g_Button11_On;	// 11
		//XIAHGE_API extern BOOL	  g_Button11_Down;	// 11
		//XIAHGE_API extern BOOL	  g_Button11_Up;	// 11

		//// SPECIAL !!	What the FUCK!!

		//// # Z-1축
		//XIAHGE_API extern BOOL	  g_ButtonZ1_On;	// Z1
		//XIAHGE_API extern BOOL	  g_ButtonZ1_Down;	// Z1
		//XIAHGE_API extern BOOL	  g_ButtonZ1_Up;	// Z1

		//// # Z-2축
		//XIAHGE_API extern BOOL	  g_ButtonZ2_On;	// Z2
		//XIAHGE_API extern BOOL	  g_ButtonZ2_Down;	// Z2
		//XIAHGE_API extern BOOL	  g_ButtonZ2_Up;	// Z2



		//XIAHGE_API extern BOOL	  UpdateInput_Pad();
		//XIAHGE_API extern void	  Setup_Pad(int mode);
		//XIAHGE_API extern void	  Clear_Pad();

	};

};
