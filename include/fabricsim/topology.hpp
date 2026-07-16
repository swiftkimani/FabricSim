#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace fabricsim {

// A simple representation of a node in our network topology
struct Node {
    int id;
    std::string type; // e.g., "host", "edge_switch", "core_switch"
};

// Represents a directed or undirected link between two nodes
struct Link {
    int source_id;
    int target_id;
    double capacity_gbps; // Link bandwidth
};

/**
 * @brief Base class for network topologies.
 * 
 * Module 3 Exercise:
 * Represent a topology as a graph. You can use an adjacency list:
 * std::unordered_map<int, std::vector<int>> adjacency_list;
 */
class Topology {
public:
    Topology() = default;
    virtual ~Topology() = default;

    // Add a node to the graph
    void add_node(int id, const std::string& type);

    // Add a bi-directional link between two nodes
    void add_link(int node_a, int node_b, double capacity);

    // Get all neighbors for a given node
    const std::vector<int>& get_neighbors(int node_id) const;

    // Generate a k-ary Fat-Tree (Clos) topology
    void generate_fat_tree(int k, double link_capacity);

    // Retrieve all nodes of a specific type (e.g., "host_gpu")
    std::vector<int> get_nodes_by_type(const std::string& type) const;

    // Helper to specifically get all host GPUs
    std::vector<int> get_hosts() const { return get_nodes_by_type("host_gpu"); }

    // Get the total number of nodes
    int get_num_nodes() const { return nodes_.size(); }

    // Get the total number of links (directed)
    int get_num_links() const { return links_.size(); }

private:
    std::unordered_map<int, Node> nodes_;
    std::unordered_map<int, std::vector<int>> adjacency_list_;
    std::vector<Link> links_;
};

} // namespace fabricsim
