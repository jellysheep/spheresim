
#ifdef ENGINE_THRUST

#include "ThrustCalculator.h"

#include <thrust/transform_reduce.h>
#include <thrust/functional.h>

#include <cusp/hyb_matrix.h>
#include <cusp/print.h>

// This functor implements the dot product between 3d vectors
struct DotProduct3D : public thrust::binary_function<Float3,Float3,float>
{
    __host__ __device__
        float operator()(const Float3& a, const Float3& b) const
        {
            return thrust::get<0>(a) * thrust::get<0>(b) +    // x components
                   thrust::get<1>(a) * thrust::get<1>(b) +    // y components
                   thrust::get<2>(a) * thrust::get<2>(b);     // z components
        }
};
// This functor implements the dot product between 2d vectors
struct DotProduct2D : public thrust::binary_function<Float2,Float2,float>
{
    __host__ __device__
        float operator()(const Float2& a, const Float2& b) const
        {
            return thrust::get<0>(a) * thrust::get<0>(b) +    // x components
                   thrust::get<1>(a) * thrust::get<1>(b);     // y components
        }
};

FloatTupleIterator3D getZipIterator(FloatIterator x, FloatIterator y, FloatIterator z){
	return thrust::make_zip_iterator(make_tuple(x, y, z));
}
FloatTupleIterator2D getZipIterator(FloatIterator x, FloatIterator y){
	return thrust::make_zip_iterator(make_tuple(x, y));
}

template <>
void ThrustCalculator<3,true>::buildZipIterators(){
	spherePosFirst = getZipIterator(spherePos[0].begin(), spherePos[1].begin(), spherePos[2].begin());
	spherePosLast = getZipIterator(spherePos[0].end(), spherePos[1].end(), spherePos[2].end());
	sphereSpeedFirst = getZipIterator(sphereSpeed[0].begin(), sphereSpeed[1].begin(), sphereSpeed[2].begin());
	sphereSpeedLast = getZipIterator(sphereSpeed[0].end(), sphereSpeed[1].end(), sphereSpeed[2].end());
}
template <>
void ThrustCalculator<2,false>::buildZipIterators(){
	spherePosFirst = getZipIterator(spherePos[0].begin(), spherePos[1].begin());
	spherePosLast = getZipIterator(spherePos[0].end(), spherePos[1].end());
	sphereSpeedFirst = getZipIterator(sphereSpeed[0].begin(), sphereSpeed[1].begin());
	sphereSpeedLast = getZipIterator(sphereSpeed[0].end(), sphereSpeed[1].end());
}

// This functor returns a vector in a range of [0;b]
struct RandPosInBox : public thrust::unary_function<float,float>{
    const float b;
    RandPosInBox(float _b) : b(_b) {}
    
    __host__ __device__
        float operator()(const float& a) const
        {
            return b*((rand()%(1<<31))/((float)(1<<31)));
            //a-(b*((int)(a/b)));
        }
};

// This functor just returns 0
struct GetZero : public thrust::unary_function<float,float>{
    
    __host__ __device__
        float operator()(const float& a) const
        {
            return 0;
        }
};

template <int dims, bool _3D_>
ThrustCalculator<dims,_3D_>::ThrustCalculator():Calculator(){
	spherePos = new thrust::device_vector<float>[dims];
	sphereSpeed = new thrust::device_vector<float>[dims];
	sphereForce = new thrust::device_vector<float>[dims];
	for(int i = 0; i<dims; i++){
		spherePos[i] = thrust::device_vector<float>(spheresCount);
		//thrust::generate(spherePos[i].begin(), spherePos[i].end(), rand);
		thrust::transform(spherePos[i].begin(), spherePos[i].end(), spherePos[i].begin(), RandPosInBox(boxSize.s[i]));
		
		sphereSpeed[i] = thrust::device_vector<float>(spheresCount);
		thrust::generate(sphereSpeed[i].begin(), sphereSpeed[i].end(), rand);
		
		sphereForce[i] = thrust::device_vector<float>(spheresCount);
		thrust::transform(sphereForce[i].begin(), sphereForce[i].end(), sphereForce[i].begin(), GetZero());
	}
	buildZipIterators();
	thrust::transform(sphereSpeedFirst, sphereSpeedLast, sphereSpeedFirst, VectorLength(2.f));
	for(int i = 0; i<spheresCount && i<10; i++){
		std::cout<<"Vector length of vector "<<i<<": "<<
			sqrt(sphereSpeed[0][i]*sphereSpeed[0][i]
			+ sphereSpeed[1][i]*sphereSpeed[1][i])<<'\n';
	}
	for(int i = 0; i<spheresCount && i<10; i++){
		std::cout<<"Pos. of sphere "<<i<<": "<<
			spherePos[0][i]<<','<<spherePos[1][i]<<'\n';
	}
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::doStep(){
}

template <int dims, bool _3D_>
bool ThrustCalculator<dims,_3D_>::saveFrame(){
	return true;
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::save(){
}


template <int dims, bool _3D_>
bool ThrustCalculator<dims,_3D_>::isFixed(int i){
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::loadConfig(const char* file){
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::sphereCountChanged_subclass(int i){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::maxSphereCountChanged_subclass(int i){
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::initSphere(int i){
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::calcWallResistance(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::calcBallResistance(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::sumUpForces(){
}



template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::fpsChanged(scalar timeInterval){
}


template <int dims, bool _3D_>
Sphere* ThrustCalculator<dims,_3D_>::getSphere(int i){
}

template <int dims, bool _3D_>
Sphere* ThrustCalculator<dims,_3D_>::getDirectSphere(int i){
}


template <int dims, bool _3D_>
scalar ThrustCalculator<dims,_3D_>::getTemperature(){
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::boxSizeChanged(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::gravityChanged(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updateG(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updateAirResistance(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updateWallResistance(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updateEModul(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updatePoisson(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updateElasticity(){
}

template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::updateGridSize(){
}


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::loadConfig(){
}



template class ThrustCalculator<2,false>;
template class ThrustCalculator<3,true>;

#endif
