/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _TWODIMARRAY_HPP_
#define _TWODIMARRAY_HPP_

#include <Array.hpp>

namespace SphereSim
{
	class ArrayException;
	
	template <typename T, bool extremeSpeed=true, bool throwExceptions=true>
	class TwoDimArray
	{
	private:
		TwoDimArray(const TwoDimArray<T>&);
		
		TwoDimArray& operator=(const TwoDimArray<T>&);
		
		Array<T> values;
		
		T** subArrays;
		
		unsigned int* counter;
		
		const unsigned int constInnerSize;
		
		unsigned int outerSize;
		
		void deleteAll()
		{
			if(counter != NULL)
			{
				delete[] counter;
				counter = NULL;
			}
			if(subArrays != NULL)
			{
				delete[] subArrays;
				subArrays = NULL;
			}
		}
		
		void initArrays()
		{
			unsigned int totalSize = constInnerSize * outerSize;
			values.resize(totalSize);
			subArrays = new T*[outerSize];
			counter = new unsigned int[outerSize];
			for(unsigned int i = 0; i<outerSize; i++)
			{
				counter[i] = 0;
				subArrays[i] = &values[constInnerSize*i];
			}
		}
		
	public:
		TwoDimArray(const unsigned int size)
			:constInnerSize(size), outerSize(0), values(0), counter(NULL), subArrays(NULL)
		{
		}
		
		TwoDimArray(const unsigned int size, const unsigned int size2)
			:constInnerSize(size), outerSize(size2), values(0), counter(NULL), subArrays(NULL)
		{
			if(size2 > 0)
				initArrays();
		}
		
		~TwoDimArray()
		{
			deleteAll();
		}
		
		void resize(const unsigned int size)
		{
			if(size != outerSize && size > 0)
			{
				deleteAll();
				outerSize = size;
				values.resize(size);
				initArrays();
			}
		}
		
		inline T* operator[](const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(index >= outerSize || subArrays == NULL || subArrays[index] == NULL)
				{
					qDebug()<<"TwoDimArray::operator[] error.";
					if(throwExceptions)
						throw ArrayException();
					return NULL;
				}
			}
			return subArrays[index];
		}
		
		inline const T* operator[](const unsigned int index) const
		{
			if(!extremeSpeed)
			{
				if(index >= outerSize || subArrays == NULL || subArrays[index] == NULL)
				{
					qDebug()<<"TwoDimArray::operator[] const error.";
					if(throwExceptions)
						throw ArrayException();
					return NULL;
				}
			}
			return subArrays[index];
		}
		
		inline void addElement(const unsigned int index, T& element)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize || subArrays == NULL || subArrays[index] == NULL)
				{
					qDebug()<<"TwoDimArray::addElement error.";
					if(throwExceptions)
						throw ArrayException();
					return;
				}
			}
			if(counter[index] < constInnerSize)
				subArrays[index][counter[index]++] = element;
			else
			{
				qDebug()<<"TwoDimArray::addElement error: full."<<counter[index]<<constInnerSize;
				if(throwExceptions)
					throw ArrayException();
			}
		}
		
		inline bool addElementIfNotContained(const unsigned int index, T& element)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize || subArrays == NULL || subArrays[index] == NULL)
				{
					qDebug()<<"TwoDimArray::addElementIfNotContained error.";
					if(throwExceptions)
						throw ArrayException();
					return false;
				}
			}
			if(counter[index] < constInnerSize)
			{
				T* subArray = subArrays[index];
				for(unsigned int i = 0; i<counter[index]; ++i)
				{
					if(subArray[i] == element)
						return false;
				}
				subArray[counter[index]++] = element;
				return true;
			}
			qDebug()<<"TwoDimArray::addElementIfNotContained error: full.";
			if(throwExceptions)
				throw ArrayException();
			return false;
		}
		
		inline unsigned int count()
		{
			return outerSize;
		}
		
		inline unsigned int getCounter(const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize)
				{
					qDebug()<<"TwoDimArray::getCount error.";
					if(throwExceptions)
						throw ArrayException();
					return 0;
				}
			}
			return counter[index];
		}
		
		inline void resetCounter(const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize)
				{
					qDebug()<<"TwoDimArray::resetCounter error.";
					if(throwExceptions)
						throw ArrayException();
					return;
				}
			}
			counter[index] = 0;
		}
		
		inline T* data()
		{
			return values;
		}
		
		inline const T* constData() const
		{
			return values;
		}
			
	};
}

#endif
