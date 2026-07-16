#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. Create a UDP socket
    // AF_INET = IPv4, SOCK_DGRAM = UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }

    // 2. Configure the address we want to listen on
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // Convert port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces

    // 3. Bind the socket to the port
    if (bind(sock, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed! Is port " << PORT << " already in use?" << std::endl;
        close(sock);
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
    close(sock);
    return 0;
}
