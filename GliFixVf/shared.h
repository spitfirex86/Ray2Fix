#pragma once

typedef enum tagTWEAKS
{
	TWK_NO_TWEAKS = 0,
	
	TWK_SHOWPOS = 1 << 0,
	TWK_LUMCONTROL = 1 << 1,
	TWK_MODLOADER = 1 << 2,
	
	TWK_ALL_TWEAKS = (1 << 3) - 1
} TWEAKS;
