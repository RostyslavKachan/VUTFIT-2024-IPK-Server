#include "Server_reply_construc.h"
#include "Server.h"
// Parses a buffer into a vector of words separated by whitespace.
std::vector<std::string> get_words(const char* buffer) {
    std::istringstream ss(buffer);
    std::vector<std::string> words(std::istream_iterator<std::string>{ss}, std::istream_iterator<std::string>{});
    return words;
}
// Constructs a positive authentication response and copies it into the provided buffer.
void construct_Pos_Auth_Reply(char* buffer){
    std::string res = "REPLY OK IS AUTH success\r\n" ;
    std::strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE  - 1] = '\0';
}
void construct_Neg_Auth_Reply(char* buffer){
    std::string res = "REPLY NOK IS AUTH unsuccess\r\n" ;
    std::strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE  - 1] = '\0';
}
void const_success_join(char* buffer, const string& displayname, const string& channel){
    string  res = "MSG FROM Server IS "+ displayname + " has joined " + channel + "\r\n";
    std::strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE  - 1] = '\0';
}

void const_leave_channel(char* buffer, const string& displayname, const string& channel){
    string  res = "MSG FROM Server IS "+ displayname + " has left " + channel + "\r\n";
    std::strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE  - 1] = '\0';
}
void const_join_reply(char* buffer){
    std::string res = "REPLY OK IS JOIN success\r\n" ;
    std::strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE  - 1] = '\0';
}
void const_neg_join_reply(char* buffer){
    std::string res = "REPLY NOK IS JOIN unsuccess\r\n" ;
    std::strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE  - 1] = '\0';
}


// Constructs an error message and copies it into the provided buffer.
void const_err(char* buffer){
    string res = "ERR FROM Server ";
    string str = "Invalid message from client";
    res = res +" IS "+ str + '\r' + '\n';
    strncpy(buffer, res.c_str(), BUFSIZE - 1);
    buffer[BUFSIZE - 1] = '\0';
    return ;
}
