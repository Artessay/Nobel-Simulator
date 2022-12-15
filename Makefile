# version
VERSION	  =

# cross compile tool chain
CROSS_COMPILE = # riscv64-linux-gnu- # mingw32-
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)g++
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
CPP     = $(CC) -E
MAKE 	= $(CROSS_COMPILE)make

# directions
BIN		= .\bin
SRC    	= .\src
INC     = .\inc
LIB    	= .\lib
OBJS	= main.o
DEPS    = $(OBJS:.o=.d)

TARGET 	= Nobel
LIBNAME	= cg
CFLAGS  = -I$(INC) -O2 -Wall -MMD
LFLAGS	= -L$(LIB)
LIBRARY	= -lglfw3dll -lassimpdll

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS) lib$(LIBNAME).a
	$(CC) $(LFLAGS) $< -l$(LIBNAME) $(LIBRARY) -o $(BIN)\$@$(VERSION)

lib$(LIBNAME).a:
	$(MAKE) -C $(SRC)

-include $(DEPS)

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: run
run:
	$(BIN)\$(TARGET)$(VERSION)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(BIN)\$(TARGET).exe $(LIB)\lib$(LIBNAME).a
	$(MAKE) clean -C $(SRC)