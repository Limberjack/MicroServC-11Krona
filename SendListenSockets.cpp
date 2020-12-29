#include "Client.h"

SendSocket::SendSocket(std::string server_ip, uint16_t port, Client *client) {
    sockaddr_in sendSockAddr;
    bzero((char *) &sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_UNIX;
    sendSockAddr.sin_addr.s_addr = INADDR_ANY;
    sendSockAddr.sin_port = htons(port);
    this->sd = socket(AF_UNIX, SOCK_STREAM, 0);
    while (connect(sd, (sockaddr *) &sendSockAddr, sizeof(sendSockAddr)) < 0);
    std::string tmp(Roles::Server_role + SpecialSymbols::MESSAGE_PART_DIVISION + client->getRole() +
                    SpecialSymbols::MESSAGE_PART_DIVISION + Request::reg_me_in);
    write(this->sd, tmp.c_str(), tmp.length());
    char tmp_array[5];
    read(this->sd, tmp_array, sizeof(tmp_array));
    this->connect_name = std::string(tmp_array);
    //printf("SendSocket Connected to Server!\n");
    this->is_running = true;
}

SendSocket::SendSocket(int sd) {
    this->is_running = true;
    this->sd = sd;
    write(sd, SpecialSymbols::CONFIRM.c_str(), SpecialSymbols::CONFIRM.length());
    //printf("ListenSocket from Client was connected!\n");
}

ListenSocket *SendSocket::regListenSocket(std::string server_ip, uint16_t port, Client *client) {
    printf("3\n");
    return new ListenSocket(server_ip, port, this->connect_name, client);
}

void SendSocket::send(std::string msg) {
    std::lock_guard<std::mutex> lock(this->mut);
    //printf("sending msg :%s\n", msg.c_str());
    char tmp_array[5];
    write(this->sd, msg.c_str(), msg.length());
    if (!read(this->sd, tmp_array, sizeof(tmp_array))) {
        this->stop();
        return;
    }
}

void SendSocket::stop() {
    //this->send(Roles::Server_role + " " + this->client->getRole() + " " + Request::log_me_out);
    close(this->sd);
    this->is_running = false;
}

bool SendSocket::isActive() {
    return this->is_running;
}

//=====================================================================================================================//


ListenSocket::ListenSocket(std::string server_ip, uint16_t port, std::string name, Client *client) {

    //hostent *host;
    sockaddr_in sendSockAddr;
    //host = gethostbyname(server_ip.c_str());
    bzero((char *) &sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = INADDR_ANY;
    sendSockAddr.sin_port = htons(port);
    this->sd = socket(AF_INET, SOCK_STREAM, 0);
    while (connect(sd, (sockaddr *) &sendSockAddr, sizeof(sendSockAddr)) < 0);

    write(this->sd, name.c_str(), name.length());
    char tmp_array[5];
    read(this->sd, tmp_array, sizeof(tmp_array));
    this->is_running = true;
    //printf("ListenSocket Connected to Server!\n");
}

ListenSocket::ListenSocket(int sd, std::string name) {
    this->sd = sd;
    write(sd, name.c_str(), name.length());
    this->is_running = true;
    //printf("SendSocket from Client was connected!\n");
}

std::string ListenSocket::recv() {
    //printf("recv was called\n");
    std::lock_guard<std::mutex>lock(this->mut);
    int n = read(this->sd, this->msg.body, this->msg.length);
    if (!n)
        this->stop();
    write(this->sd, SpecialSymbols::CONFIRM.c_str(), SpecialSymbols::CONFIRM.length());
    std::string toReturn(msg.body);
    memset(msg.body, '\0', n);
    //printf("got msg: %s\n", msg.body);
    return toReturn;
}

void ListenSocket::stop() {
    //this->send(Roles::Server_role + " " + this->client->getRole() + " " + Request::log_me_out);
    this->is_running = false;
    close(this->sd);
}

bool ListenSocket::isActive() {
    return this->is_running;
}
