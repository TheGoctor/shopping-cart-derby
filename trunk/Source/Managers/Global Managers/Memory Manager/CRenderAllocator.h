////////////////////////////////////////////////////////////////////////////////
//	File			:	CRenderAllocator.h
//	Date			:	3/29/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Allocates memory into the Render Heap for STL containers
////////////////////////////////////////////////////////////////////////////////

#ifndef _CRENDERALLOCATOR_H_
#define _CRENDERALLOCATOR_H_

#include <memory>
using namespace std;

#include "CMemoryManager.h"

#if MM_LEAK_DETECTION
// This version of the class uses leak detection

		// TEMPLATE CLASS CRenderAllocator
template<class _Ty>
	class CRenderAllocator : public _Allocator_base<_Ty>
	{	// generic CRenderAllocator for objects of class _Ty
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
		{	// convert an CRenderAllocator<_Ty> to an CRenderAllocator <_Other>
		typedef CRenderAllocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	CRenderAllocator() _THROW0()
		{	// construct default CRenderAllocator (do nothing)
		}

	CRenderAllocator(const CRenderAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CRenderAllocator(const CRenderAllocator<_Other>&) _THROW0()
		{	// construct from a related CRenderAllocator (do nothing)
		}

	template<class _Other>
		CRenderAllocator<_Ty>& operator=(const CRenderAllocator<_Other>&)
		{	// assign from a related CRenderAllocator (do nothing)
		return (*this);
		}
//vv
	void deallocate(pointer _Ptr, size_type)
		{	
			// deallocate object at _Ptr, ignore size
			//::operator delete(_Ptr);
			CMemoryManager::GetInstance()->Deallocate((char*)_Ptr, HEAPID_RENDER);
		}

	pointer allocate(size_type _Count)
		{	
			// allocate array of _Count elements
			//return (_Allocate(_Count, (pointer)0));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_RENDER, __FILE__, __LINE__);
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	
			// allocate array of _Count elements, ignore hint
			//return (allocate(_Count));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_RENDER, __FILE__, __LINE__);
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

		// CRenderAllocator TEMPLATE OPERATORS
template<class _Ty,
	class _Other> inline
	bool operator==(const CRenderAllocator<_Ty>&, const CRenderAllocator<_Other>&) _THROW0()
	{	// test for CRenderAllocator equality (always true)
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const CRenderAllocator<_Ty>&, const CRenderAllocator<_Other>&) _THROW0()
	{	// test for CRenderAllocator inequality (always false)
	return (false);
	}

		// CLASS CRenderAllocator<void>
template<> class _CRTIMP2_PURE CRenderAllocator<void>
	{	// generic CRenderAllocator for type void
public:
	typedef void _Ty;
	typedef _Ty _FARQ *pointer;
	typedef const _Ty _FARQ *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	// convert an CRenderAllocator<void> to an CRenderAllocator <_Other>
		typedef CRenderAllocator<_Other> other;
		};

	CRenderAllocator() _THROW0()
		{	// construct default CRenderAllocator (do nothing)
		}

	CRenderAllocator(const CRenderAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CRenderAllocator(const CRenderAllocator<_Other>&) _THROW0()
		{	// construct from related CRenderAllocator (do nothing)
		}

	template<class _Other>
		CRenderAllocator<_Ty>& operator=(const CRenderAllocator<_Other>&)
		{	// assign from a related CRenderAllocator (do nothing)
		return (*this);
		}
	};

#else
// This version of the class does not use leak detection

		// TEMPLATE CLASS CRenderAllocator
template<class _Ty>
	class CRenderAllocator : public _Allocator_base<_Ty>
	{	// generic CRenderAllocator for objects of class _Ty
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
		{	// convert an CRenderAllocator<_Ty> to an CRenderAllocator <_Other>
		typedef CRenderAllocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	CRenderAllocator() _THROW0()
		{	// construct default CRenderAllocator (do nothing)
		}

	CRenderAllocator(const CRenderAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CRenderAllocator(const CRenderAllocator<_Other>&) _THROW0()
		{	// construct from a related CRenderAllocator (do nothing)
		}

	template<class _Other>
		CRenderAllocator<_Ty>& operator=(const CRenderAllocator<_Other>&)
		{	// assign from a related CRenderAllocator (do nothing)
		return (*this);
		}
//vv
	void deallocate(pointer _Ptr, size_type)
		{	
			// deallocate object at _Ptr, ignore size
			//::operator delete(_Ptr);
			CMemoryManager::GetInstance()->Deallocate((char*)_Ptr, HEAPID_RENDER);
		}

	pointer allocate(size_type _Count)
		{	
			// allocate array of _Count elements
			//return (_Allocate(_Count, (pointer)0));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_RENDER);
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	
			// allocate array of _Count elements, ignore hint
			//return (allocate(_Count));
			return (pointer)CMemoryManager::GetInstance()->Allocate(sizeof(_Ty)*_Count, HEAPID_RENDER);
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

		// CRenderAllocator TEMPLATE OPERATORS
template<class _Ty,
	class _Other> inline
	bool operator==(const CRenderAllocator<_Ty>&, const CRenderAllocator<_Other>&) _THROW0()
	{	// test for CRenderAllocator equality (always true)
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const CRenderAllocator<_Ty>&, const CRenderAllocator<_Other>&) _THROW0()
	{	// test for CRenderAllocator inequality (always false)
	return (false);
	}

		// CLASS CRenderAllocator<void>
template<> class _CRTIMP2_PURE CRenderAllocator<void>
	{	// generic CRenderAllocator for type void
public:
	typedef void _Ty;
	typedef _Ty _FARQ *pointer;
	typedef const _Ty _FARQ *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	// convert an CRenderAllocator<void> to an CRenderAllocator <_Other>
		typedef CRenderAllocator<_Other> other;
		};

	CRenderAllocator() _THROW0()
		{	// construct default CRenderAllocator (do nothing)
		}

	CRenderAllocator(const CRenderAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		CRenderAllocator(const CRenderAllocator<_Other>&) _THROW0()
		{	// construct from related CRenderAllocator (do nothing)
		}

	template<class _Other>
		CRenderAllocator<_Ty>& operator=(const CRenderAllocator<_Other>&)
		{	// assign from a related CRenderAllocator (do nothing)
		return (*this);
		}
	};

#endif	// MM_LEAK_DETECTION

#endif // _CRENDERALLOCATOR_H_