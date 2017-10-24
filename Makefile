#=====================================================================================
#
#      Filename: gdbtrace.cpp
#       Created: 10/23/2017 09:55:37
# Last Modified: 10/23/2017 18:25:53
#
#   Description: 
#
#       Version: 1.0
#      Revision: none
#      Compiler: gcc
#
#        Author: ANHONG
#         Email: anhonghe@gmail.com
#  Organization: USTC
#
#=====================================================================================

CXX := g++

.PHONY: all clean

all:
	$(CXX) -std=c++11 -g gdbtrace.cpp -c
	$(CXX) -std=c++11 -fPIC -shared -g gdbtrace.cpp -o libgdbtrace.so
	$(CXX) -std=c++11 -g main.cpp -o gdbtrace

test:
	$(CXX) -std=c++11 -g -I. sample.cpp gdbtrace.o

clean:
	rm -rf *.o *.so *.a *.out gdbtrace
