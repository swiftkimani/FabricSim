#include <iostream>
#include <vector>
#include <fabricsim/topology.hpp>
#include <fabricsim/routing_engine.hpp>
#include <fabricsim/simulator.hpp>
#include <fabricsim/traffic_generator.hpp>

using namespace fabricsim;

int main() {
    std::cout << "--- Generating Telemetry Stream ---\n";

    // 1. Setup Network (k=4 Fat-Tree)
    Topology topo;
    topo.generate_fat_tree(4, 100.0);
    RoutingEngine router(topo);
    
    // 2. Initialize Simulator with adaptive routing = true, path_logging = false, telemetry_stream = true
    Simulator sim(topo, router, 50, true, false, true);

    // 3. Generate some heavy traffic (e.g. All-to-All)
    TrafficGenerator traffic_gen;
    std::vector<Flow> all_to_all_flows = traffic_gen.generate_all_to_all(topo, 100000); // 100 KB per flow (to reduce sim time)
    std::cout << "Generated " << all_to_all_flows.size() << " all-to-all flows.\n";
    
    // Inject and run for 100 ticks
    sim.inject_traffic(all_to_all_flows);
    
    std::cout << "Running simulation for 100 ticks. Telemetry will be streamed to telemetry.csv...\n";
    sim.run(100);

    std::cout << "Done! Run scripts/visualize_heatmap.py to see the results.\n";
    
    return 0;
}
