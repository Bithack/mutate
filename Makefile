
CC = gcc
CFLAGS = -Wall -Wno-missing-braces -std=gnu99 `pkg-config --cflags MagickWand`
LDFLAGS =  -Wall -std=gnu99 -ldl -lm `pkg-config --libs MagickWand`
OBJECTS = main.o
DEPS = mutate.h

mutate: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o mutate

%.o: src/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o 2>/dev/null || true
	rm mutate 2>/dev/null || true
