#include <iostream>
#include "Server.h"
#include "TCP.h"
#include "Server_reply_construc.h"

using namespace std;



int main(int argc, char* argv[]) {
    int opt;
    u_int16_t port = 4567; // Default port
    u_int16_t timeout = 250; // Default timeout
    u_int8_t retransmission = 3; // Default retransmission attempts
    string hostname = "0.0.0.0"; // Default hostname


    // Parsing command line arguments
    while ((opt = getopt(argc, argv, "l:p:d:r:h")) != -1) {
        switch (opt) {
            case 'l': // Hostname
                hostname = optarg;
                break;
            case 'p': // Port
                port = atoi(optarg);
                break;
            case 'd': // Timeout
                timeout = atoi(optarg);
                break;
            case 'r': // Retransmission
                retransmission = atoi(optarg);
                break;
            case 'h': // Help
                cout << "Usage: .ipk24chat-server [options]\n"
                     << "Options:\n"
                     << "  -l <IP>       Set the hostname IP (default: 0.0.0.0)\n"
                     << "  -p <port>     Set the port number (default: 4567)\n"
                     << "  -d <duration> Set the timeout duration (default: 250)\n"
                     << "  -r <number>   Set the retransmission attempts (default: 3)\n"
                     << "  -h            Display this help and exit\n";
                return EXIT_SUCCESS;
        }
    }
    //check if port is out of possible range
    if (port < 1 || port > 65535) {
        cerr << "Error: Invalid port number\n";
        return EXIT_FAILURE;
    }
    //server constructor
    Server server(port, hostname, timeout, retransmission);
    server.server_run();


    return 0;
}
