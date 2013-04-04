
#ifndef _EIGEN_CALCULATOR_EVECTOR_H_
#define _EIGEN_CALCULATOR_EVECTOR_H_

#define NDEBUG
#include <Eigen/Dense>
using namespace Eigen;

#define parallelFor 
//_Pragma("omp parallel for if(spheresCount>500)")

struct Pos {
	int posOfSphere, sphereAtPos;
};

#define useSSE 0

#if useSSE
	//Vector4d and Vector4f are vectorized
	//typedef Eigen::Matrix<scalar, 4, 1> eVector;
	#if _double_
		//typedef Vector4d eVector;
		#define eVector Eigen::Matrix<scalar, ((dims+1)/2)*2, 1>
	#else
		//typedef Vector4f eVector;
		#define eVector Eigen::Matrix<scalar, ((dims+3)/4)*4, 1>
	#endif
#else
	//typedef Eigen::Matrix<scalar, 3, 1> eVector;
	#define eVector Eigen::Matrix<scalar, dims, 1>
	/*#if _double_
		//typedef Vector3d eVector;
		#define eVector Eigen::Matrix<scalar, dims, 1>
	#else
		//typedef Vector3f eVector;
		#define eVector Eigen::Matrix<scalar, dims, 1>
	#endif*/
#endif



template <bool condition, typename Then, typename Else>
struct IF
{
    typedef Then RET;
};

template <class Then, typename Else>
struct IF<false, Then, Else>
{
    typedef Else RET;
};

template <typename T>
T pow_int(T a, unsigned int b){
	return (b==0 ? 1 : a*pow_int(a,b-1));
}

#endif
