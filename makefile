# ### A simple basic-purpose makefile ###
# 1) Change the executable name.
# 2) Change source path.

# Executable name
EXEC=shell

# Source files
SRC=./src/
C_SRC=$(wildcard $(SRC)*.c)
H_SRC=$(wildcard $(SRC)*.h)

# Objects
OBJ=$(subst src,obj,$(subst .c,.o,$(C_SRC)))

# Compiler
CC=cc

# Compiler flags
CFLAGS=-c -lm -Wall
CC_FLAGS=$(CFLAGS)


# Compiling and linking
#
all: objFolder $(OBJ)

./obj/%.o: ./src/%.c $(H_SRC)
	@ echo 'Building $@ from $<...'
	@ $(CC) -o $@ $< $(CC_FLAGS)
	@ echo 'ok'

install: binFolder all
	@ echo 'Building binaries using $(CC)...'
	@ $(CC) -o ./bin/$(EXEC) ./obj/shell.o ./obj/entry.o \
	./obj/fat.o
	@ echo 'ok'

objFolder:
	@ [ -d ./obj ] || mkdir ./obj

binFolder:
	@ [ -d ./bin ] || mkdir ./bin

clean:
	@ echo 'Cleanning up...'
	@ rm -rf ./obj/*.o ./obj $(EXEC)
	@ rm -rf ./bin/* ./bin
	@ echo 'ok'

.PHONY: all install clean $(EXEC)
