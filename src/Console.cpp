/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Console.hpp"

using namespace SphereSim;

Console::Console()
    :color(Color::white), font(0), stream()
{
}

Console::Console(unsigned short c, unsigned short f)
    :color(c), font(f), stream()
{
}

std::mutex Console::mutex = {};
