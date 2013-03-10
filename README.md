safe_ptr
========

A smart pointer with some reference guarantees.

In C++ when passing a non-const reference to a function the possibility of that function modifying the object referenced is somewhat obscured by the fact that on the surface this is indistinguishable from passing by const pointer, const reference or value all of which would guarantee the object remains unchanged by the function. Of course looking at the function signature would reveal the precise nature of the parameter passing but on first look, without closer inspection, the exact method of parameter passing is not obvious.

Take for example the following snippet. While we can guess from the context that 'value' is probably passed by non-const reference, and while we could find out for sure by looking at the signature of Find(), we cannot immediately say for sure how 'value' is passed without further context.

```cpp
if (Find(value, "value")) { cout << "value is: " << value; }
```

Sometimes coding guidelines therefore specify in an attempt to avoid this issue that non-const references must be avoided. However, on closer inspection it becomes clear that non-const pointers suffer from much of the same issue in that, unless the reference operator is used on an instance inline (i.e. Find(&value, "value")), the call is again indistinguishable on the surface from passing by value or const pointer/reference. What we should aim for is a solution that allows us to avoid both non-const references and non-const pointers while maintaining the desirable properties of passing by non-const reference, namely the guarantee of non-null 'pointer' which can significantly simplify the code as no further null-pointer checks are required down the call stack, and immutability.

With that in mind wouldn't it be nice to simply be able to write code like in the following example?

```cpp
if (Find(by_ref(value), "value")) { cout << "value is:" << value; }
```

This would make it crystal clear that 'value' is intended to be passed by reference and therefore no guarantees are being made to the caller by the function as to the object remaining unchanged or in fact indicating to the contrary that the object is expected to change, not unlike <a title="ref (C# Reference)" href="http://msdn.microsoft.com/en-us/library/14akc2c7.aspx" target="_blank">C#'s ref</a> keyword and similar constructs in other languages, while the caller makes a guarantee to the function that the reference is non-null.

Now that we know what we want the parameter passing to look like, lets think about how we can implement the by_ref construct as well as enforce the use of it for non-const pointers/references.

A good starting point is a class template that wraps a pointer and which is only assignable from a reference of the element type:

```cpp
template <class T>
class safe_ptr
{
public:
  safe_ptr(T& r) : p(&r) {}

private:
  T* p;
}
```

Without going into further details of the implementation for the moment this would allow us to write:

```cpp
.. Find(safe_ptr<std::string>(value), "value") ..
```

That's not ideal as we now need to repeat the type of 'value' whenever we pass by reference. The compiler requires this as the template type can not otherwise be deducted. Luckily we can work around this by using a factory-function template like this:

```cpp
template <class T>
safe_ptr<T> by_ref(T& r)
{
  assert(&r);
  return safe_ptr<T>(r);
}
```

Now the complier is able to deduce the type and we can actually write:

```cpp
.. Find(by_ref(value), "value") ..
```

And we can now also require callers to use by_ref() by making sure we use parameters of type safe_ptr in the relevant function signatures. For Find() for example we would declare as follows:

```cpp
bool Find(by_ref<std::string> value, const char* name);
```

You may ask why we wrap a pointer and call the wrapper safe_ptr instead of wrapping a pointer. The reason simply is that it is not currently possible to overload the '.' operator so we can't implement smart_ptr with full reference semantics. However what we can do is implement a pointer wrapper that has pointer semantics but with the guarantees of a reference, hence the name safe_ptr.

