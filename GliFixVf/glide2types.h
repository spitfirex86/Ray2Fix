#pragma once


#define FXTRUE 1
#define FXFALSE 0

#define GLIDE_NUM_TMU 2

#define FX_CALL __stdcall


typedef unsigned char FxU8;
typedef signed char FxI8;
typedef unsigned short FxU16;
typedef signed short FxI16;
typedef signed long FxI32;
typedef unsigned long FxU32;
typedef int FxBool;
typedef float FxFloat;
typedef double FxDouble;


typedef struct GrTmuVertex
{
	float sow;
	float tow;
	float oow;
}
GrTmuVertex;

typedef struct GrVertex
{
	float x, y, z;
	float r, g, b;
	float ooz;
	float a;
	float oow;
	GrTmuVertex tmuvtx[GLIDE_NUM_TMU];
}
GrVertex;


#define GR_COMBINE_FUNCTION_ZERO 0x0
#define GR_COMBINE_FUNCTION_NONE GR_COMBINE_FUNCTION_ZERO
#define GR_COMBINE_FUNCTION_LOCAL 0x1
#define GR_COMBINE_FUNCTION_LOCAL_ALPHA 0x2
#define GR_COMBINE_FUNCTION_SCALE_OTHER 0x3
#define GR_COMBINE_FUNCTION_BLEND_OTHER GR_COMBINE_FUNCTION_SCALE_OTHER
#define GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL 0x4
#define GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA 0x5 
#define GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL 0x6
#define GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL 0x7
#define GR_COMBINE_FUNCTION_BLEND GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL
#define GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL_ALPHA 0x8
#define GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL 0x9
#define GR_COMBINE_FUNCTION_BLEND_LOCAL GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL
#define GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA 0x10

#define GR_COMBINE_FACTOR_ZERO 0x0
#define GR_COMBINE_FACTOR_NONE GR_COMBINE_FACTOR_ZERO
#define GR_COMBINE_FACTOR_LOCAL 0x1
#define GR_COMBINE_FACTOR_OTHER_ALPHA 0x2
#define GR_COMBINE_FACTOR_LOCAL_ALPHA 0x3
#define GR_COMBINE_FACTOR_TEXTURE_ALPHA 0x4
#define GR_COMBINE_FACTOR_DETAIL_FACTOR GR_COMBINE_FACTOR_TEXTURE_ALPHA
#define GR_COMBINE_FACTOR_LOD_FRACTION 0x5
#define GR_COMBINE_FACTOR_ONE 0x8
#define GR_COMBINE_FACTOR_ONE_MINUS_LOCAL 0x9
#define GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA 0xa
#define GR_COMBINE_FACTOR_ONE_MINUS_LOCAL_ALPHA 0xb
#define GR_COMBINE_FACTOR_ONE_MINUS_TEXTURE_ALPHA 0xc
#define GR_COMBINE_FACTOR_ONE_MINUS_DETAIL_FACTOR GR_COMBINE_FACTOR_ONE_MINUS_TEXTURE_ALPHA
#define GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION 0xd

#define GR_COMBINE_LOCAL_ITERATED 0x0
#define GR_COMBINE_LOCAL_CONSTANT 0x1
#define GR_COMBINE_LOCAL_NONE GR_COMBINE_LOCAL_CONSTANT
#define GR_COMBINE_LOCAL_DEPTH 0x2

#define GR_COMBINE_OTHER_ITERATED 0x0
#define GR_COMBINE_OTHER_TEXTURE 0x1
#define GR_COMBINE_OTHER_CONSTANT 0x2
#define GR_COMBINE_OTHER_NONE GR_COMBINE_OTHER_CONSTANT

#define GR_BLEND_ZERO 0x0
#define GR_BLEND_SRC_ALPHA 0x1
#define GR_BLEND_SRC_COLOR 0x2
#define GR_BLEND_DST_COLOR GR_BLEND_SRC_COLOR
#define GR_BLEND_DST_ALPHA 0x3 
#define GR_BLEND_ONE 0x4
#define GR_BLEND_ONE_MINUS_SRC_ALPHA 0x5
#define GR_BLEND_ONE_MINUS_SRC_COLOR 0x6
#define GR_BLEND_ONE_MINUS_DST_COLOR GR_BLEND_ONE_MINUS_SRC_COLOR 
#define GR_BLEND_ONE_MINUS_DST_ALPHA 0x7
#define GR_BLEND_RESERVED_8 0x8
#define GR_BLEND_RESERVED_9 0x9
#define GR_BLEND_RESERVED_A 0xa
#define GR_BLEND_RESERVED_B 0xb
#define GR_BLEND_RESERVED_C 0xc
#define GR_BLEND_RESERVED_D 0xd
#define GR_BLEND_RESERVED_E 0xe
#define GR_BLEND_ALPHA_SATURATE 0xf
#define GR_BLEND_PREFOG_COLOR GR_BLEND_ALPHA_SATURATE

#define GR_COLORCOMBINE_ZERO 0x0
#define GR_COLORCOMBINE_CCRGB 0x1
#define GR_COLORCOMBINE_ITRGB 0x2
#define GR_COLORCOMBINE_ITRGB_DELTA0 0x3
#define GR_COLORCOMBINE_DECAL_TEXTURE 0x4
#define GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB 0x5
#define GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB 0x6
#define GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0 0x7
#define GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_ADD_ALPHA 0x8
#define GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA 0x9
#define GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA_ADD_ITRGB 0xa
#define GR_COLORCOMBINE_TEXTURE_ADD_ITRGB 0xb
#define GR_COLORCOMBINE_TEXTURE_SUB_ITRGB 0xc
#define GR_COLORCOMBINE_CCRGB_BLEND_ITRGB_ON_TEXALPHA 0xd
#define GR_COLORCOMBINE_DIFF_SPEC_A 0xe
#define GR_COLORCOMBINE_DIFF_SPEC_B 0xf
#define GR_COLORCOMBINE_ONE 0x10


extern void (FX_CALL *grAlphaBlendFunction)( FxI32 rgb_sf, FxI32 rgb_df, FxI32 alpha_sf, FxI32 alpha_df );
extern void (FX_CALL *grAlphaCombine)( FxI32 function, FxI32 factor,FxI32 local, FxI32 other,FxBool invert );
extern void (FX_CALL *guColorCombineFunction)( FxI32 fnc );
extern void (FX_CALL *grDrawLine)( const GrVertex *v1, const GrVertex *v2 );

extern FxI32 *Vd_GLI_GLIDE1_xTmuNumber;
