#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <string>
#include <fabricsim/topology.hpp>
#include <fabricsim/routing_engine.hpp>
#include <fabricsim/packet.hpp>
#include <fabricsim/flow.hpp>

namespace fabricsim {

class Simulator {
public:
    // Initialize the simulator with a topology, routing engine, and buffer size for nodes
    Simulator(const Topology& topo, RoutingEngine& router, size_t max_buffer_size, bool use_adaptive_routing = false, bool enable_path_logging = false, bool enable_telemetry_stream = false);

    // Destructor to close the telemetry file if open
    ~Simulator();

    // Inject a list of flows into the network (converts them to packets at the source host)
    void inject_traffic(const std::vector<Flow>& flows);

    // Run the simulation for a specific number of ticks
    void run(int max_ticks);

    // Get statistics
    int get_total_dropped() const { return dropped_packets_; }
    int get_total_delivered() const { return delivered_packets_; }

    // Print p50 and p99 benchmark stats
    void print_benchmark_stats() const;

private:
    void print_packet_trace(const Packet& p) const;

    const Topology& topo_;
    RoutingEngine& router_;
    size_t max_buffer_size_;
    bool use_adaptive_routing_;
    bool enable_path_logging_;
    bool enable_telemetry_stream_;
    std::ofstream telemetry_file_;

    int dropped_packets_ = 0;
    int delivered_packets_ = 0;

    std::vector<int> latencies_; // Stores latency of all delivered DATA packets

    // Node buffers: node_id -> queue of Packets currently sitting in that node
    std::unordered_map<int, std::queue<Packet>> buffers_;
};

} // namespace fabricsim
