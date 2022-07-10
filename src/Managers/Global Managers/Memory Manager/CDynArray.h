////////////////////////////////////////////////////////////////////////////////////////////////
// File :			DynArray.h
// Author :			Joseph Leybovich
// Created :		03.01.10
// Last Modified :	7/26/11
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
template <typename Type>
class CDynArray
{
	EHeapID m_eHeap;			// The Heap the DynArray uses
	unsigned int m_nCount;		// The number of elements currently in the array
	unsigned int m_nCapacity;	// The maximum number of elements the array can hold
	Type* m_pArr;				// Pointer to the array

public:
	///////////////////////////////////////////////////////////////////////////////
	// Function :	Constructor
	// Notes : Constructs an empty array (Size 0 Capacity 0)
	///////////////////////////////////////////////////////////////////////////////
	CDynArray(EHeapID eHeap = HEAPID_DYNARRAY)
	{
		m_eHeap = eHeap;
		m_nCount = m_nCapacity = 0;
		m_pArr = NULL;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function :	Destructor
	// Notes : cleans up any dynamic memory
	///////////////////////////////////////////////////////////////////////////////
	~CDynArray()
	{
		clear();
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function :	Copy Constructor
	///////////////////////////////////////////////////////////////////////////////
	CDynArray(const CDynArray<Type>& that)
	{
		this->m_eHeap = that.m_eHeap;
		this->m_nCount = that.m_nCount;
		this->m_nCapacity = that.m_nCapacity;
		m_pArr = MMNEWARRAYEX(Type, this->m_nCapacity, m_eHeap);
		for(unsigned int i=0; i<m_nCapacity; ++i)
			m_pArr[i] = that.m_pArr[i];
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function :	Assignment Operator
	///////////////////////////////////////////////////////////////////////////////
	CDynArray<Type>& operator=(const CDynArray<Type>& that)
	{
		if (this != &that)		// Paranoia check
		{
			MMDELARRAYEX(m_pArr, this->m_eHeap);
			this->m_nCount = that.m_nCount;
			this->m_nCapacity = that.m_nCapacity;
			m_pArr = MMNEWARRAYEX(Type, m_nCapacity, m_eHeap);
			for(unsigned int i=0; i<m_nCapacity; ++i)
				m_pArr[i] = that.m_pArr[i];
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
	Type& operator[](const unsigned int& index) 
	{ 
		if(index >= m_nCapacity)
			throw; // You're not doing it right

		if(index >= m_nCount)
			m_nCount = index+1;

		return m_pArr[index]; 
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function :	count
	// Returns : int - returns the number of items being stored
	// Notes : this function returns the number of items being stored, 
	///////////////////////////////////////////////////////////////////////////////
	unsigned int size() const 
	{ 
		return m_nCount; 
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function : capacity
	// Returns : int - returns the number of items the array can store before 
	//		the next resize
	// Notes : this function returns the number of items the array can store, 
	//		not the number of bytes
	///////////////////////////////////////////////////////////////////////////////
	unsigned int capacity() const 
	{ 
		return m_nCapacity; 
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function : data
	// Returns : Type* - returns the pointer to the data
	// Notes : this function is intended to be used for reading, not writing
	///////////////////////////////////////////////////////////////////////////////
	Type* data() const 
	{ 
		return m_pArr; 
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function :	clear
	// Notes : cleans up any dynamic memory and resets size and capacity to 0
	///////////////////////////////////////////////////////////////////////////////
	void clear()
	{
		if(m_pArr)
			MMDELARRAYEX(m_pArr, m_eHeap);
		m_nCount = m_nCapacity = 0;
		m_pArr = NULL;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function :	decreaseSize
	// Notes : decreases size
	///////////////////////////////////////////////////////////////////////////////
	void decreaseSize() 
	{ 
		--m_nCount;	
	}

	///////////////////////////////////////////////////////////////////////////////
	// Function : append
	// Parameters : item - the item to be appended to the next open spot
	// Notes : this function will append the item to the next open spot. if 
	//		no room exists, the array's capacity will be doubled and then 
	//		the item will be added
	///////////////////////////////////////////////////////////////////////////////
	void push_back(Type& item)
	{
		if(m_nCount == m_nCapacity)				// Check for full array
			resize();							// Reserve double the space

		m_pArr[m_nCount++] = item;				// Append item and increment size
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
		if(newCap < m_nCapacity && 0 != newCap)
			return;

		// Check for empty
		if(m_nCount == 0)
			m_pArr = MMNEWARRAYEX(Type, 1, m_eHeap);

		Type* tempArr = m_pArr;			// Temp array to hold old data

		// Check for default
		if(0 == newCap)
		{
			// Set Initial Capacity or Double Capacity
			m_nCapacity = m_nCapacity == 0 ? 1 : m_nCapacity << 1;
		}
		else
			m_nCapacity = newCap;				// Set to new capacity

		m_pArr = MMNEWARRAYEX(Type, m_nCapacity, m_eHeap); // Expand array

		// Copy old data
		for(unsigned int i=0; i<m_nCount; ++i)
			m_pArr[i] = tempArr[i];

		MMDELARRAYEX(tempArr, m_eHeap);
	}
};
#endif