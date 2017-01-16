include makefile.define

default:all

PATS = geohash.o mapinfo.o ShareMemory.o main.o

LIB_BASE_OBJS = geohash.o mapinfo.o ShareMemory.o main.o

LIB_BASE = mapchinapos

all: mapchinapos

# ?????
all:$(LIB_BASE) makefile

$(LIB_BASE):$(PATS)
	$(CC) -rdynamic -o $(LIB_BASE) $(LIB_BASE_OBJS) $(LIBS)

# ??
clean:
	rm -rf *.o  $(LIB_BASE) $(LIB_BASE_OBJS)
cl:
	rm -rf *.o 
