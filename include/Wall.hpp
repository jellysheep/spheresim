/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _WALL_HPP_
#define _WALL_HPP_

#include "Vector.hpp"

class QDataStream;

namespace SphereSim
{
    /** \brief Physical data of a 2D wall. */
    struct Wall2D
    {
        /** \brief Position of the wall. */
        Vector2 pos;
        /** \brief Wall vector. */
        Vector2 vec;

    };

    /** \brief Physical data of a 3D wall. */
    struct Wall3D
    {
        /** \brief Position of the wall. */
        Vector3 pos;
        /** \brief First wall vector. */
        Vector3 vec1;
        /** \brief Second wall vector. */
        Vector3 vec2;

    };
}

#endif
