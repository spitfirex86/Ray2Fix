#include "framework.h"
#include "r2fn.h"


/*
 * Function pointers
 */

DIEnumDevicesCallback R2_fn_InputEnum = OFFSET(0x495170);
BOOL (*R2_fn_SuspendGame)() = OFFSET(0x4016E0);

char * (*R2_fn_szGetStringFromTextOrStringParam)( void *param ) = OFFSET(0x4829D0);


void (*GLI_xAdjustCameraToViewport2)(GLD_tdstDeviceAttributes *p_stDev, GLD_tdstViewportAttributes *p_stVpt, GLI_tdstCamera *p_stCam ) = OFFSET(0x422AB0);

AI_tdstNodeInterpret * (*R2_fn_p_stSPOSuperimpoed)( HIE_tdstSuperObject * p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree ) = OFFSET(0x4710C0);

void (*HIE_fn_SO_vSetSuperimposedFlag)( HIE_tdstSuperObject *hSO ) = OFFSET(0x45B9B0);
void (*fn_vRemoveObjectInSectorList)( HIE_tdstSuperObject *hCharacter ) = OFFSET(0x412810);
void (*GLI_vGet3DVertexFromScreenPos) (
	GLD_tdstViewportAttributes *p_stVpt,
	MTH3D_tdstVector *p_stPos3D,
	MTH_tdxReal xRatioX,
	MTH_tdxReal xRatioY,
	MTH_tdxReal xSize
	) = OFFSET(0x422940);
void (*MEC_vInitTranslation) (
	DNM_tdstDynamics *p_stDynamic,
	HIE_tdstSuperObject *hSupObj,
	MTH3D_tdstVector *p_stVector
	) = OFFSET(0x432DA0);
void (*HIE_fn_vComputeNewRelativeMatrix)( HIE_tdstSuperObject *hSprObj ) = OFFSET(0x45C030);
