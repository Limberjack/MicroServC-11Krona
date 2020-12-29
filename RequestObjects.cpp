#include "Client.h"

using namespace LocalClient;


RequestAnswerObject::RequestAnswerObject(std::string request_body, Handlers::RequestHandler *handler,
                                         SocketClient *client) : RequestObject() {
    this->request_body = request_body;
    this->handler = handler;
    this->client = client;
    this->isRunning = true;
    this->isDead = false;
}

RequestAnswerObject::~RequestAnswerObject() {
    this->stop();
}

void RequestAnswerObject::life() {
    std::thread([&]() {
        this->response_body = this->handler->doSomething(this->request_body);
        this->isWaiting = true;
        this->client->getAskingThread()->addRequestObject(this);
        while (this->isWaiting) {
            if (!this->isRunning) {
                this->isDead = true;
                return;
            }
        }
        this->isDead = true;
        this->isRunning = false;
    }).detach();
}

bool RequestAnswerObject::isYours(std::string message) {
    return false;
}

void RequestAnswerObject::stop() {
    this->isRunning = false;
    while (this->isDead);
}

bool RequestAnswerObject::isActive() {
    return this->isRunning;
}

std::string RequestAnswerObject::getNeededMessage() {
    this->isWaiting = false;
    return this->response_body + " -<-" + this->request_body + "->-";
}

AskReceiveAnswerObject::AskReceiveAnswerObject(std::string request_body, Handlers::RequestHandler *handler,
                                               SocketClient *client) : RequestObject() {
    //printf("Initializing AskReceiveAnswerObject\n");
    this->request_body = request_body;
    this->handler = handler;
    this->client = client;

    this->isDead = false;
    this->isWaiting = false;
    this->isRunning = true;
    //printf("Initializing AskReceiveAnswerObject is done\n");
}

AskReceiveAnswerObject::~AskReceiveAnswerObject() {
    this->stop();
}

void AskReceiveAnswerObject::life() {
    std::thread([&]() {
        //this->needed_message = this->handler->doSomething(this->request_body);
        //printf("1\n");
        this->isWaiting = true;
        this->needed_message = request_body;
        this->client->getListeningThread()->addRequestObject(this);
        this->client->getAskingThread()->addRequestObject(this);
        while (this->isWaiting) {
            if (!this->isRunning) {
                this->isDead = true;
                return;
            }
        }

        //printf("2\n");
        //здесь сервер нам ответил. у нас вызвали isYours(...). снова встаем в очередь на отправку, чтобы сказать OK
        //this->isWaiting = true;
        this->handler->doSomething(this->response_body);
        this->needed_message = Response::OK +  " -<-" + this->request_body + "->-";
        this->client->getAskingThread()->addRequestObject(this);
        this->isDead = true;
        this->stop();
    }).detach();
}

void AskReceiveAnswerObject::stop() {
    this->isRunning = false;
    while (!this->isDead);
}

bool AskReceiveAnswerObject::isActive() {
    return this->isRunning;
}

std::string AskReceiveAnswerObject::getNeededMessage() {
    return this->needed_message;
}

bool AskReceiveAnswerObject::isYours(std::string message) {

    //printf("%s <=> %s\n", this->request_body.c_str(), message.c_str());
    if (message.find("-<-" + this->request_body + "->-") != std::string::npos) {
        //printf("resp: %d\n", message.find("-<-"));
        this->response_body = message.substr(0, message.find("-<-"));
        this->isWaiting = false;
        printf("response_body: %s\n", this->response_body.c_str());
        return true;
    }
    return false;
}

std::string AskReceiveAnswerObject::getResponseBody() {
    return this->response_body;
}

