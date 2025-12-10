//
// Created by Michael Lukyanov on 07/12/2025.
//

#ifndef SHELL_STARTER_CPP_AUTOCOMPLETER_H
#define SHELL_STARTER_CPP_AUTOCOMPLETER_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


class Autocompleter {
public:
    Autocompleter();
    void addCommand(const std::string& cmd);
	std::vector<std::string> startsWith(const std::string& prefix);

private:
    struct Node {
        bool isEnd {false};
        std::unordered_map<char, std::unique_ptr<Node>> children;
    };
    std::unique_ptr<Node> root;
	void dfs(Node* node, std::string current, std::vector<std::string>& results);

};


#endif //SHELL_STARTER_CPP_AUTOCOMPLETER_H