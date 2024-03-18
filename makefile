CC = gcc
CFLAGS = -Wall -Wextra -std=c99  
LDFLAGS = -lm
# -fsanitize=address
SOURCES = mlpt.c valid_check.c ptbr_setup.c

OBJECTS = $(SOURCES:.c=.o)

HEADERS = mlpt.h config.h 

all: libmlpt.a

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

libmlpt.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

clean: 
	rm -f $(OBJECTS) libmlpt.a

# TARGET = pagetable

# $(TARGET): $(SOURCES) $(HEADERS)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# clean:
# 	rm -f $(TARGET)

# .PHONY: all clean

# all: $(TARGET)
