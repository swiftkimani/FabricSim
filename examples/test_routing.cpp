#include <fabricsim/routing_engine.hpp>
#include <fabricsim/topology.hpp>
#include <iostream>
#include <set>

using namespace fabricsim;

void print_path(const std::vector<int> &path) {
  for (size_t i = 0; i < path.size(); ++i) {
    std::cout << path[i];
    if (i < path.size() - 1)
      std::cout << " -> ";
  }
  std::cout << std::endl;
}

int main() {
  std::cout << "--- Testing ECMP Routing Engine ---" << std::endl;

  Topology topo;
  try {
    std::cout << "Generating a k=4 Fat-Tree Topology..." << std::endl;
    topo.generate_fat_tree(4, 100.0);

    std::cout << "Pre-computing ECMP BFS routing tables... ";
    RoutingEngine router(topo);
    std::cout << "Done!" << std::endl;

    // Pick two hosts. In our generation order for k=4 (36 nodes):
    // Cores: 1..4
    // Pod 0 (Agg: 5,6. Edge: 7,8)
    // Pod 1 (Agg: 9,10. Edge: 11,12)
    // ...
    // Hosts: 21..36
    // Let's pick Host 21 (Pod 0) to Host 36 (Pod 3).
    int src_host = 21;
    int dst_host = 36;

    std::cout << "\nSending 10 different flows from Node " << src_host
              << " to Node " << dst_host << "..." << std::endl;

    std::set<std::vector<int>> unique_paths;

    for (int i = 1; i <= 10; ++i) {
      Flow f;
      f.flow_id = i;
      f.src_id = src_host;
      f.dst_id = dst_host;
      f.size_bytes = 1024; // 1 KB
      f.start_time = 0.0;

      std::vector<int> path = router.compute_full_path_ecmp(f);
      unique_paths.insert(path);

      std::cout << "Flow " << i << " path: ";
      print_path(path);
    }

    std::cout << "\nTotal unique paths taken: " << unique_paths.size()
              << std::endl;
    if (unique_paths.size() > 1) {
      std::cout << "SUCCESS: ECMP successfully balanced traffic across "
                   "multiple equal-cost paths!"
                << std::endl;
    } else {
      std::cout << "WARNING: Only 1 path used. Hash function might not be "
                   "distributing well."
                << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
