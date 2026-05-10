CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
SRC      = task27.cpp Matrix.cpp Solver.cpp interface.cpp
TARGET   = hitori

ifeq ($(OS), Windows_NT)
    TARGET = hitori.exe
    RM     = del /Q
else
    TARGET = hitori
    RM     = rm -f
endif


.PHONY: run clean
 
all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
 
run: all
	./$(TARGET)
 
clean:
	$(RM) $(TARGET)
