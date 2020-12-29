#include "Client.h"


Message::Message() {
    this->length = 4069;
    this->body = new char[this->length];
}

Message::~Message() {
    delete this->body;
}

RequestBody::RequestBody(std::string body, std::string sender_name) {
    this->body = body;
    this->sender_name = sender_name;
}

using namespace LocalClient;

SocketClient::SocketClient(std::string server_ip, uint16_t port, std::string role) {
    this->handler = new Handlers::RequestHandler();
    this->role = role;
    /*host = gethostbyname(server_ip.c_str());
    bzero((char *) &sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = INADDR_ANY;
    //inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    sd = socket(AF_INET, SOCK_STREAM, 0);
    this->handler->addAction(new SocketClientLMO(this));*/

    SendSocket* sendSocket = new SendSocket(server_ip, port, this);
    this->at = new AskingThread(this, sendSocket);
    printf("1\n");
    this->lt = new ListeningThread(this, sendSocket->regListenSocket(server_ip, port, this));
}


SocketClient::~SocketClient() {
    printf("client is dead\n");
    this->stop();
}

std::string SocketClient::sendRequest(std::string request_body) {
    printf("    about to send message:\"%s\"\n", request_body.c_str());
    AskReceiveAnswerObject requestObject = AskReceiveAnswerObject(request_body, this->handler, this);
    requestObject.life();
    while (requestObject.isActive());
    std::string response = requestObject.getResponseBody();
    requestObject.stop();
    return response;
}

void SocketClient::addRequestObject(RequestObject *request) {
    this->requests.push_back(std::shared_ptr<RequestObject>(request));
}

ListeningThread *SocketClient::getListeningThread() {
    return this->lt;
}

AskingThread *SocketClient::getAskingThread() {
    return this->at;
}

std::string SocketClient::getRole() {
    return this->role;
}

bool SocketClient::isActive() {
    return this->isRunning;
}

void SocketClient::stop() {
    printf("client was stopped\n");
    this->isRunning = false;
    this->at->stop();
    this->lt->stop();
    delete this->at;
    delete this->lt;
}

Handlers::RequestHandler *SocketClient::getHandler() {
    return this->handler;
}

SocketClientLMO::SocketClientLMO(SocketClient *client) {
    this->client = client;
};

bool SocketClientLMO::isYours(std::string &message) {
    if (message.find(this->client->getRole() + " " + Request::log_me_out) != std::string::npos)
        return true;
    return false;
}

std::string SocketClientLMO::act(std::string &message) {
    printf("I kill client\n");
    this->client->stop();
    return Response::OK;
};


