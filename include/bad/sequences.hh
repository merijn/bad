#ifndef BAD_SEQUENCES_HH
#define BAD_SEQUENCES_HH

#include <cstdint>
#include <utility>

#include "bad/attributes.hh"
#include "bad/common.hh"
#include "bad/errors.hh"

/// \file
/// \brief type level integer sequences
/// \author Edward Kmett
///
/// \defgroup sequences_group sequences
/// \brief type level sequences

namespace bad::sequences {

  /// sequence construction, shorter `std::make_integer_sequence`
  template <class T, T x>
  using make_iseq = std::make_integer_sequence<T,x>;

  /// make a sequence with type inference
  template <auto x>
  using make_aseq = make_iseq<decltype(x), x>;

  /// make an index sequence
  template <size_t x>
  using make_seq = make_iseq<size_t, x>;

  /// make a sequence of signed distances
  template <ptrdiff_t x>
  using make_sseq = make_iseq<ptrdiff_t, x>;

  // * application

  namespace detail {
    /// \meta
    template <template <class T, T...> class, class X>
    struct seq_apply_ {
      static_assert(no<X>, "seq_apply: not a sequence");
    };

    /// \meta
    template <template <class T, T...> class F, class X, X ... xs>
    struct seq_apply_<F,iseq<X,xs...>> {
      using type = F<X,xs...>;
    };
  }

  /// apply the type of a sequence and its parameter pack to a template, analogous to `std::apply` for tuples
  /// \ingroup sequences_group
  template <template <class T, T...> class F, class L>
  using seq_apply = typename detail::seq_apply_<F,L>::type;

  namespace detail {
    /// \meta
    template <template <auto x, decltype(x) ...> class, class X>
    struct seq_auto_apply_ {
      static_assert(no<X>, "seq_auto_apply: not a sequence");
    };

    /// \meta
    template <template <auto x, decltype(x) ...> class F, class X, X x, X ... xs>
    struct seq_auto_apply_<F,iseq<X,x,xs...>> {
      using type = F<x,xs...>;
    };
  }

  /// apply a non-empty sequence to a template that uses auto to infer the sequennce type
  /// \ingroup sequences_group
  template <template <auto x, decltype(x) ...> class F, class L>
  using seq_auto_apply = typename detail::seq_auto_apply_<F,L>::type;

  namespace detail {
    /// \meta
    template <class T, template <T...> class, class>
    struct iseq_apply_ {
      static_assert(no<T>, "iseq_apply: not a sequence");
    };

    /// \meta
    template <class T, template <T...> class F, T... xs>
    struct iseq_apply_<T,F,iseq<T,xs...>> {
      using type = F<xs...>;
    };
  }

  /// Apply a sequence of values of known type \p T to a given template.
  /// \ingroup sequences_group
  template <class T, template <T...> class F, class L>
  using iseq_apply = typename detail::iseq_apply_<T,F,L>::type;

  // * ranges

  namespace detail {
    /// \meta
    template <typename T, T x, typename S>
    struct seq_range_ {
      static_assert(no<T>, "seq_range: bad arguments");
    };

    /// \meta
    template <typename T, T x, T... ys>
    struct seq_range_<T,x,iseq<T,ys...>> {
      using type = iseq<decltype(x),(x+ys)...>;
    };
  }

  /// return the range `[x,y)` as a sequence, with type matching the type of the common type of `x` and `y`.
  /// \ingroup sequences_group
  template <auto x, auto y>
  using seq_range = typename detail::seq_range_<std::common_type_t<decltype(x),decltype(y)>,x,make_aseq<y-x>>::type;

  namespace detail {
    /// \meta
    template <class T, class, class>
    struct seq_append__ {
      static_assert(no<T>, "seq_append: bad sequences (type mismatch?)");
    };

    /// \meta
    template <class T, T... ts, class S, S... ss>
    struct seq_append__<T,iseq<T,ts...>, iseq<S,ss...>> {
      using type = iseq<std::common_type_t<T,S>,ts...,ss...>;
    };

    /// \meta
    template <class T, class...>
    struct seq_append_ {
      static_assert(no<T>, "seq_append: bad sequences");
    };

    /// \meta
    template <class T>
    struct seq_append_<T> {
      using type = iseq<T>;
    };

    /// \meta
    template <class T, T...ts, class...ss>
    struct seq_append_<T,iseq<T,ts...>, ss...> {
      using type = typename seq_append__<T,iseq<T,ts...>,typename seq_append_<T,ss...>::type>::type;
    };
  }

