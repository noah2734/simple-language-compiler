#ifndef __AST__h__
#define __AST__h__
#include <string>
#include <vector>
#include "lexer.h"

struct A_Node {
    std::pair<TokenType, std::string> value;
    A_Node* parent;
    A_Node* left_child;
    A_Node* right_child;
};

class AST {
public:
    std::vector<std::string> toPrint;
    A_Node* root;
    AST() {
        root->parent = nullptr;
    }

    void breadthFirstTraversal(A_Node* node);
};

#endif