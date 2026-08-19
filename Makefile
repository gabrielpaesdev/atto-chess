CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra $(shell sdl2-config --cflags)
LDFLAGS  := $(shell sdl2-config --libs)
TARGET   := chess

SRCS := board.cpp moves.cpp game.cpp render.cpp ai.cpp main.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

board.o:  board.cpp  board.h  types.h
moves.o:  moves.cpp  moves.h  board.h  types.h
game.o:   game.cpp   game.h   board.h  moves.h  types.h
render.o: render.cpp render.h board.h  types.h font8x8.h piece.h
ai.o:     ai.cpp     ai.h     board.h  game.h   types.h
main.o:   main.cpp   board.h  game.h   render.h ai.h types.h

clean:
	rm -f $(OBJS) $(TARGET)
