CC=g++
CFLAGS=-Wall -fno-elide-constructors -pedantic-errors -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Winit-self -Wuninitialized -Wmissing-declarations -Werror -std=c++98

# Requires gtest. 
# For example for mac: http://stackoverflow.com/questions/20746232/how-to-properly-setup-googletest-on-os-x-aside-from-xcode
# For ubuntu: http://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/

LDFLAGS=

ODIR=.
LDIR =../lib

LIBS=-lopentracing -lgtest -lgtest_main -lpthread

_OBJ = opentracing.t.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)  $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
