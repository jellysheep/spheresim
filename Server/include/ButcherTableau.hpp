/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _BUTCHERTABLEAU_HPP_
#define _BUTCHERTABLEAU_HPP_

#include "Vector.hpp"

namespace SphereSim
{

    /** \brief Butcher tableau values for an adaptive stepsize Runge-Kutta method. */
    class ButcherTableau
    {
    public:
        /** \brief Runge-Kutta method order and array size. */
        unsigned char order;
        /** \brief Central matrix. */
        Scalar a[10][10];
        /** \brief First bottom vector. */
        Scalar b[10];
        /** \brief Second bottom vector. */
        Scalar b_[10];
        /** \brief Left vector. */
        Scalar c[10];

        /** \brief Initialize empty ButcherTableau. */
        ButcherTableau();

        /** \copydoc ButcherTableau
         * \param order Runge-Kutta method order. */
        ButcherTableau(unsigned char order);

        /** \brief Initialization of the Butcher tableau.
         * \param order Runge-Kutta method order and array size.
         * \param a Central matrix.
         * \param b First bottom vector.
         * \param b_ Second bottom vector.
         * \param c Left vector. */
        ButcherTableau(unsigned char order, const Scalar* a, const Scalar* b,
            const Scalar* b_, const Scalar* c);

        ButcherTableau(const ButcherTableau&) = default;
        ButcherTableau& operator=(const ButcherTableau&) = default;

    };

}

#endif /*_BUTCHERTABLEAU_HPP_*/
