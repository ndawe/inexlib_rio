// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file bush.license for terms.

#include "os"

#include <iostream>
#include <cstdlib>

int main() {
  std::cout << bush::os()
            << "-"
            << bush::processor()
            << "-"
            << bush::compiler()
            << std::endl;
  return EXIT_SUCCESS;
}

