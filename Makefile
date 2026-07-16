CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Directories
BIN_DIR = bin
EXAMPLES_DIR = examples
SRC_DIR = src

# Targets
all: $(BIN_DIR)/udp_sender $(BIN_DIR)/udp_receiver $(BIN_DIR)/test_topology $(BIN_DIR)/test_traffic $(BIN_DIR)/test_routing $(BIN_DIR)/test_simulator $(BIN_DIR)/test_adaptive_routing $(BIN_DIR)/visualize_adaptive

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Examples
$(BIN_DIR)/udp_sender: $(EXAMPLES_DIR)/udp_sender.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/udp_receiver: $(EXAMPLES_DIR)/udp_receiver.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/test_topology: $(EXAMPLES_DIR)/test_topology.cpp $(SRC_DIR)/topology.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/test_traffic: $(EXAMPLES_DIR)/test_traffic.cpp $(SRC_DIR)/topology.cpp $(SRC_DIR)/traffic_generator.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/test_routing: $(EXAMPLES_DIR)/test_routing.cpp $(SRC_DIR)/topology.cpp $(SRC_DIR)/routing_engine.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/test_simulator: $(EXAMPLES_DIR)/test_simulator.cpp $(SRC_DIR)/topology.cpp $(SRC_DIR)/traffic_generator.cpp $(SRC_DIR)/routing_engine.cpp $(SRC_DIR)/simulator.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/test_adaptive_routing: $(EXAMPLES_DIR)/test_adaptive_routing.cpp $(SRC_DIR)/topology.cpp $(SRC_DIR)/traffic_generator.cpp $(SRC_DIR)/routing_engine.cpp $(SRC_DIR)/simulator.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/visualize_adaptive: $(EXAMPLES_DIR)/visualize_adaptive.cpp $(SRC_DIR)/topology.cpp $(SRC_DIR)/traffic_generator.cpp $(SRC_DIR)/routing_engine.cpp $(SRC_DIR)/simulator.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
