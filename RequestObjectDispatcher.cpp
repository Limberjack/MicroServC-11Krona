#include "Server.h"

void Server::RequestObjectDispatcher::addRequestObject(RequestObject *ro) {
    this->all_request_objects.push_back(std::shared_ptr<RequestObject>(ro));
}

void Server::RequestObjectDispatcher::stop() {
    while (!this->all_request_objects.empty()) {
        for (auto i = this->all_request_objects.begin(); i != this->all_request_objects.end(); i++) {
            i->get()->stop();
        }
    }
}

Server::RequestObjectDispatcher::~RequestObjectDispatcher() {
    stop();
}