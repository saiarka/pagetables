CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -fsanitize=address 
LDFLAGS = -lm

SOURCES = main.c mlpt.c valid_check.c

HEADERS = mlpt.h config.h

TARGET = pagetable

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

all: $(TARGET)
