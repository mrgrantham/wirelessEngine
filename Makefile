#compiler
CC=gcc

CFLAGS=-c -g
LDFLAGS=
SOURCES=main.c LISAreceiver.c LISAtransmitter.c queue.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=engine

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo clean ...
	@rm -fr $(EXECUTABLE) *.o *.exe
