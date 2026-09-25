CC := gcc
TARGET := main_test
CFLAGS := -I -Wall -Wextra
SRCS := $(wildcard *.c)

$(TARGET):$(SRCS)
	$(CC) -g $(CFLAGS) $^ -o $@
	./main_test

clean:
	-rm -f $(TARGET) *.o