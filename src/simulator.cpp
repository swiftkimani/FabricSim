#include <fabricsim/simulator.hpp>
#include <iostream>
#include <algorithm>

namespace fabricsim {

Simulator::Simulator(const Topology& topo, RoutingEngine& router, size_t max_buffer_size, bool use_adaptive_routing, bool enable_path_logging, bool enable_telemetry_stream)
    : topo_(topo), router_(router), max_buffer_size_(max_buffer_size), use_adaptive_routing_(use_adaptive_routing), enable_path_logging_(enable_path_logging), enable_telemetry_stream_(enable_telemetry_stream) {
    // Initialize empty queues for all nodes
    int num_nodes = topo_.get_num_nodes();
    for (int i = 1; i <= num_nodes; ++i) {
        buffers_[i] = std::queue<Packet>();
    }

    if (enable_telemetry_stream_) {
        telemetry_file_.open("telemetry.csv");
        if (telemetry_file_.is_open()) {
            // Write CSV header
            telemetry_file_ << "tick,node_id,queue_depth\n";
        } else {
            std::cerr << "Warning: Could not open telemetry.csv for writing.\n";
        }
    }
}

Simulator::~Simulator() {
    if (telemetry_file_.is_open()) {
        telemetry_file_.close();
    }
}

void Simulator::inject_traffic(const std::vector<Flow>& flows) {
    int packet_id_counter = 1;
    for (const Flow& f : flows) {
        // Convert a massive flow into physical packets.
        // For simplicity, let's say 1 Flow = 10 Packets for our simulation baseline, 
        // regardless of size_bytes, just so we can see queue buildup.
        int num_packets = 10;
        for (int i = 0; i < num_packets; ++i) {
            Packet p;
            p.packet_id = packet_id_counter++;
            p.flow_id = f.flow_id;
            p.src_id = f.src_id;
            p.dst_id = f.dst_id;
            p.size_bytes = 1500; // MTU
            p.enqueue_tick = 0; // Injected at tick 0
            p.path_trace.push_back(f.src_id); // Stamp source node
            
            // Inject into the source node's buffer!
            buffers_[f.src_id].push(p);
        }
    }
}

void Simulator::run(int max_ticks) {
    int num_nodes = topo_.get_num_nodes();

    for (int tick = 0; tick < max_ticks; ++tick) {
        // We need a temporary snapshot of who is sending what this tick,
        // so packets moved this tick don't get moved again in the same tick.
        std::vector<std::pair<int, Packet>> packets_to_move;

        // 1. For every node, pop 1 packet and route it
        for (int node_id = 1; node_id <= num_nodes; ++node_id) {
            if (!buffers_[node_id].empty()) {
                Packet p = buffers_[node_id].front();
                buffers_[node_id].pop();

                if (node_id == p.dst_id) {
                    // Packet arrived at destination!
                    if (p.type == PacketType::DATA) {
                        delivered_packets_++;
                        latencies_.push_back(tick - p.enqueue_tick);
                        
                        if (enable_path_logging_) {
                            print_packet_trace(p);
                        }
                    } else if (p.type == PacketType::TELEMETRY) {
                        // Consumed telemetry packet. Update routing weights!
                        if (use_adaptive_routing_) {
                            router_.process_telemetry(p);
                        }
                    }
                } else {
                    // Route to next hop
                    int next_hop = 0;
                    if (use_adaptive_routing_) {
                        next_hop = router_.get_next_hop_adaptive(node_id, p.dst_id, p);
                    } else {
                        next_hop = router_.get_next_hop_ecmp_packet(node_id, p.dst_id, p);
                    }
                    packets_to_move.push_back({next_hop, p});
                }
            }
        }

        // 2. Push packets into the next hop buffers (check for congestion!)
        for (const auto& move : packets_to_move) {
            int target_node = move.first;
            const Packet& p = move.second;

            if (buffers_[target_node].size() >= max_buffer_size_) {
                // BUFFER FULL! Congestion!
                dropped_packets_++;
            } else {
                // ... Telemetry generation logic ...
                // Check if we are crossing the 80% threshold exactly now
                size_t threshold = static_cast<size_t>(max_buffer_size_ * 0.8);
                if (buffers_[target_node].size() == threshold) {
                    // Only print telemetry warning if path logging is disabled to avoid cluttering the visualizer
                    if (!enable_path_logging_) {
                        std::cout << "[TELEMETRY] Node " << target_node 
                                  << " queue at 80%, broadcasting warning!" << std::endl;
                    }
                    
                    // Generate telemetry packets to neighbors
                    const auto& neighbors = topo_.get_neighbors(target_node);
                    for (int neighbor : neighbors) {
                        Packet telemetry_p;
                        telemetry_p.packet_id = -1; // special ID
                        telemetry_p.flow_id = -1;
                        telemetry_p.src_id = target_node;
                        telemetry_p.dst_id = neighbor;
                        telemetry_p.size_bytes = 64; // small control packet
                        telemetry_p.type = PacketType::TELEMETRY;
                        telemetry_p.enqueue_tick = tick; // Generated this tick

                        if (buffers_[neighbor].size() < max_buffer_size_) {
                            buffers_[neighbor].push(telemetry_p);
                        } else {
                            dropped_packets_++; // Telemetry packet dropped due to extreme congestion
                        }
                    }
                }

                // Push packet and stamp the passport!
                Packet updated_p = p;
                updated_p.path_trace.push_back(target_node);
                buffers_[target_node].push(updated_p);
            }
        }
        
        if (use_adaptive_routing_) {
            router_.decay_penalties(); // Decay penalties at the end of the tick
        }

        // Stream telemetry for this tick
        if (enable_telemetry_stream_ && telemetry_file_.is_open()) {
            for (int node_id = 1; node_id <= num_nodes; ++node_id) {
                telemetry_file_ << tick << "," << node_id << "," << buffers_[node_id].size() << "\n";
            }
        }
    }
}

void Simulator::print_benchmark_stats() const {
    if (latencies_.empty()) {
        std::cout << "   [Benchmark] No packets delivered, cannot compute latency percentiles." << std::endl;
        return;
    }

    std::vector<int> sorted_latencies = latencies_;
    std::sort(sorted_latencies.begin(), sorted_latencies.end());

    int p50_index = sorted_latencies.size() * 0.50;
    int p99_index = sorted_latencies.size() * 0.99;

    std::cout << "   [Benchmark] Delivered Packets: " << sorted_latencies.size() << std::endl;
    std::cout << "   [Benchmark] p50 Latency: " << sorted_latencies[p50_index] << " ticks" << std::endl;
    std::cout << "   [Benchmark] p99 Latency: " << sorted_latencies[p99_index] << " ticks" << std::endl;
}

void Simulator::print_packet_trace(const Packet& p) const {
    std::cout << "[Flow " << p.flow_id << " | Packet " << p.packet_id << "] Path: ";
    for (size_t i = 0; i < p.path_trace.size(); ++i) {
        std::cout << p.path_trace[i];
        if (i < p.path_trace.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
}

} // namespace fabricsim
