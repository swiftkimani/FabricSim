#pragma once

namespace fabricsim {

// Represents a chunk of data (a flow) that needs to travel from one node to another
struct Flow {
    int flow_id;
    int src_id;
    int dst_id;
    double size_bytes;
    double start_time; // For future event-driven simulation
};

} // namespace fabricsim
