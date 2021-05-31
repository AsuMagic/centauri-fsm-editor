#ifndef ONIDEV_CORE_GSL_SPAN
#define ONIDEV_CORE_GSL_SPAN

#include <onidev/core/contract.h>
#include <cstddef>
#include <memory>

// c++11 onidev's version of gsl::span

// https://github.com/martinmoene/gsl-lite/blob/master/include/gsl/gsl-lite.h

namespace od
{
inline namespace gsl
{

template< class T, class U >
inline constexpr T narrow_cast(U && u) noexcept
{
    return static_cast<T>( std::forward<U>( u ) );
}

// tag to select span constructor taking a container
struct with_container_t{
    constexpr with_container_t(){} 
} constexpr with_container;

template<class T>
class span
{
    template<class U> friend class span;

public:
    typedef std::ptrdiff_t index_type;
    typedef T element_type;
    typedef T& reference;
    typedef T* pointer;
    typedef T const* const_pointer;
    typedef T const& const_reference;
    
    typedef pointer iterator;
    typedef const_pointer const_iterator;
    
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef typename std::iterator_traits<iterator>::difference_type difference_type;
    
    span() noexcept:
        _first(nullptr),
        _last (nullptr)
    {
        Expects(size() == 0);
    }

    span(std::nullptr_t, index_type size_in):
        _first(nullptr),
        _last (nullptr)
    {
        Expects(size_in == 0);
    }

    span(reference data_in):
        span(&data_in, 1)
    {
    }
    
    span(element_type &&) = delete;
    
    span(pointer first_in, pointer last_in):
        _first(first_in),
        _last (last_in )
    {
        Expects(first_in <= last_in);
    }

    span(pointer data_in, index_type size_in):
        _first( data_in ),
        _last ( data_in + size_in )
    {
        Expects( size_in == 0 || (size_in > 0 && data_in != nullptr) );
    }

    template<class U>
    span(U*& data_in, index_type size_in):
        _first(data_in),
        _last (data_in + size_in)
    {
        Expects(size_in == 0 || (size_in > 0 && data_in != nullptr));
    }

    template< class U >
    span(U * const & data_in, index_type size_in):
        _first(data_in),
        _last (data_in + size_in)
    {
        Expects(size_in == 0 || (size_in > 0 && data_in != nullptr));
    }
    
    template< class U, size_t N >
    span(U (&arr)[N]):
        _first( &arr[0] ),
        _last ( &arr[0] + N )
    {
    }

    template< class U, size_t N >
    span( std::array< U, N > & arr ):
        _first( arr.data() ),
        _last ( arr.data() + N )
    {
    }

    template< class U, size_t N >
    span( std::array< U, N > const & arr ):
        _first( arr.data() ),
        _last ( arr.data() + N )
    {
    }

    // SFINAE enable only if Cont has a data() member function
    template< class Cont, class = decltype(std::declval<Cont>().data()) >
    span(Cont & cont):
        _first( cont.data() ),
        _last ( cont.data() + cont.size() )
    {
    }
    
    template< class Cont >
    span( with_container_t, Cont & cont ):
        _first( cont.size() == 0 ? nullptr : &cont[0] ),
        _last ( cont.size() == 0 ? nullptr : &cont[0] + cont.size() )
    {
    }
    
    span( std::shared_ptr<element_type> const& ptr ):
        _first( ptr.get() ),
        _last ( ptr.get() ? ptr.get() + 1 : 0 )
    {
    }

    template< class ArrayElementType = typename std::add_pointer<element_type>::type >
    span( std::unique_ptr<ArrayElementType> const & ptr, index_type count ):
        _first( ptr.get() ),
        _last ( ptr.get() + count )
    {
    }

    span( std::unique_ptr<element_type> const & ptr ):
        _first( ptr.get() ),
        _last ( ptr.get() ? ptr.get() + 1 : 0 )
    {
    }

    span(span &&) = default;
    span(span const &) = default;

    template< class U >
    span( span<U> const & other ):
        _first( other.begin() ),
        _last ( other.end() )
    {
    }

    span& operator=( span && ) = default;
    span& operator=( span const & ) = default;

    span first( index_type count ) const noexcept
    {
        Expects( count <= this->size() );
        return span( this->data(), count );
    }

    span last( index_type count ) const noexcept
    {
        Expects( count <= this->size() );
        return span( this->data() + this->size() - count, count );
    }

    span subspan( index_type offset ) const noexcept
    {
        Expects( offset <= this->size() );
        return span( this->data() + offset, this->length() - offset );
    }

    span subspan( index_type offset, index_type count ) const noexcept
    {
        Expects( offset <= this->size() && count <= this->size() - offset );
        return span( this->data() + offset, count );
    }

    iterator begin() const noexcept
    {
        return iterator( _first );
    }

    iterator end() const noexcept
    {
        return iterator( _last );
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator( begin() );
    }

    const_iterator cend() const noexcept
    {
        return const_iterator( end() );
    }

    reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator( end() );
    }

    reverse_iterator rend() const noexcept
    {
        return reverse_iterator( begin() );
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator( cend() );
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator( cbegin() );
    }

    reference operator[]( index_type index ) const
    {
       return at(index);
    }

    reference operator()( index_type index ) const
    {
       return at(index);
    }

    reference at(index_type index) const
    {
       Expects(index < size());
       return _first[index];
    }

    pointer data() const noexcept
    {
        return _first;
    }

    bool empty() const noexcept
    {
        return size() == 0;
    }

    index_type size() const noexcept
    {
        return _last - _first;
    }

    index_type length() const noexcept
    {
        return size();
    }

    index_type size_bytes() const noexcept
    {
        return size() * narrow_cast<index_type>( sizeof( element_type ) );
    }

    index_type length_bytes() const noexcept
    {
        return size_bytes();
    }

    void swap(span& other) noexcept
    {
        using std::swap;
        swap( _first, other._first );
        swap( _last , other._last  );
    }
    
    template<class U>
    span<U> as_span() const noexcept
    {
        Expects( ( this->size_bytes() % sizeof(U) ) == 0 );
        return span< U >( reinterpret_cast<U *>( this->data() ), this->size_bytes() / sizeof( U ) );
    }

private:
    pointer _first;
    pointer _last;
};

}

}

#endif
