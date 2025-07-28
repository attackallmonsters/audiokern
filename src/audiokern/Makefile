########################################
#         DSP-Only Makefile            #
########################################

CXX = g++
UNAME := $(shell uname -m)

CXXFLAGS_HOST = -DHOST_SINGLE_PRECISION

ifeq ($(UNAME),x86_64)
	CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -fPIC -Iinclude
endif

ifeq ($(UNAME),armv7l)
	CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -fPIC -Iinclude -mfpu=neon -mfloat-abi=hard -march=armv7-a -DUSE_SINGLE_PRECISION
endif

OBJ_NAME    = libaudiokern
SRC_DIR     = src
OBJ_DIR     = ../../obj/$(OBJ_NAME)
LIB_DIR     = ../../lib
LIB_NAME    = $(OBJ_NAME).a
OUT_FILE    = $(LIB_DIR)/$(LIB_NAME)

# === DSP sources ===
DSP_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
DSP_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DSP_SOURCES))

# === Build rules ===
all: $(OUT_FILE)

$(OUT_FILE): $(DSP_OBJECTS)
	@mkdir -p $(LIB_DIR)
	@echo "Creating static library $@"
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Debug/Release ===
debug:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS_BASE) -O0 -g -DDEBUG $(CXXFLAGS_HOST)" all

release:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS_BASE) -O3 $(CXXFLAGS_HOST)" all

# === Clean ===
clean:
	rm -rf $(OBJ_DIR) $(OUT_FILE)

.PHONY: all clean debug release
