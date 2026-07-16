#pragma once
#include <vector>

namespace fabricsim {

enum class PacketType {
    DATA,
    TELEMETRY
};

// Represents a physical packet traveling across the fabric.
// A Flow is split into many Packets.
struct Packet {
    int packet_id;
    int flow_id;
    int src_id;
    int dst_id;
    double size_bytes; // Usually around 1500 bytes (MTU)
    PacketType type = PacketType::DATA;
    int enqueue_tick = 0; // Tick when the packet entered the network

    std::vector<int> path_trace; // Stamps of node IDs visited
};

} // namespace fabricsim
