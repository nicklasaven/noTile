

CPPFLAGS=$(shell sdl2-config --cflags) -Wall -Wextra -DSQLITE_ENABLE_RTREE=1 -O3
LDLIBS=-I. $(shell sdl2-config --libs) -lGLEW -lGL  -lm -lpthread -ldl 
all:    mem_handling.o ext/sqlite/sqlite3.o handle_db.o shader_utils.o  utils.o getData.o eventHandling.o twkb.o varint.o twkb_decode.o rendering.o main.o  
	gcc -o noTile mem_handling.o ext/sqlite/sqlite3.o shader_utils.o handle_db.o utils.o getData.o  eventHandling.o twkb.o varint.o twkb_decode.o rendering.o main.o $(CPPFLAGS) $(LDLIBS)
clean:
	rm -f *.o ext/sqlite/*.o
.PHONY: all clean
