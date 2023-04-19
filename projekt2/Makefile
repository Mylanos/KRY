# the compiler: python
PYTHON = python3

#CFLAGS  = -g -Wall --std c++17 -Wextra -pedantic -Werror -g
# The build target 
TARGET = server.py



all:
	$(PYTHON) $(TARGET) -t $(TYPE) -p $(PORT)

run: 
	$(PYTHON) $(TARGET) -t $(TYPE) -p $(PORT)

help:
	$(PYTHON) $(TARGET) -h
#clean:
#	$(RM) $(TARGET)