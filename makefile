.PHONY: clean env lint test

TARGET_EXEC ?= main

CXXFLAGS := -std=c++17 -ggdb -MMD -MP -fopenmp
CFLAGS   := -MMD -MP

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./include

# Source files
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -o -name '*.c' -o -name '*.s')
HDRS := $(shell find $(SRC_DIRS) -name '*.hpp' -o -name '*.h')
TSTS := $(shell find tests -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) /usr/local/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

PKG_CONFIG := pkg-config
LUA_VERSION := lua5.3
LUA_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(LUA_VERSION))
LUA_LIBS := $(shell $(PKG_CONFIG) --libs $(LUA_VERSION))

LLAMA_CFLAGS := -I/usr/local/include
LLAMA_LIBS := -L/usr/local/lib -Wl,--start-group -lllama -lggml -lggml-cpu -lggml-base -Wl,--end-group
GGML_LIBS    := $(patsubst lib%.a,-l%,$(notdir $(wildcard $(LLAMA_PREFIX)/lib/libggml*.a)))

CPPFLAGS ?= $(INC_FLAGS) $(LUA_CFLAGS) $(LLAMA_CFLAGS)
LDFLAGS ?= -lboost_system -lpthread $(LUA_LIBS) -L/usr/local/lib -lqjs -ldl -lstdc++fs $(LLAMA_LIBS) -lm -fopenmp

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(CPPFLAGS)

# Assembly
$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

env:
	docker build -t gzrbt .
	docker run --rm -it -v $(shell pwd):/app gzrbt /bin/bash

-include $(DEPS)

lint:
	clang-format -i --style=file $(SRCS) $(HDRS) $(TSTS)
	# clang-tidy $(SRCS) $(HDRS) $(TSTS) --fix

TOBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.cpp.o,$(TSTS))
TARGET_TEST  := $(BUILD_DIR)/test

$(TARGET_TEST): $(filter-out %/main.cpp.o,$(OBJS)) $(TOBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

test: $(TARGET_TEST)
	$<
