
#ifdef ENGINE_THRUST

#include "ThrustCalculator.h"

#include <thrust/transform_reduce.h>
#include <thrust/functional.h>
#include <thrust/sequence.h>

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



// This functor tests regular code...
struct addVectorContents : public thrust::unary_function<float,float>
{
    const int l;
    const thrust::device_ptr<float> ptr;
    addVectorContents(int l_, thrust::device_ptr<float> ptr_) : l(l_), ptr(ptr_) {}
    
    __host__ __device__
        float operator()(const float& a) const
        {
			float f = a;
			for(int i = 0; i<l; i++){
				f+=ptr[i];
			}
			return f;
        }
};

template <int dims, bool _3D_>
ThrustCalculator<dims,_3D_>::ThrustCalculator():EigenCalculator_Engine<dims,_3D_>(){
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
	
	
	int numCells = EigenCalculator_Engine<dims,_3D_>::numCells;
	spheresPerCell = thrust::device_vector<int>(numCells*numCells*(_3D_?numCells:1));
	cellIndices_ = new int[numCells*numCells*(_3D_?numCells:1)];
	cellIndices = thrust::device_vector<int>(cellIndices_, cellIndices_+numCells*numCells*(_3D_?numCells:1));
	
	
	
	thrust::device_vector<float> content(1);
	content[0] = 5.f;
	thrust::device_ptr<float> contentPtr = content.data();
	std::cout<<"content before: "<<content[0]<<'\n';
	thrust::transform(content.begin(), content.end(), content.begin(), addVectorContents(1, contentPtr));
	std::cout<<"content after: "<<content[0]<<'\n';
}

struct NumGreaterOne {
	__host__ __device__
	bool operator()(const int& a, const int& b) {
		return a>1 && b<=1;
	}
};


template <int dims, bool _3D_>
void ThrustCalculator<dims,_3D_>::collideSpheresPerCell(){
	int numCells = EigenCalculator_Engine<dims,_3D_>::numCells;
	int* numSpheresInCell = EigenCalculator_Engine<dims,_3D_>::numSpheresInCell;
	thrust::sequence(cellIndices.begin(), cellIndices.end());
	spheresPerCell = thrust::device_vector<int>(numSpheresInCell, numSpheresInCell+(numCells*numCells*(_3D_?numCells:1)));
	thrust::sort_by_key(spheresPerCell.begin(), spheresPerCell.end(), cellIndices.begin(), NumGreaterOne());
	/*
	for(int i = 0; i<numCells*numCells*(_3D_?numCells:1); i+=100){
		std::cout<<"Spheres in cell "<<cellIndices[i]<<": "<<numSpheresInCell[cellIndices[i]]<<'\n';
	}//*/
	//thrust::copy(cellIndices.begin(), cellIndices.end(), cellIndices_);
	EigenCalculator_Engine<dims,_3D_>::collideSpheresPerCell(true,cellIndices_);
}


template class ThrustCalculator<2,false>;
template class ThrustCalculator<3,true>;

#endif
