#=====================================================================================
#
#      Filename: gdbtrace.cpp
#       Created: 10/23/2017 09:55:37
# Last Modified: 10/23/2017 10:24:00
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

test:
	$(CXX) -std=c++11 -g -I. sample.cpp gdbtrace.o

clean:
	rm -rf *.o *.so *.a *.out
