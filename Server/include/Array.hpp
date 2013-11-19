/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ARRAY_HPP_
#define _ARRAY_HPP_

#include <TwoDimArray.hpp>

#include <QDebug>
#include <exception>

namespace SphereSim
{
	template <typename T, bool extremeSpeed, bool throwExceptions>
	class TwoDimArray;
	
	class ArrayException : public std::exception
	{
		const char* what() const noexcept
		{
			return "Array exception.";
		}
	};
	
	template <typename T, bool extremeSpeed=false, bool throwExceptions=true>
	class Array
	{
	private:
		Array();
		
		Array(const Array<T>&);
		
		Array& operator=(const Array<T>&);
		
		T* data;
		
		unsigned int counter;
		
		unsigned int size;
		
		void deleteAll()
		{
			if(data != NULL)
			{
				delete[] data;
				data = NULL;
			}
			counter = 0;
		}
		
		void initArrays()
		{
			data = new T[size];
			counter = 0;
		}
		
	public:
		Array(const unsigned int _size)
			:size(_size), data(NULL), counter(0)
		{
			if(size>0)
				initArrays();
		}
		
		~Array()
		{
			deleteAll();
		}
		
		void changeSize(const unsigned int _size)
		{
			if(_size != size && _size > 0)
			{
				deleteAll();
				size = _size;
				initArrays();
			}
		}
		
		inline T& operator[](const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"Array::operator[] error.";
					if(throwExceptions)
						throw ArrayException();
				}
			}
			return data[index];
		}
		
		inline const T& operator[](const unsigned int index) const
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"Array::operator[] const error.";
					if(throwExceptions)
						throw ArrayException();
				}
			}
			return data[index];
		}
		
		inline void addElement(T& element)
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"Array::addElement error.";
					if(throwExceptions)
						throw ArrayException();
					return;
				}
			}
			if(counter < size)
				data[counter++] = element;
			else
			{
				qDebug()<<"Array::addElement error: full."<<counter<<size;
				if(throwExceptions)
					throw ArrayException();
			}
		}
		
		inline bool addElementIfNotContained(T& element)
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"Array::addElement error.";
					if(throwExceptions)
						throw ArrayException();
					return false;
				}
			}
			if(counter < size)
			{
				for(unsigned int i = 0; i<counter; ++i)
				{
					if(data[i] == element)
						return false;
				}
				data[counter++] = element;
				return true;
			}
			qDebug()<<"Array::addElementIfNotContained error: full.";
			if(throwExceptions)
				throw ArrayException();
			return false;
		}
		
		inline unsigned int getCount()
		{
			return counter;
		}
		
		inline void resetCounter()
		{
			counter = 0;
		}
		
		friend class TwoDimArray<T, extremeSpeed, throwExceptions>;
			
	};
}

#endif
