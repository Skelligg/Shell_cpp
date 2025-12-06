//
// Created by Michael Lukyanov on 27/11/2025.
//

#include "shell.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <unistd.h>   // fork, execvp
#include <fcntl.h>      // O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, etc.
#include <sys/stat.h>

#include <sys/wait.h> // waitpid

shell::shell() {
    builtInCommands["echo"] = [this](const std::string& cmd){ echoCommand(cmd); };
    builtInCommands["type"] = [this](const std::string& cmd){ typeCommand(cmd); };
    builtInCommands["cd"] = [this](const std::string& cmd){ cdCommand(cmd); };
    builtInCommands["pwd"] = [this](const std::string& cmd){ pwdCommand(); };
    builtInCommands["exit"] = [this](const std::string& cmd){};
}

void shell::run() {
    std::string cmd;

    while (true) {

        std::cout << "$ ";

        std::getline(std::cin, cmd);

        std::string action { parseAction(cmd) };
        outputRedirect(cmd);

        if (exitCommand(cmd)) {
            break;
        }

        auto it = builtInCommands.find(action);
        if (it != builtInCommands.end()) {
            it->second(cmd);
        } else {
            if (!cmd.empty()) runExternalCommand(cmd);
        }
        restoreOutput();
        std::cout << std::flush;
        std::cerr << std::flush;
    }
}

std::string shell::parseAction(const std::string& cmd) {
    return cmd.substr(0,cmd.find_first_of(' '));
}

int shell::echoCommand(const std::string& cmd) {
    size_t pos = cmd.find_first_of(' ');
    if (pos == std::string::npos) {
        std::cout << '\n';
        return 0;
    }

    std::string output {cmd.substr(pos + 1)};
    if (output.front() == '"' && output.back() == '"' || output.front() == '\'' && output.back() == '\'') {
        output = output.substr(1,output.size()-2);
    }

    std::cout << output << '\n';
    return 0;
}

int shell::typeCommand(const std::string& cmd) {
    size_t pos = cmd.find_first_of(' ');
    if (pos == std::string::npos) {
        return 0;
    }
    std::string param1 { cmd.substr(pos + 1)};
    if ( builtInCommands.contains(param1)) {
        std::cout << param1 << " is a shell builtin" << '\n';
    }

    else {
        std::string dirFound {findExternalCommand(param1)};
        if (!dirFound.empty()) std::cout << param1 << " is " << dirFound << '\n';
        else std::cerr << param1 << ": not found" << '\n';
        return 1;
    }
    return 0;
}

int shell::pwdCommand() {
    std::cout << std::filesystem::current_path().string() << '\n';
    return 0;

    // Below is initial solution using PATH_MAX which is not defined on all systems so failed on codecrafters alpine linux sandbox
    // char buffer[PATH_MAX];
    // if (getcwd(buffer, sizeof(buffer))) {
    //     std::cout << buffer << '\n';
    // }
}

int shell::cdCommand(const std::string& cmd) {
    size_t pos = cmd.find_first_of(' ');
    if (pos == std::string::npos) {
        return 0;
    }
    std::string dir { cmd.substr(pos + 1)};
    if (dir == "~") {
        const char* home = getenv("HOME");
        if (home) dir = home;
    }

    if (chdir(dir.c_str())) {
        std::cerr << "cd: " << dir << ": No such file or directory" << '\n';
        return 1;
    }
    return 0;
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

void shell::outputRedirect(std::string& cmd) {
    size_t pos = cmd.find_first_of('>');
    if (pos == std::string::npos)
        return;

    savedStdOut = dup(STDOUT_FILENO);

    std::string filename{};
    if (cmd[pos+1] == '>') {
        filename = cmd.substr(pos+2);
    }
    else {
        filename = cmd.substr(pos+1);
    }

    auto start = filename.find_first_not_of(" \t");
    auto end   = filename.find_last_not_of(" \t");
    filename = filename.substr(start, end - start + 1);

    if (pos) {
        if ( cmd[pos-1] != '2' && cmd[pos-1] != '&') {
            if (cmd[pos+1] == '>') {
                int fd = open(filename.c_str(),
                    O_WRONLY | O_CREAT | O_APPEND,
                    0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            else {
                int fd = open(filename.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

        }
        else if ( cmd[pos-1] == '2') {
            int fd = open(filename.c_str(),
              O_WRONLY | O_CREAT | O_TRUNC,
              0644);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        if (cmd[pos-1] == '1' || cmd[pos-1] == '2') {
            cmd = cmd.substr(0, pos-1);
        } else {
            cmd = cmd.substr(0, pos);
        }

        while (!cmd.empty() && std::isspace(cmd.back())) {
            cmd.pop_back();
        }
    }

}

void shell::restoreOutput() {
    if (savedStdOut != -1) {
        dup2(savedStdOut, STDOUT_FILENO);
        close(savedStdOut);
        savedStdOut = -1;
    }
}

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
            exit(1);
        }
        // Parent process -> wait for child to finish
        int status;
        waitpid(pid, &status, 0);

    }
    else printError(cmd);
}


void shell::printError(const std::string& cmd) {
    std::cerr << cmd <<": command not found" << '\n';
    std::cerr << std::flush;

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
