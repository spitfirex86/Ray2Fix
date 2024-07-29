.586
.MODEL FLAT, C

PUBLIC FIX_Code4Patch_Pos
PUBLIC FIX_Code4Patch_Size

tdstDisplayMode	STRUCT
	dwWidth			dd ?
	dwHeight		dd ?
tdstDisplayMode ENDS

EXTERN CFG_stDispMode :tdstDisplayMode
EXTERN CFG_stActualDispMode :tdstDisplayMode
EXTERN CFG_xActualRatio :REAL4

.DATA
ALIGN 4
	xOnePer1000		REAL4		0.001f
	xOrigRatio		REAL4		0.75f
	xOrigHeight		REAL4		480.f
	xTwo			REAL4		2.f


	pJumpTo_Pos		dd			475C88h
	pJumpTo_Size	dd			475C9Eh

.CODE

ALIGN 4
FIX_Code4Patch_Pos:
	fmul	xOnePer1000
	;; ratio = (xActualVptHeight / xActualVptWidth) / 0.75
	fld		CFG_xActualRatio
	fdiv	xOrigRatio
	;; addToCenter = (1 - ratio) / 2
	fld1
	fsub	st(0), st(1)
	fdiv	xTwo
	;; (x * 0.001) * ratio + addToCenter
	fxch
	fmulp	st(2), st(0)
	faddp	st(1), st(0)
	fstp	dword ptr [esp]
	jmp		ds:pJumpTo_Pos

ALIGN 4
FIX_Code4Patch_Size:
	fild	dword ptr CFG_stDispMode.dwHeight
	fdiv	xOrigHeight
	;; ratio = (xActualVptHeight / xActualVptWidth) / 0.75
	fld		CFG_xActualRatio
	fdiv	xOrigRatio
	;; (xActualVptHeight / 480) * ratio
	fmulp	st(1), st(0)
	sub		esp, 4
	fstp	dword ptr [esp]
	pop		eax
	mov		ecx, eax
	jmp		ds:pJumpTo_Size

END
