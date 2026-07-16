#include <fabricsim/routing_engine.hpp>
#include <queue>
#include <stdexcept>

namespace fabricsim {

RoutingEngine::RoutingEngine(const Topology& topo) : topo_(topo) {
    compute_routing_tables();
}

void RoutingEngine::compute_routing_tables() {
    int num_nodes = topo_.get_num_nodes();

    // The topology IDs are 1-indexed up to num_nodes
    // We compute shortest paths via BFS from every possible destination backwards.
    for (int dst = 1; dst <= num_nodes; ++dst) {
        std::vector<int> distances(num_nodes + 1, -1);
        std::queue<int> q;
        
        distances[dst] = 0;
        q.push(dst);

        // Standard BFS
        while (!q.empty()) {
            int current = q.front();
            q.pop();

            int current_dist = distances[current];

            const auto& neighbors = topo_.get_neighbors(current);
            for (int neighbor : neighbors) {
                if (distances[neighbor] == -1) {
                    distances[neighbor] = current_dist + 1;
                    q.push(neighbor);
                }
            }
        }

        // Now populate the valid equal-cost next hops for every node targeting `dst`.
        // If a neighbor is exactly 1 step closer to the destination, it's a valid next hop.
        for (int u = 1; u <= num_nodes; ++u) {
            if (u == dst || distances[u] == -1) continue;

            const auto& neighbors = topo_.get_neighbors(u);
            for (int v : neighbors) {
                if (distances[v] == distances[u] - 1) {
                    routing_table_[dst][u].push_back(v);
                }
            }
        }
    }
}

unsigned int RoutingEngine::hash_flow(const Flow& flow) const {
    // A simple hash function combining src, dst, and flow_id
    // In a real switch, this hashes the IP 5-tuple.
    unsigned int h = 17;
    h = h * 31 + flow.src_id;
    h = h * 31 + flow.dst_id;
    h = h * 31 + flow.flow_id;
    return h;
}

unsigned int RoutingEngine::hash_packet(const Packet& packet) const {
    // ECMP must ensure all packets from the same flow take the same path!
    // Therefore, we hash based on the flow_id, NOT the packet_id.
    unsigned int h = 17;
    h = h * 31 + packet.src_id;
    h = h * 31 + packet.dst_id;
    h = h * 31 + packet.flow_id;
    return h;
}

int RoutingEngine::get_next_hop_ecmp(int current_node, int dst_node, const Flow& flow) const {
    auto dst_it = routing_table_.find(dst_node);
    if (dst_it == routing_table_.end()) {
        throw std::runtime_error("Destination not found in routing table.");
    }

    auto node_it = dst_it->second.find(current_node);
    if (node_it == dst_it->second.end()) {
        throw std::runtime_error("No path found from current node to destination.");
    }

    const auto& next_hops = node_it->second;
    if (next_hops.empty()) {
        throw std::runtime_error("Empty next hop list.");
    }

    // ECMP selection: hash modulo number of paths
    unsigned int h = hash_flow(flow);
    int selected_index = h % next_hops.size();

    return next_hops[selected_index];
}

int RoutingEngine::get_next_hop_ecmp_packet(int current_node, int dst_node, const Packet& packet) const {
    auto dst_it = routing_table_.find(dst_node);
    if (dst_it == routing_table_.end()) {
        throw std::runtime_error("Destination not found in routing table.");
    }

    auto node_it = dst_it->second.find(current_node);
    if (node_it == dst_it->second.end()) {
        throw std::runtime_error("No path found from current node to destination.");
    }

    const auto& next_hops = node_it->second;
    if (next_hops.empty()) {
        throw std::runtime_error("Empty next hop list.");
    }

    // ECMP selection: hash modulo number of paths
    unsigned int h = hash_packet(packet);
    int selected_index = h % next_hops.size();

    return next_hops[selected_index];
}

void RoutingEngine::process_telemetry(const Packet& telemetry_packet) {
    // The source of the telemetry packet is the node that is congested
    int congested_node = telemetry_packet.src_id;
    // Increase its penalty!
    node_penalties_[congested_node] += 10;
}

void RoutingEngine::decay_penalties() {
    for (auto& pair : node_penalties_) {
        if (pair.second > 0) {
            pair.second -= 1; // Slowly forgive congested nodes
        }
    }
}

int RoutingEngine::get_next_hop_adaptive(int current_node, int dst_node, const Packet& packet) const {
    auto dst_it = routing_table_.find(dst_node);
    if (dst_it == routing_table_.end()) {
        throw std::runtime_error("Destination not found in routing table.");
    }

    auto node_it = dst_it->second.find(current_node);
    if (node_it == dst_it->second.end()) {
        throw std::runtime_error("No path found from current node to destination.");
    }

    const auto& next_hops = node_it->second;
    if (next_hops.empty()) {
        throw std::runtime_error("Empty next hop list.");
    }

    // ADAPTIVE HEURISTIC:
    // Filter out next hops that have a penalty > 0
    std::vector<int> healthy_hops;
    for (int hop : next_hops) {
        auto pen_it = node_penalties_.find(hop);
        int penalty = (pen_it != node_penalties_.end()) ? pen_it->second : 0;
        
        // If penalty is low/zero, it's healthy!
        if (penalty == 0) {
            healthy_hops.push_back(hop);
        }
    }

    // If ALL next hops are congested, we have no choice but to use standard ECMP
    // over all of them. Otherwise, we hash over ONLY the healthy ones!
    const auto& hops_to_use = healthy_hops.empty() ? next_hops : healthy_hops;

    unsigned int h = hash_packet(packet);
    int selected_index = h % hops_to_use.size();

    return hops_to_use[selected_index];
}



std::vector<int> RoutingEngine::compute_full_path_ecmp(const Flow& flow) const {
    std::vector<int> path;
    int current = flow.src_id;
    int dst = flow.dst_id;

    path.push_back(current);

    while (current != dst) {
        int next_hop = get_next_hop_ecmp(current, dst, flow);
        path.push_back(next_hop);
        current = next_hop;
    }

    return path;
}

} // namespace fabricsim
