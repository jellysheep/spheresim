/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _TWODIMARRAY_HPP_
#define _TWODIMARRAY_HPP_

namespace SphereSim
{
	template <typename T, bool extremeSpeed=false>
	class TwoDimArray
	{
	private:
		TwoDimArray(const TwoDimArray<T>&);
		
		TwoDimArray& operator=(const TwoDimArray<T>&);
		
		T** data;
		
		unsigned int* counter;
		
		const unsigned int constInnerSize;
		
		unsigned int outerSize;
		
		void deleteAll()
		{
			if(data != NULL)
			{
				for(unsigned int i = 0; i<outerSize; i++)
				{
					if(data[i] != NULL)
					{
						delete[] data[i];
						data[i] = NULL;
					}
				}
				delete[] data;
				data = NULL;
			}
			if(counter != NULL)
			{
				delete[] counter;
				counter = NULL;
			}
		}
		
		void initArrays()
		{
			data = new T*[outerSize];
			counter = new unsigned int[outerSize];
			for(unsigned int i = 0; i<outerSize; i++)
			{
				data[i] = new T[constInnerSize];
				counter[i] = 0;
			}
		}
		
	public:
		TwoDimArray(const unsigned int size)
			:constInnerSize(size), outerSize(0), data(NULL), counter(NULL)
		{
		}
		
		TwoDimArray(const unsigned int size, const unsigned int size2)
			:constInnerSize(size), outerSize(size2), data(NULL), counter(NULL)
		{
			if(size2 > 0)
				initArrays();
		}
		
		~TwoDimArray()
		{
			deleteAll();
		}
		
		void changeSize(const unsigned int size)
		{
			if(size != outerSize && size > 0)
			{
				deleteAll();
				outerSize = size;
				initArrays();
			}
		}
		
		inline T* operator[](const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"TwoDimArray::operator[] error.";
					return NULL;
				}
			}
			return data[index];
		}
		
		inline const T* operator[](const unsigned int index) const
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"TwoDimArray::operator[] const error.";
					return NULL;
				}
			}
			return data[index];
		}
		
		inline void addElement(const unsigned int index, T& element)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || data == NULL || index >= outerSize || data[index] == NULL)
				{
					qDebug()<<"TwoDimArray::addElement error.";
					return;
				}
			}
			if(counter[index] < constInnerSize)
				data[index][counter[index]++] = element;
		}
		
		inline unsigned int getCount(const unsigned int index)
		{
			
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize)
				{
					qDebug()<<"TwoDimArray::getCount error.";
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
					return;
				}
			}
			counter[index] = 0;
		}
			
	};
}

#endif
