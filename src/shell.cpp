//
// Created by Michael Lukyanov on 27/11/2025.
//

#include "shell.h"
#include <iostream>

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
    if ( builtInCommands.contains(param1)) std::cout << param1 << " is a shell builtin" << '\n';
    else std::cout << param1 << ": not found" << '\n';
}

void shell::printError(const std::string& cmd) {
    std::cout << cmd <<": command not found" << '\n';
}


bool shell::exitCommand(const std::string& cmd) {
    return cmd == "exit";
}