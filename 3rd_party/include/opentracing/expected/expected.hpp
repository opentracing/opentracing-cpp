// Copyright (C) 2016 Martin Moene.
//
// This version targets C++11 and later.
//
// This code is licensed under the MIT License (MIT).
//
// expected lite is based on:
//   A proposal to add a utility class to represent expected monad - Revision 2
//   by Vicente J. Botet Escriba and Pierre Talbot.

#ifndef OPENTRACING_EXPECTED_LITE_HPP
#define OPENTRACING_EXPECTED_LITE_HPP

#include <cassert>
#include <exception>
#include <functional>
#include <initializer_list>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <opentracing/version.h>

#define  expected_lite_VERSION "0.0.0"

// Compiler detection:

#define nsel_CPP11_OR_GREATER  ( __cplusplus >= 201103L )
#define nsel_CPP14_OR_GREATER  ( __cplusplus >= 201402L )

#if nsel_CPP14_OR_GREATER
# define nsel_constexpr14 constexpr
#else
# define nsel_constexpr14 /*constexpr*/
#endif

// Method enabling

#define nsel_REQUIRES(...) \
    typename std::enable_if<__VA_ARGS__, void*>::type = 0

#define nsel_REQUIRES_0(...) \
    template< bool B = (__VA_ARGS__), typename std::enable_if<B, int>::type = 0 >

#define nsel_REQUIRES_T(...) \
    typename = typename std::enable_if< (__VA_ARGS__), opentracing::expected_detail::enabler >::type

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

template< typename T, typename E >
class expected;

namespace expected_detail {

/// for nsel_REQUIRES_T

enum class enabler{};

/// discriminated union to hold value or 'error'.

template< typename T, typename E >
union storage_t
{
    friend class expected<T,E>;

private:
    typedef T value_type;
    typedef E error_type;

    // no-op construction
    storage_t() {}
    ~storage_t() {}

    void construct_value( value_type const & v )
    {
        new( &m_value ) value_type( v );
    }

    void construct_value( value_type && v )
    {
        new( &m_value ) value_type( std::forward<T>( v ) );
    }

    void destruct_value()
    {
        m_value.~value_type();
    }

    void construct_error( error_type const & e )
    {
        new( &m_error ) error_type( e );
    }

    void construct_error( error_type && e )
    {
        new( &m_error ) error_type( std::move( e ) );
    }

    void destruct_error()
    {
        m_error.~error_type();
    }

    constexpr value_type const & value() const &
    {
        return m_value;
    }

    value_type & value() &
    {
        return m_value;
    }

    constexpr value_type && value() const &&
    {
        return std::move( m_value );
    }

    const value_type * value_ptr() const
    {
        return &m_value;
    }

    value_type * value_ptr()
    {
        return &m_value;
    }

    error_type const & error() const
    {
        return m_error;
    }

    error_type & error()
    {
        return m_error;
    }

private:
    value_type m_value;
    error_type m_error;
};

/// discriminated union to hold only 'error'.

template< typename E >
union storage_t<void, E>
{
    friend class expected<void,E>;

private:
    typedef void value_type;
    typedef E error_type;

    // no-op construction
    storage_t() {}
    ~storage_t() {}

    void construct_error( error_type const & e )
    {
        new( &m_error ) error_type( e );
    }

    void construct_error( error_type && e )
    {
        new( &m_error ) error_type( std::move( e ) );
    }

    void destruct_error()
    {
        m_error.~error_type();
    }

    error_type const & error() const
    {
        return m_error;
    }

    error_type & error()
    {
        return m_error;
    }

private:
    error_type m_error;
};

} // namespace expected_detail

/// class unexpected_type

template< typename E = std::error_code >
class unexpected_type
{
public:
    typedef E error_type;

    unexpected_type() = delete;

    nsel_REQUIRES_0(
        std::is_copy_constructible<error_type>::value )

    constexpr explicit unexpected_type( error_type const & error )
    : m_error( error )
    {}

    nsel_REQUIRES_0(
        std::is_move_constructible<error_type>::value )

    constexpr explicit unexpected_type( error_type && error )
    : m_error( std::move( error ) )
    {}

    constexpr error_type const & value() const
    {
        return m_error;
    }

    error_type & value()
    {
        return m_error;
    }

private:
    error_type m_error;
};

/// class unexpected_type, std::exception_ptr specialization

