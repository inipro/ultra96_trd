SRC = src/perfapm-client-test.c

OBJ = $(SRC:.c=.o)

all: perfapm-client-test

perfapm-client-test: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) -lperfapm-client

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

install: perfapm-client-test
	cp perfapm-client-test $(DESTDIR)/usr/bin

clean:
	rm -f perfapm-client-test src/*.o
