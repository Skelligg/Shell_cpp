//
// Created by Michael Lukyanov on 27/11/2025.
//

#include "shell.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <unistd.h>   // fork, execvp
#include <sys/wait.h> // waitpid

shell::shell() {
    builtInCommands["echo"] = [this](const std::string& cmd){ echoCommand(cmd); };
    builtInCommands["type"] = [this](const std::string& cmd){ typeCommand(cmd); };
    builtInCommands["pwd"] = [this](const std::string& cmd){ pwdCommand(); };
    builtInCommands["exit"] = [this](const std::string& cmd){};
}

void shell::run() {
    std::string cmd;

    while (true) {
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;

        std::cout << "$ ";

        std::getline(std::cin, cmd);

        std::string action { parseAction(cmd) };

        if (exitCommand(cmd)) {
            break;
        }

        auto it = builtInCommands.find(action);
        if (it != builtInCommands.end()) {
            it->second(cmd);
        } else {
            if (!cmd.empty()) runExternalCommand(cmd);
        }

    }
}

std::string shell::parseAction(const std::string& cmd) {
    return cmd.substr(0,cmd.find_first_of(' '));
}

void shell::echoCommand(const std::string& cmd) {
    size_t pos = cmd.find_first_of(' ');
    if (pos == std::string::npos) {
        std::cout << '\n';
        return;
    }
    std::cout << cmd.substr(pos + 1) << '\n';
}

void shell::typeCommand(const std::string& cmd) {
    size_t pos = cmd.find_first_of(' ');
    if (pos == std::string::npos) {
        return;
    }
    std::string param1 { cmd.substr(pos + 1,cmd.size())};
    if ( builtInCommands.contains(param1)) {
        std::cout << param1 << " is a shell builtin" << '\n';
    }

    else {
        std::string dirFound {findExternalCommand(param1)};
        if (!dirFound.empty()) std::cout << param1 << " is " << dirFound << '\n';
        else std::cout << param1 << ": not found" << '\n';

    }
}

void shell::pwdCommand() {
    std::cout << std::filesystem::current_path().string() << '\n';
    // Below is initial solution using PATH_MAX which is not defined on all systems so failed on codecrafters alpine linux sandbox
    // char buffer[PATH_MAX];
    // if (getcwd(buffer, sizeof(buffer))) {
    //     std::cout << buffer << '\n';
    // }
}


bool shell::exitCommand(const std::string& cmd) {
    return cmd == "exit";
}

// When a command isn't a builtin, your shell should:
//
//     Search for an executable with the given name in the directories listed in PATH (just like type does)
//     If found, execute the program
//     Pass any arguments from the command line to the program
//
// For example, if the user types custom_exe arg1 arg2, your shell should:
//
//     Find custom_exe in PATH
//     Execute it with three arguments: custom_exe (the program name), arg1, and arg2

void shell::runExternalCommand(const std::string& cmd) {
    std::vector args { split(cmd, ' ')};
    std::string cmdFound {findExternalCommand(args[0])};
    if (!cmdFound.empty()) {
        pid_t pid = fork();
        if (pid == 0) {
            std::vector<char*> argv;
            for (const auto& a : args) {
                argv.push_back(const_cast<char*>(a.c_str()));
            }
            argv.push_back(nullptr);
            execvp(cmdFound.c_str(), argv.data());

            // If we reach here, exec failed:
            perror("execvp");
            _exit(1);
        }
        else {
            // Parent process -> wait for child to finish
            int status;
            waitpid(pid, &status, 0);
        }
    }
    else printError(cmd);
}


void shell::printError(const std::string& cmd) {
    std::cout << cmd <<": command not found" << '\n';
}

std::string shell::findExternalCommand(const std::string& cmd) {
    std::string pathVar { std::getenv("PATH") };
    std::vector pathDirs { split(pathVar,':')};

    for (std::string& dir : pathDirs) {
        dir.append("/" + cmd);
        if (access(dir.c_str(), X_OK) == 0) {
            return dir;
        }
    }
    return "";
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
