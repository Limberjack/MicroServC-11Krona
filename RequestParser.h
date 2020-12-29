#ifndef SOCKETLIBRARY_REQUESTPARSER_H
#define SOCKETLIBRARY_REQUESTPARSER_H

#include "SocketDictionary.h"

class RequestParser;

class ParsedMessage;

class ParsedMessage {
    friend RequestParser;
    std::string sender;
    std::string address;
    std::string command;
    std::string payload;

    ParsedMessage() = default;

    void setSender(const std::string &sender);

    void setAddress(const std::string &address);

    void setCommand(const std::string &command);

    void setPayload(const std::string &payload);

public:
    const std::string &getSender() const;

    const std::string &getAddress() const;

    const std::string &getCommand() const;

    const std::string &getPayload() const;
};

class RequestParser {
public:
    static ParsedMessage* parse(std::string msg);
};


#endif //SOCKETLIBRARY_REQUESTPARSER_H
