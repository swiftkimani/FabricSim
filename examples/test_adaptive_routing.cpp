#include <iostream>
#include <fabricsim/topology.hpp>
#include <fabricsim/routing_engine.hpp>
#include <fabricsim/traffic_generator.hpp>
#include <fabricsim/simulator.hpp>

using namespace fabricsim;

void run_sim_scenario(bool use_adaptive) {
    Topology topo;
    topo.generate_fat_tree(4, 100.0);
    RoutingEngine router(topo);

    TrafficGenerator tg;
    std::vector<Flow> all_to_all_flows = tg.generate_all_to_all(topo, 1e9);

    Simulator sim(topo, router, 5, use_adaptive);
    sim.inject_traffic(all_to_all_flows);

    sim.run(500);

    std::cout << "   Packets Delivered: " << sim.get_total_delivered() << std::endl;
    std::cout << "   Packets Dropped  : " << sim.get_total_dropped() << std::endl;
    sim.print_benchmark_stats();
}

int main() {
    std::cout << "--- Testing Adaptive Routing vs ECMP ---" << std::endl;

    try {
        std::cout << "\n[Scenario 1] Running Standard ECMP (No Adaptive Routing)..." << std::endl;
        run_sim_scenario(false);

        std::cout << "\n[Scenario 2] Running Adaptive Routing (Telemetry Enabled)..." << std::endl;
        run_sim_scenario(true);

        std::cout << "\nCompare the dropped packets above! Adaptive routing should dramatically reduce drops." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
