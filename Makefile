all: blub

WARNINGS = -w
DEBUG = -ggdb -fno-omit-frame-pointer -D DEBUG_RENDERER_PRINTID
OPTIMIZE = -O2
STANDARD = c++17
INCLUDEDIRS = -I/usr/local/include/SDL2 -I/usr/include/SDL2/ -I./include
LIBS = -lSDL2 -lSDL2_gpu -lpthread
SRC_DIR = src
BIN_DIR = bin
SRC = $(wildcard $(SRC_DIR)/*.cpp)

blub: $(SRC)
	mkdir -p $(BIN_DIR)
	$(CXX) -std=$(STANDARD) -o $(BIN_DIR)/$@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(INCLUDEDIRS) $(LIBS) $^

clean:
	rm -rf $(BIN_DIR)

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
# LD_LIBRARY_PATH for SDL_gpu, change may be needed
run:
	LD_LIBRARY_PATH=/usr/local/lib $(BIN_DIR)/blub -x 1920 -y 1080

profile:
	LD_LIBRARY_PATH=/usr/local/lib valgrind --tool=callgrind $(BIN_DIR)/blub -x 1920 -y 1080
