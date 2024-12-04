#ifndef IPK2_SERVER_REPLY_CONSTRUC_H
#define IPK2_SERVER_REPLY_CONSTRUC_H
#include <string>
#include <vector>





std::vector<std::string> get_words(const char* buffer);
void construct_Pos_Auth_Reply(char* buffer);
void construct_Neg_Auth_Reply(char* buffer);
void const_success_join(char* buffer, const std::string& displayname, const std::string& channel);
void const_leave_channel(char* buffer, const std::string& displayname, const std::string& channel);
void const_err(char* buffer);
void const_join_reply(char* buffer);
void const_neg_join_reply(char* buffer);






#endif //IPK2_SERVER_REPLY_CONSTRUC_H
