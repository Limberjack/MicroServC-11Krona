//
// Created by limberjack on 21.11.2020.
//

#include "RequestParser.h"

const std::string &ParsedMessage::getSender() const {
    return sender;
}

const std::string &ParsedMessage::getAddress() const {
    return address;
}

const std::string &ParsedMessage::getCommand() const {
    return command;
}

const std::string &ParsedMessage::getPayload() const {
    return payload;
}

void ParsedMessage::setSender(const std::string &sender) {
    printf("sender: %s\n", sender.c_str());
    ParsedMessage::sender = sender;
}

void ParsedMessage::setAddress(const std::string &address) {
        printf("adress: %s\n", address.c_str());
    ParsedMessage::address = address;
}

void ParsedMessage::setCommand(const std::string &command) {
        printf("command: %s\n", command.c_str());
    ParsedMessage::command = command;
}

void ParsedMessage::setPayload(const std::string &payload) {
        printf("payload: %s\n", payload.c_str());
    ParsedMessage::payload = payload;
}


ParsedMessage* RequestParser::parse(std::string msg) {
    if(msg.find(SpecialSymbols::MESSAGE_PART_DIVISION) == std::string::npos)
        return nullptr;

    ParsedMessage* parsedMessage = new ParsedMessage();
    std::string tmp;
    tmp = msg.substr(0, msg.find(SpecialSymbols::MESSAGE_PART_DIVISION));
    msg = msg.erase(0, msg.find(SpecialSymbols::MESSAGE_PART_DIVISION) + SpecialSymbols::MESSAGE_PART_DIVISION.length());
    parsedMessage->setAddress(tmp);
    tmp = msg.substr(0, msg.find(SpecialSymbols::MESSAGE_PART_DIVISION));
    msg = msg.erase(0, msg.find(SpecialSymbols::MESSAGE_PART_DIVISION) + SpecialSymbols::MESSAGE_PART_DIVISION.length());
    parsedMessage->setSender(tmp);
    tmp = msg.substr(0, msg.find(SpecialSymbols::MESSAGE_PART_DIVISION));
    msg = msg.erase(0, msg.find(SpecialSymbols::MESSAGE_PART_DIVISION));
    parsedMessage->setCommand(tmp);
    if(msg.length() < SpecialSymbols::MESSAGE_PART_DIVISION.length())
        return parsedMessage;
    msg.erase(0, SpecialSymbols::MESSAGE_PART_DIVISION.length());
    parsedMessage->setPayload(msg);
    return parsedMessage;
}