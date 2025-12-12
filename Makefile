PROJECT_NAME := helbreath_client
SRC_DIR := Sources/HelbreathRaylib
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := Sources/HelbreathRaylib
DEPENDENCIES_INCLUDE := Sources/Dependencies/Includes
SHARED_INCLUDE := Sources/Dependencies/Shared

CXX := g++
CXXFLAGS := -std=c++20 -g -w -I$(INCLUDE_DIR) -I$(DEPENDENCIES_INCLUDE) -I$(SHARED_INCLUDE) -MMD -MP

LDFLAGS := -lraylib -lsqlite3 -lpthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -I/Users/l.eduardo/repos/Helbreath-3.82/Dependencies/Server/Includes
    LDFLAGS += -L/opt/homebrew/lib
endif

SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
TARGET := $(BIN_DIR)/$(PROJECT_NAME)

.PHONY: all clean run

all: $(TARGET)
	@echo "Build complete: $(TARGET)"

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

run: all
	@cd $(BIN_DIR) && ./$(PROJECT_NAME)

clean:
	@rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
	@rm -rf $(BIN_DIR)/$(PROJECT_NAME)
	@echo "Clean complete"

-include $(OBJECTS:.o=.d)
