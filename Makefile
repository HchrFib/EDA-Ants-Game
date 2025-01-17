# Detectar el sistema operativo
ifeq ($(shell uname -s), Linux)
    ifeq ($(shell uname -m), x86_64)
        SELECTED_DUMMY_OBJ = AIDummy.o.Linux64
    else
        SELECTED_DUMMY_OBJ = AIDummy.o.Linux32
    endif
else ifeq ($(shell uname -s), Darwin)
    SELECTED_DUMMY_OBJ = AIDummy.o.macOs
else ifeq ($(OS), Windows_NT)
    ifeq ($(PROCESSOR_ARCHITECTURE), AMD64)
        SELECTED_DUMMY_OBJ = AIDummy.o.Win64.Cygwin
    else
        SELECTED_DUMMY_OBJ = AIDummy.o.Win32.MinGW
    endif
endif

DUMMY_OBJ = AIDummy.o

# Add here any extra .o player files you want to link to the executable
EXTRA_OBJ =

# Configuration
OPTIMIZE = 2 # Optimization level    (0 to 3)
DEBUG    = 0 # Compile for debugging (0 or 1)
PROFILE  = 0 # Compile for profile   (0 or 1)

# For debugging matches against Dummy
# OPTIMIZE = 0, DEBUG = 1

# For debugging matches among your players
# OPTIMIZE = 0, DEBUG = 1 and add -D_GLIBCXX_DEBUG at the end of DEBUG_FLAGS

# Flags
ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif

ifeq ($(strip $(DEBUG)),1)
	DEBUGFLAGS=-g -O0 -fno-inline #-D_GLIBCXX_DEBUG 
endif

CXXFLAGS = -std=c++11 -Wall -Wno-unused-variable -fPIC $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE))
LDFLAGS  = -std=c++11                            $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE))

# The following two lines will detect all your players (files matching "AI*.cc")
PLAYERS_SRC = $(wildcard AI*.cc)
PLAYERS_OBJ = $(patsubst %.cc, %.o, $(PLAYERS_SRC)) $(EXTRA_OBJ) $(DUMMY_OBJ)

# Rules
OBJ = Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o Utils.o 

all: Game

clean:
	rm -rf Game  *.o *.exe Makefile.deps

# Crear un alias para AIDummy.o
$(DUMMY_OBJ): $(SELECTED_DUMMY_OBJ)
	cp $(SELECTED_DUMMY_OBJ) $@

Game:  $(OBJ) Game.o Main.o $(PLAYERS_OBJ) 
	$(CXX) $^ -o $@ $(LDFLAGS)

SecGame: $(OBJ) SecGame.o SecMain.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lrt

%.exe: %.o $(OBJ) SecGame.o SecMain.o 
	$(CXX) $^ -o $@ $(LDFLAGS) -lrt

Makefile.deps: *.cc
	$(CXX) $(CXXFLAGS) -MM *.cc > Makefile.deps

include Makefile.deps
