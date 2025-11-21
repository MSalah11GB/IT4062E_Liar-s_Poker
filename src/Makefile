CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lsqlite3

all: createdb

createdb: createdb.c
	$(CC) $(CFLAGS) -o createdb createdb.c $(LIBS)

clean:
	rm -f createdb
