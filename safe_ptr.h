
#ifndef __SAFE_PTR_H__
#define __SAFE_PTR_H__

#include <cassert>

template <class T>
class safe_ptr
{
public:

  typedef T element_type;

  explicit safe_ptr(element_type& r) noexcept : p(&r) {}

  template <class Y>
  safe_ptr(const safe_ptr<Y>& r) noexcept : p(&*r) {}

  template <class Y>
  safe_ptr& operator=(const safe_ptr<Y>& r) noexcept
  {
    p = &*r;
    return *this;
  }

  element_type& operator*() const noexcept { return *p; }
  element_type* operator->() const noexcept { return p; }

  explicit operator bool() const noexcept { return true; }

private:

  element_type* p;
};

template <class T>
safe_ptr<T> by_ref(T& r)
{
  assert(&r); // Making extra sure the caller hasn't done evil reference manipulation.
  return safe_ptr<T>(r);
}

#endif // __SAFE_PTR_H__
