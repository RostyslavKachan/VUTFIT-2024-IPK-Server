#ifndef IPK2_SERVER_H
#define IPK2_SERVER_H


#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/epoll.h>
#include <sstream>
#include <csignal>
#include <atomic>
#include <utility>
#include <string>
#include <fcntl.h>
#include <map>
#include <vector>
#include <iterator>
#include <csignal>

#include "TCP.h"
#include "Server_reply_construc.h"
#include "MSG_validate.h"
#include "Server_cout.h"

using namespace std;

#define MAX_EVENTS 30
#define BUFSIZE 1024 //buffer size


extern bool quit;


class Server{
public:
    Server(u_int16_t k_port, const string& k_hostname, u_int16_t k_timeout, u_int8_t k_retransmission);
    ~Server();
    u_int16_t get_port();
    string get_hostname();
    void server_run();
    void create_socket();
    void accept_new_client(int epollFd);
    void receive_data(char* buffer, int client_socket);
    void send_data(const char* buffer, int client_socket);
    TCP_client* findClientBySocket(const vector<TCP_client*>& clients, int socket);
    void removeClient( TCP_client* targetClient);

    static void handler_signal(int signal);
    void  setup_signal_handlers();




    //private attributes
private:
    int pr_list_socket;
    u_int16_t pr_port;
    string pr_hostname;
    u_int16_t pr_timeout;
    u_int8_t pr_retransmission;
    struct sockaddr_in serv_addr{};
    socklen_t addr_len = sizeof(serv_addr);
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];


};
#endif //IPK2_SERVER_H