template<>
class unexpected_type< std::exception_ptr >
{
public:
    typedef std::exception_ptr error_type;

    unexpected_type() = delete;

    ~unexpected_type(){}

    explicit unexpected_type( std::exception_ptr const & error )
    : m_error( error )
    {}

    explicit unexpected_type(std::exception_ptr && error )
    : m_error( std::move( error ) )
    {}

    template< typename E >
    explicit unexpected_type( E error )
    : m_error( std::make_exception_ptr( error ) )
    {}

    std::exception_ptr const & value() const
    {
        return m_error;
    }

    std::exception_ptr & value()
    {
        return m_error;
    }

private:
    std::exception_ptr m_error;
};

// unexpected: relational operators

template< typename E >
constexpr bool operator==( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return x.value() == y.value();
}

template< typename E >
constexpr bool operator!=( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ! ( x == y );
}

template< typename E >
constexpr bool operator<( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return x.value() < y.value();
}

template< typename E >
constexpr bool operator>( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ( y < x );
}

template< typename E >
constexpr bool operator<=( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ! ( y < x  );
}

template< typename E >
constexpr bool operator>=( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ! ( x < y );
}

inline constexpr bool operator<( unexpected_type<std::exception_ptr> const & /*x*/, unexpected_type<std::exception_ptr> const & /*y*/ )
{
    return false;
}

inline constexpr bool operator>( unexpected_type<std::exception_ptr> const & /*x*/, unexpected_type<std::exception_ptr> const & /*y*/ )
{
    return false;
}

inline constexpr bool operator<=( unexpected_type<std::exception_ptr> const & x, unexpected_type<std::exception_ptr> const & y )
{
    return ( x == y );
}

inline constexpr bool operator>=( unexpected_type<std::exception_ptr> const & x, unexpected_type<std::exception_ptr> const & y )
{
    return ( x == y );
}

// unexpected: traits

template <typename E>
struct is_unexpected : std::false_type {};

template <typename E>
struct is_unexpected< unexpected_type<E> > : std::true_type {};

// unexpected: factory

template< typename E>
nsel_constexpr14 auto
make_unexpected( E && v) -> unexpected_type< typename std::decay<E>::type >
{
    return unexpected_type< typename std::decay<E>::type >( v );
}

/*nsel_constexpr14*/ inline auto
make_unexpected_from_current_exception() -> unexpected_type< std::exception_ptr >
{
    return unexpected_type< std::exception_ptr >( std::current_exception() );
}

/// in-place tag: construct a value in-place (should come from std::experimental::optional)

struct in_place_t{};

constexpr in_place_t in_place{};

/// unexpect tag, in_place_unexpected tag: construct an error

struct in_place_unexpected_t{};

constexpr in_place_unexpected_t unexpect{};
constexpr in_place_unexpected_t in_place_unexpected{};

/// expected access error

template< typename E >
class bad_expected_access : public std::logic_error
{
public:
    typedef E error_type;

    explicit bad_expected_access( error_type error )
    : logic_error( "bad_expected_access" )
    , m_error( error )
    {}

    constexpr error_type const & error() const
    {
        return m_error;
    }

    error_type & error()
    {
        return m_error;
    }

private:
    error_type m_error;
};

/// class error_traits

template< typename Error >
struct error_traits
{
    static void rethrow( Error const & e )
    {
        throw bad_expected_access<Error>{ e };
    }
};

template<>
struct error_traits< std::exception_ptr >
{
    static void rethrow( std::exception_ptr const & e )
    {
        std::rethrow_exception( e );
    }
};

template<>
struct error_traits< std::error_code >
{
    static void rethrow( std::error_code const & e )
    {
        throw std::system_error( e );
    }
};

/// class expected

template< typename T, typename E = std::error_code >
class expected
{
public:
    typedef T value_type;
    typedef E error_type;

    // constructors

    nsel_REQUIRES_0(
        std::is_default_constructible<T>::value )

    nsel_constexpr14 expected() noexcept
    (
        std::is_nothrow_default_constructible<T>::value
    )
    : has_value_( true )
    {
        contained.construct_value( value_type() );
    }

//    nsel_REQUIRES_0(
//        std::is_copy_constructible<T>::value &&
//        std::is_copy_constructible<E>::value )

    nsel_constexpr14 expected( expected const & rhs )
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( rhs.contained.value() );
        else               contained.construct_error( rhs.contained.error() );
    }

