#include <iostream>
#include <fabricsim/topology.hpp>
#include <fabricsim/traffic_generator.hpp>

using namespace fabricsim;

int main() {
    std::cout << "--- Testing Traffic Generator ---" << std::endl;

    Topology topo;
    TrafficGenerator tg;

    try {
        std::cout << "Generating a k=4 Fat-Tree Topology..." << std::endl;
        topo.generate_fat_tree(4, 100.0);
        
        int num_hosts = topo.get_hosts().size();
        std::cout << "Topology generated successfully with " << num_hosts << " hosts." << std::endl;

        std::cout << "Generating All-to-All Traffic..." << std::endl;
        // Generate an all-to-all pattern where each flow is 1GB
        std::vector<Flow> all_to_all_flows = tg.generate_all_to_all(topo, 1e9);

        std::cout << "Generated " << all_to_all_flows.size() << " flows." << std::endl;
        
        // For 16 hosts, we expect 16 * 15 = 240 flows.
        if (num_hosts == 16 && all_to_all_flows.size() == 240) {
            std::cout << "SUCCESS: Math checks out! Incast congestion conditions created." << std::endl;
        } else {
            std::cout << "WARNING: Expected 240 flows for k=4, but got " << all_to_all_flows.size() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
