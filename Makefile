CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=500 -lpthread -pedantic
LDFLAGS =

# Source files for the application target
APPLICATION_SOURCES = application.c memoryADT.c
APPLICATION_OBJECTS = $(APPLICATION_SOURCES:.c=.o)
APPLICATION_EXECUTABLE = application.out

# Source files for the vista target
VISTA_SOURCES = vista.c memoryADT.c
VISTA_OBJECTS = $(VISTA_SOURCES:.c=.o)
VISTA_EXECUTABLE = vista.out

# Source files for the slave target
SLAVE_SOURCES = slave.c
SLAVE_OBJECTS = $(SLAVE_SOURCES:.c=.o)
SLAVE_EXECUTABLE = slave.out

# Source files for the slave target
MD5SLAVE_SOURCES = md5Slave.c
MD5SLAVE_OBJECTS = $(MD5SLAVE_SOURCES:.c=.o)
MD5SLAVE_EXECUTABLE = md5Slave.out

# Get a list of all .h files in the directory
HEADERS = $(wildcard *.h)

.PHONY: all application vista md5Slave slave clean

all: application vista md5Slave slave

application: $(APPLICATION_EXECUTABLE)

vista: $(VISTA_EXECUTABLE)

slave: $(SLAVE_EXECUTABLE)

md5Slave: $(MD5SLAVE_EXECUTABLE)

$(APPLICATION_EXECUTABLE): $(APPLICATION_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(APPLICATION_OBJECTS) -h $(HEADERS) -o $@

$(VISTA_EXECUTABLE): $(VISTA_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(VISTA_OBJECTS) -h $(HEADERS) -o $@

$(SLAVE_EXECUTABLE): $(SLAVE_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SLAVE_OBJECTS) -h $(HEADERS) -o $@

$(MD5SLAVE_EXECUTABLE): $(MD5SLAVE_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(MD5SLAVE_OBJECTS) -h $(HEADERS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(APPLICATION_OBJECTS) $(APPLICATION_EXECUTABLE) $(VISTA_OBJECTS) $(VISTA_EXECUTABLE) $(SLAVE_OBJECTS) $(SLAVE_EXECUTABLE) $(MD5SLAVE_OBJECTS) $(MD5SLAVE_EXECUTABLE)