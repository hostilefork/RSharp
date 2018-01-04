CC = gcc
CFLAGS = -ansi -I./includes/

SOURCES := $(wildcard base/os/*.c base/*.c base/types/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

all: $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o rsharp

clean:
	for fichier in $(OBJECTS); do rm -f $$fichier; done
