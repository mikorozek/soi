# Makefile

# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -pthread

# Name of the output executable
TEST1 = test1
TEST2 = test2

# Source files
SRC = producer_consumer_monitor.cpp # Add other .cpp files here

SRC1 = $(SRC) threads1.cpp test1.cpp
SRC2 = $(SRC) threads2.cpp test2.cpp

# Object files
OBJ1 = $(SRC1:.cpp=.o)
OBJ2 = $(SRC2:.cpp=.o)

# Build all targets
all: $(TEST1) $(TEST2)

# Linking the target executable
$(TEST1): $(OBJ1)
	$(CXX) $(CXXFLAGS) $(OBJ1) -o $(TEST1)

$(TEST2): $(OBJ2)
	$(CXX) $(CXXFLAGS) $(OBJ2) -o $(TEST2)

# Compiling source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ1) $(OBJ2) $(TEST1) $(TEST2)
