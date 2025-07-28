########################################
#         lfo / lfo~ Pd Wrapper        #
########################################

CXX = g++
UNAME := $(shell uname -m)

CXXFLAGS_HOST = -DHOST_SINGLE_PRECISION

ifeq ($(UNAME),x86_64)
	CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -fPIC -Iinclude -I../audiokern/include
	DEPLOY_DIR = ../../bin/lfo/linux_x64
endif

ifeq ($(UNAME),armv7l)
	CXXFLAGS_BASE = -Wall -Wextra -std=c++17 -fPIC -Iinclude -I../audiokern/include-mfpu=neon -mfloat-abi=hard -march=armv7-a -DUSE_SINGLE_PRECISION
	DEPLOY_DIR = ../../bin/lfo/linux_arm
endif

LIB_DIR   = ../../lib
DSP_LIB   = $(LIB_DIR)/libaudiokern.a

# === [lfo] ===
LFO_NAME      = lfo
LFO_SRC_FILE  = src/lfo.cpp
LFO_OBJ_FILE  = ../../obj/$(LFO_NAME)/lfo.o
LFO_OUT       = $(LIB_DIR)/$(LFO_NAME).pd_linux

# === [lfo~] ===
LFO_T_NAME     = lfo~
LFO_T_SRC_FILE = src/lfo~.cpp
LFO_T_OBJ_FILE = ../../obj/$(LFO_T_NAME)/lfo~.o
LFO_T_OUT      = $(LIB_DIR)/$(LFO_T_NAME).pd_linux

# === Build Target ===
all: $(LFO_OUT) $(LFO_T_OUT) deploy

# === Link Targets ===
$(LFO_OUT): $(LFO_OBJ_FILE) $(DSP_LIB)
	@mkdir -p $(LIB_DIR)
	@echo "Linking $@"
	$(CXX) -shared -o $@ $^

$(LFO_T_OUT): $(LFO_T_OBJ_FILE) $(DSP_LIB)
	@mkdir -p $(LIB_DIR)
	@echo "Linking $@"
	$(CXX) -shared -o $@ $^

# === Compile Sources ===
$(LFO_OBJ_FILE): $(LFO_SRC_FILE)
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LFO_T_OBJ_FILE): $(LFO_T_SRC_FILE)
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Debug / Release ===
debug:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS_BASE) -O0 -g -DDEBUG $(CXXFLAGS_HOST)" all

release:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS_BASE) -O3 $(CXXFLAGS_HOST)" all

# === Deploy ===
deploy:
	@mkdir -p $(DEPLOY_DIR)
	@echo "Deploying to $(DEPLOY_DIR)"
	cp $(LFO_OUT) $(DEPLOY_DIR)/lfo.pd_linux
	cp $(LFO_T_OUT) $(DEPLOY_DIR)/lfo~.pd_linux

# === Clean ===
clean:
	rm -rf ../../obj/$(LFO_NAME) ../../obj/$(LFO_T_NAME) $(LFO_OUT) $(LFO_T_OUT)

.PHONY: all clean debug release deploy
