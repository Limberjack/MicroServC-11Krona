#ifndef _LOC_SOCK_SERV_H___
#define _LOC_SOCK_SERV_H___

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <queue>
#include <memory>
#include <exception>

#include "SocketDictionary.h"
#include "Client.h"

namespace Server {

    class SocketServer;

    class RquestObject;

    class AskingThread;

    class ListeningThread;

    class RequestObjectDispatcher;

    class Participant;


    class RequestObject {
    protected:
        bool isDead;
        bool isRunning;
        bool isWaiting;
        ParsedMessage* parsedMessage;
        Server::Participant *sender;
        std::string addressee, destination, body;
        std::string request_body;
        std::string response_body;
        std::string message_to_send;
        SocketServer *server;
    private:
        //virtual void addSelfToTheAskOrder(AskingThread* at);
        //virtual void addSelfToTheListenList(ListeningThread* lt);
        virtual void life();

    public:
        virtual bool isYours(std::string msg);

        virtual bool isActive();

        virtual std::string getRequestBody();

        virtual std::string getResponseBody();

        virtual std::string getMessageToSend();

        virtual void setResponseBody(std::string response_body);

        virtual ~RequestObject();

        RequestObject(std::string msg, SocketServer *server);

        void stop();
    };

    class AskingThread {
        SendSocket *sendSocket;
        bool isRunning;
        unsigned int isAlive;
        std::queue<RequestObject *> ask_request_order;
        SocketServer *server;

        void life();

    public:
        AskingThread(SendSocket *sendSocket, SocketServer *server);

        ~AskingThread();

        bool isActive();

        void addRequestObject(RequestObject *ro);

        void say(std::string msg);

        void stop();

        void isConfirmed();

        void confirmMessage();
    };

    class ListeningThread {

        int isAlive;
        bool isDead;
        bool isRunning;
        std::vector<RequestObject *> awaiting_for_response_list;
        SocketServer *server;
        ListenSocket *listenSocket;
        Participant* participant;

        void findOwner(RequestBody msg);

        void life();

    public:
        ListeningThread(ListenSocket *listenSocket, Participant* participant, SocketServer* server);

        ~ListeningThread();

        bool isActive();

        void addRequestObject(RequestObject *ro);

        void stop();
    };

    class RequestObjectDispatcher {
        std::vector<std::shared_ptr<RequestObject>> all_request_objects;
    public:
        RequestObjectDispatcher() = default;

        ~RequestObjectDispatcher();

        void addRequestObject(RequestObject *ro);

        void stop();
    };

    class Participant : Client {
        std::string role;
        std::string name;
        bool isRunning;
        //uint8_t counter;
        AskingThread *at;
        ListeningThread *lt;
        SocketServer *server;

    public:
        Participant(int send_sd, std::string name, std::string role, SocketServer *server);

        ~Participant();

        void setRole(const std::string &role);

        bool isActive();

        int getSd();

        void addSendSocket(int socket_sd);

        std::string getRole();

        std::string getID();

        void stop();

        ListeningThread *getListeningThread();

        AskingThread *getAskingThread();

    };

    class SocketServer {
        std::vector<std::shared_ptr<Participant>> particepants;
        RequestObjectDispatcher rod;
        sockaddr_in servAddr;
        socklen_t newSockAddrSize;
        int serverSd;
        bool isRunning;
        uint8_t counter;
    public:
        SocketServer(std::string server_ip, uint64_t port);

        ~SocketServer();

        RequestObjectDispatcher &getDispatcher();

        Participant *getByRole(std::string role);

        Participant *getByName(std::string name);

        void start();

        bool isActive();

        void stop();
    };
};

#endif