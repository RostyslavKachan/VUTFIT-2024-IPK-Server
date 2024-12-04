#ifndef IPK2_TCP_H
#define IPK2_TCP_H
//#include "Server.h"
#include <string>
using namespace std;

enum IPK_States{
    ACCEPT,
    AUTH,  // authentication
    OPEN, // communication with server
    ERROR, // error state
    END // close connection and close socket
};

class TCP_client{
public:
    TCP_client(int k_client_socket,IPK_States state);
    ~TCP_client()
    {};
    int get_socket();
    IPK_States get_state();
    void set_state(IPK_States state);
    void set_displayname(const string& name);
    string get_channel();
    void set_channel(const string& channel);
    string get_displayname();
    u_int16_t get_port();
    void set_port(u_int16_t port);
    string get_hostname();
    void set_hostname(string hostname);
private:
    int pr_client_sock;
    string pr_channelID = "default"; // default value when object creates
    IPK_States pr_state;
    string pr_displayname;
    u_int16_t pr_port;
    string pr_hostname;
};


#endif //IPK2_TCP_H
