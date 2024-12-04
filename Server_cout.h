#ifndef IPK2_SERVER_COUT_H
#define IPK2_SERVER_COUT_H
#include "Server.h"
void receive_data_cout(TCP_client* ptr, const string& type);
void send_data_cout(TCP_client* ptr, const char* buffer);
#endif //IPK2_SERVER_COUT_H