//    nsel_REQUIRES_0(
//        std::is_move_constructible<T>::value &&
//        std::is_move_constructible<E>::value )

    nsel_constexpr14 expected( expected && rhs )
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( rhs.contained.value() ) );
        else               contained.construct_error( std::move( rhs.contained.error() ) );
    }

    nsel_REQUIRES_0(
        std::is_copy_constructible<T>::value )

    nsel_constexpr14 expected( value_type const & rhs )
    : has_value_( true )
    {
        contained.construct_value( rhs );
    }

    nsel_REQUIRES_0(
        std::is_move_constructible<T>::value )

    nsel_constexpr14 expected( value_type && rhs ) noexcept
    (
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_move_constructible<E>::value
    )
    : has_value_( true )
    {
        contained.construct_value( std::move( rhs ) );
    }

    template <typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, Args&&...>::value ) >

    nsel_constexpr14 explicit expected( in_place_t, Args&&... args )
    : has_value_( true )
    {
        contained.construct_value( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, std::initializer_list<U>, Args&&...>::value ) >

    nsel_constexpr14 explicit expected( in_place_t, std::initializer_list<U> il, Args&&... args )
    : has_value_( true )
    {
        contained.construct_value( il, std::forward<Args>( args )... );
    }

    nsel_REQUIRES_0(
        std::is_copy_constructible<E>::value )

    nsel_constexpr14 expected( unexpected_type<E> const & error )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    nsel_REQUIRES_0(
        std::is_move_constructible<E>::value )

    nsel_constexpr14 expected( unexpected_type<E> && error )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename... Args, nsel_REQUIRES_T(
        std::is_constructible<E, Args&&...>::value ) >

    nsel_constexpr14 explicit expected( in_place_unexpected_t, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, std::initializer_list<U>, Args&&...>::value ) >

    nsel_constexpr14 explicit expected( in_place_unexpected_t, std::initializer_list<U> il, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( il, std::forward<Args>( args )... );
    }

    // destructor

    ~expected()
    {
        if ( has_value() ) contained.destruct_value();
        else               contained.destruct_error();
    }

    // assignment

//    nsel_REQUIRES(
//        std::is_copy_constructible<T>::value &&
//        std::is_copy_assignable<T>::value &&
//        std::is_copy_constructible<E>::value &&
//        std::is_copy_assignable<E>::value )

    expected operator=( expected const & rhs )
    {
        expected( rhs ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_move_constructible<T>::value &&
//        std::is_move_assignable<T>::value &&
//        std::is_move_constructible<E>::value &&
//        std::is_move_assignable<E>::value )

    expected & operator=( expected && rhs ) noexcept
    (
        std::is_nothrow_move_assignable<T>::value &&
        std::is_nothrow_move_constructible<T>::value&&
        std::is_nothrow_move_assignable<E>::value &&
        std::is_nothrow_move_constructible<E>::value )
    {
        expected( std::move( rhs ) ).swap( *this );
        return *this;
    }

    template< typename U, nsel_REQUIRES_T(
        std::is_constructible<T,U>::value &&
        std::is_assignable<T&, U>::value ) >

    expected & operator=( U && v )
    {
        expected( std::forward<U>( v ) ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    expected & operator=( unexpected_type<E> const & u )
    {
        expected( std::move( u ) ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    expected & operator=( unexpected_type<E> && u )
    {
        expected( std::move( u ) ).swap( *this );
        return *this;
    }

    template< typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, Args&&...>::value ) >

    void emplace( Args &&... args )
    {
        expected( in_place, std::forward<Args>(args)... ).swap( *this );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value ) >

    void emplace( std::initializer_list<U> il, Args &&... args )
    {
        expected( in_place, il, std::forward<Args>(args)... ).swap( *this );
    }

//    nsel_REQUIRES(
//        std::is_move_constructible<T>::value &&
//        std::is_move_constructible<E>::value )

    void swap( expected & rhs ) noexcept
    (
        std::is_nothrow_move_constructible<T>::value && noexcept( std::swap( std::declval<T&>(), std::declval<T&>() ) ) &&
        std::is_nothrow_move_constructible<E>::value && noexcept( std::swap( std::declval<E&>(), std::declval<E&>() ) ) )
    {
        using std::swap;

        if      (   bool(*this) &&   bool(rhs) ) { swap( contained.value(), rhs.contained.value() ); }
        else if ( ! bool(*this) && ! bool(rhs) ) { swap( contained.error(), rhs.contained.error() ); }
        else if (   bool(*this) && ! bool(rhs) ) { error_type t( std::move( rhs.error() ) );
                                      // TBD - ??  rhs.contained.destruct_error();
                                                   rhs.contained.construct_value( std::move( contained.value() ) );
                                      // TBD - ??  contained.destruct_value();
                                                   contained.construct_error( std::move( t ) );
                                                   swap( has_value_, rhs.has_value_ ); }
        else if ( ! bool(*this) &&   bool(rhs) ) { rhs.swap( *this ); }
    }

    // observers

    constexpr value_type const * operator ->() const
    {
        return assert( has_value() ), contained.value_ptr();
    }

    value_type * operator ->()
    {
        return assert( has_value() ), contained.value_ptr();
    }

    constexpr value_type const & operator *() const &
    {
        return assert( has_value() ), contained.value();
    }

    value_type & operator *() &
    {
        return assert( has_value() ), contained.value();
    }

    constexpr value_type && operator *() const &&
    {
        return assert( has_value() ), std::move( contained.value() );
    }

    value_type && operator *() &&
    {
        return assert( has_value() ), std::move( contained.value() );
    }


    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    constexpr bool has_value() const noexcept
    {
        return has_value_;
    }

    constexpr value_type const & value() const &
    {
        return has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() );
    }

    value_type & value() &
    {
        return has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() );
    }

    value_type && value() &&
    {
        return has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() );
    }

    constexpr error_type const & error() const &
    {
        return assert( ! has_value() ), contained.error();
    }

    error_type & error() &
    {
        return assert( ! has_value() ), contained.error();
    }

    constexpr error_type && error() const &&
    {
        return assert( ! has_value() ), std::move( contained.error() );
    }

    constexpr unexpected_type<E> get_unexpected() const
    {
        return make_unexpected( contained.error() );
    }

    template< typename Ex >
    bool has_exception() const
    {
        return ! has_value() && std::is_base_of< Ex, decltype( get_unexpected().value() ) >::value;
    }

    template< typename U, nsel_REQUIRES_T(
        std::is_copy_constructible<T>::value &&
        std::is_convertible<U&&, T>::value ) >

    value_type value_or( U && v ) const &
    {
        return has_value()
            ? contained.value()
            : static_cast<T>( std::forward<U>( v ) );
    }

    template< typename U, nsel_REQUIRES_T(
        std::is_move_constructible<T>::value &&
        std::is_convertible<U&&, T>::value ) >

    value_type value_or( U && v ) &&
    {
        return has_value()
            ? std::move( contained.value() )
            : static_cast<T>( std::forward<U>( v ) );
    }

    // unwrap()

//  template <class U, class E>
//  constexpr expected<U,E> expected<expected<U,E>,E>::unwrap() const&;

//  template <class T, class E>
//  constexpr expected<T,E> expected<T,E>::unwrap() const&;

//  template <class U, class E>
//  expected<U,E> expected<expected<U,E>, E>::unwrap() &&;

//  template <class T, class E>
//  template expected<T,E> expected<T,E>::unwrap() &&;

    // factories

//  template <typename Ex, typename F>
//  expected<T,E> catch_exception(F&& f);

//  template <typename F>
//  expected<decltype(func(declval<T>())),E> map(F&& func) ;

//  template <typename F>
//  'see below' bind(F&& func);

//  template <typename F>
//  expected<T,E> catch_error(F&& f);

//  template <typename F>
//  'see below' then(F&& func);

private:
    bool has_value_;
    expected_detail::storage_t<T,E> contained;
};

/// class expected, void specialization

template< typename E >
class expected<void, E>
{
public:
    typedef void value_type;
    typedef E error_type;

    template< typename U >
    struct rebind
    {
        typedef expected<U, error_type> type;
    };

    // constructors

    constexpr expected() noexcept
    : has_value_( true )
    {
    }

   // nsel_REQUIRES_0(
   //     std::is_copy_constructible<E>::value )

    nsel_constexpr14 expected( expected const & rhs )
    : has_value_( rhs.has_value_ )
    {
        if ( ! has_value() ) contained.construct_error( rhs.contained.error() );
    }

    nsel_REQUIRES_0(
        std::is_move_constructible<E>::value )

    nsel_constexpr14 expected( expected && rhs ) noexcept
    (
        true    // TBD - see also non-void specialization
    )
    : has_value_( rhs.has_value_ )
    {
        if ( ! has_value() ) contained.construct_error( std::move( rhs.contained.error() ) );
    }

    //nsel_REQUIRES_0(
    //    std::is_default_constructible<E>::value )

    constexpr explicit expected( in_place_t )
    : has_value_( true )
    {
    }

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    nsel_constexpr14 expected( unexpected_type<E> const & error )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    // ?? expected( unexpected_type<E> && error )

    template <class Err>
    nsel_constexpr14 expected( unexpected_type<Err> const & error )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

   // destructor

    ~expected()
    {
        if ( ! has_value() ) contained.destruct_error();
    }

    // assignment

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_copy_assignable<E>::value )

    expected & operator=(expected const & rhs )
    {
        expected( rhs ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_move_constructible<E>::value &&
//        std::is_move_assignable<E>::value )

    expected & operator=( expected && rhs ) noexcept
    (
        std::is_nothrow_move_assignable<E>::value &&
        std::is_nothrow_move_constructible<E>::value )
    {
        expected( std::move( rhs ) ).swap( *this );
        return *this;
    }

    void emplace()
    {}

    // swap

//    nsel_REQUIRES(
//        std::is_move_constructible<E>::value )

    void swap( expected & rhs ) noexcept
    (
        std::is_nothrow_move_constructible<E>::value && noexcept( std::swap( std::declval<E&>(), std::declval<E&>() ) )
    )
    {
        using std::swap;

        if      ( ! bool(*this) && ! bool(rhs) ) { swap( contained.error(), rhs.contained.error() ); }
        else if (   bool(*this) && ! bool(rhs) ) { contained.construct_error( std::move( rhs.error() ) );
                                                   swap( has_value_, rhs.has_value_ ); }
        else if ( ! bool(*this) &&   bool(rhs) ) { rhs.swap( *this ); }
    }

    // observers

    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    constexpr bool has_value() const noexcept
    {
        return has_value_;
    }

    void value() const
    {}

    constexpr error_type const & error() const &
    {
        return assert( ! has_value() ), contained.error();
    }

    error_type & error() &
    {
        return assert( ! has_value() ), contained.error();
    }

    constexpr error_type && error() const &&
    {
        return assert( ! has_value() ), std::move( contained.error() );
    }

    constexpr unexpected_type<error_type> get_unexpected() const
    {
        return make_unexpected( contained.error() );
    }

    template <typename Ex>
    bool has_exception() const
    {
        return ! has_value() && std::is_base_of< Ex, decltype( get_unexpected().value() ) >::value;
    }

//  template constexpr 'see below' unwrap() const&;
//
//  template 'see below' unwrap() &&;

    // factories

//  template <typename Ex, typename F>
//  expected<void,E> catch_exception(F&& f);
//
//  template <typename F>
//  expected<decltype(func()), E> map(F&& func) ;
//
//  template <typename F>
//  'see below' bind(F&& func) ;
//
//  template <typename F>
//  expected<void,E> catch_error(F&& f);
//
//  template <typename F>
//  'see below' then(F&& func);

private:
    bool has_value_;
    expected_detail::storage_t<void,E> contained;
};

// expected: relational operators

template <typename T, typename E>
constexpr bool operator==( expected<T,E> const & x, expected<T,E> const & y )
{
    return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

template <typename T, typename E>
constexpr bool operator!=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(x == y);
}

template <typename T, typename E>
constexpr bool operator<( expected<T,E> const & x, expected<T,E> const & y )
{
    return (!y) ? false : (!x) ? true : *x < *y;
}

template <typename T, typename E>
constexpr bool operator>( expected<T,E> const & x, expected<T,E> const & y )
{
    return (y < x);
}

template <typename T, typename E>
constexpr bool operator<=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(y < x);
}

template <typename T, typename E>
constexpr bool operator>=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(x < y);
}

// expected: comparison with unexpected_type

template <typename T, typename E>
constexpr bool operator==( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return (!x) ? x.get_unexpected() == u : false;
}

template <typename T, typename E>
constexpr bool operator==( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ( x == u );
}

template <typename T, typename E>
constexpr bool operator!=( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ! ( x == u );
}

template <typename T, typename E>
constexpr bool operator!=( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ! ( x == u );
}

template <typename T, typename E>
constexpr bool operator<( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return (!x) ? ( x.get_unexpected() < u ) : false;
}

template <typename T, typename E>
constexpr bool operator<( unexpected_type<E> const & u, expected<T,E> const & x )
{
  return (!x) ? ( u < x.get_unexpected() ) : true ;
}

template <typename T, typename E>
constexpr bool operator>( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ( u < x );
}

template <typename T, typename E>
constexpr bool operator>( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ( x < u );
}

template <typename T, typename E>
constexpr bool operator<=( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ! ( u < x );
}

template <typename T, typename E>
constexpr bool operator<=( unexpected_type<E> const & u, expected<T,E> const & x)
{
    return ! ( x < u );
}

template <typename T, typename E>
constexpr bool operator>=( expected<T,E> const & x, unexpected_type<E> const & u  )
{
    return ! ( u > x );
}

template <typename T, typename E>
constexpr bool operator>=( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ! ( x > u );
}

// expected: comparison with T

template <typename T, typename E>
constexpr bool operator==( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x == v : false;
}

template <typename T, typename E>
constexpr bool operator==(T const & v, expected<T,E> const & x )
{
    return bool(x) ? v == *x : false;
}

template <typename T, typename E>
constexpr bool operator!=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x != v : true;
}

