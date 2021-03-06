/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "ButcherTableau.hpp"
#include "Console.hpp"

using namespace SphereSim;

ButcherTableau::ButcherTableau()
    :ButcherTableau(0)
{
}

ButcherTableau::ButcherTableau(unsigned char _order)
    :order(_order), a(), b(), b_(), c()
{
}

ButcherTableau::ButcherTableau(unsigned char _order, const Scalar* _a, const Scalar* _b,
    const Scalar* _b_, const Scalar* _c)
    :ButcherTableau(_order)
{
    unsigned short counter = 0;
    Scalar checksum;
    for (unsigned char x = 0; x<order; x++)
    {
        checksum = 0.0;
        for (unsigned char y = 0; y<order; y++)
        {
            a[x][y] = _a[counter++];
            checksum += a[x][y];
        }
        b[x] = _b[x];
        b_[x] = _b_[x];
        c[x] = _c[x];
        if (fabs(checksum-c[x])>0.0001*(checksum+c[x]))
        {
            Console()<<Console::red<<Console::bold
                <<"Error: Butcher tableau checksum is wrong!\n";
        }
    }
}
