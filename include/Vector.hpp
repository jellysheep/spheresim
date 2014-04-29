/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#include <cassert>
#include <cmath>

#if USE_DOUBLE
    #define FLOATING_TYPE double
#else
    #define FLOATING_TYPE float
#endif /*USE_DOUBLE*/
typedef FLOATING_TYPE Scalar;

namespace SphereSim
{

    /** Vector class. */
    class Vector3
    {
    private:
        Scalar x[3];

    public:
        Vector3()
            :x{0, 0, 0}
        {
        }
        Vector3(const Scalar x0, const Scalar x1, const Scalar x2)
            :x{x0, x1, x2}
        {
        }
        Vector3(const Vector3& v)
            :x{v.x[0], v.x[1], v.x[2]}
        {
        }
        Vector3& operator=(const Vector3& v)
        {
            x[0] = v.x[0];
            x[1] = v.x[1];
            x[2] = v.x[2];
            return *this;
        }

        Scalar& operator()(const unsigned char index)
        {
            assert(index < 3);
            return x[index];
        }
        const Scalar& operator()(const unsigned char index) const
        {
            assert(index < 3);
            return x[index];
        }
        bool operator==(const Vector3& v) const
        {
            return (x[0] == v.x[0]) && (x[1] == v.x[1]) && (x[2] == v.x[2]);
        }

        Scalar squaredNorm() const
        {
            return (x[0]*x[0]) + (x[1]*x[1]) + (x[2]*x[2]);
        }
        Scalar norm() const
        {
            return std::sqrt(squaredNorm());
        }
        Scalar dot(const Vector3& v) const
        {
            return (x[0]*v.x[0]) + (x[1]*v.x[1]) + (x[2]*v.x[2]);
        }
        void setZero()
        {
            x[0] = 0;
            x[1] = 0;
            x[2] = 0;
        }

        void operator*=(const Scalar s)
        {
            x[0] *= s;
            x[1] *= s;
            x[2] *= s;
        }
        void operator/=(const Scalar s)
        {
            x[0] /= s;
            x[1] /= s;
            x[2] /= s;
        }
        void operator+=(const Vector3& v)
        {
            x[0] += v.x[0];
            x[1] += v.x[1];
            x[2] += v.x[2];
        }
        void operator-=(const Vector3& v)
        {
            x[0] -= v.x[0];
            x[1] -= v.x[1];
            x[2] -= v.x[2];
        }

        void set_ax(const Scalar s, const Vector3& v)
        {
            x[0] = s*v.x[0];
            x[1] = s*v.x[1];
            x[2] = s*v.x[2];
        }
        void add_ax(const Scalar s, const Vector3& v)
        {
            x[0] += s*v.x[0];
            x[1] += s*v.x[1];
            x[2] += s*v.x[2];
        }
        void set_axpy(const Scalar s, const Vector3& v, const Vector3& v2)
        {
            x[0] = s*v.x[0] + v2.x[0];
            x[1] = s*v.x[1] + v2.x[1];
            x[2] = s*v.x[2] + v2.x[2];
        }

        Scalar distance(const Vector3& v) const
        {
            const Scalar dx0 = x[0]-v.x[0];
            const Scalar dx1 = x[1]-v.x[1];
            const Scalar dx2 = x[2]-v.x[2];
            return std::sqrt(dx0*dx0 + dx1*dx1 + dx2*dx2);
        }
    };

}

#endif /*_VECTOR_HPP_*/
