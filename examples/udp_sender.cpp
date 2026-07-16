#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fabricsim/raii_socket.hpp>

const int SERVER_PORT = 8080;
const char* SERVER_IP = "127.0.0.1";

int main(int argc, char* argv[]) {
    // Check if user provided a message, otherwise use a default
    std::string message = "Hello, FabricSim! This is a UDP test.";
    if (argc > 1) {
        message = argv[1];
    }

    // 1. Create a UDP socket
    int raw_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (raw_sock < 0) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }
    
    // Wrap it in RAII
    fabricsim::ManagedSocket managed_sock(raw_sock);
    int sock = managed_sock.get();

    // 2. Configure the destination address (where are we sending this?)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Convert IP address from string to binary format
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported" << std::endl;
        // No need to manually close(sock), RAII does it!
        return 1;
    }

    // 3. Send the message
    std::cout << "Sending message to " << SERVER_IP << ":" << SERVER_PORT << "..." << std::endl;
    
    int bytes_sent = sendto(sock, message.c_str(), message.length(), 0, 
                           (const struct sockaddr *)&server_addr, sizeof(server_addr));
                           
    if (bytes_sent < 0) {
        std::cerr << "Failed to send message!" << std::endl;
    } else {
        std::cout << "Successfully sent " << bytes_sent << " bytes." << std::endl;
    }

    // 4. Clean up
    // No need to manually close(sock), RAII does it when `managed_sock` goes out of scope.
    return 0;
}
