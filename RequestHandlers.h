#include "SocketDictionary.h"
#include <vector>
#include <memory>

namespace Handlers {

    class Action {
    public:
        virtual ~Action() = default;

        virtual bool isYours(std::string &message) = 0;

        virtual std::string act(std::string &message) = 0;
    };

    class RequestHandler {
        std::vector<std::shared_ptr<Action>> actions;
    public:
        virtual RequestHandler &addAction(Action *action);

        virtual std::string doSomething(std::string message);

        virtual ~RequestHandler() = default;
    };
};
