#include "TCP.h"
//constructor
TCP_client::TCP_client(int k_client_socket,IPK_States state):
pr_client_sock(k_client_socket),
pr_state(state)
{}
// GETTERS AND SETTERS
int TCP_client::get_socket() {
    return pr_client_sock;
}

IPK_States TCP_client::get_state() {
    return pr_state;
}

void TCP_client::set_displayname(const string &name) {
    pr_displayname = name;
}

void TCP_client::set_state(IPK_States state) {
    pr_state = state;
}

string TCP_client::get_displayname() {
    return pr_displayname;
}

void TCP_client::set_channel(const string &channel) {
    pr_channelID = channel;
}

string TCP_client::get_channel() {
    return pr_channelID;
}

u_int16_t TCP_client::get_port() {
    return pr_port;
}

void TCP_client::set_port(u_int16_t port) {
    pr_port = port;
}

string TCP_client::get_hostname() {
    return pr_hostname;
}

void TCP_client::set_hostname(string hostname) {
    pr_hostname = hostname;
}
