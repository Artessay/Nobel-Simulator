# basic config
INC	    = ..\inc
OUT	    = ..\lib
INC_LIB = .\include
TARGET  = cg

# cross compile tool chain
CROSS_COMPILE = # riscv64-linux-gnu-
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)g++
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
CPP     = $(CC) -E
MAKE 	= $(CROSS_COMPILE)make

# compile flags
DEBUG   =
CFLAGS  = -I$(INC) -I$(INC_LIB) -O3 -Wall -MMD

# directions
DIRECTIONS = . \
			 ./model \
			 ./third_party \
			 ./utility


# files
C_FILES   = $(foreach dirs, $(DIRECTIONS), $(wildcard $(dirs)/*.c))
CPP_FILES = $(foreach dirs, $(DIRECTIONS), $(wildcard $(dirs)/*.cpp))
# C_FILES   = $(wildcard *.c)
# CPP_FILES = $(wildcard *.cpp)

# objects and dependes
SOURCES = $(C_FILES) $(CPP_FILES)
OBJECTS = $(patsubst %.c, %.o, $(C_FILES)) $(patsubst %.cpp, %.o, $(CPP_FILES))
DEPENDS = $(OBJECTS:.o=.d)
LIBRARIES = model
 
# link
lib$(TARGET).a: $(OBJECTS)
	$(AR) -rcs $(OUT)/$@ $^

-include $(DEPENDS)

# compile
%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@
%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@

# clean
.PHONY: clean
clean:
	rm -f *.d *.o
	rm -f model\*.d model\*.o
	rm -f third_party\*.d third_party\*.o
	rm -f utility\*.d utility\*.o
#	$(foreach dirs, $(DIRECTIONS), $(del $(dirs)\*.o))
#	del $(OBJECTS) $(DEPENDS)