#include "Server.h"

vector<TCP_client*> clients;

bool quit(false); // variable for catching SIGINT

Server::~Server() {}

Server::Server(u_int16_t k_port, const string& k_hostname, u_int16_t k_timeout, u_int8_t k_retransmission):
pr_port(k_port),
pr_hostname(k_hostname),
pr_timeout(k_timeout),
pr_retransmission(k_retransmission)
{}

u_int16_t Server::get_port() {
    return pr_port;
}

string Server::get_hostname() {
    return pr_hostname;
}


void Server::create_socket() {
    // Create a socket using IPv4 and TCP.
    pr_list_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (pr_list_socket == -1) {
        cerr << "ERR: Socket creation failed\n"; // Check if socket creation failed
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    // Set the SO_REUSEADDR option to allow the local address to be reused.
    setsockopt(pr_list_socket, SOL_SOCKET, SO_REUSEADDR,
               &enable, sizeof(enable));

    memset((&serv_addr), 0, this->addr_len); // Clear the serv_addr structure
    serv_addr.sin_family = AF_INET; // Set address family to IPv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //accept connections from any IP address
    serv_addr.sin_port = htons(this->pr_port);
    // Bind the socket to the previously defined IP address and port.
    if (bind(pr_list_socket, (struct sockaddr *) &serv_addr, addr_len) < 0) {
        cerr << "ERR: Error bind\n"; // Check if socket creation failed
        close(pr_list_socket);
        exit(EXIT_FAILURE);
    }
    // Set the socket to listen for incoming connections,
    if (listen(pr_list_socket, MAX_EVENTS) < 0) {
        cerr << "ERR: Error listen\n"; // Check if socket creation failed
        close(pr_list_socket);
        exit(EXIT_FAILURE);
    }
    return;
}

void Server::server_run() {
    create_socket();
    setup_signal_handlers();
    char buffer[BUFSIZE];
    vector <string> words;
    int epollFd = epoll_create1(0);  // Create epoll instance
    if (epollFd == -1) {
        cerr << "ERR: Could not create epoll instance\n";
        close(pr_list_socket); // Close the socket on error
        exit(EXIT_FAILURE);
    }

    event.events = EPOLLIN;
    event.data.fd = pr_list_socket;

    int rval;
    // Add the server's listening socket to the epoll instance to be monitored .
    rval = epoll_ctl(epollFd, EPOLL_CTL_ADD, pr_list_socket, &event);
    if(rval != 0){
        cerr << "ERR: Epoll_ctl is unsuccessful" << endl;
        exit(EXIT_FAILURE);
    }
    while(1){
        int even_count = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (even_count == -1) {
            if (quit){ // CTRL+C handling
                for (size_t i = 0; i < clients.size(); i++) {
                    if (clients[i] != nullptr && (clients[i]->get_state() != ERROR) && (clients[i]->get_state() != END)) {
                        send_data_cout(clients[i],"BYE");
                        send_data("BYE\r\n", clients[i]->get_socket());
                    }
                }

                //close sockets and delete clients
                for (size_t i = 0; i < clients.size(); i++) {
                    if (clients[i] != nullptr) {
                        close(clients[i]->get_socket());
                        delete clients[i];
                        clients[i] = nullptr;
                    }
                }

                clients.clear();

                close(epollFd);   //close epoll descriptor
                close(pr_list_socket); //close welcome socket
                exit(EXIT_SUCCESS);
            }
            else{
                cerr << "ERR: Error epoll_wait" << endl;
                exit(EXIT_FAILURE);}
        }

        for (int n = 0; n < even_count; n++) {
            // If the event is on the listening socket, accept a new client connection.
            if (events[n].data.fd == pr_list_socket) {
                accept_new_client(epollFd);
            }
            else{
                // Handle data  from an existing client socket.
                int client_sock = events[n].data.fd;

                if (events[n].events & EPOLLIN) { // Check if the event is for data to read.
                    receive_data(buffer, client_sock);
                    TCP_client* ptr = findClientBySocket(clients,client_sock); //return pointer on actual client
                    if(ptr == nullptr){
                        break;
                    }
                    //if client in state end ,send all clients that hi leave , close his socket, remove from clients
                    if(ptr->get_state() == END){
                        const_leave_channel(buffer,ptr->get_displayname(), ptr->get_channel());
                        for (TCP_client* other_sock : clients) {
                            if(other_sock->get_state() == OPEN){
                                if(other_sock->get_channel() == ptr->get_channel()){
                                    if (other_sock->get_socket() != client_sock) {
                                        send_data(buffer, other_sock->get_socket());
                                    }
                                }
                            }
                        }
                        close(ptr->get_socket());
                        removeClient(ptr);

                        break;

                    }
                    // Start state , client authentication
                    if(ptr->get_state() == ACCEPT){
                        words = get_words(buffer); // vector of words from buffer
                        if(validateAUTH(words)){ // check if all words are ok
                            receive_data_cout(ptr, transform_toupper(words[0]));
                            construct_Pos_Auth_Reply(buffer);
                            send_data(buffer, client_sock);
                            send_data_cout(ptr,buffer);
                            ptr->set_displayname(words[3]); //initialization
                            ptr->set_state(OPEN);
                            const_success_join(buffer, ptr->get_displayname(), ptr->get_channel());
                            for (TCP_client* other_sock : clients) {
                                if(other_sock->get_state() == OPEN){
                                    if(other_sock->get_channel() == ptr->get_channel()){

                                            send_data(buffer, other_sock->get_socket());

                                    }
                                }
                            }
                        }
                        else{ // do authentication one more time
                            ptr->set_state(AUTH);
                            construct_Neg_Auth_Reply(buffer);
                            send_data(buffer, client_sock);
                            send_data_cout(ptr,buffer);
                        }

                        break;
                    }
                    if(ptr->get_state() == AUTH){

                        words = get_words(buffer);

                        if(transform_toupper(words[0]) == "BYE"){
                            receive_data_cout(ptr, transform_toupper(words[0]));
                            ptr->set_state(END);
                        }
                        if(validateAUTH(words)){
                            receive_data_cout(ptr,transform_toupper(words[0]));
                            construct_Pos_Auth_Reply(buffer);
                            send_data(buffer, client_sock);
                            send_data_cout(ptr,buffer);
                            ptr->set_displayname(words[3]);
                            ptr->set_state(OPEN);
                            const_success_join(buffer, ptr->get_displayname(), ptr->get_channel());
                            //Send all connected clients msg about success join
                            for (TCP_client* other_sock : clients) {
                                if(other_sock->get_state() == OPEN){
                                    if(other_sock->get_channel() == ptr->get_channel()){

                                            send_data(buffer, other_sock->get_socket());


                                    }
                                }
                            }
                        }
                        else{
                            construct_Neg_Auth_Reply(buffer);
                            send_data(buffer, client_sock);
                            send_data_cout(ptr,buffer);
                        }
                        break;
                    }

                    if(ptr->get_state() == OPEN){
                        istringstream iss(buffer);
                        string first_word, second_word, third_word, fourth_word;
                        iss >> first_word >> second_word >> third_word >> fourth_word;

                        if(transform_toupper(first_word) == "BYE"){
                            receive_data_cout(ptr, transform_toupper(first_word));
                            ptr->set_state(END);
                        }
                        else if(transform_toupper(first_word)== "MSG" && transform_toupper(second_word) == "FROM" && transform_toupper(fourth_word)== "IS"){
                            if(third_word != ptr->get_displayname())
                                ptr->set_displayname(third_word);

                            receive_data_cout(ptr, transform_toupper(first_word));
                            for (TCP_client* other_sock : clients) {
                                if(other_sock->get_state() == OPEN){
                                    if(other_sock->get_channel() == ptr->get_channel()){
                                        if (other_sock->get_socket() != client_sock) {
                                            send_data(buffer, other_sock->get_socket());
                                            send_data_cout(other_sock,"MSG");
                                        }
                                    }
                                }
                            }
                        }
                        else if(transform_toupper(first_word) == "JOIN" && transform_toupper(third_word) == "AS"){
                            receive_data_cout(ptr, transform_toupper(first_word));
                            if(isValidUsernameOrChannel(second_word)){
                                const_leave_channel(buffer,ptr->get_displayname(), ptr->get_channel()); //construct msg about leaving channel
                                for (TCP_client* other_sock : clients) {
                                    if(other_sock->get_state() == OPEN){
                                        if(other_sock->get_channel() == ptr->get_channel()){
                                            if (other_sock->get_socket() != client_sock) {
                                                send_data(buffer, other_sock->get_socket());
                                            }
                                        }
                                    }
                                }
                                const_join_reply(buffer);
                                send_data(buffer,ptr->get_socket());
                                send_data_cout(ptr,buffer);


                                ptr->set_channel(second_word);
                                const_success_join(buffer, ptr->get_displayname(), ptr->get_channel()); // construct msg about good success to channel
                                for (TCP_client* other_sock : clients) {
                                    if(other_sock->get_state() == OPEN){
                                        if(other_sock->get_channel() == ptr->get_channel()){

                                                send_data(buffer, other_sock->get_socket());

                                        }
                                    }
                                }
                            }
                            else{
                                const_neg_join_reply(buffer);
                                send_data(buffer,ptr->get_socket());
                                send_data_cout(ptr,buffer);
                            }


                        }

                        else if(transform_toupper(first_word) == "ERR" && transform_toupper(second_word) == "FROM" && transform_toupper(fourth_word) == "IS"){
                            receive_data_cout(ptr, transform_toupper(first_word));
                            send_data("BYE\r\n",ptr->get_socket());
                            send_data_cout(ptr,"BYE");
                            ptr->set_state(END);
                        }
                        // wrong msg from client
                        else{
                            const_err(buffer);
                            ptr->set_state(END);
                            send_data(buffer, ptr->get_socket());
                            send_data_cout(ptr,buffer);
                            send_data("BYE\r\n",ptr->get_socket());
                            send_data_cout(ptr,"BYE");
                        }
                        break;
                    }
                }
            }
        }
    }


}

void Server::accept_new_client(int epollFd) {
    // Define a structure to store client address info.
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    // Accept a new connection on the server's listening socket.
    int client_socket = accept(pr_list_socket, (struct sockaddr *) &client_addr, &addrlen);
    // Set the newly accepted socket to non-blocking mode using fcntl.
    int status = fcntl(client_socket, F_SETFL, fcntl(client_socket, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1){
        cerr <<"ERR: Error in calling fcntl" << endl;

    }
    // Create a new TCP_client object for managing the new connection.
     TCP_client* new_client = new TCP_client(client_socket,ACCEPT);
    //Add client to vector
    clients.push_back(new_client);
    // Convert the numeric IP address to a readable string format.
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
    new_client->set_hostname(ip);
    new_client->set_port(ntohs(client_addr.sin_port));


    event.events = EPOLLIN;
    event.data.fd = client_socket;
    int rval;
    // Register the new socket with the epoll instance to monitor events.
    rval = epoll_ctl(epollFd, EPOLL_CTL_ADD, client_socket, &event);
    if(rval != 0){
        cerr << "ERR: Epoll_ctl is unsuccessful" << endl;
        exit(EXIT_FAILURE);
    }

}
// Receive data from a client socket.
void Server::receive_data(char* buffer, int client_socket) {

    bzero(buffer, BUFSIZE); // Clear the buffer
    ssize_t bytesrc = recv(client_socket, buffer, BUFSIZE, 0);
    if (bytesrc < 0) {
        cerr << "ERR: Error in recvfrom" << endl;
        exit(EXIT_FAILURE);
    }
    return;
}
// Send data to a client socket.
void Server::send_data(const char* buffer, int client_socket) {
    ssize_t bytestx = send(client_socket, buffer, strlen(buffer), 0); // Send buffer content to server
    if (bytestx < 0) {// Check for send error
        cerr<<"ERR: Error in sendto" << endl;
        exit(EXIT_FAILURE);
    }
}
// Find a client in the clients vector by their socket descriptor.
TCP_client* Server::findClientBySocket(const std::vector<TCP_client*>& clients, int socket) {
    for (auto client : clients) {
        if (client->get_socket() == socket) {
            return client;
        }
    }
    return nullptr;
}
// Remove a client from vector and clean up.
void Server::removeClient(TCP_client* targetClient) {
    auto it = std::find(clients.begin(), clients.end(), targetClient);
    if (it != clients.end()) {
        delete *it;
        *it = nullptr;
        clients.erase(it);
    }
}

// Signal handler for gracefully finish program
void Server::handler_signal(int signal) {
        quit = true;


}
// Set up signal handlers for the server.
void Server::setup_signal_handlers() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handler_signal;// Set the handler function.
    sigemptyset(&sigIntHandler.sa_mask); // Initialize signal set to be empty
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);// Register handler for SIGINT
}

