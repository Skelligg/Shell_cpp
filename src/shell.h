//
// Created by Michael Lukyanov on 27/11/2025.
//

#ifndef SHELL_STARTER_CPP_SHELL_H
#define SHELL_STARTER_CPP_SHELL_H
#include <functional>
#include <string>
#include <unordered_map>

class shell {
public:
    shell();
    void run();
private:

    std::unordered_map<std::string, std::function<void(const std::string&)>> builtInCommands;

    std::string parseAction(const std::string&);

    void echoCommand(const std::string&);
    void typeCommand(const std::string&);
    bool exitCommand(const std::string&);

    void runExternalCommand(const std::string&);
    void printError(const std::string&);

    std::string findExternalCommand(const std::string& cmd);
    std::vector<std::string> split(const std::string& str, char delimiter);
};


#endif //SHELL_STARTER_CPP_SHELL_H