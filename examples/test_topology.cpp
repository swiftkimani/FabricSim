#include <iostream>
#include <fabricsim/topology.hpp>

using namespace fabricsim;

int main() {
    std::cout << "--- Testing Topology Graph ---" << std::endl;

    Topology topo;

    try {
        std::cout << "Generating a k=4 Fat-Tree Topology..." << std::endl;
        
        topo.generate_fat_tree(4, 100.0);
        
        std::cout << "Topology generated successfully!" << std::endl;
        std::cout << "Total Nodes in Graph: " << topo.get_num_nodes() << std::endl;
        std::cout << "Total Links in Graph (directed): " << topo.get_num_links() << std::endl;

        // Verify a specific edge switch's neighbors (an edge switch in a k=4 fat-tree should connect to 2 aggregation switches and 2 hosts = 4 neighbors)
        // From our generation order:
        // Cores = 4 (IDs: 1, 2, 3, 4)
        // Pod 0 Aggs = 2 (IDs: 5, 6)
        // Pod 0 Edges = 2 (IDs: 7, 8)
        int sample_edge_id = 7;
        const auto& edge_neighbors = topo.get_neighbors(sample_edge_id);
        std::cout << "Node " << sample_edge_id << " (an edge_switch) is connected to " << edge_neighbors.size() << " nodes." << std::endl;
        std::cout << "Neighbors of Node " << sample_edge_id << ": ";
        for (int neighbor_id : edge_neighbors) {
            std::cout << neighbor_id << " ";
        }
        std::cout << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
