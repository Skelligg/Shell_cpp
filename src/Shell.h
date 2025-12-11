//
// Created by Michael Lukyanov on 27/11/2025.
//

#ifndef SHELL_STARTER_CPP_SHELL_H
#define SHELL_STARTER_CPP_SHELL_H
#include <functional>
#include <string>
#include <unordered_map>

#include "Autocompleter.h"

class Shell {
public:
    Shell();
    void run();
private:
    int savedStdOut;
    std::unordered_map<std::string, std::function<void(const std::string&)>> builtInCommands;
    Autocompleter autocompleter{};

    std::string parseAction(const std::string&);

    int echoCommand(const std::string&);
    int typeCommand(const std::string&);
    int pwdCommand();
    int cdCommand(const std::string&);
    bool exitCommand(const std::string&);

	void handleInput(std::string&);
    void outputRedirect(std::string&);
    void restoreOutput();

    void runExternalCommand(const std::string&);
    void printError(const std::string&);

	bool is_executable(const std::string& path);
	std::vector<std::string> findAllExternalCommands();
    std::string findExternalCommand(const std::string& cmd);
    std::vector<std::string> split(const std::string& str, char delimiter);
};


#endif //SHELL_STARTER_CPP_SHELL_H