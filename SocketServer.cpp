#include "Server.h"

Server::SocketServer::SocketServer(std::string server_ip, uint64_t port) {
    this->serverSd = socket(AF_UNIX, SOCK_STREAM, 0);
    this->servAddr.sin_family = AF_UNIX;
    this->servAddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    this->servAddr.sin_port = htons(port);
    bind(this->serverSd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    listen(this->serverSd, 100); //количество ожидаемых подклчений
}

void Server::SocketServer::start() {
    this->isRunning = true;
    std::thread([&]() {
        printf("Socket connection cycle has began\n");
        int consd;
        while (isRunning) {
            consd = accept(serverSd, (struct sockaddr *) NULL, NULL);
            if (consd < 0)
                continue;

            char tmp_buff[20];
            memset(tmp_buff, '\0', 20);
            read(consd, tmp_buff, 20);
            std::string tmp_string(tmp_buff);
            if(tmp_string.find(Roles::Server_role) != std::string::npos && tmp_string.find(Request::reg_me_in) != std::string::npos) {

                tmp_string = tmp_string.erase(tmp_string.find(Roles::Server_role), Roles::Server_role.length());
                tmp_string = tmp_string.erase(tmp_string.find(Request::reg_me_in), Request::reg_me_in.length());
                while(tmp_string.find(SpecialSymbols::MESSAGE_PART_DIVISION) != std::string::npos)
                    tmp_string=tmp_string.erase(tmp_string.find(SpecialSymbols::MESSAGE_PART_DIVISION), SpecialSymbols::MESSAGE_PART_DIVISION.length());

                Participant* participant = new Participant(consd, std::to_string(rand() % 1000), tmp_string,this);
                participant->setRole(tmp_string);
                particepants.push_back(std::shared_ptr<Participant>(participant));
                printf("Role: %s name:%s\n", tmp_string.c_str(), participant->getID().c_str());
            }
            else {
                //printf("666 %s\n", tmp_string.c_str());
                Participant* participant = this->getByName(tmp_string);
                if(participant == nullptr)
                    continue;
                participant->addSendSocket(consd);
            }
        }
    }).detach();
}

Server::SocketServer::~SocketServer() {
    this->stop();
}

void Server::SocketServer::stop() {
    this->isRunning = false;
    bool is_everyone_dead;
    while (!is_everyone_dead) {
        is_everyone_dead = true;
        for (auto i = this->particepants.begin(); i != this->particepants.end(); i++) {
            if (i->get()->isActive()) {
                i->get()->stop();
                is_everyone_dead = false;
            }
        }
    }
    this->rod.stop();
}

Server::RequestObjectDispatcher &Server::SocketServer::getDispatcher() {
    return this->rod;
}

Server::Participant *Server::SocketServer::getByRole(std::string role) {
    for (auto i = this->particepants.begin(); i != this->particepants.end();) {
        //printf("Role:%s i:%s\n", role.c_str(), i->get()->getRole().c_str());
        if (i->get()->getRole().compare(role) == 0)
            return i->get();
        if (i->get()->isActive() == false)
            i = this->particepants.erase(i);
        else
            i++;
    }
    return nullptr;
}

Server::Participant *Server::SocketServer::getByName(std::string name) {
    for (auto i = this->particepants.begin(); i != this->particepants.end(); i++) {
       // printf("sd:%d i->sd:%d\n", sd, i->get()->getSd());
        if (i->get()->getID() == name) {

            //printf("sd:%d i->sd:%d\n", sd, i->get()->getSd());
            if (i->get()->isActive() == true) {
                printf("i->ID:%s is taken\n", name.c_str());
                return i->get();
            } else {
                printf("i->ID:%s is broken\n", name.c_str());
                this->particepants.erase(i);
                return nullptr;
            }
        }
        /*if(i->get()->isActive() == false) {
            i = this->particepants.erase(i);
            printf("sd:%d i->sd:%d was killed\n", sd, i->get()->getSd());
        }
        else 
            i;*/
    }
    return nullptr;
}

bool Server::SocketServer::isActive() {
    return this->isRunning;
}
