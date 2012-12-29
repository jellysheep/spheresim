
#ifndef _CIRCLES_H_
#define _CIRCLES_H_

#include "CL/cl.hpp"
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


#endif
