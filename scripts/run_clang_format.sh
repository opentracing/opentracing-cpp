#!/bin/sh
find . -path ./c++11/3rd_party -prune -o \( -name '*.h' -or -name '*.cpp' \) \
  -exec clang-format -i {} \;
