## freeglut Compile ##
PROJ_NAME=main
C_SOURCE=$(wildcard *.cpp)
H_SOURCE=$(wildcard *.hpp)
CC=g++
CFLAGS=-c -I"C:\MinGW\freeglut\include"
LDFLAGS=-L"C:\MinGW\freeglut\lib\x64" -lfreeglut -lopengl32 -lglu32
OBJ=$(C_SOURCE:.cpp=.o)
EXE=$(PROJ_NAME).exe

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.cpp %.hpp
	$(CC) $< $(CFLAGS) -o $@

main.o: main.cpp $(H_SOURCE)
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm ".\$(EXE)"