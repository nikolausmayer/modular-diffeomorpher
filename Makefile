##
#
# Author: Nikolaus Mayer, 2017 (mayern@cs.uni-freiburg.de)
#
##

## Compiler
CXX ?= gcc

## Compiler flags; extended in 'debug'/'release' rules
CXXFLAGS += -fPIC -W -Wall -Wextra -Wpedantic -DNDEBUG -std=c++11 -pthread -Dcimg_display=0

## Linker flags
LDFLAGS = -pthread


default: example

.PHONY: example


example: generate warp
	./generate && ./warp grid.ppm iflow.pfm inverse grid-warped.ppm && ./warp grid-warped.ppm flow.pfm inverse grid-unwarped.ppm

	
## Remove built object files and the main executable
clean:
	$(info ... deleting built object files and executable  ...)
	-rm src/*.o warp generate flow.pfm iflow.pfm grid-warped.ppm grid-unwarped.ppm


generate: src/generate.o src/WarpFields.o
	$(CXX) $^ $(LDFLAGS) -o $@

warp: src/warp.o 
	$(CXX) $^ $(LDFLAGS) -o $@


src/WarpFields.o: src/WarpFields.cpp
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/generate.o: src/generate.cpp
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/warp.o: src/warp.cpp
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) -c $< -o $@


