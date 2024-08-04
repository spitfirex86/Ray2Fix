#include "framework.h"
#include <detours.h>
#include "fix.h"
#include "imports.h"
#include "r2fn.h"
#include "config.h"
#include "GliFixVf.h"

#include <ACP_Ray2.h>


char *szConfigMenu = "/C:Ray2Fix Config";
char szVersionString[50];


/*
 * Detours
 */

BOOL CALLBACK FIX_fn_InputEnum( void *lpddi, void *pvRef )
{
	// Call original function
	R2_fn_InputEnum(lpddi, pvRef);

	// HACK: Input device enumeration fix
	// Rayman2 returns DIENUM_STOP when it encounters a device type it cannot handle.
	// This usually causes it to skip enumeration of gamepads on modern PCs.
	// Here we fix this by always returning DIENUM_CONTINUE.
	return DIENUM_CONTINUE;
}

BOOL FIX_fn_SuspendGame()
{
	// HACK: Disable suspended game state
	// Disregard whatever the game's trying to do here and always return TRUE.
	// This prevents the game from suspending itself when the focus is lost.
	return TRUE;
}

char * FIX_fn_szGetStringFromTextOrStringParam( void *param )
{
	// Call original function
	char *result = R2_fn_szGetStringFromTextOrStringParam(param);

	// HACK: Replace "Site Rayman2" in main menu with a shortcut to R2FixCfg
	if ( !_stricmp(GAM_fn_p_szGetLevelName(), "menu") )
	{
		// Part 1: Replace menu text.
		if ( !strcmp(result, "/C:Site Rayman2")
			|| !strcmp(result, "/C:Web de Rayman2")
			|| !strcmp(result, "/C:Sito di Rayman2")
			|| !strcmp(result, "/C:Rayman2 - Webseite") )
		{
			//return szConfigMenu;
			return szVersionString;
		}

		// Part 2: Open R2FixCfg with ShellExecute.
		if ( !strcmp(result, "http://www.rayman2.com/") )
		{
			if ( CFG_fn_bOpenConfigTool() )
				SendMessage(GAM_fn_hGetWindowHandle(), WM_CLOSE, 0, 0);
		}
	}

	return result;
}


void FIX_xAdjustCameraToViewport2(GLD_tdstDeviceAttributes *p_stDev, GLD_tdstViewportAttributes *p_stVpt, GLI_tdstCamera *p_stCam )
{
	MTH_tdxReal xVptWidthInPix = (MTH_tdxReal)CFG_stActualDispMode.dwWidth;
	MTH_tdxReal xVptHeightInPix = (MTH_tdxReal)CFG_stActualDispMode.dwHeight;
	MTH_tdxReal xVptWidthInMeters = xVptWidthInPix * p_stDev->xPixelDimensionX;
	MTH_tdxReal xVptHeightInMeters = xVptHeightInPix * p_stDev->xPixelDimensionY;
	MTH_tdxReal xVptRatio = xVptHeightInMeters / xVptWidthInMeters;

	MTH_tdxReal xUsedRatio = p_stCam->xRatio;
	if ( xVptRatio != 0 )
		xUsedRatio = xVptRatio;

	// Normally Y-FoV is calculated based on X-FoV. We want it the other way around
	// Was previously calculated as follows:
	//p_stCam->xAlphaX = GLI_M_Div(p_stCam->xAlphaY, xVptRatio);
	// Now, we use this formula to calculate the new X-FOV:
	// newFOV = 2*atan(( newAR )/( defaultAR ) * tan( default FOV / 2))
	p_stCam->xAlphaX = 2.0f * (float)atan( (1.0 / xUsedRatio) / (1.0 / 0.75) * tan((p_stCam->xAlphaY / 0.75) / 2.0) );

	GLI_xAdjustCameraToViewport2(p_stDev, p_stVpt, p_stCam);
}

