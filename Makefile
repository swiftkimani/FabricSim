CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Directories
BIN_DIR = bin
EXAMPLES_DIR = examples
SRC_DIR = src

# Targets
all: $(BIN_DIR)/udp_sender $(BIN_DIR)/udp_receiver

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Examples
$(BIN_DIR)/udp_sender: $(EXAMPLES_DIR)/udp_sender.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/udp_receiver: $(EXAMPLES_DIR)/udp_receiver.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
