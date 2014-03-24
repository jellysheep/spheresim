/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <Object.hpp>
#include <Vector.hpp>

#include <iostream>
#include <type_traits>

namespace SphereSim
{
    struct ObjectException : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Wrong type specified.";
        }
    };

    template <typename T>
    struct Visitor
    {
        virtual T operator()(bool*)
        {
            throw ObjectException();
        }
        virtual T operator()(int*)
        {
            throw ObjectException();
        }
        virtual T operator()(double*)
        {
            throw ObjectException();
        }
        virtual T operator()(float*)
        {
            throw ObjectException();
        }
        virtual T operator()(Vector3*)
        {
            throw ObjectException();
        }
        virtual T operator()(std::string*)
        {
            throw ObjectException();
        }
    };

    struct ConstructorVisitor : public Visitor<void*>
    {
        void* operator()(bool*)
        {
            return new bool;
        }
        void* operator()(int*)
        {
            return new int;
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
        void operator()(int* i)
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
        bool operator()(int* i)
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
        T operator()(int* i)
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
            return visitor((int*)data);
        case DOUBLE:
            return visitor((double*)data);
        case FLOAT:
            return visitor((float*)data);
        case VECTOR3:
            return visitor((Vector3*)data);
        case STRING:
            return visitor((std::string*)data);
        default:
            throw ObjectException();
        }
    }

    template <typename T>
    bool Object::set(const T& data)
    {
        return applyVisitor(SetterVisitor<T>(data));
    }

    template bool Object::set<bool>(const bool&);
    template bool Object::set<int>(const int&);
    template bool Object::set<double>(const double&);
    template bool Object::set<float>(const float&);
    template bool Object::set<Vector3>(const Vector3&);
    template bool Object::set<std::string>(const std::string&);

    template <typename T>
    T Object::get() const
    {
        return applyVisitor(GetterVisitor<T>());
    }

    template bool Object::get<bool>() const;
    template int Object::get<int>() const;
    template double Object::get<double>() const;
    template float Object::get<float>() const;
    template Vector3 Object::get<Vector3>() const;
    template std::string Object::get<std::string>() const;

    /// serialization:

    template <typename T>
    T getTypeFromBytes(const QByteArray &bytes);

    template <>
    bool getTypeFromBytes<bool>(const QByteArray &bytes)
    {
        return bytes[0]==0?false:true;
    }

    template <>
    int getTypeFromBytes<int>(const QByteArray &bytes)
    {
        int l = 0;
        char* chars = (char*)&l;
        for (int i = 0; i<4; i++)
        {
            chars[i] = bytes.at(i);
        }
        return l;
    }

    template <>
    long getTypeFromBytes<long>(const QByteArray &bytes)
    {
        long l = 0;
        char* chars = (char*)&l;
        for (int i = 0; i<8; i++)
        {
            chars[i] = bytes.at(i);
        }
        return l;
    }

    template <>
    double getTypeFromBytes<double>(const QByteArray &bytes)
    {
        long l = getTypeFromBytes<long>(bytes);
        return *((double*)&l);
    }

    template <>
    float getTypeFromBytes<float>(const QByteArray &bytes)
    {
        int i = getTypeFromBytes<int>(bytes);
        return *((float*)&i);
    }

    template <>
    Vector3 getTypeFromBytes<Vector3>(const QByteArray &bytes)
    {
        Vector3 v;
        v(0) = getTypeFromBytes<double>(bytes.mid(0, 8));
        v(1) = getTypeFromBytes<double>(bytes.mid(8, 8));
        v(2) = getTypeFromBytes<double>(bytes.mid(16, 8));
        return v;
    }

    template <>
    std::string getTypeFromBytes<std::string>(const QByteArray &bytes)
    {
        std::string str(bytes.constData(), bytes.size());
        return str;
    }

    template <typename T>
    QByteArray getBytesFromType(const T &t);

    template <>
    QByteArray getBytesFromType<bool>(const bool &t)
    {
        return QByteArray(1, t?255:0);
    }

    template <>
    QByteArray getBytesFromType<int>(const int &t)
    {
        QByteArray bytes(4, 0);
        int l = t;
        char* chars = (char*)&l;
        for (int i = 0; i<4; i++)
        {
            bytes[i] = chars[i];
        }
        return bytes;
    }

    template <>
    QByteArray getBytesFromType<long>(const long &t)
    {
        QByteArray bytes(8, 0);
        long l = t;
        char* chars = (char*)&l;
        for (int i = 0; i<8; i++)
        {
            bytes[i] = chars[i];
        }
        return bytes;
    }

    template <>
    QByteArray getBytesFromType<double>(const double &t)
    {
        long l = *((long*)&t);
        return getBytesFromType<long>(l);
    }

    template <>
    QByteArray getBytesFromType<float>(const float &t)
    {
        int i = *((int*)&t);
        return getBytesFromType<int>(i);
    }

    template <>
    QByteArray getBytesFromType<Vector3>(const Vector3 &t)
    {
        QByteArray bytes, tmp;
        double d0 = t(0), d1 = t(1), d2 = t(2);
        tmp = getBytesFromType<double>(d0);
        bytes += tmp;
        tmp = getBytesFromType<double>(d1);
        bytes += tmp;
        tmp = getBytesFromType<double>(d2);
        bytes += tmp;
        return bytes;
    }

    template <>
    QByteArray getBytesFromType<std::string>(const std::string &t)
    {
        return QByteArray(t.c_str(), t.size());
    }

    QByteArray Object::getData() const
    {
        QByteArray bytes;
        bytes.append((char)type);
        switch (type)
        {
        case BOOL:
            bytes.append(getBytesFromType(*(bool*)data));
            break;
        case INT:
            bytes.append(getBytesFromType(*(int*)data));
            break;
        case DOUBLE:
            bytes.append(getBytesFromType(*(double*)data));
            break;
        case FLOAT:
            bytes.append(getBytesFromType(*(float*)data));
            break;
        case VECTOR3:
            bytes.append(getBytesFromType(*(Vector3*)data));
            break;
        case STRING:
            bytes.append(getBytesFromType(*(std::string*)data));
            break;
        default:
            throw ObjectException();
        }
        return bytes;
    }

    bool Object::setData(const QByteArray &bytes_)
    {
        Type t = (Type)bytes_[0];
        QByteArray bytes = bytes_.mid(1);
        switch (t)
        {
        case BOOL:
            return set(getTypeFromBytes<bool>(bytes));
        case INT:
            return set(getTypeFromBytes<int>(bytes));
        case DOUBLE:
            return set(getTypeFromBytes<double>(bytes));
        case FLOAT:
            return set(getTypeFromBytes<float>(bytes));
        case VECTOR3:
            return set(getTypeFromBytes<Vector3>(bytes));
        case STRING:
            return set(getTypeFromBytes<std::string>(bytes));
        default:
            throw ObjectException();
        }
    }

}