void FIX_vDraw2DSpriteWithPercent( GLD_tdstViewportAttributes *p_stVpt, MTH_tdxReal XMin, MTH_tdxReal YMin, MTH_tdxReal XMax, MTH_tdxReal YMax, GLI_tdstMaterial *hMaterial )
{
	MTH_tdxReal x1, x2, y1, y2;
	GLI_tdstAligned2DVector a4_st2DVertex[4];

	MTH_tdxReal xVptWidth = (MTH_tdxReal)p_stVpt->dwWidth;
	MTH_tdxReal xVptHeight = (MTH_tdxReal)p_stVpt->dwHeight;
	
	float ratio = 1.0f;
	float addToCenter = 0.0f;

	if ( XMin != 0.0f || XMax != 100.0f )
	{
		ratio = CFG_xActualRatio / 0.75f;
		addToCenter = (xVptWidth - (xVptWidth * ratio)) / 2;
	}

	x1 = (XMin / 100.0f) * xVptWidth * ratio + addToCenter;
	x2 = (XMax / 100.0f) * xVptWidth * ratio + addToCenter;
	y1 = (YMin / 100.0f) * xVptHeight;
	y2 = (YMax / 100.0f) * xVptHeight;

	a4_st2DVertex[0].xX = x2;
	a4_st2DVertex[0].xY = y1;
	a4_st2DVertex[0].xOoZ = 1.0f;
	a4_st2DVertex[1].xX = x1;
	a4_st2DVertex[1].xY = y1;
	a4_st2DVertex[1].xOoZ = 1.0f;
	a4_st2DVertex[2].xX = x1;
	a4_st2DVertex[2].xY = y2;
	a4_st2DVertex[2].xOoZ = 1.0f;
	a4_st2DVertex[3].xX = x2;
	a4_st2DVertex[3].xY = y2;
	a4_st2DVertex[3].xOoZ = 1.0f;

	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals = *GLI_BIG_GLOBALS;
	p_stGlobals->hCurrentMaterial = hMaterial;
	p_stGlobals->lCurrentDrawMask = p_stGlobals->lHierachDrawMask =
		p_stGlobals->hCurrentMaterial->ulMaterialType = 0xFFFFFFFF - GLI_C_Mat_lIsTestingBackface - GLI_C_Mat_lIsNotGrided;
	
	GLI_vDoMaterialSelection(p_stGlobals);
	p_stGlobals->xTextureDontAcceptFog = 1;

#if N64FONT
	if ( *GLI_g_bForceAAAColor )
	{
		p_stGlobals->ulColorInitForSprite &= 0xFF000000;
		p_stGlobals->ulColorInitForSprite |= GLI_a3_ForcedAAAColor[0] << 16 | GLI_a3_ForcedAAAColor[1] << 8 | GLI_a3_ForcedAAAColor[2];
	}

	GLI_DRV_vSendSpriteToClip(a4_st2DVertex, *GLI_g_fZValueForSprite, p_stGlobals);
#else
	if ( *GLI_g_bForceAAAColor )
	{
		unsigned long a4_ulColors[4];
		unsigned long ulAlpha = p_stGlobals->ulColorInitForSprite & 0xFF000000;

		a4_ulColors[0] = a4_ulColors[1] = 0xFFFFFF | ulAlpha;
		a4_ulColors[2] = a4_ulColors[3] = GLI_a3_ForcedAAAColor[0] << 16 | GLI_a3_ForcedAAAColor[1] << 8 | GLI_a3_ForcedAAAColor[2] | ulAlpha;

		GLI_DRV_vSendSpriteToClipWithColors(a4_st2DVertex, a4_ulColors, *GLI_g_fZValueForSprite, p_stGlobals);
	}
	else
		GLI_DRV_vSendSpriteToClip(a4_st2DVertex, *GLI_g_fZValueForSprite, p_stGlobals);
#endif // N64FONT
}

