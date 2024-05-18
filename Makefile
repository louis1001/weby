OUTPUT_DIR = build
SRC_FILES = $(wildcard src/*.c)
CLANG_OPTS = -Wimplicit-fallthrough -pedantic-errors -Werror -Wall -Wextra
INCLUDE_DIR = -I./include/

all: webser

# Build and run the program
webser: $(SRC_FILES)
	mkdir -p $(OUTPUT_DIR)
	clang $(CLANG_OPTS) $(INCLUDE_DIR) -o $(OUTPUT_DIR)/$@ $^

build: $(SRC_FILES)
	mkdir -p $(OUTPUT_DIR)
	clang -o $(INCLUDE_DIR) $(OUTPUT_DIR)/webser $(CLANG_OPTS) $^

run: webser
	./$(OUTPUT_DIR)/webser

# Clean the build directory
clean:
	rm -rf $(OUTPUT_DIR)
