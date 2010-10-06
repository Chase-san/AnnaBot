CC = g++
C_FLAGS = -O3 -Wall -c -fmessage-length=0 -fno-exceptions

LIBS = -lws2_32

EXE = anna
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

O_DIR = bin

all: $(EXE)

make: $(OBJ)
	
clean:
	rm -f $(OBJ) $(EXE)
	
%.o: %.cpp
	$(CC) $(C_FLAGS) -o $@ $<

%.o: %.c
	$(CC) $(C_FLAGS) -o $@ $<
	
$(EXE): $(OBJ)
	$(CC) -s -o $@ $(OBJ) $(LIBS)