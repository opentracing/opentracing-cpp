
mkdir -f gtest-build
cd gtest-build

GTEST_DIR=../googletest/googletest

g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
ar -rv libgtest.a gtest-all.o
