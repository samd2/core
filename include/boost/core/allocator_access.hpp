/*
Copyright 2020 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef BOOST_CORE_ALLOCATOR_ACCESS_HPP
#define BOOST_CORE_ALLOCATOR_ACCESS_HPP

#include <boost/config.hpp>
#if !defined(BOOST_NO_CXX11_ALLOCATOR)
#include <memory>
#else
#include <new>
#if !defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
#include <type_traits>
#endif
#endif
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#include <utility>
#endif

namespace boost {

template<class A>
struct allocator_value_type {
    typedef typename A::value_type type;
};

#if !defined(BOOST_NO_CXX11_ALLOCATOR)
template<class A>
struct allocator_pointer {
    typedef typename std::allocator_traits<A>::pointer type;
};

template<class A>
struct allocator_const_pointer {
    typedef typename std::allocator_traits<A>::const_pointer type;
};

template<class A>
struct allocator_void_pointer {
    typedef typename std::allocator_traits<A>::void_pointer type;
};

template<class A>
struct allocator_const_void_pointer {
    typedef typename std::allocator_traits<A>::const_void_pointer type;
};

template<class A>
struct allocator_difference_type {
    typedef typename std::allocator_traits<A>::difference_type type;
};

template<class A>
struct allocator_size_type {
    typedef typename std::allocator_traits<A>::size_type type;
};

template<class A>
struct allocator_propagate_on_container_copy_assignment {
    typedef typename std::allocator_traits<A>::
        propagate_on_container_copy_assignment type;
};

template<class A>
struct allocator_propagate_on_container_move_assignment {
    typedef typename std::allocator_traits<A>::
        propagate_on_container_move_assignment type;
};

template<class A>
struct allocator_propagate_on_container_swap {
    typedef typename std::allocator_traits<A>::
        propagate_on_container_swap type;
};

#if defined(__cpp_lib_allocator_traits_is_always_equal)
template<class A>
struct allocator_is_always_equal {
    typedef typename std::allocator_traits<A>::is_always_equal type;
};
#else
namespace detail {

template<class>
struct alloc_void {
    typedef void type;
};

template<class A, class = void>
struct alloc_equal {
    typedef typename std::is_empty<A>::type type;
};

template<class A>
struct alloc_equal<A,
    typename alloc_void<typename A::is_always_equal>::type> {
    typedef typename A::is_always_equal type;
};

} /* detail */

template<class A>
struct allocator_is_always_equal {
    typedef typename detail::alloc_equal<A>::type type;
};
#endif

template<class A, class T>
struct allocator_rebind {
    typedef typename std::allocator_traits<A>::template rebind_alloc<T> type;
};

template<class A>
inline typename allocator_pointer<A>::type
allocator_allocate(A& a, typename allocator_size_type<A>::type n,
    typename allocator_const_void_pointer<A>::type h)
{
    return std::allocator_traits<A>::allocate(a, n, h);
}

template<class A, class T, class... Args>
inline void
allocator_construct(A& a, T* p, Args&&... args)
{
    std::allocator_traits<A>::construct(a, p, std::forward<Args>(args)...);
}

template<class A, class T>
inline void
allocator_destroy(A& a, T* p)
{
    std::allocator_traits<A>::destroy(a, p);
}

template<class A>
inline typename allocator_size_type<A>::type
allocator_max_size(const A& a)
{
    return std::allocator_traits<A>::max_size(a);
}

template<class A>
inline A
allocator_select_on_container_copy_construction(const A& a)
{
    return std::allocator_traits<A>::select_on_container_copy_construction(a);
}
#else
template<class A>
struct allocator_pointer {
    typedef typename A::pointer type;
};

template<class A>
struct allocator_const_pointer {
    typedef typename A::const_pointer type;
};

template<class A>
struct allocator_void_pointer {
    typedef typename A::template rebind<void>::other::pointer type;
};

template<class A>
struct allocator_const_void_pointer {
    typedef typename A::template rebind<void>::other::const_pointer type;
};

