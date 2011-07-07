////////////////////////////////////////////////////////////////////////////////////////////////
// File :			DynArray.h
// Author :			Joseph Leybovich
// Created :		03.01.10
// Last Modified :	03.03.10
// Purpose :		A dynamic array whioh uses the Memory Manager
// Notes :			
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// HEADER PROTECTION
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DYNARRAY_H_
#define DYNARRAY_H_

////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////////////////////
#include "CMemoryManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Type = int>
class CDynArray
{
	unsigned int m_size, m_capacity;
	Type* arr;
	
public:
		///////////////////////////////////////////////////////////////////////////////
		// Function :	Constructor
		// Notes : Constructs an empty array (Size 0 Capacity 0)
		///////////////////////////////////////////////////////////////////////////////
		CDynArray()
		{
			m_size = m_capacity = 0;
			arr = NULL;
		}

		///////////////////////////////////////////////////////////////////////////////
		// Function :	Destructor
		// Notes : cleans up any dynamic memory
		///////////////////////////////////////////////////////////////////////////////
		~CDynArray()
		{
			if(arr)
				MMDELARRAYEX(arr, HEAPID_GENERAL);
		}

		///////////////////////////////////////////////////////////////////////////////
		// Function :	Copy Constructor
		///////////////////////////////////////////////////////////////////////////////
		CDynArray(const CDynArray<Type>& that)
		{
			this->m_size = that.size();
			this->m_capacity = that.capacity();
			arr = MMNEWARRAYEX(Type, capacity(), HEAPID_GENERAL);
			for(unsigned int i = 0; i < size(); ++i)
			{
				this->arr[i] = that.arr[i];
			}
		}

		///////////////////////////////////////////////////////////////////////////////
		// Function :	Assignment Operator
		///////////////////////////////////////////////////////////////////////////////
		CDynArray<Type>& operator=(const CDynArray<Type>& that)
		{
			if (this != &that)		// Paranoia check
			{
				this->m_size = that.size();
				this->m_capacity = that.capacity();
				arr = MMNEWARRAYEX(Type, capacity(), HEAPID_GENERAL);
				for(unsigned int i = 0; i < size(); ++i)
				{
					this->arr[i] = that.arr[i];
				}
			}
			return *this;
		}

		///////////////////////////////////////////////////////////////////////////////
		// Function : operator[]
		// Parameters : index - the index to access
		// Return : Type & - returns the array element at [index]
		// Notes : performs no error checking. user should ensure index is 
		//		valid with the size() method
		///////////////////////////////////////////////////////////////////////////////
		Type& operator[](const unsigned int& index) { return arr[index]; }

		///////////////////////////////////////////////////////////////////////////////
		// Function :	size
		// Returns : int - returns the number of items being stored
		// Notes : this function returns the number of items being stored, 
		///////////////////////////////////////////////////////////////////////////////
		unsigned int size() const { return m_size; }

		///////////////////////////////////////////////////////////////////////////////
		// Function : capacity
		// Returns : int - returns the number of items the array can store before 
		//		the next resize
		// Notes : this function returns the number of items the array can store, 
		//		not the number of bytes
		///////////////////////////////////////////////////////////////////////////////
		unsigned int capacity() const { return m_capacity; }

		///////////////////////////////////////////////////////////////////////////////
		// Function :	clear
		// Notes : cleans up any dynamic memory and resets size and capacity to 0
		///////////////////////////////////////////////////////////////////////////////
		void clear()
		{
			if(arr)
				MMDELARRAYEX(arr, HEAPID_GENERAL);
			m_size = m_capacity = 0;
			arr = NULL;
		}

		///////////////////////////////////////////////////////////////////////////////
		// Function :	decreaseSize
		// Notes : decreases size
		///////////////////////////////////////////////////////////////////////////////
		void decreaseSize() { m_size--;	}

		///////////////////////////////////////////////////////////////////////////////
		// Function : append
		// Parameters : item - the item to be appended to the next open spot
		// Notes : this function will append the item to the next open spot. if 
		//		no room exists, the array's capacity will be doubled and then 
		//		the item will be added
		///////////////////////////////////////////////////////////////////////////////
		void push_back(const Type& item)
		{
			if(size() == capacity())				// Check for full array
				resize();							// Reserve double the space

			arr[size()] = item;						// Append item
			m_size++;								// Increment size
		}

		///////////////////////////////////////////////////////////////////////////////
		// Function : reserve
		// Parameters : newCap - the new capacity
		// Notes : 	- default parameter - reserve more space in the array, based on the 
		//		expansion rate (100%, 1 minimum).
		//		- non-default parameter, expand to the specified capacity
		//		- if newCap is LESS than the current capacity, do nothing. 
		//		This function should NOT make the array smaller.
		///////////////////////////////////////////////////////////////////////////////
		void resize(const unsigned int & newCap = 0)
		{
			// Check for shrink
			if(newCap < capacity() && 0 != newCap)
				return;

			// Check for empty
			if(m_size == 0)
				arr = MMNEWARRAYEX(Type, 1, HEAPID_GENERAL);

			Type* tempArr = arr;			// Temp array to hold old data
			
			// Check for default
			if(0 == newCap)
			{
				if(0 == capacity())	
				{
					m_capacity = 1;					// Set inital capacity
				}
				else
					m_capacity = capacity() * 2;	// Double capacity
			}
			else
				m_capacity = newCap;				// Set to new capacity

			arr = MMNEWARRAYEX(Type, capacity(), HEAPID_GENERAL); // Expand array

			// Copy old data
			for(unsigned int i = 0; i < size(); ++i)
				arr[i] = tempArr[i];

			MMDELARRAYEX(tempArr, HEAPID_GENERAL);
		}
};
#endif