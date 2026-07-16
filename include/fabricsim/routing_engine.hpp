#pragma once

#include <vector>
#include <unordered_map>
#include <fabricsim/topology.hpp>
#include <fabricsim/flow.hpp>
#include <fabricsim/packet.hpp>

namespace fabricsim {

class RoutingEngine {
public:
    RoutingEngine(const Topology& topo);

    /**
     * @brief Computes the shortest paths from all nodes to all destinations.
     * This populates routing_table_.
     */
    void compute_routing_tables();

    /**
     * @brief Gets the next hop using ECMP hashing.
     */
    int get_next_hop_ecmp(int current_node, int dst_node, const Flow& flow) const;

    /**
     * @brief Gets the next hop using ECMP hashing for a specific packet.
     */
    int get_next_hop_ecmp_packet(int current_node, int dst_node, const Packet& packet) const;

    /**
     * @brief Gets the next hop using Adaptive Routing (avoids penalized nodes).
     */
    int get_next_hop_adaptive(int current_node, int dst_node, const Packet& packet) const;

    /**
     * @brief Processes a telemetry packet and updates node penalties.
     */
    void process_telemetry(const Packet& telemetry_packet);

    /**
     * @brief Slowly decays all penalties over time (called every tick).
     */
    void decay_penalties();

    /**
     * @brief Computes the full path from src to dst for a specific flow using ECMP.
     */
    std::vector<int> compute_full_path_ecmp(const Flow& flow) const;

private:
    const Topology& topo_;

    // routing_table_[destination][current_node] = vector of equal-cost next-hop nodes
    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> routing_table_;

    // Penalty map: node_id -> penalty score
    // mutable so it can be updated inside const routing functions if needed, 
    // but process_telemetry isn't const anyway.
    std::unordered_map<int, int> node_penalties_;

    // Simple hash function for flow IDs
    unsigned int hash_flow(const Flow& flow) const;

    // Simple hash function for packets (based on their flow_id)
    unsigned int hash_packet(const Packet& packet) const;
};

} // namespace fabricsim
