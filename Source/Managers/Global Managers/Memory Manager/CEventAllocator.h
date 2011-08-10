////////////////////////////////////////////////////////////////////////////////
//	File			:	CEventAllocator.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Allocates memory into the Event Heap for STL containers
////////////////////////////////////////////////////////////////////////////////

#ifndef _CEVENTALLOCATOR_H_
#define _CEVENTALLOCATOR_H_

#include <memory>
using namespace std;

#include "CMemoryManager.h"

#if MM_LEAK_DETECTION
// This version of the class uses leak detection

		// TEMPLATE CLASS CEventAllocator
template<class _Ty>
	class CEventAllocator : public _Allocator_base<_Ty>
	{	// generic CEventAllocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert an CEventAllocator<_Ty> to an CEventAllocator <_Other>
		typedef CEventAllocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	CEventAllocator() _THROW0()
		{	// construct default CEventAllocator (do nothing)
		}

	CEventAllocator(const CEventAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CEventAllocator(const CEventAllocator<_Other>&) _THROW0()
		{	// construct from a related CEventAllocator (do nothing)
		}

	template<class _Other>
		CEventAllocator<_Ty>& operator=(const CEventAllocator<_Other>&)
		{	// assign from a related CEventAllocator (do nothing)
		return (*this);
		}
//vv
	void deallocate(pointer _Ptr, size_type)
		{	
			// deallocate object at _Ptr, ignore size
			//::operator delete(_Ptr);
			CMemoryManager::GetInstance()->Deallocate((char*)_Ptr, HEAPID_EVENT);
		}

	pointer allocate(size_type _Count)
		{	
			// allocate array of _Count elements
			//return (_Allocate(_Count, (pointer)0));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_EVENT, __FILE__, __LINE__);
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	
			// allocate array of _Count elements, ignore hint
			//return (allocate(_Count));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_EVENT, __FILE__, __LINE__);
		}
//^^
	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		// CEventAllocator TEMPLATE OPERATORS
template<class _Ty,
	class _Other> inline
	bool operator==(const CEventAllocator<_Ty>&, const CEventAllocator<_Other>&) _THROW0()
	{	// test for CEventAllocator equality (always true)
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const CEventAllocator<_Ty>&, const CEventAllocator<_Other>&) _THROW0()
	{	// test for CEventAllocator inequality (always false)
	return (false);
	}

		// CLASS CEventAllocator<void>
template<> class _CRTIMP2_PURE CEventAllocator<void>
	{	// generic CEventAllocator for type void
public:
	typedef void _Ty;
	typedef _Ty _FARQ *pointer;
	typedef const _Ty _FARQ *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	// convert an CEventAllocator<void> to an CEventAllocator <_Other>
		typedef CEventAllocator<_Other> other;
		};

	CEventAllocator() _THROW0()
		{	// construct default CEventAllocator (do nothing)
		}

	CEventAllocator(const CEventAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CEventAllocator(const CEventAllocator<_Other>&) _THROW0()
		{	// construct from related CEventAllocator (do nothing)
		}

	template<class _Other>
		CEventAllocator<_Ty>& operator=(const CEventAllocator<_Other>&)
		{	// assign from a related CEventAllocator (do nothing)
		return (*this);
		}
	};

#else
// This version of the class does not use leak detection

		// TEMPLATE CLASS CEventAllocator
template<class _Ty>
	class CEventAllocator : public _Allocator_base<_Ty>
	{	// generic CEventAllocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert an CEventAllocator<_Ty> to an CEventAllocator <_Other>
		typedef CEventAllocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	CEventAllocator() _THROW0()
		{	// construct default CEventAllocator (do nothing)
		}

	CEventAllocator(const CEventAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CEventAllocator(const CEventAllocator<_Other>&) _THROW0()
		{	// construct from a related CEventAllocator (do nothing)
		}

	template<class _Other>
		CEventAllocator<_Ty>& operator=(const CEventAllocator<_Other>&)
		{	// assign from a related CEventAllocator (do nothing)
		return (*this);
		}
//vv
	void deallocate(pointer _Ptr, size_type)
		{	
			// deallocate object at _Ptr, ignore size
			//::operator delete(_Ptr);
			CMemoryManager::GetInstance()->Deallocate((char*)_Ptr, HEAPID_EVENT);
		}

	pointer allocate(size_type _Count)
		{	
			// allocate array of _Count elements
			//return (_Allocate(_Count, (pointer)0));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_EVENT);
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	
			// allocate array of _Count elements, ignore hint
			//return (allocate(_Count));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_EVENT);
		}
//^^
	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		// CEventAllocator TEMPLATE OPERATORS
template<class _Ty,
	class _Other> inline
	bool operator==(const CEventAllocator<_Ty>&, const CEventAllocator<_Other>&) _THROW0()
	{	// test for CEventAllocator equality (always true)
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const CEventAllocator<_Ty>&, const CEventAllocator<_Other>&) _THROW0()
	{	// test for CEventAllocator inequality (always false)
	return (false);
	}

		// CLASS CEventAllocator<void>
template<> class _CRTIMP2_PURE CEventAllocator<void>
	{	// generic CEventAllocator for type void
public:
	typedef void _Ty;
	typedef _Ty _FARQ *pointer;
	typedef const _Ty _FARQ *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	// convert an CEventAllocator<void> to an CEventAllocator <_Other>
		typedef CEventAllocator<_Other> other;
		};

	CEventAllocator() _THROW0()
		{	// construct default CEventAllocator (do nothing)
		}

	CEventAllocator(const CEventAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CEventAllocator(const CEventAllocator<_Other>&) _THROW0()
		{	// construct from related CEventAllocator (do nothing)
		}

	template<class _Other>
		CEventAllocator<_Ty>& operator=(const CEventAllocator<_Other>&)
		{	// assign from a related CEventAllocator (do nothing)
		return (*this);
		}
	};

#endif	// MM_LEAK_DETECTION

#endif // _CEVENTALLOCATOR_H_