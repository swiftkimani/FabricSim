#include <iostream>
// std::string not needed here
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fabricsim/raii_socket.hpp>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. Create a UDP socket
    // AF_INET = IPv4, SOCK_DGRAM = UDP
    int raw_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (raw_sock < 0) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }
    
    // RAII wrapper: The socket will automatically close when `managed_sock` goes out of scope!
    fabricsim::ManagedSocket managed_sock(raw_sock);
    int sock = managed_sock.get();

    // 2. Configure the address we want to listen on
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // Convert port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces

    // 3. Bind the socket to the port
    if (bind(sock, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed! Is port " << PORT << " already in use?" << std::endl;
        // No need to call close(sock) here! RAII takes care of it.
        return 1;
    }

    std::cout << "Receiver is listening on UDP port " << PORT << "..." << std::endl;

    // 4. Wait for incoming messages
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true) {
        // recvfrom blocks until data is received
        int bytes_received = recvfrom(sock, (char *)buffer, BUFFER_SIZE, 
                                      MSG_WAITALL, (struct sockaddr *)&client_addr, &client_len);
        
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the string
            std::cout << "Received: '" << buffer << "' from " 
                      << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) 
                      << std::endl;
        }
    }

    // 5. Clean up (we'll never actually reach this in an infinite loop, but good practice)
    // No need to manually close(sock)! When main() exits, `managed_sock` is destroyed and closes the socket.
    return 0;
}
