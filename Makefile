all: ullimp

ullimp: ullimp.cpp ullimp.hpp digraph.hpp map.hpp order.hpp
	g++ -O3 -o ullimp $<

@PHONY: clean

clean:
	rm ullimp