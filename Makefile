CC = gcc
# 1. Fixed the -g flag
CFLAGS = -Wall -Wextra -pthread -g
# 2. Fixed the include directory name
INCLUDES = -I./include -I./src

# 3. Fixed the thread_local_cache file extension
SRCS = src/api.c src/central_cache.c src/internals.c src/thread_local_cache.c
OBJS = $(SRCS:.c=.o)

TARGET = allocator_test

all: $(TARGET)

$(TARGET): $(OBJS) main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) main.o

%.o: %.c	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) main.o $(TARGET)