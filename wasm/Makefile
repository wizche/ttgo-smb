# Build LVGL for WebAssembly: wasm/lvgl.html, lvgl.js, lvgl.wasm

###############################################################################
# Typical Compile to WebAssembly with emscripten
# emcc hello.c -s WASM=1 -o hello.html

# Define $(CSRCS)
LVGL_DIR 	  := .
LVGL_DIR_NAME := lvgl
include lvgl.mk

# WebAssembly C and C++ Source Files
WASM_CSRCS := \
	lv_port_disp.c 

# Build LVGL app: wasm/lvgl.html, lvgl.js, lvgl.wasm
TARGETS:= lvgl

DEPS   := lv_conf.h

# Use emscripten compiler
CC     := emcc
CPP    := em++

# Options for emscripten. We specify the C and Rust WebAssembly functions to be exported.
CCFLAGS := \
	-g \
	-I . \
	-I lvgl/lv_core \
	-D SIMULATOR \
	-D LV_USE_DEMO_WIDGETS \
	-s WASM=1 \
    -s "EXPORTED_FUNCTIONS=[ '_main', '_get_display_buffer', '_get_display_width', '_get_display_height', '_test_display', '_init_display', '_render_display', '_render_widgets', '_init_simulator', '_update_date_time']" 

LDFLAGS :=

MAINS  := $(addsuffix .o, $(TARGETS) )
OBJ    := \
	$(MAINS) \
	$(CSRCS:.c=.o) \
	$(WASM_CSRCS:.c=.o)

SIMSRC := $(shell find ../src/ -name "*.cpp")
SIMSRC := $(filter-out ../src/main.cpp, $(SIMSRC))
SIMSRC += simulator.cpp
SIMOBJ := \
	$(SIMSRC:.cpp=.o) 

SPRITESRC += $(shell find ../src/ -name "*.c")
SPRITEOBJ := \
	$(SPRITESRC:.c=.o)

.PHONY: all clean

all: $(TARGETS)

clean:
	rm ../src/*/*.o || true
	rm *.o || true
	rm wasm/*.o || true
	rm wasm/*.wasm || true
	rm wasm/*.js || true
	rm wasm/*.txt || true
	rm -r $(HOME)/.emscripten_cache || true

$(OBJ): %.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

$(info $$OBJ is [${OBJ}])

$(SIMOBJ): %.o : %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CCFLAGS)

$(SPRITEOBJ): %.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

$(info $$SIMOBJ is [${SIMOBJ}])
$(info $$SPRITEOBJ is [${SPRITEOBJ}])

$(TARGETS): % : $(filter-out $(MAINS), $(OBJ)) $(SIMOBJ) $(SPRITEOBJ) %.o
	$(CC) -o $@.html \
	-Wl,--start-group \
	$(LIBS) \
	$^ \
	-Wl,--end-group \
	$(CCFLAGS) \
	$(LDFLAGS)
