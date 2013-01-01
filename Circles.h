
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
	double size;
	//float x[3];  //padding
} Circle;

extern int circlesCount, _3D_;
extern double max_speed, E, elastic, gravity;
extern cl_double3 boxSize;
extern bool saveBool;
extern int edges;
extern double step;

#define min(a,b) (((a)<(b))?(a):(b))

#endif