AI_tdstNodeInterpret * FIX_fn_p_stSPOSuperimpoed( HIE_tdstSuperObject * p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree )
{
	unsigned long ulProcId = p_stTree[-1].uParam.ulValue;
	AI_tdstGetSetParam stParam;
	HIE_tdstSuperObject *hNewSuperObjPerso;

	MTH_tdxReal xVptWidth = (MTH_tdxReal)GAM_g_stEngineStructure->stFixViewportAttr.dwWidth;
	MTH_tdxReal xVptHeight = (MTH_tdxReal)GAM_g_stEngineStructure->stFixViewportAttr.dwHeight;

	if ( ulProcId == 409 )
	{
		AI_fn_vGetUltraOperatorPerso(AI_fn_ucGetProcedureUltraOperator(ulProcId), p_SuperObjPerso, &hNewSuperObjPerso);

		long lX, lY, lZ;
		MTH_tdxReal xX, xY, xZ;
		MTH3D_tdstVector stVertex;
		DNM_tdstDynam *hDynam = HIE_M_hSuperObjectGetActor(hNewSuperObjPerso)->hDynam;
		HIE_fn_SO_vSetSuperimposedFlag(hNewSuperObjPerso);
		fn_vRemoveObjectInSectorList(hNewSuperObjPerso);

		POS_tdstCompletePosition *hPersoGlobalMatrix = hNewSuperObjPerso->p_stGlobalMatrix;

		p_stTree = AI_fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
		lX = stParam.uParam.lValue;
		p_stTree = AI_fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
		lY = stParam.uParam.lValue;
		p_stTree = AI_fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
		lZ = stParam.uParam.lValue;

		/****************************************************************************
		* SPT : superimposed ratio fix
		****************************************************************************/
		float ratio = CFG_xActualRatio / 0.75f;
		float addToCenter = (xVptWidth - (xVptWidth * ratio)) / 2 / xVptWidth;

		xX = ((float)lX / 1000.f * ratio) + addToCenter;
		xY = (float)lY / 1000.f;
		xZ = (float)lZ / 1000.f;
		/****************************************************************************/

		GLI_vGet3DVertexFromScreenPos(&GAM_g_stEngineStructure->stFixViewportAttr, &stVertex, xX, xY, xZ);
		POS_fn_vSetTranslationVector(hPersoGlobalMatrix, &stVertex);

		hPersoGlobalMatrix->eType = MTH_C_Type_CompleteMatrix;
		/* SPT : superimposed ratio fix */
		hPersoGlobalMatrix->stTransformMatrix.stCol_2.z = hPersoGlobalMatrix->stTransformMatrix.stCol_0.x = xVptHeight / 480.f * ratio;

		if ( hDynam )
		{
			DNM_tdstDynamics *p_stDynamics = hDynam->p_stDynamics;
			MEC_vInitTranslation(p_stDynamics, hNewSuperObjPerso, &stVertex);
		}
		HIE_fn_vComputeNewRelativeMatrix(hNewSuperObjPerso);

		return p_stTree;
	}
	else if ( ulProcId == 410 )
	{
		AI_fn_vGetUltraOperatorPerso(AI_fn_ucGetProcedureUltraOperator(ulProcId), p_SuperObjPerso, &hNewSuperObjPerso);

		MTH3D_tdstVector stVertex;
		DNM_tdstDynam *hDynam = HIE_M_hSuperObjectGetActor(hNewSuperObjPerso)->hDynam;
		HIE_fn_SO_vSetSuperimposedFlag(hNewSuperObjPerso);
		fn_vRemoveObjectInSectorList(hNewSuperObjPerso);

		POS_tdstCompletePosition *hPersoGlobalMatrix = hNewSuperObjPerso->p_stGlobalMatrix;

		p_stTree = AI_fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
		stVertex.x = stParam.uParam.xValue;
		p_stTree = AI_fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
		stVertex.y = stParam.uParam.xValue;
		p_stTree = AI_fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
		stVertex.z = stParam.uParam.xValue;

		POS_fn_vSetTranslationVector(hPersoGlobalMatrix,&stVertex);

		hPersoGlobalMatrix->eType = MTH_C_Type_CompleteMatrix;
		/* SPT : superimposed ratio fix */
		float ratio = CFG_xActualRatio / 0.75f;
		hPersoGlobalMatrix->stTransformMatrix.stCol_2.z = hPersoGlobalMatrix->stTransformMatrix.stCol_0.x = xVptHeight / 480.f * ratio;

		if ( hDynam )
		{
			DNM_tdstDynamics *p_stDynamics = hDynam->p_stDynamics;
			MEC_vInitTranslation(p_stDynamics, hNewSuperObjPerso, &stVertex);
		}
		HIE_fn_vComputeNewRelativeMatrix(hNewSuperObjPerso);

		return p_stTree;
	}

	return R2_fn_p_stSPOSuperimpoed(p_SuperObjPerso, p_stTree);
}


extern void *FIX_Code4Patch_Pos;
extern void *FIX_Code4Patch_Size;

void fn_vApplyPatch( void *pTarget, void *data, size_t size )
{
	DWORD dwOldProtect, dwNewProtect = PAGE_EXECUTE_READWRITE;
	VirtualProtect(pTarget, size, dwNewProtect, &dwOldProtect);
	memcpy(pTarget, data, size);
	VirtualProtect(pTarget, size, dwOldProtect, &dwNewProtect);
}

/* note: this does not account for overwritten instructions */
void fn_vApplyJmpToCode( void *pJmpAt, void *pCode )
{
	unsigned char code[] = { 0xB8, 0, 0, 0, 0, 0xFF, 0xE0 };
	memcpy(code+1, &pCode, sizeof(void*));

	fn_vApplyPatch(pJmpAt, code, sizeof(code));
}

