CFLAGS = -Wall -g
LIBS= -lgps
EXECUTABLE = reverse-geo

OBJS = parson.o reverse-geocode.o

default:$(EXECUTABLE)

$(EXECUTABLE):$(OBJS)
	$(CC) -o $(EXECUTABLE) $(CFLAGS) $(OBJS) $(LIBS)

%.o:%.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(EXECUTABLE) $(OBJS)