  /// append (possibly several) sequences
  /// \param T the type elements present in all of the sequences, and the type of elements in the result sequence
  /// \ingroup sequences_group
  template <class T, class... S>
  using seq_append = typename detail::seq_append_<T,S...>::type;

  // * reify

  namespace detail {
    /// \meta
    template <class T>
    struct reify_ {
      static_assert(no<T>, "reify: not a sequence");
    };

    /// \meta
    template <class T, T...xs>
    struct reify_<iseq<T, xs...>> {
      /// This type synonym allows us to return a reference to an array, rather than a pointer
      /// ensuring `sizeof` can be used to calculate the length of the `value` directly.
      typedef T const type[sizeof...(xs)];
      static constexpr T const value[sizeof...(xs)] = { xs ... };
    };
  }

  /// extract a runtime array of elements for a given sequence.
  /// This array is explicitly NOT null terminated so that
  ///
  /// `sizeof(reify<iseq<T,xs...>>)/sizeof(T)` == `sizeof...(xs)`;
  ///
  /// This may cause complications if you want to pass it to C library functions as a `const char *`!
  ///
  /// \ingroup sequences_group
  template <class S>
  constexpr typename detail::reify_<S>::type & reify = detail::reify_<S>::value;

  /// the product of a non-empty parameter pack of numbers, with type inference
  /// \ingroup sequences_group
  template <auto... xs>
  constexpr auto prod = (...*xs);

  /// the product of a parameter pack of numbers
  /// \ingroup sequences_group
  template <class T, T... is>
  constexpr T prod_t = (T(1) * ... * is);

  namespace detail {
    /// \meta
    template <class T>
    struct seq_prod_ {
      static_assert(no<T>, "seq_prod: not a sequence");
    };

    /// \meta
    template <class T>
    struct seq_prod_<iseq<T>> {
      static constexpr T value = 1;
    };

    /// \meta
    template <class T, T i, T... is>
    struct seq_prod_<iseq<T,i,is...>> {
      static constexpr T value = (i * ... * is);
    };
  }

  /// the product of a sequence of numbers.
  /// \ingroup sequences_group
  template <class S>
  constexpr auto seq_prod = detail::seq_prod_<S>::value;

  /// the sum of a non-empty parameter pack of numbers, with type inference
  /// \ingroup sequences_group
  template <auto... xs>
  constexpr auto total = (...+xs);

  /// the sum of a typed parameter pack of numbers
  /// \ingroup sequences_group
  template <class T, T... xs>
  constexpr T total_t = (T(1)+...+xs);

  namespace detail {
    /// \meta
    template <class T>
    struct seq_total_ {
      static_assert(no<T>, "seq_total: not a sequence");
    };

    /// \meta
    template <class T>
    struct seq_total_<iseq<T>> {
      static constexpr T value = 0;
    };

    /// \meta
    template <class T, T i, T... is>
    struct seq_total_<iseq<T,i,is...>> {
      static constexpr T value = (i + ... + is);
    };
  }

  /// the sum of a sequence of numbers
  /// \ingroup sequences_group
  template <class S>
  constexpr auto seq_total = detail::seq_total_<S>::value;

  // * head
  template <auto x, decltype(x) ... xs>
  constexpr auto head = x;

  namespace detail {
    /// \meta
    template <class S>
    struct seq_head_ {
      static_assert(no<S>, "seq_head: not a non-empty sequence");
    };

    /// \meta
    template <class T, T ... is>
    struct seq_head_<iseq<T,is...>> {
      constexpr static T value = head<is...>;
    };
  }

  /// head of a sequence
  /// \ingroup sequences_group
  template <class S>
  constexpr auto seq_head = detail::seq_head_<S>::value;

  /// tail of a non-empty parameter pack of numbers, with type inference
  /// \ingroup sequences_group
  template <auto x,decltype(x) ... xs>
  using tail = iseq<decltype(x), xs...>;

  /// tail of a non-empty sequence
  /// \ingroup sequences_group
  template <class S>
  using seq_tail = seq_auto_apply<tail,S>;

  namespace detail {
    /// \meta
    template <auto i, class S>
    struct seq_cons_ {
      static_assert(no<S>, "seq_cons: not a sequence");
    };

    /// \meta
    template <auto i, class T, T ... is>
    struct seq_cons_<i,iseq<T,is...>> {
      using type = iseq<std::common_type_t<decltype(i),T>,i,is...>;
    };
  }

