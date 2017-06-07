#!/bin/sh
find . \( -name '*.h' -or -name '*.cpp' \) -exec clang-format -i {} \;
