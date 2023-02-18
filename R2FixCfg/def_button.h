#if !defined(M_DefineAction)
#error You should define M_DefineAction accordingly before including this file!
#endif


/********************************************************************************************************************
 * M_DefineAction ( Id, szName, szCfg )
 ********************************************************************************************************************/

	M_DefineAction( E_BAc_Null,				"(None)",					"Null" )

	M_DefineAction( E_BAc_Up,				"Up",						"Keyboard(Up)" )
	M_DefineAction( E_BAc_Down,				"Down",						"Keyboard(Down)" )
	M_DefineAction( E_BAc_Left,				"Left",						"Keyboard(Left)" )
	M_DefineAction( E_BAc_Right,			"Right",					"Keyboard(Right)" )

	M_DefineAction( E_BAc_Jump,				"Jump",						"Keyboard(A)" )
	M_DefineAction( E_BAc_Jump_Accept,		"Jump / Accept",			"Compound(Keyboard(A),Keyboard(Enter))" )
	M_DefineAction( E_BAc_Jump_Cancel,		"Jump / Cancel",			"Button(2)" )

	M_DefineAction( E_BAc_Shoot,			"Shoot",					"Keyboard(Space)" )
	M_DefineAction( E_BAc_Shoot_Accept,		"Shoot / Accept",			"Button(1)" )
	M_DefineAction( E_BAc_Shoot_Cancel,		"Shoot / Cancel",			"Compound(Keyboard(Space),Keyboard(Backspace))" )

	M_DefineAction( E_BAc_Strafe,			"Strafe / Dive",			"Button(5)" )
	M_DefineAction( E_BAc_Look,				"Look Mode",				"Keyboard(Numpad0)" )
	M_DefineAction( E_BAc_Camera_Reset,		"Reset Camera",				"Keyboard(End)" )
	M_DefineAction( E_BAc_Camera_Left,		"Camera Left",				"Button(4)" )
	M_DefineAction( E_BAc_Camera_Right,		"Camera Right",				"Button(3)" )

	M_DefineAction( E_BAc_Think,			"Think (F1)",				"Keyboard(F1)" )
	M_DefineAction( E_BAc_HUD,				"HUD (J)",					"Button(6)" )
	M_DefineAction( E_BAc_ScreenShot,		"Screenshot",				"Keyboard(F8)" )
	M_DefineAction( E_BAc_Pause,			"Pause",					"Keyboard(Esc)" )

/********************************************************************************************************************/


#if defined(M_DefineAction)
#undef M_DefineAction
#endif
