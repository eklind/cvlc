CFLAGS=-g -Wall -Wextra $(OPTFLAGS)
CC=gcc

SOURCES=$(wildcard **/*.c *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=vlcsim

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm $(OBJECTS) vlcsim
