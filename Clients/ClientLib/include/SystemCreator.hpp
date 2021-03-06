/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SYSTEMCREATOR_HPP_
#define _SYSTEMCREATOR_HPP_

#include "Vector.hpp"

class ActionSender;

namespace SphereSim
{

    /** \brief Create simulation systems. */
    class SystemCreator
    {
    private:
        ActionSender* actionSender;

    public:
        SystemCreator(ActionSender* actionSender);

        SystemCreator() = delete;
        SystemCreator(const SystemCreator&) = delete;
        SystemCreator& operator=(const SystemCreator&) = delete;

        Scalar createArgonGasSystem(unsigned short sphereCount,
            Scalar targetTemperature=473.15);

        Scalar createMacroscopicGravitationSystem(unsigned short sphereCount);

        Scalar createMacroscopic2DCollisionSystem(unsigned short sphereCount);

        Scalar createSimpleWallCollisionSystem();
    };

}

#endif /*_SYSTEMCREATOR_HPP_*/
