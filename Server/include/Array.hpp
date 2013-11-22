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
	
	template <typename T, bool extremeSpeed=true, bool throwExceptions=true>
	class Array
	{
	private:
		Array(const Array<T>&);
		
		Array& operator=(const Array<T>&);
		
		T* values;
		
		unsigned int counter;
		
		unsigned int size;
		
		void deleteAll()
		{
			if(values != NULL)
			{
				delete[] values;
				values = NULL;
			}
			counter = 0;
		}
		
		void initArrays()
		{
			values = new T[size];
			counter = 0;
		}
		
	public:
		Array(const unsigned int _size=0)
			:size(_size), values(NULL), counter(0)
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
				T* newValues = new T[_size];
				unsigned int commonSize = (size<_size?size:_size);
				memcpy(newValues, values, commonSize*sizeof(T));
				if(_size > size)
					for(unsigned int i = size; i<_size; i++)
						newValues[i] = T();
				T* oldValues = values;
				if(_size < size)
				{
					size = _size;
					values = newValues;
				}
				else
				{
					values = newValues;
					size = _size;
				}
				delete[] oldValues;
				oldValues = NULL;
			}
		}
		
		inline T& operator[](const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::operator[] error.";
					if(throwExceptions)
						throw ArrayException();
				}
			}
			return values[index];
		}
		
		inline const T& operator[](const unsigned int index) const
		{
			if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::operator[] const error.";
					if(throwExceptions)
						throw ArrayException();
				}
			}
			return values[index];
		}
		
		inline void addElement(T& element)
		{
			if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::addElement error.";
					if(throwExceptions)
						throw ArrayException();
					return;
				}
			}
			if(counter < size)
				values[counter++] = element;
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
				if(values == NULL)
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
					if(values[i] == element)
						return false;
				}
				values[counter++] = element;
				return true;
			}
			qDebug()<<"Array::addElementIfNotContained error: full.";
			if(throwExceptions)
				throw ArrayException();
			return false;
		}
		
		inline unsigned int count()
		{
			return size;
		}
		
		inline unsigned int getCounter()
		{
			return counter;
		}
		
		inline void resetCounter()
		{
			counter = 0;
		}
		
		void append(unsigned int _size=1)
		{
			changeSize(size+_size);
		}
		
		void remove(unsigned int index)
		{
			if(index >= size)
				return;
			T* newValues = new T[size-1];
			if(index > 0)
				memcpy(newValues, values, index*sizeof(T));
			if(size-index > 1)
				memcpy(&newValues[index], &values[index+1], (size-index-1)*sizeof(T));
			T* oldValues = values;
			size--;
			values = newValues;
			delete[] oldValues;
			oldValues = NULL;
		}
		
		inline T* data()
		{
			return values;
		}
		
		inline const T* constData() const
		{
			return values;
		}
		
		friend class TwoDimArray<T, extremeSpeed, throwExceptions>;
			
	};
}

#endif
