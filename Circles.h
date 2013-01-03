
#ifndef _CIRCLES_H_
#define _CIRCLES_H_

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <CL/cl_platform.h>

typedef cl_float4 cl_float3;
typedef cl_double4 cl_double3;

typedef struct Circle
{
	cl_double2 pos;
	cl_double2 speed;
	cl_double2 force;
	double size, mass, poisson, E;
	//float x[3];  //padding
} Circle;

typedef struct CircleExtension
{
	int color;
	cl_float2* trace;
	int traceCount;
	bool traceFull;
} CircleExtension;

extern int circlesCount, _3D_;
extern bool manyCircles;

extern cl_double3 boxSize;
extern cl_double2 size;

extern int renderFps;
extern double fps;
extern double speed;
extern double timeInterval;

extern double max_speed;
extern double E;
extern double poisson;
extern double elastic;
extern double gravity;
extern bool saveBool, renderBool;
extern int edges;
extern double step;
extern double G;

extern bool useCircleExtensions;
extern int traceCount;
extern bool connectTracePoints;
extern int renderBufferCount;

#define min(a,b) (((a)<(b))?(a):(b))

#endif
