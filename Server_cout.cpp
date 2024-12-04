#include "Server_cout.h"
//Construct Server output
void receive_data_cout(TCP_client* ptr, const string& type){
    string line = "RECV " + ptr->get_hostname() + ":" + to_string(ptr->get_port()) + " | " + type;
    cout << line << endl;
}
void send_data_cout(TCP_client* ptr, const char* buffer){
    istringstream iss(buffer);
    string type;
    iss >> type;
    string line = "SENT " + ptr->get_hostname() + ":" + to_string(ptr->get_port()) + " | " + type;
    cout << line << endl;
}