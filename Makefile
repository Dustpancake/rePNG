CC=gcc
IDIR=include/
CFLAGS=-I$(IDIR)

SOURCES=src/main.c src/repng.c
EXECUTABLE=repng.out

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $@

clean:
	rm ${EXECUTABLE}
