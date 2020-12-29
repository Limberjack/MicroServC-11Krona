#include "Server.h"

Server::ListeningThread::ListeningThread(ListenSocket *listenSocket, Participant *participant, SocketServer *server) {
    this->isAlive = 0;
    this->participant = participant;
    this->listenSocket = listenSocket;
    this->isRunning = true;
    this->server = server;
    this->life();
}

void Server::ListeningThread::life() {
    std::thread([&]() {

        //printf("+Started ListeningThread for SD=%d\n", this->sd);
        this->isAlive = 1;
        while (this->listenSocket->isActive() && this->isRunning && this->server->isActive()) {
            std::string tmp = this->listenSocket->recv();
            printf("    -got message on socket with ID=%s :\"%s\"\n", this->participant->getID().c_str(), tmp.c_str());
            this->findOwner(RequestBody(tmp, this->participant->getID()));
        }
        this->listenSocket->stop();
        this->isRunning = false;
        this->isAlive--;
    }).detach();
}

void Server::ListeningThread::addRequestObject(RequestObject *ro) {
    this->awaiting_for_response_list.push_back(ro);
}

void Server::ListeningThread::findOwner(RequestBody msg_) {
    std::thread([&](RequestBody msg) {
        this->isAlive++;
        if (msg.body.length() < 11) {
            this->server->getByName(msg.sender_name)->getAskingThread()->say("trash -<-" + msg.body + "->-");
            this->isAlive--;
            return;
        }
        if (msg.body.find("-<-") != std::string::npos) //если кто-то ждет этот ответ, то ищем, кто
            for (auto i = this->awaiting_for_response_list.begin();
                 i != this->awaiting_for_response_list.end() && this->isRunning && this->server->isActive(); i++) {
                if ((*i)->isYours(msg.body)) {
                    this->awaiting_for_response_list.erase(i); //если ожидающий нашелся, освобождаем его
                    break;
                };
            }
        else if (msg.body.find(Request::log_me_out) != std::string::npos)
            this->server->getByName(msg.sender_name)->stop(); //если просят отключиться - отключаем
        else { //иначе создаем новую сущность запроса
            this->server->getDispatcher().addRequestObject(
                    new RequestObject(msg.body, this->server));
        }
        this->isAlive--;
    }, msg_).detach();
}

bool Server::ListeningThread::isActive() {
    return this->isRunning;
}

Server::ListeningThread::~ListeningThread() {
    stop();
    delete this->listenSocket;
}

void Server::ListeningThread::stop() {
    this->isRunning = false;
    while (!this->awaiting_for_response_list.empty()) {
        this->awaiting_for_response_list.back()->stop();
        this->awaiting_for_response_list.pop_back();
        this->listenSocket->stop();
    }
    while (this->isAlive > 0); //дожидаемся смерти всех потоков
}

