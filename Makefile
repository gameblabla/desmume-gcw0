PRGNAME     = desmume.elf

# define regarding OS, which compiler to use
CXX = g++
LD  = gcc

# change compilation / linking flag options
CC_OPTS		= -O2 -g3
CFLAGS		= $(CC_OPTS)

CFLAGS 		+= -DFASTBUILD -DHOST_LINUX=1 -I.  -I./src -D_GNU_SOURCE=1 -D_REENTRANT
CFLAGS      += -Isrc -Isrc/cli -Isrc/addons -Isrc/utils -Isrc/utils/tinyxml -Isrc/utils/libfat -Isrc/utils/decrypt
LDFLAGS     = -lSDL -lm -lasound -lstdc++ -pthread

# Files to be compiled
SRCDIR    = ./src ./src/cli ./src/addons ./src/filter ./src/utils ./src/utils/tinyxml ./src/utils/libfat ./src/utils/decrypt
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
