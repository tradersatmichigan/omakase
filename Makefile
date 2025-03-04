CXX=g++
CXXFLAGS=-march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion \
					-Wconversion -std=c++2b -Isrc -IuSockets/src -flto=auto
IFLAGS=-I/usr/local/include/uWebSockets -I/usr/local/include/uSockets \
				 -isystem /usr/local/include/glaze
UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	LDFLAGS=-L$(shell brew --prefix openssl)/lib uWebSockets/uSockets/*.o -lz
else
	LDFLAGS=-L/usr/lib/x86_64-linux-gnu uWebSockets/uSockets/*.o -lz
endif

SRC_DIR=src
SRC_FILES=$(SRC_DIR)/example.cpp $(SRC_DIR)/benchmark.cpp $(SRC_DIR)/main.cpp
EXECUTABLES=$(patsubst $(SRC_DIR)/%.cpp,%,$(SRC_FILES))

default: main

frontend:
	bun build --outdir ./static/js --target browser frontend/js/index.tsx

uSockets:
	$(MAKE) -C uWebSockets/uSockets

%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -rf $(EXECUTABLES) $(wildcard *.dSYM)
