.PHONY: clean lint test

TARGET_EXEC ?= main

CXX := g++
CXXFLAGS := -std=c++17 -ggdb

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./include

# source files
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -o -name '*.c' -o -name '*.s')
HDRS := $(shell find $(SRC_DIRS) -name '*.hpp' -o -name '*.h')
TSTS := $(shell find tests -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

PKG_CONFIG := pkg-config
LUA_VERSION := lua5.3
LUA_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(LUA_VERSION))
LUA_LIBS := $(shell $(PKG_CONFIG) --libs $(LUA_VERSION))

CPPFLAGS ?= $(INC_FLAGS) $(LUA_CFLAGS)
LDFLAGS ?= -lboost_system -lpthread $(LUA_LIBS) -ldl -lstdc++fs

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(CPPFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

lint:
	clang-format -i --style=file $(SRCS) $(HDRS) $(TSTS)

TOBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.cpp.o,$(TSTS))
TARGET_TEST  := $(BUILD_DIR)/test

$(TARGET_TEST): $(filter-out %/main.cpp.o,$(OBJS)) $(TOBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

test: $(TARGET_TEST)
	$<
