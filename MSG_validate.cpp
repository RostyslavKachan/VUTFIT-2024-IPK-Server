#include "MSG_validate.h"

bool validateAUTH(vector<std::string> words){
    bool res = (transform_toupper(words[0]) == "AUTH") && (isValidUsernameOrChannel(words[1])) && (transform_toupper(words[2]) == "AS") && (isValidDisplayName(words[3])) &&
            (transform_toupper(words[4]) == "USING") && (isValidSecret(words[5]));

    return res;
}
bool isValidUsernameOrChannel(string words) {
    int len = words.length();// Get the length of the username
    if (len < 1 || len > 20) return false;
    for (int i = 0; i < len; ++i) {
        // Check each character for alphanumeric or hyphen, return false if not
        if (!isalnum(words[i]) && words[i] != '-') return false;
    }
    return true;
}
bool isValidMessage(char* message) {
    int length = 0; // Initialize message length counter
    if (message == nullptr) { // Check for null pointer
        return false;
    }
    for (char* ptr = message; *ptr; ++ptr) {// Iterate through each character
        // Check for non-printable characters (space is allowed)
        if (!isprint(*message) && *message != ' ') {
            return false;
        }

        ++length;

        // Check if message exceeds 1400 characters
        if (length > 1400)
            return false;

    }

    return length > 0 && length <= 1400;
}
// Function to check if a display name is valid.
bool isValidDisplayName(string words) {
    int len = words.length();
    if (len < 1 || len > 20) return false;
    for (int i = 0; i < len; ++i) {
        // Check each character for printability, return false if not
        if (!isprint(words[i])) return false;
    }
    return true;
}
// Function to check if a secret  is valid.
// A valid secret must be 1-128 characters long and can only contain alphanumeric characters and hyphens.
bool isValidSecret(string words) {
    int len = words.length();
    if (len < 1 || len > 128) return false;
    for (int i = 0; i < len; ++i) {
        if (!isalnum(words[i]) && words[i] != '-') return false;
    }
    return true;
}
//transform possible lowercase word to uppercase
string transform_toupper(string& word){
    string convert = word;
    transform(convert.begin(), convert.end(), convert.begin(),
              ::toupper);
    return convert;
}