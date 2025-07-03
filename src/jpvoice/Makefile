########################################
#         jpvoice~ Pd Wrapper          #
########################################

CXX = g++
UNAME := $(shell uname -m)

ifeq ($(UNAME),x86_64)
	CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -fPIC -Iinclude -I../dsp/include
	DEPLOY_BASE = linux_x64
endif

ifeq ($(UNAME),armv7l)
	CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -fPIC -Iinclude -I../dsp/include -mfpu=neon -mfloat-abi=hard -march=armv7-a -DUSE_SINGLE_PRECISION
	DEPLOY_BASE = linux_arm
endif

DEPLOY_DIR = ../../bin/jpvoice/$(DEPLOY_BASE)

OBJ_NAME    = jpvoice~
SRC_DIR     = src
OBJ_DIR     = ../../obj/$(OBJ_NAME)
LIB_DIR     = ../../lib
LIB_NAME    = $(OBJ_NAME).pd_linux
OUT_FILE    = $(LIB_DIR)/$(LIB_NAME)
DSP_LIB     = $(LIB_DIR)/libdsp.a

# === Source & Object files ===
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# === Build Target ===
all: $(OUT_FILE) deploy

$(OUT_FILE): $(OBJECTS) $(DSP_LIB)
	@mkdir -p $(LIB_DIR)
	@echo "Linking $@"
	$(CXX) -shared -o $@ $^

# === Compile Sources ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Debug / Release Targets ===
debug:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS_BASE) -O0 -g" all

release:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS_BASE) -O3" all

deploy:
	@mkdir -p $(DEPLOY_DIR)
	@echo "Deploying to $(DEPLOY_DIR)"
	cp $(OUT_FILE) $(DEPLOY_DIR)/$(LIB_NAME)
	cp ../../bin/adsr/$(DEPLOY_BASE)/adsr~.pd_linux $(DEPLOY_DIR)
	cp ../../bin/lfo/$(DEPLOY_BASE)/lfo~.pd_linux $(DEPLOY_DIR)

# === Clean ===
clean:
	rm -rf $(OBJ_DIR) $(OUT_FILE)

.PHONY: all clean debug release deploy
