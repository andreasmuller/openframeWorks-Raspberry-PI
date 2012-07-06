# source files.
SRC =  	$(shell ls src/*.c)

OBJ = $(SRC:.c=.o)

OUT = lib/linux/libkiss.a

# include directories
INCLUDES = -I include/ 

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -Os -m32

# compiler
CCC = gcc

.SUFFIXES: .c

default: $(OUT)

%.o: %.c
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
