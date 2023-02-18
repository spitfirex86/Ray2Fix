#pragma once


typedef enum tdeTweaks_
{
	e_TWK_NoTweaks = 0,
	
	e_TWK_ShowPos = 1 << 0,
	e_TWK_LuminosityKeys = 1 << 1,
	e_TWK_ModLoader = 1 << 2,
	
	e_TWK_AllTweaks = (1 << 3) - 1
}
tdeTweaks;
