#include "Server.h"

Server::RequestObject::RequestObject(std::string msg, SocketServer *server) {
    this->parsedMessage = RequestParser::parse(msg);
    this->sender = server->getByRole(parsedMessage->getSender());
    this->server = server;
    //this->addressee = msg.substr(0, 3); //вытащили имя отправителя
    //this->destination = msg.substr(5, 3); //вытащили адресанта
    this->isRunning = true;
    this->isWaiting = false;
    this->request_body = msg;
    this->life();
}

void Server::RequestObject::life() {
    std::thread([&]() {
        //printf("alive 1 %s\n", this->request_body.c_str());
        this->isDead = false;
        this->message_to_send = this->request_body;
        this->isWaiting = true;
        try {
            //printf("alive 1.1\n");
            Server::Participant *tmp_client = this->server->getByRole(this->parsedMessage->getAddress());
            if (tmp_client == nullptr) {
                //printf("alive 1.2\n");
                if (sender == nullptr)
                    //printf("alive 1.3\n");
                this->sender->getAskingThread()->say(Response::TRASH + " No participant with name " +
                                                     this->parsedMessage->getAddress() + " was found -<-" +
                                                     this->request_body + "->-");

                //printf("alive 1.4\n");
                this->isRunning = false;
                this->isDead = true;
                return;
            } else {
                tmp_client->getListeningThread()->addRequestObject(this);
            }

        } catch (std::exception &e) {
            printf("-Failed to get into the listening list with next request: %s\n     (what: %s)\n",
                   this->request_body.c_str(), e.what());
            this->isRunning = false;
            this->isDead = true;
            return;
        }

        //printf("alive 2\n");
        try {
            this->message_to_send = request_body;
            //printf("alive 3 %s\n", message_to_send.c_str());
            server->getByRole(this->parsedMessage->getAddress())->getAskingThread()->addRequestObject(this);
        } catch (std::system_error &e) {
            printf("-Failed to get into the asking queue with next request: %s\n     (what: %s)\n",
                   this->request_body.c_str(), e.what());
            this->isRunning = false;
            this->isDead = true;
            return;
        }
        while (this->isWaiting && this->server->isActive()) {
            if (!this->isRunning || !this->server->isActive()) {
                this->isRunning = false;
                this->isDead = true;
                return;
            }
        }

        //запрос добавляет себя в список ожидающих сообщения у слушающего треда
        //добавляется в очередь на отправку
        //ждет получения ответа
        //встает в очередь на отправку ответа спросившему
        this->message_to_send = this->response_body+ "-<-" + request_body + "->-";
        this->isWaiting = true;

        try {
           // this->message_to_send = response_body ;
            this->sender->getListeningThread()->addRequestObject(this);
            this->sender->getAskingThread()->addRequestObject(this);
            while (this->isWaiting && this->server->isActive()) {
                if (!this->isRunning || !this->server->isActive()) {
                    this->isRunning = false;
                    this->isDead = true;
                    return;
                }
            }
        } catch (std::system_error &e) {
            printf("-Failed to get into the asking queue with next response: %s\n     (what: %s)\n",
                   this->response_body.c_str(), e.what());
            this->isRunning = false;
            this->isDead = true;
            return;
        }
        printf("alive 4\n");
        //мы получили ответ от спросившего
        //завершаем жизнь запроса
        this->isRunning = false;
        this->isDead = true;
    }).detach();
}

bool Server::RequestObject::isYours(std::string msg) {
    if (msg.find("-<-" + this->request_body + "->-") != std::string::npos) {//есили ключ есть, то флаг ожидания скидываем в false
        this->setResponseBody(msg);
        this->isWaiting = false;
    }
    //printf("QQQQ %s<->\n", msg.c_str());
    return !this->isWaiting;
}

std::string Server::RequestObject::getRequestBody() {
    return this->request_body;
}

std::string Server::RequestObject::getResponseBody() {
    return this->response_body;
}

void Server::RequestObject::setResponseBody(std::string response_body) {
    this->response_body = response_body.substr(0, response_body.find("-<-"));
    printf("alive 5 %s : %s\n", this->response_body.c_str(), response_body.c_str());
}//Нужно проверить, но вроде бы все логично. мы выбираем символы от нуля в количестве символов до места вхождения "-<-"


std::string Server::RequestObject::getMessageToSend() {
    return this->message_to_send;
}

Server::RequestObject::~RequestObject() {
    this->stop();
}

void Server::RequestObject::stop() {
    if (this->isRunning) {
        this->isRunning = false; //если поток все еще действует, говорим ему перестать действовать
    }
    while (!this->isDead); //ждем, пока поток умрет
}

bool Server::RequestObject::isActive() {
    return !this->isRunning;
}