CC=g++

OPENTRACING_INCLUDE=../
OPENTRACING_SRC=$(wildcard ${OPENTRACING_INCLUDED}/*.h)
OPENTRACING_HEADERS= $(wildcard ${OPENTRACING_INCLUDE}
THIRDPARTY_ROOT=../thirdparty

GTEST_INCLUDE=${THIRDPARTY_ROOT}/googletest/googletest/include
GTEST_LIBS=${THIRDPARTY_ROOT}/

INCLUDES=-I. -I${GTEST_INCLUDE} -I${OPENTRACING_INCLUDE}

CXXFLAGS=-Wall -fno-elide-constructors -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Winit-self -Wuninitialized -Wmissing-declarations -Werror ${INCLUDES}

# Requires gtest.
# For example for mac: http://stackoverflow.com/questions/20746232/how-to-properly-setup-googletest-on-os-x-aside-from-xcode
# For ubuntu: http://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/

LDFLAGS=

ODIR=.
LDIR =${GTEST_LIBS}

LIBS=-L${GTEST_LIBS} -lgtest -lpthread

_OBJ = baggage.t.o stringref.t.o unittest.t.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cc ${OPENTRACING_SRC}
	$(CC) -c -o $@ $< $(CXXFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LIBS)  $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
