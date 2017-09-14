.PHONY: all clean cpp14 cpp11 cpp98

EXECUTABLE_O0 = test0
EXECUTABLE_O2 = test2
EXECUTABLE_O3 = test3

EXECUTABLES=$(EXECUTABLE_O0) $(EXECUTABLE_O2) $(EXECUTABLE_O3)

cpp98: all

cpp14: all

cpp11: all

all: $(EXECUTABLE_O0) $(EXECUTABLE_O2) $(EXECUTABLE_O3)

$(EXECUTABLE_O0): test0.o
	gcc -o $@ -lstdc++ $<

$(EXECUTABLE_O2): test2.o
	gcc -o $@ -lstdc++ $<

$(EXECUTABLE_O3): test3.o
	gcc -o $@ -lstdc++ $<

ifeq ($(MAKECMDGOALS), cpp14)
CPPSTDOPT=-std=c++14
else
ifeq ($(MAKECMDGOALS), cpp11)
CPPSTDOPT=-std=c++11
else
ifeq ($(MAKECMDGOALS), cpp98)
CPPSTDOPT=-std=c++98
else
CPPSTDOPT=-std=c++14
endif
endif
endif

CXXFLAGS=-c -fPIC -Wall -Werror -g -I. -I../externals/boost/include \
		 -Wno-format-extra-args -Wformat-security -Wformat-nonliteral -Wformat=2 $(CPPSTDOPT)

test0.o: test.cpp
	gcc -O0 -o $@ $< $(CXXFLAGS)

test2.o: test.cpp
	gcc -O2 -o $@ $< $(CXXFLAGS)

test3.o: test.cpp
	gcc -O3 -o $@ $< $(CXXFLAGS)

clean:
	rm -f *.o
	rm -f $(EXECUTABLES)

