
#ifndef _CIRCLES_H_
#define _CIRCLES_H_

#include <QColor>

#define _3D_ 0
#define _double_ 1

#define fastSphereRender 1

#if defined( __GNUC__ )
	#define CL_ALIGNED(_x)		  __attribute__ ((aligned(_x)))
#elif defined( _WIN32) && (_MSC_VER)
	#define CL_ALIGNED(_x)
#else
   #warning  Need to implement some method to align data here
   #define  CL_ALIGNED(_x)
#endif

typedef union
{
	float  CL_ALIGNED(8) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ float  x, y; };
   __extension__ struct{ float  s0, s1; };
   __extension__ struct{ float  lo, hi; };
#endif
}float2;

typedef union
{
	float  CL_ALIGNED(16) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ float   x, y, z, w; };
   __extension__ struct{ float   s0, s1, s2, s3; };
   __extension__ struct{ float2  lo, hi; };
#endif
}float4;

typedef  float4  float3;

typedef union
{
	double  CL_ALIGNED(16) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ double  x, y; };
   __extension__ struct{ double s0, s1; };
   __extension__ struct{ double lo, hi; };
#endif
}double2;

typedef union
{
	double  CL_ALIGNED(32) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ double  x, y, z, w; };
   __extension__ struct{ double  s0, s1, s2, s3; };
   __extension__ struct{ double2 lo, hi; };
#endif
}double4;

typedef  double4  double3;

#if _3D_
	typedef float3 float_vector;
	typedef double3 double_vector;
#else
	typedef float2 float_vector;
	typedef double2 double_vector;
#endif

#if _double_
	typedef double_vector vector;
	typedef double2 vector2;
	typedef double3 vector3;
	typedef double scalar;
#else
	typedef float_vector vector;
	typedef float2 vector2;
	typedef float3 vector3;
	typedef float scalar;
#endif

/*typedef struct vector2{
	scalar s[2];
} vector2;
typedef struct vector3{
	scalar s[3];
} vector3;*/
#if _3D_
	typedef vector3 vector;
#else
	typedef vector2 vector;
#endif

extern vector2* clVector(vector2 v);
extern vector3* clVector(vector3 v);

typedef struct Circle
{
	vector pos, oldPos;
	vector speed;
	vector force;
	scalar size, mass, poisson, E;
	unsigned short fixed; // 0 or 1
	//float x[3];  //padding
} Circle;

typedef struct CircleExtension
{
	QColor color, hsvColor;
	vector* trace;
	int traceCount;
	bool traceFull;
} CircleExtension;

typedef struct Plane
{
	vector a,b,P;
} Plane;

extern int circlesCount;
extern bool manyCircles;
extern int maxShowCirclesCount;

extern vector3 boxSize;
extern vector2 sphereSize;

extern int renderFpsMax, renderFps;
extern scalar speed, speedCorrection;
extern scalar fps,minFps;
extern scalar timeInterval;

extern scalar max_speed;
extern scalar E;
extern scalar poisson;
extern scalar elastic;
extern scalar gravity_abs;
extern vector gravity;
extern bool saveBool, renderBool, playBool;
extern const char *filename;
extern const char *viewFileExtension, *configFileExtension;
extern int edges;
extern scalar step;
extern scalar G;
extern scalar G_fact;
extern scalar airResistance;
extern bool wallResistance;

extern bool useColorsBool;
extern bool useColorHSV;
extern scalar hueStep;
extern scalar hueOffset;
extern bool useTrace;
extern long traceCount;
extern bool connectTracePoints;
extern scalar traceAmount;
extern int renderBufferCount;

extern bool reflections;
extern bool wireframe;
extern bool showCube, showLights;
extern bool useSplitKernels;
extern bool showGraph;
extern int plotFps;

extern vector3 autoRotation;

extern scalar calcSpeedFact;

//#define min(a,b) (((a)<(b))?(a):(b))



extern int rani(int i);
extern scalar rans(scalar s);
extern scalar rans(scalar s1, scalar s2);

template <class T>
T* newCopy(T* oOld, int iOld, int iNew){
	bool deleteOld = true;
	if(iNew<iOld){
		//*oNew = oOld;
		return oOld;
	}
	T* bufferNew = new T[iNew];
	memcpy(bufferNew, oOld, sizeof(T)*(iOld));
	if(deleteOld){
		delete[] oOld;
	}
	//*oNew = bufferNew;
	return bufferNew;
}

#endif
