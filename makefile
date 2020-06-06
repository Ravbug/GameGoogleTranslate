# flags needed to build the target (compiler, target name, and compiler flags)
CC = c++
CFLAGS := -I./include -std=c++14

# location of source files
source_dir = src
build_dir = build


minecraftsplitter: all
	CC $(CFLAGS) $(source_dir)/MinecraftTranslate.cpp $(source_dir)/OperationBase.cpp -o $(build_dir)/minecraftsplitter

all:
	mkdir -p build