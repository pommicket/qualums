C=g++
FLAGS=-std=c++11 `pkg-config --libs --cflags sdl2`

default: all

all: src/*.cpp src/*.h
	$(C) -o qualums src/*.cpp $(FLAGS)

clean:
	rm qualums