  /// prepend a value to a sequence
  /// the resulting sequence has whatever common type is large enough to contain both the value being consed and the values
  /// already in the list (using `std::common_type_t`)
  /// \ingroup sequences_group
  template <auto i, class S>
  using seq_cons = typename detail::seq_cons_<i,S>::type;

  namespace detail {
    /// \meta
    template <class T, T x, T ... ys>
    struct filter_ne_;

    /// \meta
    template <class T, T x>
    struct filter_ne_<T,x> {
      using type = iseq<T>;
    };

    /// \meta
    template <class T, T x, T y, T ... ys>
    struct filter_ne_<T,x,y,ys...> {
      using type = std::conditional_t<x==y, typename filter_ne_<T,x,ys...>::type, seq_cons<y,typename filter_ne_<T,x,ys...>::type>
      >;
    };
  }

  /// remove all occurrences of a specified item from a pack, return result as a sequence
  /// \ingroup sequences_group
  template <auto x, decltype(x) ... xs>
  using filter_ne = typename detail::filter_ne_<decltype(x),x,xs...>::type;

  namespace detail {
    /// \meta
    template <class T, T x, class S, T ... ys> struct filter_ne_by_;

    /// \meta
    template <class T, T x, class U> struct filter_ne_by_<T,x,iseq<U>> {
      using type = iseq<U>;
    };

    /// \meta
    template <class T, T x, class U, U u, U... us, T y, T ... ys>
    struct filter_ne_by_<T,x,iseq<U,u,us...>,y,ys...> {
      using type = std::conditional_t<x==y, typename filter_ne_by_<T,x,iseq<U,us...>,ys...>::type, seq_cons<y,typename filter_ne_by_<T,x,iseq<U,us...>,ys...>::type>>;
    };
  }

  /// remove all occurrences of a specified item from a pack, return result as a sequence
  /// \ingroup sequences_group
  template <auto x, class S, decltype(x) ... xs>
  using filter_ne_by = typename detail::filter_ne_by_<decltype(x),x,S,xs...>::type;

  // * sequence length

  namespace detail {
    /// \meta
    template <class T>
    struct seq_length_ {
      static_assert(no<T>, "seq_length: not a sequence");
    };

    /// \meta
    template <class T, T ... is>
    struct seq_length_<iseq<T,is...>> {
      static constexpr auto value = sizeof...(is);
    };
  }

  /// the length of a sequence
  /// \ingroup sequences_group
  template <class S>
  constexpr auto seq_length = detail::seq_length_<S>::value;

  // * row-major stride calculation

  namespace detail {
    /// \meta
    template <size_t, size_t...>
    struct stride_;

    /// \meta
    template <>
    struct stride_<0> {
      BAD(hd,const) // consteval
      static constexpr ptrdiff_t value() noexcept {
        return 1;
      }
    };

    /// \meta
    template <size_t N, size_t x, size_t... xs>
    struct stride_<N,x,xs...> {
      BAD(hd,const) // consteval
      static constexpr ptrdiff_t value() noexcept {
        if constexpr (N == 0) {
          return (ptrdiff_t(1) * ... * ptrdiff_t(xs));
        } else {
          return stride_<N-1,xs...>::value();
        }
      }
    };
  }

  /// compute the row-major stride of the nth dimension in a parameter pack of dimensions
  /// \ingroup sequences_group
  template <size_t N, size_t... xs>
  BAD(constinit)
  constexpr ptrdiff_t stride = detail::stride_<N,xs...>::value();

  namespace detail {
    /// \meta
    template <size_t, class T>
    struct seq_stride_ {
      static_assert(no<T>, "seq_stride: seq<...> expected");
    };

    /// \meta
    template <size_t N, size_t ... is>
    struct seq_stride_<N,seq<is...>> {
      static constexpr ptrdiff_t value = stride<N,is...>;
    };
  }

  /// compute the row-major stride of the nth dimension in a sequence of dimensions
  /// \ingroup sequences_group
  template <size_t N, class S>
  constexpr ptrdiff_t seq_stride = detail::seq_stride_<N,S>::value;

  namespace detail {
    /// \meta
    template <class, class U>
    struct row_major_ {
      static_assert(no<U>, "row_major: seq<...> expected");
    };

    /// \meta
    template <class S, size_t... is>
    struct row_major_<S,seq<is...>> {
      using type = sseq<seq_stride<is,S>...>;
    };
  }

