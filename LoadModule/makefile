include makefile.define
default:all

PATS = LoadModule.o

LIB_BASE_OBJS = LoadModule.o
LIB_BASE = libgeoload.so

all: libgeoload.so clear

all:$(LIB_BASE) makefile

$(LIB_BASE):$(PATS)
	$(CC) -shared -o $(LIB_BASE) $(LIB_BASE_OBJS) $(LIBS)

clean:
	rm -rf *.o $(LIB_BASE) $(LIB_BASE_OBJS)
clear:
	mv libgeoload.so ../
cl:
	rm -rf *.o