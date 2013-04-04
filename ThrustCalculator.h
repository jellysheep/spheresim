#ifdef ENGINE_THRUST

#ifndef _THRUST_CALCULATOR_H_
#define _THRUST_CALCULATOR_H_

#include "Spheres.h"
#include "Calculator.h"
#include "EigenCalculator_Engine.h"

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <cmath>


typedef thrust::tuple<float, float> Float2;
typedef thrust::tuple<float, float, float> Float3;

typedef thrust::device_vector<float>::iterator FloatIterator;

typedef thrust::tuple<FloatIterator, FloatIterator, FloatIterator> FloatIteratorTuple3D;
typedef thrust::tuple<FloatIterator, FloatIterator> FloatIteratorTuple2D;
typedef thrust::zip_iterator<FloatIteratorTuple3D> FloatTupleIterator3D;
typedef thrust::zip_iterator<FloatIteratorTuple2D> FloatTupleIterator2D;

// This functor puts the length of a vector to a certain value
struct VectorLength3D : public thrust::unary_function<Float3,Float3>{
    const float b;
    VectorLength3D(float _b) : b(_b) {}
    
    __host__ __device__
        Float3& operator()(const Float3& a) const
        {
			float fact = b / sqrt(thrust::get<0>(a)*thrust::get<0>(a)
				+ thrust::get<1>(a)*thrust::get<1>(a)
				+ thrust::get<2>(a)*thrust::get<2>(a));
            return *(new Float3(thrust::get<0>(a) * fact, thrust::get<1>(a)*fact, thrust::get<2>(a)*fact));
        }
};
// This functor puts the length of a vector to a certain value
struct VectorLength2D : public thrust::unary_function<Float2,Float2>{
    const float b;
    VectorLength2D(float _b) : b(_b) {}
    
    __host__ __device__
        Float2& operator()(const Float2& a) const
        {
			float fact = b / sqrt(thrust::get<0>(a)*thrust::get<0>(a)
				+ thrust::get<1>(a)*thrust::get<1>(a));
            return *(new Float2(thrust::get<0>(a) * fact, thrust::get<1>(a)*fact));
        }
};

template <int dims, bool _3D_>
class ThrustCalculator: public EigenCalculator_Engine<dims,_3D_> {
	typedef typename IF<_3D_, 
		FloatIteratorTuple3D, 
		FloatIteratorTuple2D
	>::RET FloatIteratorTuple;
	typedef typename IF<_3D_, 
		FloatTupleIterator3D, 
		FloatTupleIterator2D
	>::RET FloatTupleIterator;
	
	typedef typename IF<_3D_, Float3, Float2>::RET FloatTuple;
	
	typedef typename IF<_3D_, VectorLength3D, VectorLength2D>::RET VectorLength;

protected:
	
	thrust::device_vector<float> *spherePos, *sphereSpeed, *sphereForce;
	FloatTupleIterator spherePosFirst, spherePosLast;
	FloatTupleIterator sphereSpeedFirst, sphereSpeedLast;
	void buildZipIterators();
	
	thrust::device_vector<int> spheresPerCell, cellIndices;
	int* cellIndices_;
	
public:
	ThrustCalculator();
};

typedef ThrustCalculator<3,true> ThrustCalculator3D;
typedef ThrustCalculator<2,false> ThrustCalculator2D;

#endif  /* _THRUST_CALCULATOR_H_ */

#endif
