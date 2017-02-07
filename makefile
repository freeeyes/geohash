include makefile.define

default:all

PATS = ./GeoHash/geohash.o \
	   ./GeoHash/mapinfo.o \
	   ./GeoHash/posinfopool.o \
	   ./GeoHash/areainfopool.o \
	   ./GeoHash/ShareMemory.o \
	   main.o

LIB_BASE_OBJS = ./GeoHash/geohash.o \
	            ./GeoHash/mapinfo.o \
	            ./GeoHash/posinfopool.o \
	            ./GeoHash/areainfopool.o \
	            ./GeoHash/ShareMemory.o \
	            main.o

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