  /// compute all row-major strides given a set of dimensions
  ///
  /// the result is returned as a \ref bad::sseq "sseq", so we can play games with
  /// negating strides to flip dimensions over
  ///
  /// \ingroup sequences_group
  template <class S>
  using row_major = typename detail::row_major_<S, make_seq<seq_length<S>>>::type;

  // * indexing

  namespace detail {
    /// \meta
    template <size_t N, auto... xs>
    BAD(hd,const)
    constexpr auto nth_() noexcept {
      static_assert(N < sizeof...(xs), "index out of bounds");
      constexpr decltype(head<xs...>) args[] {xs ...};
      return args[N];
    }
  }

  template <size_t N, auto... xs>
  constexpr auto nth = detail::nth_<N,xs...>();

  namespace detail {
    /// \meta
    template <size_t, class S>
    struct seq_nth_ {
      static_assert(no<S>,"seq_nth: not a sequence");
    };

    /// \meta
    template <size_t N, class T, T... xs>
    struct seq_nth_<N,iseq<T,xs...>> {
      static constexpr auto value = nth<N,xs...>;
    };
  }

  /// return the nth member of a sequence
  /// \ingroup sequences_group
  template <size_t N, class S>
  constexpr auto seq_nth = detail::seq_nth_<N,S>::value;

  /// return the last member of a sequence
  /// \ingroup sequences_group
  template <class S>
  constexpr auto seq_last = seq_nth<seq_length<S>-1,S>;

  /// backpermute a sequence with a pack of indices
  ///
  /// `backpermute<iseq<T,a,b,c,d>,0,3,2,3,1,0> = iseq<T,a,d,c,d,b,a>`
  /// \ingroup sequences_group
  template <class S, size_t... is>
  using backpermute = iseq<typename S::value_type, seq_nth<is,S> ...>;

  namespace detail {
    /// \meta
    template <class, class>
    struct seq_backpermute_;

    /// \meta
    template <class S, class I, I ... is>
    struct seq_backpermute_<S, iseq<I,is...>> {
      using type = backpermute<S, is...>;
    };
  }

  /// backpermute a sequence with an index sequence of indices
  //
  /// `seq_backpermute<iseq<T,a,b,c,d>,seq<0,3,2,3,1,0>> = iseq<T,a,d,c,d,b,a>`
  /// \ingroup sequences_group
  template <class S, class T>
  using seq_backpermute = typename detail::seq_backpermute_<S,T>::type;

  /// returns all but the last entry in the non-empty sequence \p S
  /// \ingroup sequences_group
  template <class S>
  using seq_init = seq_backpermute<S, make_seq<seq_length<S>-1>>;

  /// drop the last \p N entries from a sequence
  /// \ingroup sequences_group
  template <size_t N, class S>
  using seq_drop_last = seq_backpermute<S, make_seq<seq_length<S>-std::max(N, seq_length<S>)>>;

  namespace detail {
    /// \meta
    // have to lump it and take the crappy error in the 0 argument case
    template <auto...>
    struct pack_transpose_;

    /// \meta
    template <auto i>
    struct pack_transpose_<i> {
      static_assert(no<decltype(i)>,"pack_transpose: 1 item in the pack, need at least 2");
    };

    /// \meta
    template <auto i, auto j>
    struct pack_transpose_<i,j> {
      using type = aseq<j,i>;
    };

    /// \meta
    template <auto i, auto j, auto k, auto... ls>
    struct pack_transpose_<i,j,k,ls...> {
      using type = seq_cons<i,typename pack_transpose_<j,k,ls...>::type>;
    };
  }

  /// swap the last two dimensions in a (possibly heteregeneous) parameter pack of integral values
  /// \ingroup sequences_group
  template <auto... is>
  using pack_transpose = typename detail::pack_transpose_<is...>::type;

  namespace detail {
    /// \meta
    template <class S>
    struct seq_transpose_ {
      static_assert(no<S>,"seq_transpose: not a sequence, or not long enough");
    };

    /// \meta
    template <class T, T i, T j, T ... is>
    struct seq_transpose_<iseq<T, i, j, is...>> {
      using type = pack_transpose<i, j, is...>;
    };
  }

  /// swap the last two entries in a sequence
  /// \ingroup sequences_group
  template <class S>
  using seq_transpose = typename detail::seq_transpose_<S>::type;

  // * skip the nth element

  namespace detail {
    /// \meta
    template <size_t, class, class>
    struct seq_skip_nth_;

