#include <fabricsim/traffic_generator.hpp>

namespace fabricsim {

std::vector<Flow> TrafficGenerator::generate_all_to_all(const Topology& topo, double flow_size_bytes) {
    std::vector<Flow> all_flows;
    
    // Get all hosts in the topology
    std::vector<int> hosts = topo.get_hosts();

    int flow_counter = 1;

    // Every host sends to every other host
    for (int src : hosts) {
        for (int dst : hosts) {
            if (src == dst) continue; // A host doesn't send to itself

            Flow f;
            f.flow_id = flow_counter++;
            f.src_id = src;
            f.dst_id = dst;
            f.size_bytes = flow_size_bytes;
            f.start_time = 0.0; // Incast happens when everyone fires at t=0

            all_flows.push_back(f);
        }
    }

    return all_flows;
}

} // namespace fabricsim
