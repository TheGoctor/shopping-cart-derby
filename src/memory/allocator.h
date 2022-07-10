////////////////////////////////////////////////////////////////////////////////
//	File			:	Allocator.h
//	Date			:	3/29/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Allocates memory into the General Heap for STL containers
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "enums.h"

#include <memory>

namespace scd {

template <class _Ty, heap _heap = heap::general>
class allocator
  : public _Allocator_base<_Ty> {
public:
  typedef _Allocator_base<_Ty> _Mybase;
  typedef typename _Mybase::value_type value_type;
  typedef value_type _FARQ* pointer;
  typedef value_type _FARQ& reference;
  typedef const value_type _FARQ* const_pointer;
  typedef const value_type _FARQ& const_reference;

  typedef _SIZT size_type;
  typedef _PDFT difference_type;

  template <class _Other>
  struct rebind { // convert an scd::allocator<_Ty> to an scd::allocator <_Other>
    typedef scd::allocator<_Other> other;
  };

  pointer address(reference _Val) const { // return address of mutable _Val
    return (&_Val);
  }

  const_pointer
    address(const_reference _Val) const { // return address of nonmutable _Val
    return (&_Val);
  }

  scd::allocator() _THROW0() { // construct default scd::allocator (do nothing)
  }

  scd::allocator(const scd::allocator<_Ty>&)
    _THROW0() { // construct by copying (do nothing)
  }

  template <class _Other>
  scd::allocator(const scd::allocator<_Other>&)
    _THROW0() { // construct from a related scd::allocator (do nothing)
  }

  template <class _Other>
  scd::allocator<_Ty>& operator=(const scd::allocator<_Other>&) { // assign from a
                                                          // related scd::allocator
                                                          // (do nothing)
    return (*this);
  }
  // vv
  void deallocate(pointer _Ptr, size_type) {
    // deallocate object at _Ptr, ignore size
    //::operator delete(_Ptr);
    CMemoryManager::GetInstance()->Deallocate((char*)_Ptr, _heap);
  }

  pointer allocate(size_type _Count) {
    // allocate array of _Count elements
    // return (_Allocate(_Count, (pointer)0));
    return (pointer)CMemoryManager::GetInstance()->Allocate(
      sizeof(_Ty) * _Count, _heap, __FILE__, __LINE__);
  }

  pointer allocate(size_type _Count, const void _FARQ*) {
    // allocate array of _Count elements, ignore hint
    // return (allocate(_Count));
    return (pointer)CMemoryManager::GetInstance()->Allocate(
      sizeof(_Ty) * _Count, _heap, __FILE__, __LINE__);
  }
  //^^
  void construct(pointer _Ptr,
                 const _Ty& _Val) { // construct object at _Ptr with value _Val
    _Construct(_Ptr, _Val);
  }

  void destroy(pointer _Ptr) { // destroy object at _Ptr
    _Destroy(_Ptr);
  }

  _SIZT max_size() const _THROW0() { // estimate maximum array size
    _SIZT _Count = (_SIZT)(-1) / sizeof(_Ty);
    return (0 < _Count ? _Count : 1);
  }
};

// scd::allocator TEMPLATE OPERATORS
template <class _Ty, class _Other>
inline bool operator==(const scd::allocator<_Ty>&, const scd::allocator<_Other>&)
  _THROW0() { // test for scd::allocator equality (always true)
  return (true);
}

template <class _Ty, class _Other>
inline bool operator!=(const scd::allocator<_Ty>&, const scd::allocator<_Other>&)
  _THROW0() { // test for scd::allocator inequality (always false)
  return (false);
}

} // namespace scd
