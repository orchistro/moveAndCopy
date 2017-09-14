.PHONY: all clean cpp14 cpp11

EXECUTABLE_O0 = test0
EXECUTABLE_O2 = test2
EXECUTABLE_O3 = test3

EXECUTABLES=$(EXECUTABLE_O0) $(EXECUTABLE_O2) $(EXECUTABLE_O3)

cpp14: all

cpp11: all

all: $(EXECUTABLE_O0) $(EXECUTABLE_O2) $(EXECUTABLE_O3)

$(EXECUTABLE_O0): test0.o
	g++ -o $@ -lstdc++ $< -pg

$(EXECUTABLE_O2): test2.o
	g++ -o $@ -lstdc++ $< -pg

$(EXECUTABLE_O3): test3.o
	g++ -o $@ -lstdc++ $< -pg

ifeq ($(MAKECMDGOALS), cpp14)
CPPSTDOPT=-std=c++14
else
ifeq ($(MAKECMDGOALS), cpp11)
CPPSTDOPT=-std=c++11
else
endif
endif

CXXFLAGS=-c -fPIC -Wall -Werror -g -I. -I../externals/boost/include \
		 -Wno-format-extra-args -Wformat-security -Wformat-nonliteral -Wformat=2 $(CPPSTDOPT) -pg

test0.o: test.cpp
	g++ -O0 -o $@ $< $(CXXFLAGS)

test2.o: test.cpp
	g++ -O2 -o $@ $< $(CXXFLAGS)

test3.o: test.cpp
	g++ -O3 -o $@ $< $(CXXFLAGS)

clean:
	rm -f *.o
	rm -f $(EXECUTABLES)
