/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <Console.hpp>

using namespace SphereSim;

InternalConsole::InternalConsole():color(Color::white),font(0){
}

InternalConsole::InternalConsole(quint16 c, quint16 f):color(c),font(f){
}

QTextStream InternalConsole::console(stdout);

InternalConsole Console::out;
InternalConsole Console::bold(Color::white, 1<<Format::bold);

InternalConsole Console::black(Color::black, 0);
InternalConsole Console::red(Color::red, 0);
InternalConsole Console::green(Color::green, 0);
InternalConsole Console::yellow(Color::yellow, 0);
InternalConsole Console::blue(Color::blue, 0);
InternalConsole Console::magenta(Color::magenta, 0);
InternalConsole Console::cyan(Color::cyan, 0);
InternalConsole Console::white(Color::white, 0);

InternalConsole Console::blackBold(Color::black, 1<<Format::bold);
InternalConsole Console::redBold(Color::red, 1<<Format::bold);
InternalConsole Console::greenBold(Color::green, 1<<Format::bold);
InternalConsole Console::yellowBold(Color::yellow, 1<<Format::bold);
InternalConsole Console::blueBold(Color::blue, 1<<Format::bold);
InternalConsole Console::magentaBold(Color::magenta, 1<<Format::bold);
InternalConsole Console::cyanBold(Color::cyan, 1<<Format::bold);
InternalConsole Console::whiteBold(Color::white, 1<<Format::bold);
