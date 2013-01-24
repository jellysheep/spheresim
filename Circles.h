
#ifndef _CIRCLES_H_
#define _CIRCLES_H_

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <CL/cl_platform.h>

#define _3D_ 0
#define _double_ 0

typedef cl_float4 cl_float3;
typedef cl_double4 cl_double3;

#if _3D_
	typedef cl_float3 cl_float_vector;
	typedef cl_double3 cl_double_vector;
#else
	typedef cl_float2 cl_float_vector;
	typedef cl_double2 cl_double_vector;
#endif

#if _double_
	typedef cl_double_vector cl_vector;
	typedef cl_double2 cl_vector2;
	typedef cl_double3 cl_vector3;
	typedef double scalar;
#else
	typedef cl_float_vector cl_vector;
	typedef cl_float2 cl_vector2;
	typedef cl_float3 cl_vector3;
	typedef float scalar;
#endif

typedef struct vector2{
	scalar s[2];
} vector2;
typedef struct vector3{
	scalar s[3];
} vector3;
#if _3D_
	typedef vector3 vector;
#else
	typedef vector2 vector;
#endif

extern cl_vector2* clVector(vector2 v);
extern cl_vector3* clVector(vector3 v);

typedef struct Circle
{
	cl_vector pos, oldPos;
	cl_vector speed;
	cl_vector force;
	scalar size, mass, poisson, E;
	//float x[3];  //padding
} Circle;

typedef struct CircleExtension
{
	int color;
	vector* trace;
	int traceCount;
	bool traceFull;
} CircleExtension;

extern int circlesCount;
extern bool manyCircles;

extern vector3 boxSize;
extern vector2 size;

extern int renderFpsMax, renderFps;
extern scalar speed, speedCorrection;
extern scalar fps,minFps;
extern scalar timeInterval;

extern scalar max_speed;
extern scalar E;
extern scalar poisson;
extern scalar elastic;
extern scalar gravity;
extern bool saveBool, renderBool;
extern int edges;
extern scalar step;
extern scalar G;
extern scalar G_fact;
extern scalar airResistance;
extern bool wallResistance;

extern bool useColoursBool;
extern bool useTrace;
extern long traceCount;
extern bool connectTracePoints;
extern int renderBufferCount;

extern bool reflections;
extern bool useSplitKernels;

extern vector2 autoRotation;

#define min(a,b) (((a)<(b))?(a):(b))

#endif
