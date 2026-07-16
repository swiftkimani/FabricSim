#include <fabricsim/topology.hpp>
#include <stdexcept>

namespace fabricsim {

void Topology::add_node(int id, const std::string& type) {
    if (nodes_.find(id) != nodes_.end()) {
        throw std::invalid_argument("Node ID already exists.");
    }
    nodes_[id] = {id, type};
    // Initialize empty adjacency list for this node
    adjacency_list_[id] = std::vector<int>();
}

void Topology::add_link(int node_a, int node_b, double capacity) {
    // Check if nodes exist
    if (nodes_.find(node_a) == nodes_.end() || nodes_.find(node_b) == nodes_.end()) {
        throw std::invalid_argument("Both nodes must exist before linking.");
    }

    // Add to adjacency list (undirected graph means we link both ways)
    adjacency_list_[node_a].push_back(node_b);
    adjacency_list_[node_b].push_back(node_a);

    // Keep a record of the links
    links_.push_back({node_a, node_b, capacity});
    links_.push_back({node_b, node_a, capacity});
}

const std::vector<int>& Topology::get_neighbors(int node_id) const {
    auto it = adjacency_list_.find(node_id);
    if (it == adjacency_list_.end()) {
        throw std::invalid_argument("Node ID does not exist.");
    }
    return it->second;
}

std::vector<int> Topology::get_nodes_by_type(const std::string& type) const {
    std::vector<int> result;
    for (const auto& pair : nodes_) {
        if (pair.second.type == type) {
            result.push_back(pair.first);
        }
    }
    return result;
}

void Topology::generate_fat_tree(int k, double link_capacity) {
    if (k % 2 != 0) {
        throw std::invalid_argument("k must be an even number for a standard Fat-Tree.");
    }

    int num_core_switches = (k / 2) * (k / 2);
    int num_pods = k;
    int num_agg_per_pod = k / 2;
    int num_edge_per_pod = k / 2;
    int num_hosts_per_edge = k / 2;

    int current_id = 1;

    // 1. Generate Core Switches
    std::vector<int> core_switches;
    for (int i = 0; i < num_core_switches; ++i) {
        add_node(current_id, "core_switch");
        core_switches.push_back(current_id);
        current_id++;
    }

    // 2. Generate Pods (Aggregation & Edge Switches)
    std::vector<std::vector<int>> agg_switches(num_pods);
    std::vector<std::vector<int>> edge_switches(num_pods);

    for (int p = 0; p < num_pods; ++p) {
        // Aggregation switches for this pod
        for (int i = 0; i < num_agg_per_pod; ++i) {
            add_node(current_id, "aggregation_switch");
            agg_switches[p].push_back(current_id);
            current_id++;
        }
        // Edge switches for this pod
        for (int i = 0; i < num_edge_per_pod; ++i) {
            add_node(current_id, "edge_switch");
            edge_switches[p].push_back(current_id);
            current_id++;
        }
    }

    // 3. Generate Hosts
    std::vector<int> hosts;
    int total_hosts = num_pods * num_edge_per_pod * num_hosts_per_edge;
    for (int i = 0; i < total_hosts; ++i) {
        add_node(current_id, "host_gpu");
        hosts.push_back(current_id);
        current_id++;
    }

    // 4. Wire Edge to Aggregation (within the same pod)
    // Every edge switch in a pod connects to every aggregation switch in that pod
    for (int p = 0; p < num_pods; ++p) {
        for (int e : edge_switches[p]) {
            for (int a : agg_switches[p]) {
                add_link(e, a, link_capacity);
            }
        }
    }

    // 5. Wire Aggregation to Core
    // The i-th aggregation switch in any pod connects to the core switches in the i-th stride.
    // Core switches are indexed [0 .. (k/2)^2 - 1]
    int stride = k / 2;
    for (int p = 0; p < num_pods; ++p) {
        for (int i = 0; i < num_agg_per_pod; ++i) {
            int agg_node = agg_switches[p][i];
            for (int j = 0; j < stride; ++j) {
                int core_index = i * stride + j;
                int core_node = core_switches[core_index];
                add_link(agg_node, core_node, link_capacity);
            }
        }
    }

    // 6. Wire Hosts to Edge
    // Each edge switch connects to `k/2` distinct hosts
    int host_idx = 0;
    for (int p = 0; p < num_pods; ++p) {
        for (int e : edge_switches[p]) {
            for (int h = 0; h < num_hosts_per_edge; ++h) {
                int host_node = hosts[host_idx++];
                add_link(host_node, e, link_capacity);
            }
        }
    }
}

} // namespace fabricsim
