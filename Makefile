CXX = g++
CXXFLAGS = -march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion \
					-Wconversion -std=c++2b -Isrc -IuSockets/src -flto=auto
IFLAGS = -I/usr/local/include/uWebSockets -I/usr/local/include/uSockets \
				 -isystem /usr/local/include/glaze
LDFLAGS = -L/usr/lib/x86_64-linux-gnu uWebSockets/uSockets/*.o -lz

SRC_DIR = src
SRC_FILES = $(SRC_DIR)/example.cpp $(SRC_DIR)/benchmark.cpp $(SRC_DIR)/main.cpp
EXECUTABLES = $(patsubst $(SRC_DIR)/%.cpp,%,$(SRC_FILES))

all: uSockets main

uSockets:
	$(MAKE) -C uWebSockets/uSockets

main:
	$(CXX) $(CXXFLAGS) $(IFLAGS) $(SRC_FILES) $(LDFLAGS) -o main

%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -rf $(EXECUTABLES) $(wildcard *.dSYM)
