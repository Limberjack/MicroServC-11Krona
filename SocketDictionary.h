#ifndef _SOCK_DICKT_H___
#define _SOCK_DICKT_H___

#include<iostream>

namespace Roles {
    const std::string CGI_role = "cgi";
    const std::string Modem_role = "mod";
    const std::string Server_role = "srv";
    const std::string User_Console_role = "ucr";
};

namespace Request {
    const std::string give_me_json = "gmj";
    const std::string take_my_json = "tmj";
    const std::string reg_me_in = "reg";
    const std::string log_me_out = "lmo";
    const std::string are_you_ok = "auo"; //-

};

namespace Response {
    const std::string OK = "OK";
    const std::string NO = "NO";
    const std::string SENTRY = "SEN";
    const std::string TRASH = "TRASH";
};

namespace SpecialSymbols{
    const std::string CONFIRM = "|~|";
    const std::string MESSAGE_OPEN = "[`";
    const std::string MESSAGE_CLOSE = "[*";
    const std::string MESSAGE_PART_DIVISION = " ";
    const std::string PAYLOAD_BORDER_OPEN = "]`";
    const std::string PAYLOAD_BORDER_CLOSE = "]*";
};

namespace ServerInfo {
    const std::string server_ip = "127.0.0.1";
    const int port = 8000;
};

#endif