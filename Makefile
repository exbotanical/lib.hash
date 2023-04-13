SRC_DIR := src
DEP_DIR := deps
INCLUDE_DIR := include

CC := gcc
CFLAGS := -I$(INCLUDE_DIR) -Wall -Wextra -pedantic -std=c17

TARGET := libhash.a
EXAMPLE_TARGET := example
TEST_TARGET := test

LINK_NAME := $(patsubst lib%,%,$(patsubst %.a,%, $(TARGET)))

SRCS := $(wildcard $(SRC_DIR)/*.c) $(filter-out $(DEP_DIR)/tap.c/%, $(wildcard $(DEP_DIR)/*/*.c))
TEST_DEPS := $(wildcard $(DEP_DIR)/tap.c/*.c)

OBJS := $(patsubst %.c,%.o,$(SRCS))

TESTS := $(wildcard t/*.c)

SEPARATOR := ---------------------------

# Rule to build object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(DEP_DIR)

# Rule to create the static library
$(TARGET): $(OBJS)
	ar rcs $@ $^
	rm $(OBJS)

# Rule to build the example executable
$(EXAMPLE_TARGET): examples/main.c $(TARGET)
	$(CC) -I$(SRC_DIR) -I$(DEP_DIR) $(CFLAGS) $< -L./ -l$(LINK_NAME) -o $@ -lm

.PHONY: clean test .compile_test

clean:
	rm -f $(OBJS) $(TARGET) $(EXAMPLE_TARGET) $(TEST_TARGET)

# `make -s test`
test: $(TARGET)
	$(foreach test,$(TESTS),					  																											\
		$(MAKE) .compile_test file=$(test); 																										\
		printf "\033[1;32m\nRunning test $(patsubst t/%,%,$(test))...\n$(SEPARATOR)\n\033[0m";	\
		./test;\
		printf "\033[1;32m\n$(SEPARATOR)\n\033[0m";	\
 	)
	rm $(TEST_TARGET)

.compile_test:
	$(CC) -D debug -I$(INCLUDE_DIR) -I$(DEP_DIR) -I$(SRC_DIR) $(file) $(TEST_DEPS) -o $(TEST_TARGET) -L./ -l$(LINK_NAME) -lm
