/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Console.hpp"

using namespace SphereSim;

std::mutex Console::mutex = {};

Console::Console()
    :color(Color::white), font(0), stream()
{
}

Console::Console(unsigned short c, unsigned short f)
    :color(c), font(f), stream()
{
}

Console::~Console()
{
    flush();
}

void Console::flush()
{
    std::unique_lock<std::mutex> lock(mutex);
    std::string str = stream.str();
    stream.str(std::string());
    if (color<=Color::white)
    {
        std::ostringstream stream2;
        stream2<<"\x1b[3";
        stream2<<color;
        if (font == Format::bold)
        {
            stream2<<";1";
        }
        stream2<<"m";
        stream2<<str;
        stream2<<"\x1b[0m";
        str = stream2.str();
    }
    std::cout<<std::flush<<str<<std::flush;
}
