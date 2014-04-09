/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <iostream>
#include <mutex>
#include <sstream>

namespace SphereSim
{

    /** \brief Types of Format for the console output. */
    namespace Format
    {
        /** \see Format */
        enum Format
        {
            /** \brief Print console output in bold.
             * Bold text is usually brighter than normal. */
            bold = 1
        };
    }

    /** \brief Color used for the console output. */
    namespace Color
    {
        /** \see Color */
        enum Color
        {
            /** \brief Print console output in black. */
            black,
            /** \brief Print console output in red. */
            red,
            /** \brief Print console output in green. */
            green,
            /** \brief Print console output in yellow. */
            yellow,
            /** \brief Print console output in blue. */
            blue,
            /** \brief Print console output in magenta. */
            magenta,
            /** \brief Print console output in cyan. */
            cyan,
            /** \brief Print console output in white. */
            white
        };
    }

    /** \brief Helper class used for formatted console output. */
    class Console
    {
    private:

        /** \copydoc Color
         * \see Color */
        unsigned short color;
        /** \copydoc Format
         * \see Format */
        unsigned short font;

        static std::mutex mutex;
        std::stringstream stream;

    public:
        /** \brief Initialize console. */
        Console();

        /** \copydoc Console
         * \param c Console color.
         * \param f Text formatting. */
        Console(unsigned short c, unsigned short f);

        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;

        ~Console();

        /** \brief Output formatted string.
         * \param t Object to be printed to console. */
        template <typename T>
        Console& operator<<(T t)
        {
            stream<<t;
            return *this;
        }

        void flush();

    };

}

#endif
