#include <iostream>

#include "chan_utitlity.h"

int main() {
  go::Chan<int> ch;
  go::Select{
          go::Case{
                  ch,
                  [](int x) {
                    std::cout << "ch received: " << x << '\n';
                  }},
          go::Case{
                  114 >> ch, [&]() {
                    std::cout << "ch sent." << '\n';
                  }},
  };
  std::cout << "done\n";

  return 0;
}