#if !defined(M_DefineAction)
#error You should define M_DefineAction accordingly before including this file!
#endif


/********************************************************************************************************************
 * M_DefineAction ( Id, szName, szCfg_X, szCfg_Y )
 ********************************************************************************************************************/

	M_DefineAction( E_AAc_Null,				"(None)",					"Null",							"Null" )

	M_DefineAction( E_AAc_Movement,			"Movement",					"Axis(X)",						"Axis(Y)" )
	M_DefineAction( E_AAc_Camera,			"Camera L/R",				"Split(Button(3),Button(4))",	"Null" )
	M_DefineAction( E_AAc_Camera_Inverted,	"Camera L/R (Inverted)",	"Split(Button(4),Button(3))",	"Null" )

/********************************************************************************************************************/


#if defined(M_DefineAction)
#undef M_DefineAction
#endif
