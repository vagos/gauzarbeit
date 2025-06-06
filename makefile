TARGET_EXEC ?= main

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -ggdb

# Project directories
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./include

# Source files
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -o -name '*.c' -o -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Include directories
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# External libraries via pkg-config
PKG_CONFIG := pkg-config
LUA_VERSION := lua5.3
LUA_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(LUA_VERSION))
LUA_LIBS := $(shell $(PKG_CONFIG) --libs $(LUA_VERSION))

# Include and library flags
CPPFLAGS ?= $(INC_FLAGS) $(LUA_CFLAGS)
LDFLAGS ?= -lboost_system -lpthread $(LUA_LIBS) -ldl -lstdc++fs

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(CPPFLAGS)

# # assembly
# $(BUILD_DIR)/%.s.o: %.s
# 	$(MKDIR_P) $(dir $@)
# 	$(AS) $(ASFLAGS) -c $< -o $@

# # c source
# $(BUILD_DIR)/%.c.o: %.c
# 	$(MKDIR_P) $(dir $@)
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

# Format C++ sources according to .clang-format
.PHONY: format
FORMAT_SRCS := $(shell find $(SRC_DIRS) include -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \))
format:
	clang-format -i --style=file $(FORMAT_SRCS)
