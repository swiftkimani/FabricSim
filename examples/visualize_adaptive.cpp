#include <iostream>
#include <vector>
#include <fabricsim/topology.hpp>
#include <fabricsim/routing_engine.hpp>
#include <fabricsim/simulator.hpp>
#include <fabricsim/flow.hpp>

using namespace fabricsim;

int main() {
    std::cout << "======================================================\n";
    std::cout << "  FABRIC SIMULATOR: ADAPTIVE ROUTING VISUALIZATION\n";
    std::cout << "======================================================\n\n";

    // 1. Setup Network (k=4 Fat-Tree)
    Topology topo;
    topo.generate_fat_tree(4, 100.0);
    RoutingEngine router(topo);
    
    // Enable adaptive routing and path logging
    Simulator sim(topo, router, 10, true, true);

    std::cout << "[Step 1] Sending packets for Flow 1 (Host 21 -> Host 36)...\n";
    std::cout << "         The network is completely healthy.\n";
    
    // Create Flow 1
    Flow f1;
    f1.flow_id = 1;
    f1.src_id = 21;  // Host 21 (Pod 0)
    f1.dst_id = 36;  // Host 36 (Pod 3)
    f1.size_bytes = 1500 * 2; // 2 packets worth

    std::vector<Flow> initial_flows = {f1};
    sim.inject_traffic(initial_flows);

    // Run enough ticks for packets to arrive
    sim.run(20);
    std::cout << "\n";

    std::cout << "[Step 2] WARNING! Core Switch 4 is experiencing heavy congestion!\n";
    std::cout << "         Core Switch 4 broadcasts TELEMETRY packets to warn neighbors.\n";
    
    // Manually inject a telemetry warning to the routing engine
    Packet warning;
    warning.src_id = 4; // Core switch 4 is congested!
    warning.type = PacketType::TELEMETRY;
    router.process_telemetry(warning);

    std::cout << "\n[Step 3] Sending MORE packets for the exact same Flow 1...\n";
    
    // Inject more traffic for the EXACT same flow
    sim.inject_traffic(initial_flows);
    
    // Run again
    sim.run(20);
    std::cout << "\n";

    std::cout << "======================================================\n";
    std::cout << "Notice how the second batch of packets took a different\n";
    std::cout << "path to avoid Core Switch 4! The Adaptive Router works!\n";
    std::cout << "======================================================\n";

    return 0;
}
