#include "Server.h"

Server::AskingThread::AskingThread(SendSocket* sendSocket, SocketServer *server) {
    this->isAlive = 0;
    this->server = server;
    this->sendSocket = sendSocket;
    this->isRunning = true;
    this->life();
}

void Server::AskingThread::life() {
    std::thread([&]() {
        this->isAlive = 1;
        std::string tmp;
        while (this->isRunning && this->sendSocket->isActive()) {
            while (!this->ask_request_order.empty() && this->isRunning && this->server->isActive()) {
                //printf("b %s\n", this->ask_request_order.front()->getMessageToSend().c_str());
                tmp = this->ask_request_order.front()->getMessageToSend();
                this->say(tmp);
                this->ask_request_order.pop();
            }
        }
        this->isRunning = false;
        this->isAlive--;
    }).detach();
}

bool Server::AskingThread::isActive() {
    return this->isRunning;
}

Server::AskingThread::~AskingThread() {
    stop();
    delete this->sendSocket;
}

void Server::AskingThread::stop() {
    this->isRunning = false;
    while (!this->ask_request_order.empty()) {
        this->ask_request_order.front()->stop();
        this->ask_request_order.pop();
        this->sendSocket->stop();
    }
    while (this->isAlive > 0); //дожидаемся смерти всех потоков
}

void Server::AskingThread::addRequestObject(RequestObject *ro) {
    //printf("a  %s\n", ro->getMessageToSend().c_str());
    this->ask_request_order.push(ro);
}

void Server::AskingThread::say(std::string tmp) {
    this->sendSocket->send(tmp);
}

