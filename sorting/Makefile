#
# Makefile for Comp 15 Sorting Assignment
#
# More about Makefiles and the variables.
#
# CXX is the standard variable whose value is used for compiling C++ 
# programs.
#
# CXXFLAGS is used when compiling C++ programs.  None of the rules below 
# mention it, because it's used by default for C++ compilations.  You will
# should see these flags appear when you make your programs.
#
# make knows how to make a .o file from a .cpp file of the same name, 
# so only the dependencies are listed below for .o files.  make will
# use the default rule to compile them, which uses CXX and CXXFLAGS.
#
# CXXFLAGS is used for compilation by default
# LDFLAGS  is used for linking by default
#
#
# You may add .cpp and .h files if that will make your implementation
# cleaner.  Be sure to update the build rules and the list of files for
# provide accordingly.
#
CXX      = g++
CXXFLAGS = -Wall -Wextra -Wconversion -std=c++11 -g
LDFLAGS  = -g

sortnums:  sortnums.o IntVector.o
	${CXX} ${LDFLAGS} -o sortnums $^

sortnums.o:  sortnums.cpp  IntVector.h
IntVector.o: IntVector.cpp IntVector.h


clean:
	rm -rf sortnums *.o *.dSYM

make provide:
	provide comp15 hw5 sortnums.cpp IntVector.cpp IntVector.h \
	        ReadMe.md Makefile
