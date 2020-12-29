#include "Server.h"

Server::Participant::Participant(int send_sd, std::string name, std::string role, SocketServer* server){
    printf("+New connection with SD=%d\n", send_sd);
    this->role = role;
    this->name = name;
    this->server = server;
    this->isRunning = true;
    this->role = "emp";
    ListenSocket* listenSocket = new ListenSocket(send_sd, this->name);
    //this->lt = new ListeningThread(this->sd, this->server);
    this->lt = new ListeningThread(listenSocket, this, this->server);
}

Server::Participant::~Participant(){
    this->stop();
    delete this->lt;
    delete this->at;
}

bool Server::Participant::isActive(){
    return this->isRunning;
}

std::string Server::Participant::getRole(){
    return this->role;
}

void Server::Participant::stop(){
    this->isRunning = false;
    this->at->stop();
    this->lt->stop();
    //close(this->sd);
}

Server::ListeningThread* Server::Participant::getListeningThread(){
    return this->lt;
}

Server::AskingThread* Server::Participant::getAskingThread(){
    return this->at;
}

std::string Server::Participant::getID() {
    return name;
}

void Server::Participant::addSendSocket(int socket_sd) {
    this->at = new AskingThread(new SendSocket(socket_sd),this->server);
}

void Server::Participant::setRole(const std::string &role) {
    this->role = role;
}
