/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#include <cmath>

#if USE_DOUBLE
    #define FLOATING_TYPE double
#else
    #define FLOATING_TYPE float
#endif
typedef FLOATING_TYPE Scalar;

/** Vector class. */
class Vector3
{
private:
    Scalar x, y, z;

public:
    Vector3()
        :x(0), y(0), z(0)
    {
    }
    Vector3(const Scalar x, const Scalar y, const Scalar z)
        :x(x), y(y), z(z)
    {
    }
    Vector3(const Vector3& v)
        :x(v.x), y(v.y), z(v.z)
    {
    }
    Vector3& operator=(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Scalar& operator()(const unsigned char index)
    {
        return *(&x + index);
    }
    const Scalar& operator()(const unsigned char index) const
    {
        return *(&x + index);
    }
    bool operator==(const Vector3& v) const
    {
        return (x == v.x) && (y == v.y) && (z == v.z);
    }

    Scalar squaredNorm() const
    {
        return (x*x) + (y*y) + (z*z);
    }
    Scalar norm() const
    {
        return std::sqrt(squaredNorm());
    }
    Scalar dot(const Vector3& v) const
    {
        return (x*v.x) + (y*v.y) + (z*v.z);
    }
    void setZero()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    void operator*=(const Scalar s)
    {
        x *= s;
        y *= s;
        z *= s;
    }
    void operator/=(const Scalar s)
    {
        x /= s;
        y /= s;
        z /= s;
    }
    void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
};

#endif
