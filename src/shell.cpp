//
// Created by Michael Lukyanov on 27/11/2025.
//

#include "shell.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

shell::shell() {
    builtInCommands["echo"] = [this](const std::string& cmd){ echoCommand(cmd); };
    builtInCommands["type"] = [this](const std::string& cmd){ typeCommand(cmd); };
    builtInCommands["exit"] = [this](const std::string& cmd){ exitCommand(cmd); };
}

void shell::run() {
    std::string cmd;

    while (!exitCommand(cmd)) {
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;

        std::cout << "$ ";

        std::getline(std::cin, cmd);

        std::string action { parseAction(cmd) };

        auto it = builtInCommands.find(action);
        if (it != builtInCommands.end()) {
            it->second(cmd);
        } else {
            if (cmd.empty())continue;
            printError(cmd);
        }

    }
}

std::string shell::parseAction(const std::string& cmd) {
    return cmd.substr(0,cmd.find_first_of(' '));
}

void shell::echoCommand(const std::string& cmd) {
    std::cout << cmd.substr(cmd.find_first_of(' ') + 1,cmd.size()) << '\n';
}

void shell::typeCommand(const std::string& cmd) {
    std::string param1 { cmd.substr(cmd.find_first_of(' ') + 1,cmd.size())};
    std::string dirFound;
    if ( builtInCommands.contains(param1)) {
        std::cout << param1 << " is a shell builtin" << '\n';
    }

    else {
        std::string pathVar { std::getenv("PATH") };
        std::vector pathDirs { split(pathVar,':')};

        for (std::string& dir : pathDirs) {
            dir.append("/" + param1);
            if (access(dir.c_str(), X_OK) == 0) {
                dirFound = dir;
                break;
            }
        }
        if (!dirFound.empty()) std::cout << param1 << " is " << dirFound << '\n';
        else std::cout << param1 << ": not found" << '\n';

    }

}

void shell::printError(const std::string& cmd) {
    std::cout << cmd <<": command not found" << '\n';
}

std::vector<std::string> shell::split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }

    return parts;
}


bool shell::exitCommand(const std::string& cmd) {
    return cmd == "exit";
}