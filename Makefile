# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall --std c++17 -Wextra -pedantic -Werror -g
# The build target 
TARGET = kry
CXXOPTS = cxxopts.hpp

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(CXXOPTS)

clean:
	$(RM) $(TARGET)