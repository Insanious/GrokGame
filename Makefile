PROJECT_NAME = GrokGame

SFML_PATH = external/SFML-3.0.0
SFML_LIB_PATH = $(SFML_PATH)/lib
SFML_INCLUDE_PATH = $(SFML_PATH)/include

CXX = g++
CXXFLAGS = -std=c++17 -I./src -I/usr/local/include -I$(SFML_INCLUDE_PATH) -o0 -g
CXXFLAGS += -Wall -Wextra -Wpedantic -Werror -Wnon-virtual-dtor
LDFLAGS = -L$(SFML_LIB_PATH) -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-audio-s -lsfml-network-s
LDFLAGS += -lX11 -lXrandr -lXi -lXcursor -ludev -lpthread -ldl -lfreetype

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
HEADER = $(wildcard src/*.hpp)
OUT = $(PROJECT_NAME)

all: $(OUT)

$(OUT): build/pch.hpp.gch $(OBJ)
	mkdir -p out
	$(CXX) $(OBJ) -o out/$(OUT) $(LDFLAGS)

build/%.o: src/%.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -include src/pch.hpp -c $< -o $@

build/pch.hpp.gch: src/pch.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/pch.hpp -o build/pch.hpp.gch

clean:
	rm -rf build/* out/*
	