    /// \meta
    template <size_t N, class S, size_t ... ps>
    struct seq_skip_nth_<N,S,seq<ps...>> {
      /// \meta
      template <class Suffix>
      struct at;

      /// \meta
      template <size_t... ss>
      struct at<seq<ss...>> {
        using type = iseq<typename S::value_type, seq_nth<ps,S>..., seq_nth<ss+N,S>...>;
      };
    };
  }

  /// skip the `N`th entry in a sequence
  /// \ingroup sequences_group
  template <size_t N, class S>
  using seq_skip_nth = typename detail::seq_skip_nth_<N, S, make_seq<N>>::template at<make_seq<seq_length<S>-1-N>>::type;

  /// pull the `N`th entry to the front of the sequence. Useful for algorithms like `einsum` and the like.
  /// \ingroup sequences_group
  template <size_t N, class L>
  using seq_pull = seq_cons<seq_nth<N,L>,seq_skip_nth<N,L>>;


  namespace detail {
    /// \meta
    template <class S, template <typename S::value_type> class>
    struct seq_map_ {
      static_assert(no<S>,"seq_map: bad arguments");
    };

    /// \meta
    template <class A, A... as, template <A> class F>
    struct seq_map_<iseq<A,as...>, F> {
      using type = iseq<decltype(F<declval<A>()>::value), F<as>::value...>;
    };
  }

  /// apply a unary function to two sequences
  /// \ingroup sequences_group
  template <class S, template<typename S::value_type> class F>
  using seq_map = typename detail::seq_map_<S, F>::type;

  namespace detail {
    /// \meta
    template <class S, class>
    struct seq_map_at_ {
      static_assert(no<S>,"seq_map_at: bad arguments");
    };

    /// \meta
    template <class A, A... as, class TT>
    struct seq_map_at_<iseq<A,as...>, TT> {
      using type = iseq<decltype(TT::template at<declval<A>()>::value), TT::template at<as>::value...>;
    };
  }

  /// \ingroup sequences_group
  template <class S, class TT>
  using seq_map_at = typename detail::seq_map_at_<S, TT>::type;


  namespace detail {
    /// \meta
    template <class SA, class SB, template <typename SA::value_type,typename SB::value_type> class>
    struct seq_zip_ {
      static_assert(no<SA>,"seq_zip: bad arguments");
    };

    /// \meta
    template <class A, class B, A... as, B... bs, template <A,B> class F>
    struct seq_zip_<iseq<A,as...>,iseq<B,bs...>, F> {
      using type = iseq<decltype(F<declval<A>(),declval<B>()>::value), F<as,bs>::value...>;
    };
  }

  /// zip two sequences
  /// \ingroup sequences_group
  template <class S, class T, template<typename S::value_type,typename T::value_type> class F>
  using seq_zip = typename detail::seq_zip_<S, T, F>::type;

  template <size_t i, auto... xs>
  static constexpr auto rev_nth = nth<sizeof...(xs)-i-1,xs...>;

  namespace detail {
    /// \meta
    template <class A, class B, class I>
    struct seq_compat_ {
      static_assert(no<A>,"seq_compat: not a sequence, or incompatible sequence types");
    };

    /// \meta
    template <class A, A... as, class B, B... bs, size_t... is>
    struct BAD(empty_bases) seq_compat_<iseq<A,as...>,iseq<B,bs...>,seq<is...>> {
      template <size_t i>
      static constexpr bool compat_at = rev_nth<i,as...> == rev_nth<i,bs...>;
      using value_type = bool;
      static constexpr bool value = (compat_at<is> && ... && true);
    };
  }

  /// returns if one list is a suffix of the other
  /// TODO: use this for automatic dimension extension.
  /// \ingroup sequences_group
  template <class L, class R>
  static constexpr bool seq_compat = detail::seq_compat_<L,R,make_seq<std::min(seq_length<L>,seq_length<R>)>>::value;

  namespace detail {
  /// \meta
  template <class S>
    struct seq_for {
     static_assert(no<S>,"seq_for: not a sequence");
    };

    /// \meta
    template <class T, T... ts>
    struct seq_for<iseq<T,ts...>> {
      template <class F>
      BAD(hd,inline,flatten)
      seq_for(F f) {
         (f(ts),...,void());
      }
    };
  }

  /// \ingroup sequences_group
  template <auto N>
  using forN = detail::seq_for<make_seq<N>>;
}

namespace bad {
  using namespace bad::sequences;
}

#endif
