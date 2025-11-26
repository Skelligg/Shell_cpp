#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::string command;

  while (command != "exit") {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "$ ";

    std::getline(std::cin, command);

    std::string action { command.substr(0,command.find_first_of(' '))};
    if ( action == "echo" ) {
      std::cout << command.substr(command.find_first_of(' ') + 1,command.size()) << '\n';
    }
    else if ( command != "exit") std::cout << command <<": command not found" << '\n';
  }

}
