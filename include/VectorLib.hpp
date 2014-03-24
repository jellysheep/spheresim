/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _VECTORLIB_HPP_
#define _VECTORLIB_HPP_

#include <cstdio>

#define overloadAssignOperator(op)				\
	/*	e.g. v1 += v2					*/		\
	inline LibVector& operator op(const LibVector& v)\
	{											\
		asm("#begin");							\
		if (dim>=1)								\
		{										\
			x op v.x;							\
		}										\
		if (dim>=2)								\
		{										\
			y op v.y;							\
		}										\
		if (dim>=3)								\
		{										\
			z op v.z;							\
		}										\
		if (dim>=4)								\
		{										\
			w op v.w;							\
		}										\
		asm("#end");							\
		return *this;							\
	}											\
	inline LibVector& operator op(const T& t)	\
	{											\
	/*	e.g. v *= 3					*/			\
		asm("#begin");							\
		if (dim>=1)								\
		{										\
			x op t;								\
		}										\
		if (dim>=2)								\
		{										\
			y op t;								\
		}										\
		if (dim>=3)								\
		{										\
			z op t;								\
		}										\
		if (dim>=4)								\
		{										\
			w op t;								\
		}										\
		asm("#end");							\
		return *this;							\
	}
#define overloadTemporaryOperator(op)		\
	inline LibVector operator op(const LibVector& v) const \
	{										\
	/*	e.g. v1 + v2					*/	\
		asm("#begin");						\
		LibVector v2(*this);				\
		if (dim>=1)							\
		{									\
			v2.x op##= v.x;					\
		}									\
		if (dim>=2)							\
		{									\
			v2.y op##= v.y;					\
		}									\
		if (dim>=3)							\
		{									\
			v2.z op##= v.z;					\
		}									\
		if (dim>=4)							\
		{									\
			v2.w op##= v.w;					\
		}									\
		asm("#end");						\
		return v2;							\
	}										\
	inline LibVector operator op(const T& t) const \
	{									\
	/*	e.g. v * 3					*/	\
		asm("#begin");					\
		LibVector v2(*this);			\
		if (dim>=1)						\
		{								\
			v2.x op##= t;				\
		}								\
		if (dim>=2)						\
		{								\
			v2.y op##= t;				\
		}								\
		if (dim>=3)						\
		{								\
			v2.z op##= t;				\
		}								\
		if (dim>=4)						\
		{								\
			v2.w op##= t;				\
		}								\
		asm("#end");					\
		return v2;						\
	}
#define declareTemporaryFriendOperator(op)				\
	inline friend LibVector operator op(const T& t, const LibVector& v)	\
	{													\
	/*	e.g. 5 + v						*/				\
		asm("#begin");									\
		LibVector v2(v);								\
		if (dim>=1)										\
		{												\
			v2.x = t op v2.x;							\
		}												\
		if (dim>=2)										\
		{												\
			v2.y = t op v2.y;							\
		}												\
		if (dim>=3)										\
		{												\
			v2.z = t op v2.z;							\
		}												\
		if (dim>=4)										\
		{												\
			v2.w = t op v2.w;							\
		}												\
		asm("#end");									\
		return v2;										\
	}

//floating type and dimension (1 to 4)
/** \brief Vector calculations. */
template <typename T, quint8 dim>
class LibVector
{
public:
	T x, y, z, w;
	LibVector(const T& t)
		:x(t), y(t), z(t), w(t)
	{
	}
	LibVector(const T& a, const T& b, const T& c, const T& d)
		:x(a), y(b), z(c), w(d)
	{
	}
	LibVector(const LibVector& v)
		:x(v.x), y(v.y), z(v.z), w(v.w)
	{
	}
	~LibVector(){}

	LibVector() = delete;
	LibVector& operator=(const LibVector&) = delete;

	inline T getX() const
	{
		return x;
	}
	inline T getY() const
	{
		return y;
	}
	inline T getZ() const
	{
		return z;
	}
	inline T getW() const
	{
		return w;
	}

	inline T operator()(quint8 index)
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			return x;
		}
	}
	inline T operator[](quint8 index)
	{
		return operator()(index);
	}

	overloadAssignOperator(=)
	overloadAssignOperator(+=)
	overloadAssignOperator(-=)
	overloadAssignOperator(*=)
	overloadAssignOperator(/=)

	overloadTemporaryOperator(+)
	overloadTemporaryOperator(-)
	overloadTemporaryOperator(*)
	overloadTemporaryOperator(/)

	declareTemporaryFriendOperator(+)
	declareTemporaryFriendOperator(-)
	declareTemporaryFriendOperator(*)
	declareTemporaryFriendOperator(/)

	inline void print(const char* before)
	{
		printf("%s: ", before);
		print();
	}
	inline void print()
	{
		printf("LibVector[ ");
		if (dim>=1)
		{
			printf("x=%+5f ", (float)x);
		}
		if (dim>=2)
		{
			printf("y=%+5f ", (float)y);
		}
		if (dim>=3)
		{
			printf("z=%+5f ", (float)z);
		}
		if (dim>=4)
		{
			printf("w=%+5f ", (float)w);
		}
		printf("]\n");
	}
};

#define genType(type, c)						\
	typedef LibVector<type, 1> LibVector1##c;	\
	typedef LibVector<type, 2> LibVector2##c;	\
	typedef LibVector<type, 3> LibVector3##c;	\
	typedef LibVector<type, 4> LibVector4##c;

	genType(float, f)
	genType(double, d)
	genType(long double, ld)
	genType(qint8, c)
	genType(quint8, uc)
	genType(qint16, s)
	genType(quint16, us)
	genType(qint32, i)
	genType(quint32, ui)
	genType(qint64, l)
	genType(quint64, ul)

#endif
