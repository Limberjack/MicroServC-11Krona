#include "Client.h"

using namespace LocalClient;

ListeningThread::ListeningThread(SocketClient *client, ListenSocket *listenSocket) {
    this->client = client;
    this->isRunning = true;
    this->isDead = false;
    this->listenSocket = listenSocket;
    this->life();
}

ListeningThread::~ListeningThread() {
    this->stop();
    delete this->listenSocket;
}

void ListeningThread::addRequestObject(RequestObject *ro) {
    this->waiting_list.push_back(ro);
}

void ListeningThread::life() {
    std::thread([&]() {
        while (this->isRunning && this->listenSocket->isActive()) {
            std::string tmp = this->listenSocket->recv();
            //printf("message : %s\n", tmp.c_str());
            this->findOwner(tmp);
        }
    }).detach();
}

void ListeningThread::stop() {
    this->isRunning = false;
    this->listenSocket->stop();
    while (!this->waiting_list.empty()) {
        this->waiting_list.back()->stop();
        this->waiting_list.pop_back();
    }
};

bool ListeningThread::isActive() {
    return this->isRunning;
};

void ListeningThread::findOwner(std::string tmp_msg) {
    //printf("message that we want to search : %s\n", tmp_msg.c_str());
    std::thread([&](std::string msg) {
        for (auto i = this->waiting_list.begin(); i != this->waiting_list.end() && this->isRunning; i++) {

            printf("message that we are searching : %s\n", msg.c_str());
            if ((*(i.base()))->isYours(msg)) {
                //printf("+++++\n");
                this->waiting_list.erase(i);
                return;
            }
            if (!(*(i.base()))->isActive())
                this->waiting_list.erase(i);
        }
        if (this->isRunning) {
            RequestAnswerObject *tmp = new RequestAnswerObject(msg, this->client->getHandler(), this->client);
            tmp->life();
            this->client->addRequestObject(tmp);
        }
    }, tmp_msg).detach();
}

//=====================================================================================================================

AskingThread::AskingThread(SocketClient *client, SendSocket *sendSocket) {
    this->client = client;
    this->sendSocket = sendSocket;
    this->isRunning = true;
    this->life();
}

AskingThread::~AskingThread() {
    printf("client asking thread is dead\n");
    this->stop();
    delete this->sendSocket;
}

void AskingThread::life() {
    this->isRunning = true;
    this->isDead = false;

    std::thread([&]() {
        while (this->isRunning) {
            while (!this->ask_queue.empty() && this->isRunning) {
                std::string tmp = this->ask_queue.front()->getNeededMessage();
                if (this->sendSocket->isActive())
                    this->sendSocket->send(tmp);
                else
                    this->stop();
                this->ask_queue.pop();
            }
        }
        printf("FUCK\n");
    }).detach();
}

void AskingThread::addRequestObject(RequestObject *ro) {
    this->ask_queue.push(ro);
}

void AskingThread::stop() {
    this->isRunning = false;
    while (!this->ask_queue.empty()) {
        this->ask_queue.front()->stop();
        this->ask_queue.pop();
    }
}

bool AskingThread::isActive() {
    return this->isRunning;
}