template<class A>
struct allocator_difference_type {
    typedef typename A::difference_type type;
};

template<class A>
struct allocator_size_type {
    typedef typename A::size_type type;
};

namespace detail {

#if !defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
typedef std::false_type alloc_false;
#else
struct alloc_false {
    BOOST_STATIC_CONSTEXPR bool value = false;
};
#endif

} /* detail */

template<class A>
struct allocator_propagate_on_container_copy_assignment {
    typedef detail::alloc_false type;
};

template<class A>
struct allocator_propagate_on_container_move_assignment {
    typedef detail::alloc_false type;
};

template<class A>
struct allocator_propagate_on_container_swap {
    typedef detail::alloc_false type;
};

template<class A>
struct allocator_is_always_equal {
    typedef detail::alloc_false type;
};

template<class A, class T>
struct allocator_rebind {
    typedef typename A::template rebind<T>::other type;
};

template<class A>
inline typename allocator_pointer<A>::type
allocator_allocate(A& a, typename allocator_size_type<A>::type n,
    typename allocator_const_void_pointer<A>::type h)
{
    return a.allocate(n, h);
}

template<class A, class T>
inline void
allocator_construct(A&, T* p)
{
    ::new((void*)p) T();
}

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
template<class A, class T, class V, class... Args>
inline void
allocator_construct(A&, T* p, V&& v, Args&&... args)
{
    ::new((void*)p) T(std::forward<V>(v), std::forward<Args>(args)...);
}
#else
template<class A, class T, class V>
inline void
allocator_construct(A&, T* p, V&& v)
{
    ::new((void*)p) T(std::forward<V>(v));
}
#endif
#else
template<class A, class T, class V>
inline void
allocator_construct(A&, T* p, const V& v)
{
    ::new((void*)p) T(v);
}

template<class A, class T, class V>
inline void
allocator_construct(A&, T* p, V& v)
{
    ::new((void*)p) T(v);
}
#endif

template<class A, class T>
inline void
allocator_destroy(A&, T* p)
{
    p->~T();
    (void)p;
}

template<class A>
inline typename allocator_size_type<A>::type
allocator_max_size(const A& a)
{
    return a.max_size();
}

template<class A>
inline A
allocator_select_on_container_copy_construction(const A& a)
{
    return a;
}
#endif

template<class A>
inline typename allocator_pointer<A>::type
allocator_allocate(A& a, typename allocator_size_type<A>::type n)
{
    return a.allocate(n);
}


template<class A>
inline void
allocator_deallocate(A& a, typename allocator_pointer<A>::type p,
    typename allocator_size_type<A>::type n)
{
    a.deallocate(p, n);
}

#if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
template<class A>
using allocator_value_type_t = typename allocator_value_type<A>::type;

template<class A>
using allocator_pointer_t = typename allocator_pointer<A>::type;

template<class A>
using allocator_const_pointer_t = typename allocator_const_pointer<A>::type;

template<class A>
using allocator_void_pointer_t = typename allocator_void_pointer<A>::type;

template<class A>
using allocator_const_void_pointer_t =
    typename allocator_const_void_pointer<A>::type;

template<class A>
using allocator_difference_type_t =
    typename allocator_difference_type<A>::type;

template<class A>
using allocator_size_type_t = typename allocator_size_type<A>::type;

template<class A>
using allocator_propagate_on_container_copy_assignment_t =
    typename allocator_propagate_on_container_copy_assignment<A>::type;

template<class A>
using allocator_propagate_on_container_move_assignment_t =
    typename allocator_propagate_on_container_move_assignment<A>::type;

template<class A>
using allocator_propagate_on_container_swap_t =
    typename allocator_propagate_on_container_swap<A>::type;

template<class A>
using allocator_is_always_equal_t =
    typename allocator_is_always_equal<A>::type;

template<class A, class T>
using allocator_rebind_t = typename allocator_rebind<A, T>::type;
#endif

} /* boost */

#endif
