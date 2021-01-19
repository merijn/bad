#include "seq.hh"
namespace bad {
  // char is an 'integer type'.
  template <char...cs>
  using str = std::integer_sequence<char, cs...>;

  template <typename T, T...cs>
  str<cs...> operator""_str() {
    return {};
  }
}