template <typename T, typename E>
constexpr bool operator!=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? v != *x : true;
}

template <typename T, typename E>
constexpr bool operator<( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x < v : true;
}

template <typename T, typename E>
constexpr bool operator<( T const & v, expected<T,E> const & x )
{
    return bool(x) ? v < *x : false;
}

template <typename T, typename E>
constexpr bool operator>( T const & v, expected<T,E> const & x )
{
    return bool(x) ? *x < v : false;
}

template <typename T, typename E>
constexpr bool operator>( expected<T,E> const & x, T const & v )
{
    return bool(x) ? v < *x : false;
}

template <typename T, typename E>
constexpr bool operator<=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? ! ( *x < v ) : false;
}

template <typename T, typename E>
constexpr bool operator<=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? ! ( v < *x ) : true;
}

template <typename T, typename E>
constexpr bool operator>=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? ! ( *x < v ) : false;
}

template <typename T, typename E>
constexpr bool operator>=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? ! ( v < *x ) : true;
}

// expected: specialized algorithms

template< typename T, typename E >
void swap( expected<T,E> & x, expected<T,E> & y ) noexcept( noexcept( x.swap(y) ) )
{
    x.swap( y );
}

template< typename T>
constexpr auto make_expected( T && v ) -> expected< typename std::decay<T>::type >
{
    return expected< typename std::decay<T>::type >( std::forward<T>( v ) );
}

