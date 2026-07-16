#pragma once

#include <vector>
#include <fabricsim/topology.hpp>
#include <fabricsim/flow.hpp>

namespace fabricsim {

class TrafficGenerator {
public:
    TrafficGenerator() = default;

    /**
     * @brief Generates an all-to-all traffic pattern among all host_gpus.
     * Every host will send `flow_size_bytes` to every other host.
     */
    std::vector<Flow> generate_all_to_all(const Topology& topo, double flow_size_bytes);
};

} // namespace fabricsim
