CXX ?= c++
CXXFLAGS ?= -O3 -std=c++17 -Wall -Wextra -Wpedantic -ffp-contract=off

.PHONY: all test clean
all: build/demo

build/demo: src/search.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@

test: build/demo
	python3 -m unittest discover -s tests -v

clean:
	rm -rf build
