SRC = src/perfapm-client.c

CFLAGS := -Iinclude $(CFLAGS)

OBJ = $(SRC:.c=.o)

all: libperfapm-client.a

libperfapm-client.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

install: libperfapm-client.a
	cp libperfapm-client.a $(DESTDIR)$(libdir)
	cp include/perfapm-client.h $(DESTDIR)$(includedir)

clean:
	rm -f libperfapm-client.a src/*.o
