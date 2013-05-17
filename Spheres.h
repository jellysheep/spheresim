
#ifndef _SPHERES_H_
#define _SPHERES_H_

#include <QColor>

//#define _3D_ 1
#define _double_ 1

//#define fastSphereRender 1

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
   __extension__ struct{ float  s0, s1; };
#endif
}s_float2;

typedef union
{
	float  CL_ALIGNED(16) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ float   s0, s1, s2, s3; };
#endif
}s_float4;

typedef  s_float4  s_float3;

typedef union
{
	double  CL_ALIGNED(16) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ double s0, s1; };
#endif
}s_double2;

typedef union
{
	double  CL_ALIGNED(32) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct{ double  s0, s1, s2, s3; };
#endif
}s_double4;

typedef  s_double4  s_double3;

typedef s_float3 float_vector;
typedef s_double3 double_vector;

#if _double_
	typedef double_vector vector;
	typedef s_double2 vector2;
	typedef s_double3 vector3;
	typedef double scalar;
#else
	typedef float_vector vector;
	typedef s_float2 vector2;
	typedef s_float3 vector3;
	typedef float scalar;
#endif

/*typedef struct vector2{
	scalar s[2];
} vector2;
typedef struct vector3{
	scalar s[3];
} vector3;*/

extern vector2* clVector(vector2 v);
extern vector3* clVector(vector3 v);

typedef struct Sphere
{
	vector pos, oldPos;
	vector speed;
	vector force;
	scalar size, mass, poisson, E;
	unsigned short fixed; // 0 or 1
	//float x[3];  //padding
} Sphere;

typedef struct SphereExtension
{
	QColor color, hsvColor;
	vector* trace;
	int traceCount;
	bool traceFull;
} SphereExtension;

typedef struct Plane
{
	vector a,b,P;
} Plane;

extern bool use3D;
extern bool fastRender;

extern int spheresCount;
extern bool manySpheres;
extern int maxShowSpheresCount;

extern vector3 boxSize;
extern vector2 sphereSize;

extern bool usemethodRungeKutta;
extern bool autoSlowRender;
extern int renderFpsMax;
extern int renderFps;
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
extern const char *viewFileExtension3D, *configFileExtension3D;
extern const char *viewFileExtension2D, *configFileExtension2D;
extern int edges;
extern scalar step;
extern scalar G;
extern scalar G_fact;
extern scalar airResistance;
extern bool wallResistance;
extern bool ballResistance;

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

extern int maxCellsPerAxis;
extern int rowsPerStep, curveSteps;
extern int maxNumSpheresInCell;
extern int maxNumCollsPerSphere;

extern int magnitude;

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

extern const char* getViewFileExtension();
extern const char* getConfigFileExtension();

struct Unit{
	scalar size;
	const char* name;
};

namespace unit{
	extern Unit pm;
	extern Unit Ang;
	extern Unit m;
	extern Unit km;
	extern Unit Mkm;
	extern Unit Gkm;
	extern Unit ly;
}

extern Unit unitOfMagnitude[2];
extern Unit curUnit;

extern void errorExit();

#endif
