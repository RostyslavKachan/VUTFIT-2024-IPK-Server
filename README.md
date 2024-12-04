# **IPK Project 2**

## Table of Contents
- [IPK Project 2](#IPK-Projec-2)
    - [Description](#description)
    - [Theory](#theory)
        - [TCP](#tcp)
        - [UDP](#udp)
    - [Implementation](#implementation)
    - [Usage](#usage)
    - [Program running](#program-running)
        - [main.cpp](#main.cpp)
        - [Class Server](#class-server)
        - [Class TCP](#tclass-tcp)
        - [UML diagram](#uml-diagram)
    - [Exiting the Program](#exiting-the-program)
    - [Restrictions](#restrictions)
    - [Testing](#testing)
        - [Cases](#cases)
    - [Bibliography](#bibliography)
    
## Description

This project's task is to design and implement a server application capable of communicating with remote clients using the `IPK24-CHAT` protocol. The protocol has got two variants - each built on top of a different transport protocol: TCP or UDP.

## Theory

### TCP 
TCP[RFC9293], as a key component of the Internet protocol suite (TCP/IP), ensures the reliable, ordered, and error-checked delivery of data between applications across IP networks. It guarantees that data packets arrive in sequence and error-free by utilizing acknowledgments and resending any packets that are lost or damaged during transmission.

### UDP
UDP[RFC768] (User Datagram Protocol) is a simpler, connectionless Internet protocol that allows the sending of messages (datagrams) without establishing a dedicated end-to-end connection. Unlike TCP, UDP does not guarantee the reliable delivery of data, meaning it does not perform retransmissions of lost packets, order packets, or provide data integrity checks. This makes UDP faster and more efficient for applications where quick delivery is more important than perfect accuracy, such as streaming audio, video, or in gaming environments. UDP is used for broadcasting and multicast streaming because it supports packet switching without requiring session establishment.

## Implementation

The server is implemented in C++ in the C++20 standard. Client application is divided into source files . Header files holds system libraries and defines which i use in my code. In my project, I incorporate Object-Oriented Programming (OOP) principles to structure and organize the code efficiently.

## Usage

The client can be compiled using `make` command. The executable file is called `ipk24chat-server`. To run client use 
```shell
./ipk24chat-server
``` 
supported optional command line options:
- `-l <IP address>` - Server listening IP address for welcome sockets, default ip is `0.0.0.0`
- `-p <port>` - Server listening port (`uint16`) for welcome sockets, default port is `4567`
- `-d <timeout>` - UDP connection timeout (`uint16`) in *ms*, default value is `250 ms`
- `-r <retries>` - UDP connection retries (retransmit) number (`uint8`), default value is `3`
- `-h` - Prints program help output and exits

## Program running
### main.cpp
The main function initializes default settings for the server, including a default port of 4567, a default timeout of 250 milliseconds, a default retransmission attempt limit of 3, and a default hostname of "0.0.0.0". It then parses command-line options using the 'getopt()' function, allowing the user to customize the hostname, port number, timeout duration, and number of retransmission attempts. Then program check if port is in range(1 to 65535). After that creates a `Server` object with the specified settings and calls its `server_run()` method to start the server, which continuously handles incoming client connections and data until terminated.

### Class `Server`
This class has some important methods:
- Main method of this class is `server_run()`. It handles socket creation, signal setup for graceful shutdown, and the management of an epoll[Epoll] instance for handling multiple client connections. 

- Method `create_socket()` creates welcome socket for listening incoming connections. It is a TCP socket,  setting it up for IPv4 communications and binding it to a specified port. 
 
- Method `accept_new_client()` accept new clients and configure them to non-blocking mode, added to the client list, and their file descriptors are registered with epoll for monitoring.

- Method `findClientBySocket()` return a pointer that points to class TCP object.

- Methods `receive_data()` and `send_data()` which send/receive data to/from client socket.

- Method `removeClient()` remove client from vector `clients` . All clients are stored in global vector `clients`.

### Class `TCP`
- This class represents TCP client[Project1]. Basicaly, cpp file has object constructor/destructor , different getters/setters.This class stores all client data such as: client socket, state, displayname, port ,hostname, channel.

### UML diagram 
This UML diagram represent start connection between client and server by using function `accept()` and then sending `AUTH` message to server.

![accept_client_UML](/accept_client_UML.jpg)


## Exiting the Program
You can exit the program gracefully by press `CTRL+C`, which change flag `quit` to true , in cycle `while()`
server send all clients -> `BYE\r\n` and start close their sockets and remove them from vector `clients`. In the end close epoll , close welcome socket, and exit `SUCCESS`.

## Restrictions
I use some Unix-specific Libraries such as: 
- `<arpa/inet.h>`
- `<netdb.h>`
- `<netinet/in.h>`
- `<sys/socket.h>`
- `<unistd.h>`
- `<sys/types.h>`
- `<sys/epoll.h>`
Because of these libraries, I can't run my program on Windows.

## Testing
In this topic would be described application testing and validation. During the testing process was used such tools as `Wireshark` application to handle and analyze transmitted packets, additional Lua script to detect `IPK24-CHAT` packets. Also i used my [Project1] client for testing my server functionality.

### Cases
Connect one client to server and then server stopped(CTRL+C was pushed)
Client terminal
```
./ipk24chat-client -t tcp -s 127.0.0.1
/auth 1 2 john
Success: AUTH success
Server: john has joined default
hi
my
name is John
```
Server terminal
```
./ipk24chat-server 
RECV 127.0.0.1:56374 | AUTH
SENT 127.0.0.1:56374 | REPLY
RECV 127.0.0.1:56374 | MSG
RECV 127.0.0.1:56374 | MSG
RECV 127.0.0.1:56374 | MSG
^CSENT 127.0.0.1:56374 | BYE
```

Wireshark
No. | Time         | Source    | Destination | Protocol   | Length | Info       | Info
----|--------------|-----------|-------------|------------|--------|------------|----------------------------------
4   | 7.095013993  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 90     | C → Server | aUtH 1 aS john uSinG 2
6   | 7.095156643  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | Server → C | REPLY OK IS AUTH success
8   | 7.095174660  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 110    | Server → C | MSG FROM Server IS john has joined default
10  | 9.287385809  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 87     | C → Server | msG FRoM john iS hi
12  | 10.655122481 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 87     | C → Server | msG FRoM john iS my
14  | 15.055835287 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 97     | C → Server | msG FRoM john iS name is John
16  | 26.704685822 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 71     | Server → C | BYE



Connect 3 clients to server and comunicate with each other gradually leave server(one connect to another channel)
Client1 terminal
```
./ipk24chat-client -t tcp -s 127.0.0.1
/auth 1 2 john
Success: AUTH success
Server: john has joined default
Server: many has joined default
Server: silvia has joined default
silvia: hi
ola
many: pryvit
many: how are you
many: vut
bogdan: cau
i change channel
/join chan1
Success: JOIN success
Server: john has joined chan1
hi
i alone
```
Client2 terminal
```
./ipk24chat-client -t tcp -s 127.0.0.1
/auth 1 2 many
Success: AUTH success
Server: many has joined default
Server: silvia has joined default
silvia: hi
john: ola
pryvit
how are you
vut
/rename bogdan
cau   
john: i change channel
Server: john has left default
i need to go
```
Client3 terminal
```
./ipk24chat-client -t tcp -s 127.0.0.1
/auth 1 2 silvia
Success: AUTH success
Server: silvia has joined default
hi
john: ola
many: pryvit
many: how are you
many: vut
bogdan: cau
john: i change channel
Server: john has left default
bogdan: i need to go
Server: bogdan has left default
```
Server
```
./ipk24chat-server 
RECV 127.0.0.1:55268 | AUTH
SENT 127.0.0.1:55268 | REPLY
RECV 127.0.0.1:57578 | AUTH
SENT 127.0.0.1:57578 | REPLY
RECV 127.0.0.1:41552 | AUTH
SENT 127.0.0.1:41552 | REPLY
RECV 127.0.0.1:41552 | MSG
SENT 127.0.0.1:55268 | MSG
SENT 127.0.0.1:57578 | MSG
RECV 127.0.0.1:55268 | MSG
SENT 127.0.0.1:57578 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:57578 | MSG
SENT 127.0.0.1:55268 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:57578 | MSG
SENT 127.0.0.1:55268 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:57578 | MSG
SENT 127.0.0.1:55268 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:57578 | MSG
SENT 127.0.0.1:55268 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:55268 | MSG
SENT 127.0.0.1:57578 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:55268 | JOIN
SENT 127.0.0.1:55268 | REPLY
RECV 127.0.0.1:55268 | MSG
RECV 127.0.0.1:55268 | MSG
RECV 127.0.0.1:55268 | BYE
RECV 127.0.0.1:57578 | MSG
SENT 127.0.0.1:41552 | MSG
RECV 127.0.0.1:57578 | BYE
RECV 127.0.0.1:41552 | BYE
```
Wireshark
No. | Time          | Source    | Destination | Protocol   | Length | Info       | Info   
----|---------------|-----------|-------------|------------|--------|------------|------------------------------------------
4   | 8.255627485   | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 90     | C → Server | aUtH 1 aS john uSinG 2
6   | 8.255723995   | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | Server → C | REPLY OK IS AUTH success
8   | 8.255772114   | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 110    | Server → C | MSG FROM Server IS john has joined default
13  | 20.679266761  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 90     | C → Server | aUtH 1 aS many uSinG 2
15  | 20.679390961  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | Server → C | REPLY OK IS AUTH success
17  | 20.679408320  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 110    | Server → C | MSG FROM Server IS many has joined default
19  | 20.679443657  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 110    | Server → C | MSG FROM Server IS many has joined default
24  | 29.103363028  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | C → Server | aUtH 1 aS silvia uSinG 2
26  | 29.103455583  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | Server → C | REPLY OK IS AUTH success
28  | 29.103472683  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 112    | Server → C | MSG FROM Server IS silvia has joined default
30  | 29.103483186  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 112    | Server → C | MSG FROM Server IS silvia has joined default
32  | 29.103492885  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 112    | Server → C | MSG FROM Server IS silvia has joined default
34  | 32.471589814  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 89     | C → Server | msG FRoM silvia iS hi
35  | 32.471653844  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 89     | Server → C | msG FRoM silvia iS hi
37  | 32.471673942  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 89     | Server → C | msG FRoM silvia iS hi
40  | 37.455187418  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 88     | C → Server | msG FRoM john iS ola
41  | 37.455236406  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 88     | Server → C | msG FRoM john iS ola
43  | 37.455251061  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 88     | Server → C | msG FRoM john iS ola
46  | 64.696036141  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 91     | C → Server | msG FRoM many iS pryvit
47  | 64.696095197  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 91     | Server → C | msG FRoM many iS pryvit
49  | 64.696114703  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 91     | Server → C | msG FRoM many iS pryvit
52  | 71.135600840  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 96     | C → Server | msG FRoM many iS how are you
54  | 71.135654131  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 96     | Server → C | msG FRoM many iS how are you
56  | 71.135670666  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 96     | Server → C | msG FRoM many iS how are you
58  | 72.799194226  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 88     | C → Server | msG FRoM many iS vut
60  | 72.799248669  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 88     | Server → C | msG FRoM many iS vut
62  | 72.799288216  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 88     | Server → C | msG FRoM many iS vut
64  | 90.295743499  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 90     | C → Server | msG FRoM bogdan iS cau
66  | 90.295797555  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 90     | Server → C | msG FRoM bogdan iS cau
68  | 90.295813512  | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 90     | Server → C | msG FRoM bogdan iS cau
70  | 100.919615503 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 101    | C → Server | msG FRoM john iS i change channel
72  | 100.919673575 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 101    | Server → C | msG FRoM john iS i change channel
74  | 100.919692093 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 101    | Server → C | msG FRoM john iS i change channel
76  | 108.575388840 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 86     | C → Server | jOIn chan1 aS john
78  | 108.575501669 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 108    | Server → C | MSG FROM Server IS john has left default
80  | 108.575515373 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 108    | Server → C | MSG FROM Server IS john has left default
82  | 108.575525856 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | Server → C | REPLY OK IS JOIN success
84  | 108.575539129 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 108    | Server → C | MSG FROM Server IS john has joined chan1
86  | 123.839538114 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 87     | C → Server | msG FRoM john iS hi
88  | 129.775014905 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 92     | C → Server | msG FRoM john iS i alone
90  | 132.855766783 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 71     | C → Server | bYe
95  | 144.368123102 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 99     | C → Server | msG FRoM bogdan iS i need to go
97  | 144.368187900 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 99     | Server → C | msG FRoM bogdan iS i need to go
99  | 145.119256124 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 71     | C → Server | bYe
102 | 145.119368164 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 110    | Server → C | MSG FROM Server IS bogdan has left default
106 | 148.919254621 | 127.0.0.1 | 127.0.0.1   | IPK24-CHAT | 71     | C → Server | bYe


Testcases which also were tested:
- Rename in client and then send messages on server
- Invalid content of particular packet item, such as display name, message content
- Client pushed CTRL+C 

## Bibliography

[RFC9293] Eddy, W. _Transmission Control Protocol (TCP)_ [online]. August 2022. [cited 2024-02-11]. DOI: 10.17487/RFC9293. Available at: https://datatracker.ietf.org/doc/html/rfc9293

[RFC768] Postel, J. User Datagram Protocol [online]. March 1997. [cited 2024-02-11]. DOI: 10.17487/RFC0768. Available at: https://datatracker.ietf.org/doc/html/rfc768

[Project1] Dolejška, D. Client for a chat server using IPK24-CHAT protocol [online]. February 2024. [cited 2024-02-14]. Available at: https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%201

[Epoll] Kerrisk, epoll(7) — Linux manual page[online] [cited 2023-12-22]. Available at: https://man7.org/linux/man-pages/man7/epoll.7.html
