# Module 1: Networking Fundamentals

Many developers are highly familiar with **HTTP/HTTPS** and **WebSockets**. These live at the very top of the networking stack. For this simulator, we need to strip away the high-level abstractions and look at how data *actually* moves between machines.

## 1. The OSI Model (Where do TCP/UDP live?)
The OSI model is a conceptual framework that divides network communication into 7 layers. 

*   **Layer 7 (Application):** HTTP, WebSockets, gRPC. This is where you usually work.
*   **Layer 4 (Transport):** **TCP** and **UDP**. This is where we are going.
*   **Layer 3 (Network):** IP (Internet Protocol). Handles routing packets across networks.
*   **Layer 2 (Data Link):** Ethernet, MAC addresses. 
*   **Layer 1 (Physical):** Fiber optic cables, copper wires.

When you make an HTTP `fetch()` request, HTTP (L7) relies on TCP (L4) to make sure the data gets there safely, which relies on IP (L3) to find the server. 

## 2. What actually is a "Socket"?
In software, a **socket** is an endpoint for sending or receiving data across a computer network. 

Think of a socket as a **file descriptor** or a "doorway" managed by your Operating System (OS). 
When you want to send data over the network, your program doesn't talk to the network card directly. Instead:
1. Your program asks the OS: *"Give me a socket for IP 192.168.1.5, Port 8080."*
2. The OS gives you a handle (the socket).
3. You write bytes into that socket (just like writing to a file).
4. The OS takes those bytes, wraps them in TCP/UDP and IP headers, and hands them to the network card to shoot down the wire.

## 3. TCP vs. UDP
This is the most critical distinction for your simulator.

### TCP (Transmission Control Protocol)
*   **The Vibe:** A certified, tracked package delivery.
*   **How it works:** It requires a "handshake" to establish a connection before sending data. If a packet is lost, TCP automatically re-transmits it. It guarantees that packets arrive **in order** and **without errors**.
*   **The Catch:** This reliability requires overhead. The OS has to keep track of sequence numbers, acknowledgments (ACKs), and buffer data to reorder it. If a packet drops, everything stops and waits for the re-transmission (Head-of-Line blocking).
*   **Use cases:** Web browsing (HTTP), file transfers, emails.

### UDP (User Datagram Protocol)
*   **The Vibe:** Throwing a paper airplane across the room and hoping it lands.
*   **How it works:** Connectionless. You just dump bytes into a socket and say "send to this IP/Port." 
*   **The Catch:** It is **lossy** (packets can be dropped if the network is congested) and **unordered** (Packet B might arrive before Packet A). 
*   **Why we use it here:** It is **FAST**. There is no handshake and no OS overhead for tracking ACKs. High-performance fabrics often use UDP (or something similar) because the hardware/fabric is extremely reliable, and if something does drop, they prefer to handle the logic themselves rather than letting the OS TCP stack slow everything down.

## 4. What are "Raw Sockets"?
Normally, if you create a UDP socket, the OS automatically slaps the IP and UDP headers onto your data for you. 

A **raw socket** tells the OS: *"Back off, I will write the headers myself."* 
You manually construct the IP header (source, destination, protocol) and the TCP/UDP header in memory, and hand the raw byte-array directly to the network interface. This is used by low-level networking tools (like `ping`, packet sniffers like Wireshark, or custom routing protocols).

## 5. Datacenter Networking: RDMA and RoCE
In modern AI datacenters (like those used for training massive LLMs), even UDP through the OS kernel is too slow. The CPU spends too much time copying data from the application memory into OS memory, and then to the network card.

*   **RDMA (Remote Direct Memory Access):** Allows a network card to read/write data directly from the RAM of a GPU or CPU on *another* machine, completely bypassing the Operating System kernel on both sides. Zero CPU overhead, zero copies.
*   **RoCE (RDMA over Converged Ethernet):** RDMA originally required specialized, expensive cables (InfiniBand). RoCE is a protocol that allows RDMA to run over standard Ethernet networks. It effectively uses a UDP-like transport but guarantees lossless delivery at the hardware level using Priority Flow Control (PFC). 

In your simulator, you'll be modeling this high-performance, low-latency environment where congestion (incast) is the enemy of RDMA/RoCE.

---

### 🎯 Your Module 1 Check-In

To complete this module, you don't need to write any code. Just think through this scenario:

**"Explain, out loud or in text, what happens when Machine A sends a UDP packet to Machine B."**

*(Hint: Think about the application writing to the socket, the OS adding headers, the packet flying over the wire, and Machine B's OS receiving it and waking up the receiving application.)*