void FIX_fn_vPatchCode4( void )
{
	fn_vApplyJmpToCode((void*)0x475C7F, &FIX_Code4Patch_Pos);
	fn_vApplyJmpToCode((void*)0x475C96, &FIX_Code4Patch_Size);
}


/*
 * Functions
 */

void fn_vPreAttachHooks( void )
{
	//sprintf_s(szVersionString, sizeof(szVersionString), "/O200:%s v%s", GLI_szName, GLI_szVersion);
	snprintf(szVersionString, sizeof(szVersionString), "/C:%s v%s", GLI_szName, GLI_szVersion);
}

void FIX_fn_vAttachHooks( void )
{
	fn_vPreAttachHooks();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&R2_fn_InputEnum, (PVOID)FIX_fn_InputEnum);
	DetourAttach((PVOID*)&R2_fn_SuspendGame, (PVOID)FIX_fn_SuspendGame);
	DetourAttach((PVOID*)&R2_fn_szGetStringFromTextOrStringParam, (PVOID)FIX_fn_szGetStringFromTextOrStringParam);

	if ( CFG_bIsWidescreen && CFG_bPatchWidescreen )
	{
		DetourAttach((PVOID*)&GLI_xAdjustCameraToViewport2, (PVOID)FIX_xAdjustCameraToViewport2);
		DetourAttach((PVOID*)&GLI_vDraw2DSpriteWithPercent, (PVOID)FIX_vDraw2DSpriteWithPercent);
		DetourAttach((PVOID*)&R2_fn_p_stSPOSuperimpoed, (PVOID)FIX_fn_p_stSPOSuperimpoed);
		FIX_fn_vPatchCode4();
	}

	DetourTransactionCommit();
}

void FIX_fn_vDetachHooks( void )
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach((PVOID*)&R2_fn_InputEnum, (PVOID)FIX_fn_InputEnum);
	DetourDetach((PVOID*)&R2_fn_SuspendGame, (PVOID)FIX_fn_SuspendGame);
	DetourDetach((PVOID*)&R2_fn_szGetStringFromTextOrStringParam, (PVOID)FIX_fn_szGetStringFromTextOrStringParam);

	if ( CFG_bIsWidescreen && CFG_bPatchWidescreen )
	{
		DetourDetach((PVOID *)&GLI_xAdjustCameraToViewport2, (PVOID)FIX_xAdjustCameraToViewport2);
		DetourDetach((PVOID *)&GLI_vDraw2DSpriteWithPercent, (PVOID)FIX_vDraw2DSpriteWithPercent);
		DetourDetach((PVOID *)&R2_fn_p_stSPOSuperimpoed, (PVOID)FIX_fn_p_stSPOSuperimpoed);
	}

	DetourTransactionCommit();
}

void FIX_fn_vPatchFramerate( void )
{
	return; /* no longer necessary */

	typedef struct
	{
		char _code1[54];
		WORD nop1;
		char _code2[8];
		WORD nop2;
	} FlipDeviceCode;

	FlipDeviceCode *lpCode = DetourCodeFromPointer((PVOID)Vd_GLI_DRV_vFlipDeviceWithSyncro, NULL);

	DWORD dwOldProtect;
	DWORD dwNewProtect = PAGE_EXECUTE_READWRITE;

	VirtualProtect(lpCode, sizeof(FlipDeviceCode), dwNewProtect, &dwOldProtect);

	lpCode->nop1 = lpCode->nop2 = (WORD)0x9090;

	VirtualProtect(lpCode, sizeof(FlipDeviceCode), dwOldProtect, &dwNewProtect);
}

void FIX_fn_vRemoveModeEnum( void )
{
	typedef struct
	{
		char _code1[251];
		char nop1[30];
	} DisplayConfigCode;

	DisplayConfigCode *lpCode = DetourCodeFromPointer((PVOID)Vd_GLI_DRV_fn_lGetAllDisplayConfig, NULL);

	DWORD dwOldProtect;
	DWORD dwNewProtect = PAGE_EXECUTE_READWRITE;

	VirtualProtect(lpCode, sizeof(DisplayConfigCode), dwNewProtect, &dwOldProtect);

	memset(lpCode->nop1, 0x90, sizeof(lpCode->nop1));

	VirtualProtect(lpCode, sizeof(DisplayConfigCode), dwOldProtect, &dwNewProtect);
}