// expected<void> specialization:

inline auto make_expected() -> expected<void>
{
    return expected<void>( in_place );
}

template< typename T, typename E >
constexpr auto make_expected_from_error( E e ) -> expected<T, typename std::decay<E>::type>
{
    return expected<T, typename std::decay<E>::type>( make_unexpected( e ) );
}

END_OPENTRACING_ABI_NAMESPACE
} // namespace opentracing

namespace std {

// expected: hash support

template< typename T, typename E >
struct hash< opentracing::expected<T,E> >
{
    typedef typename hash<T>::result_type result_type;
    typedef opentracing::expected<T,E> argument_type;

    constexpr result_type operator()(argument_type const & arg) const
    {
        return arg ? std::hash<T>{}(*arg) : result_type{};
    }
};

// TBD - ?? remove? see spec.
template< typename T, typename E >
struct hash< opentracing::expected<T&,E> >
{
    typedef typename hash<T>::result_type result_type;
    typedef opentracing::expected<T&,E> argument_type;

    constexpr result_type operator()(argument_type const & arg) const
    {
        return arg ? std::hash<T>{}(*arg) : result_type{};
    }
};

// TBD - implement
// bool(e), hash<expected<void,E>>()(e) shall evaluate to the hashing true;
// otherwise it evaluates to an unspecified value if E is exception_ptr or
// a combination of hashing false and hash<E>()(e.error()).

template< typename E >
struct hash< opentracing::expected<void,E> >
{
};

} // namespace std

#undef nsel_REQUIRES
#undef nsel_REQUIRES_0
#undef nsel_REQUIRES_T

#endif // OPENTRACING_EXPECTED_LITE_HPP
