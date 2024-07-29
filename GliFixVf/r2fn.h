#pragma once

#include "framework.h"
#include "fix.h"

#include <ACP_Ray2.h>


/*
 * Function pointers
 */

extern DIEnumDevicesCallback R2_fn_InputEnum;
extern BOOL (*R2_fn_SuspendGame)();

extern char * (*R2_fn_szGetStringFromTextOrStringParam)( void *param );


extern void (*GLI_xAdjustCameraToViewport2)(GLD_tdstDeviceAttributes *p_stDev, GLD_tdstViewportAttributes *p_stVpt, GLI_tdstCamera *p_stCam );

extern AI_tdstNodeInterpret * (*R2_fn_p_stSPOSuperimpoed)( HIE_tdstSuperObject * p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree );

extern void (*HIE_fn_SO_vSetSuperimposedFlag)( HIE_tdstSuperObject *hSO );
extern void (*fn_vRemoveObjectInSectorList)( HIE_tdstSuperObject *hCharacter );
extern void (*GLI_vGet3DVertexFromScreenPos) (
	GLD_tdstViewportAttributes *p_stVpt,
	MTH3D_tdstVector *p_stPos3D,
	MTH_tdxReal xRatioX,
	MTH_tdxReal xRatioY,
	MTH_tdxReal xSize
	);
extern void (*MEC_vInitTranslation) (
	DNM_tdstDynamics *p_stDynamic,
	HIE_tdstSuperObject *hSupObj,
	MTH3D_tdstVector *p_stVector
	);
extern void (*HIE_fn_vComputeNewRelativeMatrix)( HIE_tdstSuperObject *hSprObj );