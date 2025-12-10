//
// Created by Michael Lukyanov on 07/12/2025.
//

#include "Autocompleter.h"

#include <iostream>

Autocompleter::Autocompleter() : root {std::make_unique<Node>()}
{}

void Autocompleter::addCommand(const std::string& cmd) {
	Node* cur = root.get();
	for (char c : cmd) {
		if (!cur->children.contains(c)) {
			cur->children[c] = std::make_unique<Node>();
		}
		cur = cur->children[c].get();
    }
    cur->isEnd = true;
}

std::vector<std::string> Autocompleter::startsWith(const std::string& prefix) {
	Node* cur = root.get();
	std::vector<std::string> matches;
	std::string potentialMatch;
	for (char c : prefix) {
		if (cur->children.count(c)) {
			cur = cur->children[c].get();
		}
		else return matches;
	}

	dfs(cur, prefix, matches);

	return matches;
}

// Depth-First Search, a standard tree/graph traversal algorithm.
void Autocompleter::dfs(Node* node, std::string current, std::vector<std::string>& results) {
	if (node->isEnd) {
		results.push_back(current);
	}
	for (auto& [ch, child] : node->children) {
		dfs(child.get(), current + ch, results);
	}
}