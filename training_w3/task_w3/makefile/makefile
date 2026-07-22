CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -MMD -MP

# Nếu chạy make thì Build = release / nếu chạy make BUILD=DEBUG thì nó chạy debug
BUILD ?= release

ifeq ($(BUILD), debug)
    CXXFLAGS += -g -O0 -DDEBUG
	TARGET_SUFFIX = _debug
else
	CXXFLAGS += -O3 -DNDEBUG
	TARGET_SUFFIX = 
endif

TARGET = app$(TARGET_SUFFIX)

SRCS = $(shell find src -type f -name "*.cpp") # kĩ thuật shell giúp dò tìm các file cpp bên trong src
OBJS = $(SRCS:src/%.cpp=build/%.o)
DEPS = $(OBJS:.o=.d)


all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking executable: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $< -> $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

#make run tự động build và run luôn chương trình
run: $(TARGET)
	@./$(TARGET)

clean: 
	@rm -rf build app app_debug

.PHONY: all clean run
-include $(DEPS)