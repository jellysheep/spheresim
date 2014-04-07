/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Sphere.hpp"
#include "DataTransmit.hpp"

using namespace SphereSim;

void SphereSim::writeBool(std::ostream& stream, bool b)
{
    stream.write((char*)&b, 1);
}

bool SphereSim::readBool(std::istream& stream)
{
    bool b;
    stream.read((char*)&b, 1);
    return b;
}

void SphereSim::writeChar(std::ostream& stream, unsigned char c)
{
    stream.write((char*)&c, 1);
}

char SphereSim::readChar(std::istream& stream)
{
    unsigned char c;
    stream.read((char*)&c, 1);
    return c;
}

void SphereSim::writeShort(std::ostream& stream, unsigned short s)
{
    stream.write((char*)&s, 2);
}

unsigned short SphereSim::readShort(std::istream& stream)
{
    unsigned short s;
    stream.read((char*)&s, 2);
    return s;
}

void SphereSim::writeInt(std::ostream& stream, unsigned int i)
{
    stream.write((char*)&i, 4);
}

unsigned int SphereSim::readInt(std::istream& stream)
{
    unsigned int i;
    stream.read((char*)&i, 4);
    return i;
}

void SphereSim::writeFloat(std::ostream& stream, float f)
{
    stream.write((char*)&f, 4);
}

float SphereSim::readFloat(std::istream& stream)
{
    float f;
    stream.read((char*)&f, 8);
    return f;
}

void SphereSim::writeDouble(std::ostream& stream, double d)
{
    stream.write((char*)&d, 8);
}

double SphereSim::readDouble(std::istream& stream)
{
    double d;
    stream.read((char*)&d, 8);
    return d;
}

void SphereSim::writeVector3(std::ostream& stream, Vector3 v)
{
    writeDouble(stream, v(0));
    writeDouble(stream, v(1));
    writeDouble(stream, v(2));
}

Vector3 SphereSim::readVector3(std::istream& stream)
{
    Vector3 v;
    v(0) = readDouble(stream);
    v(1) = readDouble(stream);
    v(2) = readDouble(stream);
    return v;
}

void SphereSim::writeString(std::ostream& stream, std::string s)
{
    writeShort(stream, s.size());
    stream.write(s.c_str(), s.size());
}

std::string SphereSim::readString(std::istream& stream)
{
    unsigned short size = readShort(stream);
    char* c_str = new char[size];
    stream.read(c_str, size);
    std::string s(c_str, size);
    delete[] c_str;
    return s;
}

void SphereSim::writeBasicSphereData(std::ostream& stream, Sphere& s)
{
    writeDouble(stream, s.radius);
    writeVector3(stream, s.pos);
}

void SphereSim::readBasicSphereData(std::istream& stream, Sphere& s)
{
    s.radius = readDouble(stream);
    s.pos = readVector3(stream);
}

void SphereSim::writeAllSphereData(std::ostream& stream, Sphere& s)
{
    writeDouble(stream, s.radius);
    writeDouble(stream, s.mass);
    writeVector3(stream, s.pos);
    writeVector3(stream, s.speed);
    writeVector3(stream, s.acc);
}

void SphereSim::readAllSphereData(std::istream& stream, Sphere& s)
{
    s.radius = readDouble(stream);
    s.mass = readDouble(stream);
    s.pos = readVector3(stream);
    s.speed = readVector3(stream);
    s.acc = readVector3(stream);
}
