#include <iostream>
#include <fabricsim/topology.hpp>
#include <fabricsim/routing_engine.hpp>
#include <fabricsim/traffic_generator.hpp>
#include <fabricsim/simulator.hpp>

using namespace fabricsim;

int main() {
    std::cout << "--- Testing Data-Plane Simulator ---" << std::endl;

    Topology topo;
    try {
        std::cout << "1. Generating a k=4 Fat-Tree Topology..." << std::endl;
        topo.generate_fat_tree(4, 100.0);

        std::cout << "2. Pre-computing ECMP BFS routing tables... ";
        RoutingEngine router(topo);
        std::cout << "Done!" << std::endl;

        std::cout << "3. Generating All-to-All Traffic..." << std::endl;
        TrafficGenerator tg;
        std::vector<Flow> all_to_all_flows = tg.generate_all_to_all(topo, 1e9);
        std::cout << "   Generated " << all_to_all_flows.size() << " flows." << std::endl;

        // Initialize simulator with a small buffer size (e.g., max 5 packets per switch queue)
        // A small buffer means congestion will trigger drops very quickly!
        std::cout << "4. Initializing Simulator (Max Buffer Size = 5 packets)..." << std::endl;
        Simulator sim(topo, router, 5);

        // Inject traffic
        sim.inject_traffic(all_to_all_flows);
        std::cout << "   Injected " << all_to_all_flows.size() * 10 << " packets into source queues." << std::endl;

        // Run simulation for 100 ticks
        std::cout << "5. Running Simulation for 100 ticks..." << std::endl;
        sim.run(100);

        std::cout << "\n--- Simulation Results ---" << std::endl;
        std::cout << "Packets Delivered successfully: " << sim.get_total_delivered() << std::endl;
        std::cout << "Packets Dropped due to queue overflows: " << sim.get_total_dropped() << std::endl;
        
        if (sim.get_total_dropped() > 0) {
            std::cout << "SUCCESS: We successfully simulated ECMP incast congestion!" << std::endl;
        } else {
            std::cout << "WARNING: No packets dropped. Try decreasing buffer size or increasing traffic." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
