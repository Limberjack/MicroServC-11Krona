#include "RequestHandlers.h"

using namespace Handlers;

RequestHandler &RequestHandler::addAction(Action *action) {
    this->actions.push_back(std::shared_ptr<Action>(action));
    return *this;
}

std::string RequestHandler::doSomething(std::string message) {
    for (auto i = this->actions.begin(); i != this->actions.end(); i++) {
        if (i->get()->isYours(message)) {
            //printf("_____%s\n", message.c_str());
            return i->get()->act(message);
        }
    }
    printf("-No handler for request \"%s\"\n", message.c_str());
    return Response::NO;
}


