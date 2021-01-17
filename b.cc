#include "bad.h"
#include <iostream>
#include <tuple>

using namespace autodiff;
using namespace std;

struct simple : detail::static_propagator<5, simple, int> {

};

// i have one off by one error and its infuriating.
int main (int argc, char ** argv) {
  dl open;
  tape<int> t;
  for (int i=0;i<1000;++i)
    t.push<simple>();
  for (auto & p : t)
    cout << p << endl;
  cout << t.activations << endl;

  //auto result = diff([](auto x) { auto y = x*x; auto z = y*y; auto w = z*z; return w*w; }, 2);
  //std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  return 0;
}