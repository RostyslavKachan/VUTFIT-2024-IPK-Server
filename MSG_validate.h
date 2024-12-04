#ifndef IPK2_MSG_VALIDATE_H
#define IPK2_MSG_VALIDATE_H
#include "Server.h"
bool validateAUTH(vector<std::string> words);
bool isValidUsernameOrChannel(string words);
bool isValidMessage(char* message);
bool isValidDisplayName(string words);
bool isValidSecret(string words);
string transform_toupper(string& word);


#endif //IPK2_MSG_VALIDATE_H
