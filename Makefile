SHELL=/bin/sh
CC=clang
CFLAGS=-fPIC -pedantic -Wall -Wextra -march=native -ggdb3
LDFLAGS=-shared

TARGET=./build/libpbm.so
SOURCES=$(shell echo src/*.c)
HEADERS=$(shell echo include/*.h)
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p ./build/
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
