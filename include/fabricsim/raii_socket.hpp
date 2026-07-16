#pragma once

#include <unistd.h>
#include <stdexcept>
#include <iostream>

namespace fabricsim {

/**
 * @brief An RAII (Resource Acquisition Is Initialization) wrapper for a socket file descriptor.
 * 
 * In C++, we don't use garbage collection. Instead, we tie the lifecycle of a resource 
 * (like memory, or a socket) to the lifecycle of an object. When the object goes out of 
 * scope, its destructor is automatically called, guaranteeing the resource is cleaned up.
 */
class ManagedSocket {
public:
    // Constructor acquires the resource (the socket fd)
    explicit ManagedSocket(int fd) : fd_(fd) {
        if (fd_ < 0) {
            throw std::runtime_error("Invalid socket descriptor provided.");
        }
    }

    // Destructor releases the resource
    ~ManagedSocket() {
        if (fd_ >= 0) {
            // std::cout << "[RAII] Automatically closing socket " << fd_ << std::endl;
            close(fd_);
            fd_ = -1;
        }
    }

    // Delete copy constructor and assignment operator to prevent double-closing the same socket
    ManagedSocket(const ManagedSocket&) = delete;
    ManagedSocket& operator=(const ManagedSocket&) = delete;

    // Allow move semantics (transferring ownership of the socket)
    ManagedSocket(ManagedSocket&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1; // Strip ownership from the original
    }

    ManagedSocket& operator=(ManagedSocket&& other) noexcept {
        if (this != &other) {
            if (fd_ >= 0) close(fd_);
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    // Access the raw file descriptor
    int get() const { return fd_; }

private:
    int fd_;
};

} // namespace fabricsim
