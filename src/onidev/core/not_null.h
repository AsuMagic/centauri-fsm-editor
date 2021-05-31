#ifndef ONIDEV_CORE_GSL_NOT_NULL
#define ONIDEV_CORE_GSL_NOT_NULL

#include <onidev/core/contract.h>
#include <functional> // std::hash
#include <cstddef>

// c++11 onidev's version of gsl::not_null

namespace od
{
inline namespace gsl
{

template<class T>
class not_null
{
public:
    not_null(std::nullptr_t) = delete;
    not_null(T t,
             const int l = __builtin_LINE(),
             const char* f = __builtin_FILE()):
        _ptr(t)
    {
//#if defined(GSL_THROW_ON_CONTRACT_VIOLATION)
//        if(_ptr == nullptr)
//        {
//            std::string err =
//                    "GSL: Precondition failure at " + std::string(f) + 
//                    ": " + od::toString(l) + ": " + __PRETTY_FUNCTION__;
//            throw od::gsl::fail_fast(err.c_str());
//        }
//#endif
        Expects(_ptr != nullptr);
    }
    not_null(not_null const& other): _ptr (other._ptr)
    {
    }
    template< class U, class Dummy = typename std::enable_if<std::is_convertible<U, T>::value, void>::type >
    not_null(not_null<U> const& other): _ptr(other.get())
    {
    }
    
    not_null& operator=(std::nullptr_t) = delete;
    not_null& operator=( T const & t )
    {
        _ptr = t ; 
        Expects(_ptr != nullptr);
        return *this;
    }
    not_null& operator=(not_null const& other)
    {
        _ptr = other._ptr;
        return *this;
    }
    template< class U, class Dummy = typename std::enable_if<std::is_convertible<U, T>::value, void>::type >
    not_null& operator=(not_null<U> const& other)
    {
        _ptr = other.get();
        return *this;
    }
    
    T get() const { return _ptr; }
    operator T() const { return get(); }
    T operator->() const { return get(); }
    const typename std::remove_pointer<T>::type& operator*() const { return *get(); }
    typename std::remove_pointer<T>::type& operator*() { return *_ptr; }

    bool operator==(T const& rhs) const { return    _ptr == rhs; }
    bool operator!=(T const& rhs) const { return !(*this == rhs); }

    not_null& operator++() = delete;
    not_null& operator--() = delete;
    not_null  operator++(int) = delete;
    not_null  operator--(int) = delete;
    not_null& operator+ (size_t) = delete;
    not_null& operator+=(size_t) = delete;
    not_null& operator- (size_t) = delete;
    not_null& operator-=(size_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;
    
private:
    T _ptr;
};

}

}

namespace std
{
template <class T>
struct hash<od::not_null<T>>
{
    std::size_t operator()(const od::not_null<T>& value) const {
        return hash<T>{}(value);
    }
};
}

#endif
