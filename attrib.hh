#pragma once
#include "map.hh"

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(always_inline)
#define BAD_INLINE inline __attribute__((always_inline))
#elif defined _WIN32
#define BAD_INLINE __forceinline
#else
#define BAD_INLINE inline
#endif

#if __has_attribute(flatten)
#define BAD_FLATTEN __attribute__((flatten))
#else
#define BAD_FLATTEN
#warning no flatten
#endif

// #if __clang__
// #define BAD_NO_UNIQUE_ADDRESS [[no_unique_address]]
// #else
#define BAD_NO_UNIQUE_ADDRESS
// #endif

#if __has_attribute(malloc)
#define BAD_MALLOC __attribute__((malloc))
#else
#define BAD_MALLOC
#warning no malloc
#endif


#if __has_attribute(uninitialized)
#define BAD_UNINITIALIZED __attribute__((uninitialized))
#else
#define BAD_UNINITIALIZED
#warning no uninitialized
#endif


/// only modifies data reachable through pointer arguments
#if defined(__clang__) || defined(_MSC_VER)
#define BAD_NOALIAS __declspec(noalias)
#else
#define BAD_NOALIAS
#warning no noalias
#endif

/// only modifies data reachable through pointer arguments
#if __has_attribute(require_constant_initialization)
#define BAD_CONSTINIT __attribute__((require_constant_initialization))
#else
#define BAD_CONSTINIT
#warning no constinit
#endif

/// check that this is called once on all execution paths

/// check that this is called once on all execution paths
//#if __has_attribute(called_once)
//#define BAD_CALLED_ONCE __attribute__((called_once))
//#else
//#define BAD_CALLED_ONCE
//#warning no called_once
//#endif

/// no effect other than return value, may inspect globals

/// only examines arguments, no effect other than return value
#if __has_attribute(const)
#define BAD_CONST __attribute__((const))
#else
#define BAD_CONST
#warning no const
#endif

/// no effect other than return value, may inspect globals
#if __has_attribute(pure)
#define BAD_PURE __attribute__((pure))
#else
#define BAD_PURE
#warning no pure
#endif

#ifdef __clang__
#define BAD_REINITIALIZES [[clang::reinitializes]]
#else
// TODO: support GCC
#define BAD_REINITIALIZES
#warning no reinitializes
#endif

// C++14

#if __has_attribute(assume_aligned)
#define BAD_ASSUME_ALIGNED(x) __attribute__((assume_aligned(x)))
#else
#define BAD_ASSUME_ALIGNED(x)
#warning no assume_aligned
#endif

// arg # (1-based) of the attribute that tells you the alignment of the result

#if __has_attribute(align_value)
#define BAD_ALIGN_VALUE(x) __attribute__((align_value(x)))
#else
#define BAD_ALIGN_VALUE(x)
#warning no align_value
#endif

// arg # (1-based) of the attribute that tells you the alignment of the result
#if __has_attribute(alloc_align)
#define BAD_ALLOC_ALIGN(x) __attribute__((alloc_align(x)))
#else
#define BAD_ALLOC_ALIGN(x)
#warning no alloc_align
#endif

// arg # (1-based) of the attribute that tells you the size of the result in bytes
#if __has_attribute(alloc_size)
#define BAD_ALLOC_SIZE(x) __attribute__((alloc_size(x)))
#else
#define BAD_ALLOC_SIZE(x)
#warning no alloc_size
#endif

#if __has_attribute(noescape)
#define BAD_NOESCAPE __attribute__((noescape))
#else
#define BAD_NOESCAPE
#warning no noescape
#endif

#if __has_attribute(callback)
#define BAD_CALLBACK(...) __attribute__((callback(__VA_ARGS__)))
#else
#define BAD_CALLBACK(...)
#warning no callback
#endif

#if __has_attribute(lifetimebound)
#define BAD_LIFETIMEBOUND __attribute__((lifetimebound))
#else
#define BAD_LIFETIMEBOUND
#warning no lifetimebound
#endif


#if __has_attribute(returns_nonnull)
#define BAD_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define BAD_RETURNS_NONNULL
#warning no returns_nonnull
#endif

#ifdef __CUDACC__
#define BAD_HOST __host__
#define BAD_DEVICE __device__
#define BAD_GLOBAL __global__
#else
#define BAD_HOST
#define BAD_DEVICE
#define BAD_GLOBAL
#endif

#define BAD_HD BAD_HOST BAD_DEVICE

/// @since C++17
#define BAD_MAYBE_UNUSED [[maybe_unused]]

/// @since C++14
#define BAD_DEPRECATED [[deprecated]]

/// @since C++17
#define BAD_NODISCARD [[nodiscard]]