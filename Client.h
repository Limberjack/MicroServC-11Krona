#ifndef _SOCK_PART_H___
#define _SOCK_PART_H___

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
#include "RequestHandlers.h"
#include "RequestParser.h"

struct Message {
    char *body;
    uint16_t length;

    Message();

    ~Message();
};

struct RequestBody{
    std::string body;
    std::string sender_name;

    RequestBody(std::string body, std::string sender_name);
};

class Client;

class ListenSocket;

class SendSocket;

class Client {
public:
    virtual std::string getRole() = 0;

    virtual void stop() = 0;

    virtual ~Client() = default;
};

class ListenSocket {
    int sd;
    std::mutex mut;
    Message msg;
    bool is_running;

    ListenSocket(std::string server_ip, uint16_t port, std::string name, Client *client);

    friend SendSocket;
public:
    void stop();

    std::string recv();

    ListenSocket(int sd, std::string name);

    bool isActive();
};

class SendSocket {
    int sd;
    std::string connect_name;
    std::mutex mut;
    bool is_running;

public:

    void stop();

    void send(std::string msg);

    SendSocket(std::string server_ip, uint16_t port, Client *client);

    SendSocket(int sd);

    ListenSocket *regListenSocket(std::string server_ip, uint16_t port, Client *client);

    bool isActive();

};

namespace LocalClient {

    class RequestObject; //+
    class SocketClient; //+
    class RequestAnswerObject; //+
    class AskReceiveAnswerObject; //+
    class AskingThread;//+
    class ListeningThread;//+

    class RequestObject {
    public:
        virtual void life() = 0;

        virtual bool isActive() = 0;

        virtual void stop() = 0;

        virtual std::string getNeededMessage() = 0;

        virtual bool isYours(std::string message) = 0;

        virtual ~RequestObject() = default;

        RequestObject() = default;
    };

    class RequestAnswerObject : public RequestObject {
        bool isWaiting;
        bool isRunning;
        bool isDead;
        Handlers::RequestHandler *handler;
        SocketClient *client;
        std::string request_body;
        std::string response_body;
    public:
        virtual void life();

        virtual bool isYours(std::string message);

        virtual void stop();

        virtual bool isActive();

        virtual std::string getNeededMessage();

        virtual ~RequestAnswerObject();

        RequestAnswerObject(std::string request_body, Handlers::RequestHandler *handler, SocketClient *client);
    };

    class AskReceiveAnswerObject : public RequestObject {
        bool isWaiting;
        bool isRunning;
        bool isDead;
        Handlers::RequestHandler *handler;
        SocketClient *client;
        std::string request_body;
        std::string response_body;
        std::string needed_message;

    public:
        virtual void life();

        virtual void stop();

        virtual bool isActive();

        virtual std::string getNeededMessage();

        virtual bool isYours(std::string message);

        virtual ~AskReceiveAnswerObject();

        AskReceiveAnswerObject(std::string request_body, Handlers::RequestHandler *handler, SocketClient *client);

        std::string getResponseBody();
    };

    class AskingThread {
        bool isDead;
        bool isRunning;
        std::queue<RequestObject *> ask_queue;
        SocketClient *client;
        SendSocket *sendSocket;

        void life();

    public:

        void addRequestObject(RequestObject *ro);

        void stop();

        bool isActive();

        AskingThread(SocketClient *client, SendSocket* sendSocket);

        ~AskingThread();
    };

    class ListeningThread {
        bool isDead;
        bool isRunning;
        std::vector<RequestObject *> waiting_list;
        SocketClient *client;
        ListenSocket *listenSocket;

        void life();

    public:
        void addRequestObject(RequestObject *ro);

        void stop();

        bool isActive();

        void findOwner(std::string msg);

        ListeningThread(SocketClient *client, ListenSocket* listenSocket);

        ~ListeningThread();
    };

    class SocketClient : public Client {
        int descriptor;
        std::string role;
        hostent *host;
        sockaddr_in sendSockAddr;
        int sd;
        bool isRunning;
        ListeningThread *lt;
        AskingThread *at;
        std::vector<std::shared_ptr<RequestObject>> requests;
        Handlers::RequestHandler *handler;

    public:
        SocketClient(std::string server_ip, uint16_t port, std::string role);

        ~SocketClient();

        ListeningThread *getListeningThread();

        AskingThread *getAskingThread();

        void addRequestObject(RequestObject *ro);

        std::string getRole();


        bool isActive();

        void stop();

        std::string sendRequest(std::string request_body);

        Handlers::RequestHandler *getHandler();
    };

    class SocketClientLMO : public Handlers::Action {
        SocketClient *client;
    public:
        SocketClientLMO(SocketClient *client);

        virtual bool isYours(std::string &message);

        virtual std::string act(std::string &message);
    };

    class SocketClientGMJ : public Handlers::Action {
        SocketClient *client;
    public:
        SocketClientGMJ(SocketClient *client);

        virtual bool isYours(std::string &message);

        virtual std::string act(std::string &message);
    };

    class SocketClientTMJ : public Handlers::Action {
        SocketClient *client;
    public:
        SocketClientTMJ(SocketClient *client);

        virtual bool isYours(std::string &message);

        virtual std::string act(std::string &message);
    };

};
#endif