CC=g++

OPENTRACING_INCLUDE=../
NOOP_INCLUDE=../noop

INCLUDES=-I. -I$(OPENTRACING_INCLUDE) -I$(NOOP_INCLUDE)

CXXFLAGS=-Wall -fno-elide-constructors -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Winit-self -Wuninitialized -Wmissing-declarations -Werror $(INCLUDES)

LDFLAGS=

LDIR =${GTEST_LIBS}

LIBS=-L${GTEST_LIBS} -lgtest -lpthread

ODIR=.
_OBJ = noop.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cc %.h
	$(CC) -c -o $@ $< $(CXXFLAGS)

all: $(OBJ)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

