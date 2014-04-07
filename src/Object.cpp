/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Object.hpp"
#include "Vector.hpp"
#include "DataTransmit.hpp"

#include <iostream>
#include <type_traits>
#include <stdexcept>

const char* errorMessage = "Wrong Object type specified.";

namespace SphereSim
{

    template <typename T>
    struct Visitor
    {
        virtual T operator()(bool*)
        {
            throw std::runtime_error(errorMessage);
        }
        virtual T operator()(unsigned int*)
        {
            throw std::runtime_error(errorMessage);
        }
        virtual T operator()(double*)
        {
            throw std::runtime_error(errorMessage);
        }
        virtual T operator()(float*)
        {
            throw std::runtime_error(errorMessage);
        }
        virtual T operator()(Vector3*)
        {
            throw std::runtime_error(errorMessage);
        }
        virtual T operator()(std::string*)
        {
            throw std::runtime_error(errorMessage);
        }
    };

    struct ConstructorVisitor : public Visitor<void*>
    {
        void* operator()(bool*)
        {
            return new bool;
        }
        void* operator()(unsigned int*)
        {
            return new unsigned int;
        }
        void* operator()(double*)
        {
            return new double;
        }
        void* operator()(float*)
        {
            return new float;
        }
        void* operator()(Vector3*)
        {
            return new Vector3;
        }
        void* operator()(std::string*)
        {
            return new std::string;
        }
    };

    struct DestructorVisitor : public Visitor<void>
    {
        void operator()(bool* b)
        {
            delete b;
        }
        void operator()(unsigned int* i)
        {
            delete i;
        }
        void operator()(double* d)
        {
            delete d;
        }
        void operator()(float* f)
        {
            delete f;
        }
        void operator()(Vector3* v)
        {
            delete v;
        }
        void operator()(std::string* s)
        {
            delete s;
        }
    };

    /// setter:

    template <typename T, bool arithmeticType>
    struct SetterVisitorImpl;

    template <typename T>
    struct SetterVisitorImpl<T, false> : public Visitor<bool>
    {
        T data;

        SetterVisitorImpl() = delete;
        SetterVisitorImpl(T data):data(data){}

        bool operator()(T* t)
        {
            if (*t == data)
            {
                return false;
            }
            *t = data;
            return true;
        }
    };

    template <typename T>
    struct SetterVisitorImpl<T, true> : public Visitor<bool>
    {
        T data;

        SetterVisitorImpl() = delete;
        SetterVisitorImpl(T data):data(data){}

        bool operator()(bool* b)
        {
            if (*b == data)
            {
                return false;
            }
            *b = data;
            return true;
        }
        bool operator()(unsigned int* i)
        {
            if (*i == data)
            {
                return false;
            }
            *i = data;
            return true;
        }
        bool operator()(float* f)
        {
            if (*f == data)
            {
                return false;
            }
            *f = data;
            return true;
        }
        bool operator()(double* d)
        {
            if (*d == data)
            {
                return false;
            }
            *d = data;
            return true;
        }
    };

    template <typename T>
    using SetterVisitor = SetterVisitorImpl<T, std::is_arithmetic<T>::value>;

    /// getter:

    template <typename T, bool arithmetic>
    struct GetterVisitorImpl;

    template <typename T>
    struct GetterVisitorImpl<T, false> : public Visitor<T>
    {
        T operator()(T* t)
        {
            return *t;
        }
    };

    template <typename T>
    struct GetterVisitorImpl<T, true> : public Visitor<T>
    {
        T operator()(bool* b)
        {
            return *b;
        }
        T operator()(unsigned int* i)
        {
            return *i;
        }
        T operator()(float* f)
        {
            return *f;
        }
        T operator()(double* d)
        {
            return *d;
        }
    };

    template <typename T>
    using GetterVisitor = GetterVisitorImpl<T, std::is_arithmetic<T>::value>;

    /// constructors:

    Object::Object(const Type type):type(type), data(nullptr)
    {
        data = applyVisitor(ConstructorVisitor());
    }

    Object::Object(Object&& o):type(o.type), data(o.data)
    {
        o.data = nullptr;
    }

    Object::~Object()
    {
        if (data != nullptr)
        {
            applyVisitor(DestructorVisitor());
        }
    }

    /// methods:

    template <typename T>
    T Object::applyVisitor(Visitor<T>&& visitor) const
    {
        switch (type)
        {
        case BOOL:
            return visitor((bool*)data);
        case INT:
            return visitor((unsigned int*)data);
        case DOUBLE:
            return visitor((double*)data);
        case FLOAT:
            return visitor((float*)data);
        case VECTOR3:
            return visitor((Vector3*)data);
        case STRING:
            return visitor((std::string*)data);
        default:
            throw std::runtime_error(errorMessage);
        }
    }

    template <typename T>
    bool Object::set(const T& data)
    {
        return applyVisitor(SetterVisitor<T>(data));
    }

    template bool Object::set<bool>(const bool&);
    template bool Object::set<unsigned int>(const unsigned int&);
    template bool Object::set<double>(const double&);
    template bool Object::set<float>(const float&);
    template bool Object::set<Vector3>(const Vector3&);
    template bool Object::set<std::string>(const std::string&);

    template <>
    bool Object::set<int>(const int& data)
    {
        return applyVisitor(SetterVisitor<unsigned int>(data));
    }

    template <typename T>
    T Object::get() const
    {
        return applyVisitor(GetterVisitor<T>());
    }

    template bool Object::get<bool>() const;
    template unsigned int Object::get<unsigned int>() const;
    template double Object::get<double>() const;
    template float Object::get<float>() const;
    template Vector3 Object::get<Vector3>() const;
    template std::string Object::get<std::string>() const;

    /// serialization:

    std::string Object::getData() const
    {
        std::ostringstream stream;
        stream<<(char)type;
        switch (type)
        {
        case BOOL:
            writeBool(stream, *(bool*)data);
            break;
        case INT:
            writeInt(stream, *(unsigned int*)data);
            break;
        case DOUBLE:
            writeDouble(stream, *(double*)data);
            break;
        case FLOAT:
            writeFloat(stream, *(float*)data);
            break;
        case VECTOR3:
            writeVector3(stream, *(Vector3*)data);
            break;
        case STRING:
            writeString(stream, *(std::string*)data);
            break;
        default:
            throw std::runtime_error(errorMessage);
        }
        return stream.str();
    }

    bool Object::setData(const std::string &bytes)
    {
        std::istringstream stream(bytes);
        char type;
        stream>>type;
        switch (type)
        {
        case BOOL:
            return set(readBool(stream));
        case INT:
            return set(readInt(stream));
        case DOUBLE:
            return set(readDouble(stream));
        case FLOAT:
            return set(readFloat(stream));
        case VECTOR3:
            return set(readVector3(stream));
        case STRING:
            return set(readString(stream));
        default:
            throw std::runtime_error(errorMessage);
        }
    }

}
