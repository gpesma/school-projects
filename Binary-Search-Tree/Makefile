#
# Makefile for Binary Search Tree
#
## Not using -Wextra
CXX      = clang++
CXXFLAGS = -g -Wall -Wextra
LDFLAGS  = -g

hw4: main.o BinarySearchTree.o pretty_print.o
	${CXX} ${LDFLAGS} -o hw4 main.o BinarySearchTree.o pretty_print.o

main.o:              main.cpp BinarySearchTree.h 
BinarySearchTree.o:  BinarySearchTree.cpp BinarySearchTree.h
pretty_print.o:      pretty_print.cpp BinarySearchTree.h

clean:
	rm -rf hw4 *.o *.dSYM

provide:
	provide comp15 hw4 main.cpp BinarySearchTree.cpp BinarySearchTree.h \
	        pretty_print.cpp ReadMe.md Makefile

