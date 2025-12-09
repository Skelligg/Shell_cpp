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
	std::string match;
	for (char c : prefix) {
		if (cur->children.count(c)) {
			cur = cur->children[c].get();
		}
	}
	Node* base = cur;
	for (const auto& pair : base->children) {
		cur = base;
		match = prefix;
		cur = cur->children[pair.first].get();
		match.push_back(pair.first);
		while (cur && !cur->isEnd) {
			auto it = cur->children.begin();
			match.push_back(it->first);      // the character
			cur = it->second.get();
		}
		matches.push_back(match);
	}
	return matches;
}
