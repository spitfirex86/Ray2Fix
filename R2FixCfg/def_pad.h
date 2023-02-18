#if !defined(M_DefineInput)
#error You should define M_DefineInput accordingly before including this file!
#endif


/********************************************************************************************************************
 * M_DefineInput ( Id, lCtrl, bAnalog, szCfg )
 ********************************************************************************************************************/

	M_DefineInput( E_Pad_LS,			IDC_LS,			TRUE,		"StickLeft" )
	M_DefineInput( E_Pad_RS,			IDC_RS,			TRUE,		"StickRight" )
	M_DefineInput( E_Pad_LS_Click,		IDC_LSCLICK,	FALSE,		"ButtonLS" )
	M_DefineInput( E_Pad_RS_Click,		IDC_RSCLICK,	FALSE,		"ButtonRS" )

	M_DefineInput( E_Pad_Up,			IDC_DUP,		FALSE,		"DpadUp" )
	M_DefineInput( E_Pad_Down,			IDC_DDOWN,		FALSE,		"DpadDown" )
	M_DefineInput( E_Pad_Left,			IDC_DLEFT,		FALSE,		"DpadLeft" )
	M_DefineInput( E_Pad_Right,			IDC_DRIGHT,		FALSE,		"DpadRight" )

	M_DefineInput( E_Pad_A,				IDC_AA,			FALSE,		"ButtonA" )
	M_DefineInput( E_Pad_B,				IDC_BB,			FALSE,		"ButtonB" )
	M_DefineInput( E_Pad_X,				IDC_XX,			FALSE,		"ButtonX" )
	M_DefineInput( E_Pad_Y,				IDC_YY,			FALSE,		"ButtonY" )

	M_DefineInput( E_Pad_LT,			IDC_LT,			FALSE,		"TriggerLT" )
	M_DefineInput( E_Pad_LB,			IDC_LB,			FALSE,		"ButtonLB" )
	M_DefineInput( E_Pad_RT,			IDC_RT,			FALSE,		"TriggerRT" )
	M_DefineInput( E_Pad_RB,			IDC_RB,			FALSE,		"ButtonRB" )

	M_DefineInput( E_Pad_Back,			IDC_BBACK,		FALSE,		"ButtonBack" )
	M_DefineInput( E_Pad_Start,			IDC_BSTART,		FALSE,		"ButtonStart" )

/********************************************************************************************************************/


#if defined(M_DefineInput)
#undef M_DefineInput
#endif
