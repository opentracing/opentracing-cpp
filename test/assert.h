#pragma once

#include <iostream>
#include <cstdlib>

#define ASSERT(EXPR)                                                     \
  do {                                                                   \
    if (!(EXPR)) {                                                       \
      std::cerr << "Failed in " << __FILE__ << ":" << __LINE__ << ": \"" \
                << #EXPR "\" evaluted to false\n";                       \
      std::exit(-1);                                                     \
    }                                                                    \
  } while (false);
