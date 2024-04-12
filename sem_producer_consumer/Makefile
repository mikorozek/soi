CC = gcc
CFLAGS = -Wall -g

# Shared source files
SRC = initialization.c producer_consumer.c
OBJ = $(SRC:.c=.o)

# Test programs
TEST_SRC1 = test1.c
TEST_OBJ1 = $(TEST_SRC1:.c=.o) $(OBJ)
TEST1 = test1

TEST_SRC2 = test2.c
TEST_OBJ2 = $(TEST_SRC2:.c=.o) $(OBJ)
TEST2 = test2

# Default target
all: $(TEST1) $(TEST2)
	@echo  Test programs have been compiled

# Test1 program target
$(TEST1): $(TEST_OBJ1)
	$(CC) $(CFLAGS) -o $(TEST1) $(TEST_OBJ1)

# Test2 program target
$(TEST2): $(TEST_OBJ2)
	$(CC) $(CFLAGS) -o $(TEST2) $(TEST_OBJ2)

# Generic rule for compiling .c to .o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	$(RM) *.o $(TEST1) $(TEST2)
