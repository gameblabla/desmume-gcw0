PRGNAME     = desmume

# define regarding OS, which compiler to use
CXX = g++
LD  = gcc

# change compilation / linking flag options
CC_OPTS		= -O2 -fomit-frame-pointer
CFLAGS		= $(CC_OPTS)

CFLAGS 		+= -DFASTBUILD -DPACKAGE_NAME=\"desmume\" -DPACKAGE_TARNAME=\"desmume\" -DPACKAGE_VERSION=\"svn\" -DPACKAGE_STRING=\"desmume\ svn\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DPACKAGE=\"desmume\" -DVERSION=\"svn\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DSVN_REV=0 -DSVN_REV_STR=\"0\" -DHAVE_LIBZ=1 -DHOST_LINUX=1 -I.  -I../src -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -pthread -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
CFLAGS      += -Isrc -Isrc/cli -Isrc/addons -Isrc/utils -Isrc/utils/tinyxml -Isrc/utils/libfat -Isrc/utils/decrypt
LDFLAGS     = -lSDL -lm -lz -lasound -lstdc++ -lglib-2.0 -Wl,--as-needed -flto

# Files to be compiled
SRCDIR    = ./src ./src/cli ./src/addons ./src/filter ./src/metaspu ./src/utils ./src/utils/tinyxml ./src/utils/libfat ./src/utils/decrypt
VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C   = $(notdir $(patsubst %.cpp, %.o, $(SRC_C)))
OBJS     = $(OBJ_C)

all: $(PRGNAME)

# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(LD) $(CFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
