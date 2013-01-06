
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
	typedef cl_float3 float_vector;
	typedef cl_double3 double_vector;
#else
	typedef cl_float2 float_vector;
	typedef cl_double2 double_vector;
#endif

#if _double_
	typedef double_vector vector;
	typedef cl_double2 vector2;
	typedef cl_double3 vector3;
	typedef double scalar;
#else
	typedef float_vector vector;
	typedef cl_float2 vector2;
	typedef cl_float3 vector3;
	typedef float scalar;
#endif

typedef struct Circle
{
	vector pos;
	vector speed;
	vector force;
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

extern bool useCircleExtensions;
extern bool useTrace;
extern long traceCount;
extern bool connectTracePoints;
extern int renderBufferCount;

#define min(a,b) (((a)<(b))?(a):(b))

#endif
