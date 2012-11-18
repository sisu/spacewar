DIRS:=. net
ODIR:=obj
SRC:=$(wildcard $(addsuffix /*.cpp,$(DIRS)))
OBJ:=$(patsubst %.cpp,$(ODIR)/%.o,$(SRC))
ODIRS:=$(addprefix $(ODIR)/, $(DIRS))
BIN:=play

BASEFLAGS:=-Wall -Wextra -std=c++0x -I. -MMD $(shell sdl-config --cflags)
DFLAGS:=-g
CXXFLAGS:=$(BASEFLAGS) $(DFLAGS)
LIBS=$(shell sdl-config --libs) -lGL -lGLU -lssl -lcrypto

.PHONY: all clean

all: $(ODIRS) $(BIN)

$(BIN): $(OBJ)
	g++ -o "$@" $(OBJ) $(CXXFLAGS) $(LIBS)

$(OBJ): $(ODIR)/%.o: %.cpp
	g++ "$<" -c -o "$@" $(CXXFLAGS)

clean:
	rm -rf "$(ODIR)" "$(BIN)"

$(ODIRS):
	mkdir -p "$@"

include $(wildcard $(addsuffix /*.d,$(ODIRS)))
