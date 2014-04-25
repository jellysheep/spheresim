/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

//#define NDEBUG
#include <Eigen/Dense>

#if USE_DOUBLE
    #define FLOATING_TYPE double
#else
    #define FLOATING_TYPE float
#endif
typedef FLOATING_TYPE Scalar;

/** Vector proxy class forwarding to Eigen. */
class Vector3
{
private:
    typedef Eigen::Matrix<Scalar, 3, 1> EigenVector3;
    EigenVector3 eigenVector;

public:
    Vector3()
        :eigenVector(0, 0, 0)
    {
    }
    Vector3(Scalar x, Scalar y, Scalar z)
        :eigenVector(x, y, z)
    {
    }
    Vector3(Vector3&& v)
        :eigenVector(std::move(v.eigenVector))
    {
    }
    Vector3(const Vector3& v)
        :eigenVector(v.eigenVector)
    {
    }
    explicit Vector3(const EigenVector3& v)
        :eigenVector(v)
    {
    }
    Vector3& operator=(const Vector3& v)
    {
        eigenVector = v.eigenVector;
        return *this;
    }

    ~Vector3()
    {
    }

    Scalar& operator()(unsigned char index)
    {
        return eigenVector(index);
    }
    const Scalar& operator()(unsigned char index) const
    {
        return eigenVector(index);
    }
    bool operator==(const Vector3& v) const
    {
        return eigenVector == v.eigenVector;
    }

    Scalar norm() const
    {
        return eigenVector.norm();
    }
    Scalar squaredNorm() const
    {
        return eigenVector.squaredNorm();
    }
    Scalar dot(const Vector3& v) const
    {
        return eigenVector.dot(v.eigenVector);
    }
    void setZero()
    {
        eigenVector.setZero();
    }

    void operator*=(Scalar s)
    {
        eigenVector *= s;
    }
    void operator/=(Scalar s)
    {
        eigenVector /= s;
    }
    void operator+=(Vector3 v)
    {
        eigenVector += v.eigenVector;
    }
    void operator-=(Vector3 v)
    {
        eigenVector -= v.eigenVector;
    }
};

#endif
