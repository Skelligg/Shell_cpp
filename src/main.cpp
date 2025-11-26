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

    std::cout << command <<": command not found" << '\n';
  }

}
