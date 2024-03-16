#include "parser.h"

void Parser::syntax_error() {
    std::cout << "SNYATX EORRR !!!";
    exit(1);
}

Token Parser::expect(TokenType expected_type) 
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type) {
        syntax_error();
    }
    return t;
}

Token Parser::peek_symbol() {

    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    if (t1.token_type == SEMICOLON) {
        return Token{"", END_OF_FILE, -1};
    } else if (t1.token_type == RBRAC && t2.token_type == EQUAL) {
        return Token{"", END_OF_FILE, -1};
    } else if (t1.token_type == RBRAC && t2.token_type == SEMICOLON) {
            if (isVariableAccess) {
            return Token{"", END_OF_FILE, -1};
        } else {return t1;}
    } else {
        return t1;
    }
}

stackNode Parser::terminal_peek() {
    for (int i = stack.size() - 1; i >= 0; i--) {
        if (stack[i].type == TERM) {
            return stack[i];
        }
    }
    return stackNode{};
}

int Parser::getPrecedenceKey(Token token) {
    switch (token.token_type) {
        case PLUS:
            return 0;
            break;
        case MINUS:
            return 1;
            break;
        case MULT:
            return 2;
            break;
        case DIV:
            return 3;
            break;
        case LPAREN:
            return 4;
            break;
        case RPAREN:
            return 5;
            break;
        case LBRAC:
            return 6;
            break;
        case DOT:
            return 7;
            break;
        case RBRAC:
            return 8;
            break;
        case NUM:
            return 9;
            break;
        case ID:
            return 10;
            break;
        case END_OF_FILE:
            return 11;
            break;
    }
}

void Parser::shift() {
    Token t = lexer.GetToken();
    stack.push_back(stackNode{TERM, nullptr, t});
}

void Parser::reduce() {

    Token t = peek_symbol();

    std::vector<stackNode> tempStack;

    Token last_popped_term = t;
    while (1) {

        stackNode topStack = stack[stack.size() - 1];

        if (topStack.type == EXPR) {
            tempStack.insert(tempStack.begin(), topStack);
            stack.pop_back();
            continue;
        } else {
            int last_popped_key = getPrecedenceKey(last_popped_term);
            int stackKey =  getPrecedenceKey(topStack.term);

            if (table[last_popped_key][stackKey] == '>' || table[last_popped_key][stackKey] == '=') {
                tempStack.insert(tempStack.begin(), topStack);
                stack.pop_back();
                last_popped_term = topStack.term;
                continue;
            } else {
                break;
            }
        }
    }


    if (tempStack.size() == 1) {
        if (tempStack[0].term.token_type == ID) {
            exprNode* exprRoot = new exprNode();
            exprRoot->opType = ID_OP;
            std::string id_lexeme = tempStack[0].term.lexeme;
            //assign type
            bool found = false;
            for (auto id : scalar_IDs) {
                if (id_lexeme == id) {
                    found = true;
                }
            }

            if (found) {
                exprRoot->type = SCALAR_TYPE;
            } else {
                exprRoot->type = ERROR_TYPE;
            }

            exprRoot->varName = id_lexeme;

            exprRoot->line_no = tempStack[0].term.line_no;

            stack.push_back(stackNode{EXPR, exprRoot});
            
        } else if (tempStack[0].term.token_type == NUM) {
            exprNode* exprRoot = new exprNode();
            exprRoot->opType = NUM_OP;
             std::string num_lexeme = tempStack[0].term.lexeme;
            //assign type
            exprRoot->type = SCALAR_TYPE;

            exprRoot->varName = num_lexeme;

            exprRoot->line_no = tempStack[0].term.line_no;

            stack.push_back(stackNode{EXPR, exprRoot});
        }
    }

    std::string exprString = "";

    for (int i = 0; i < tempStack.size(); i++) {
        if (tempStack[i].type == EXPR) {
            exprString += "E";
        } else {
            switch (tempStack[i].term.token_type) {
                case ID:
                    exprString +=  tempStack[i].term.lexeme;
                    break;
                case PLUS:
                    exprString +=  "+";
                    break;
                case MINUS:
                    exprString +=  "-";
                    break;
                case MULT:
                    exprString += "*";
                    break;
                case DIV:
                    exprString += "/";
                    break;
                case LBRAC:
                    exprString +=  "[";
                    break;
                case RBRAC:
                    exprString +=  "]";
                    break;
                case RPAREN:
                    exprString +=  ")";
                    break;
                case LPAREN:
                    exprString +=  "(";
                    break;
                case DOT:
                    exprString +=  ".";
                    break;
                case NUM:
                    exprString += tempStack[i].term.lexeme;
                    break;
                case END_OF_FILE:
                    exprString += "$";
                    break;
            }
        }
    }
    

    if (exprString == "") {
        syntax_error();
    }
    
    if (exprString == "E-E") {
            exprNode* exprRoot = new exprNode();
            exprRoot->opType = MINUS_OP;

            exprRoot->varName = "-";

            exprRoot->line_no = tempStack[1].term.line_no;

            exprRoot->left = tempStack[0].expr;
            exprRoot->right = tempStack[2].expr;

            tempStack[0].expr->parent = exprRoot;
            tempStack[2].expr->parent = exprRoot;

            if (tempStack[0].expr->type == SCALAR_TYPE && tempStack[2].expr->type == SCALAR_TYPE) {
                exprRoot->type = SCALAR_TYPE;
            } else if (tempStack[0].expr->type == ARRAY_TYPE && tempStack[2].expr->type == ARRAY_TYPE) {
                exprRoot->type = ARRAY_TYPE;
            } else {
                exprRoot->type = ERROR_TYPE;
            }

            stack.push_back(stackNode{EXPR, exprRoot});
            return;
    } else if (exprString == "E+E") {
        
        exprNode* exprRoot = new exprNode();
        exprRoot->opType = PLUS_OP;

        exprRoot->varName = "+";

        exprRoot->line_no = tempStack[1].term.line_no;

        exprRoot->left = tempStack[0].expr;
        exprRoot->right = tempStack[2].expr;

        tempStack[0].expr->parent = exprRoot;
        tempStack[2].expr->parent = exprRoot;

        if (tempStack[0].expr->type == SCALAR_TYPE && tempStack[2].expr->type == SCALAR_TYPE) {
            exprRoot->type = SCALAR_TYPE;
        } else if (tempStack[0].expr->type == ARRAY_TYPE && tempStack[2].expr->type == ARRAY_TYPE) {
            exprRoot->type = ARRAY_TYPE;
        } else {
            exprRoot->type = ERROR_TYPE;
        }

        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    } else if (exprString == "E*E") {

        exprNode* exprRoot = new exprNode();
        exprRoot->opType = MULT_OP;

        exprRoot->varName = "*";

        exprRoot->line_no = tempStack[1].term.line_no;

        exprRoot->left = tempStack[0].expr;
        exprRoot->right = tempStack[2].expr;

        tempStack[0].expr->parent = exprRoot;
        tempStack[2].expr->parent = exprRoot;

        if (tempStack[0].expr->type == SCALAR_TYPE && tempStack[2].expr->type == SCALAR_TYPE) {
            exprRoot->type = SCALAR_TYPE;
        } else if (tempStack[0].expr->type == ARRAY_TYPE && tempStack[2].expr->type == ARRAY_TYPE) {
            exprRoot->type = SCALAR_TYPE;
        } else {
            exprRoot->type = ERROR_TYPE;
        }

        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    } else if (exprString == "E/E") {

        exprNode* exprRoot = new exprNode();
        exprRoot->opType = DIV_OP;

        exprRoot->varName = "/";

        exprRoot->line_no = tempStack[1].term.line_no;

        exprRoot->left = tempStack[0].expr;
        exprRoot->right = tempStack[2].expr;

        tempStack[0].expr->parent = exprRoot;
        tempStack[2].expr->parent = exprRoot;

        if (tempStack[0].expr->type == SCALAR_TYPE && tempStack[2].expr->type == SCALAR_TYPE) {
            exprRoot->type = SCALAR_TYPE;
        } else if (tempStack[0].expr->type == ARRAY_TYPE && tempStack[2].expr->type == ARRAY_TYPE) {
            exprRoot->type = ERROR_TYPE;
        } else {
            exprRoot->type = ERROR_TYPE;
        }

        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    } else if (exprString == "(E)") {

        exprNode* exprRoot = new exprNode();
        exprRoot = tempStack[1].expr; // just replace (E) with E
        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    } else if (exprString == "E[E]") {

        exprNode* exprRoot = new exprNode();
        exprRoot->opType = ARRAY_ELEM_OP;

        exprRoot->varName = "[]";

        exprRoot->line_no = tempStack[1].term.line_no;

        exprRoot->left = tempStack[0].expr;
        exprRoot->right = tempStack[2].expr;

        tempStack[0].expr->parent = exprRoot;
        tempStack[2].expr->parent = exprRoot;

        if (tempStack[2].expr->type != SCALAR_TYPE) {
            exprRoot->type = ERROR_TYPE;
        } else if (tempStack[0].expr->type == ARRAY_DECL_TYPE && tempStack[2].expr->type == SCALAR_TYPE) {
            exprRoot->type = SCALAR_TYPE;
        } else if (tempStack[0].expr->type == SCALAR_TYPE || tempStack[0].expr->type == ERROR_TYPE) {
            exprRoot->type = ERROR_TYPE;
        } else {
            exprRoot->type = ERROR_TYPE;
        }

        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    } else if (exprString == "E[.]") {

        exprNode* exprRoot = new exprNode();
        exprRoot->opType = ARRAY_ELEM_OP;
        

        exprRoot->varName = "[.]";

        exprRoot->line_no = tempStack[1].term.line_no;

        exprRoot->left = tempStack[0].expr;
        

        exprRoot->left = tempStack[0].expr;
       

        tempStack[0].expr->parent = exprRoot;

        if (tempStack[0].expr->type == SCALAR_TYPE) {
            exprRoot->type = ARRAY_TYPE;
        } else {
            exprRoot->type = ERROR_TYPE;
        }

        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    }
    
}

void Parser::parse_program() 
{
    parse_decl_section();
    parse_block();
    expect(END_OF_FILE);
    
}

void Parser::parse_decl_section()
{
    parse_scalar_decl_section();
    parse_array_decl_section();
}

void Parser::parse_scalar_decl_section() 
{
    expect(SCALAR);
    parse_scalar_id_list();
}

void Parser::parse_array_decl_section()
{
    expect(ARRAY);
    parse_array_id_list();
}

void Parser::parse_scalar_id_list()
{
    Token t = expect(ID);

    scalar_IDs.push_back(t.lexeme);

    t = lexer.peek(1);

    if (t.token_type == ID) {
        parse_scalar_id_list();
    } 
}

void Parser::parse_array_id_list()
{
    Token t = expect(ID);

    array_IDs.push_back(t.lexeme);

    t = lexer.peek(1);

    if (t.token_type == ID) {
        parse_array_id_list();
    }
}

void Parser::parse_block()
{
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);

}

void Parser::parse_stmt_list()
{
    parse_stmt();
    Token t = lexer.peek(1);
    if (t.token_type == ID || t.token_type == OUTPUT || t.token_type == NUM) {
        parse_stmt_list();
    } 
}

void Parser::parse_stmt()
{
    Token t = lexer.peek(1);

    if (t.token_type == ID) {
        parse_assign_stmt();
    } else if (t.token_type == OUTPUT) {
        parse_output_stmt();
    }
}

void Parser::parse_assign_stmt()
{
    exprNode* root = new exprNode();
    root->opType = EQUAL_OP;
    root->varName = "=";
    exprNode* left_child_of_root = parse_variable_access();

    Token t = expect(EQUAL);

    root->left = left_child_of_root;
    
    isVariableAccess = false;
    exprNode* right_child_of_root = parse_expr();

    root->right = right_child_of_root;
    right_child_of_root->parent = root;

    treeRoots.push_back(root);

    expect(SEMICOLON);
}

void Parser::parse_output_stmt()
{
    expect(OUTPUT);

    parse_variable_access();

    expect(SEMICOLON);
}

exprNode* Parser::parse_variable_access()
{
    isVariableAccess = true;
    Token t = expect(ID);

    exprNode* leftnode = new exprNode();
    leftnode->opType = ID_OP;
    leftnode->varName = t.lexeme;
    //assign type

    t = lexer.peek(1);

    if (t.token_type == LBRAC) {
        expect(LBRAC);

        exprNode* rightnode = new exprNode();

        t = lexer.peek(1);

        if (t.token_type == DOT) {
            //          []
            //       ____|____
            //       |       |
            //      ID      DOT 
            expect(DOT);

            expect(RBRAC);

            exprNode* left_child_of_root = new exprNode();

            bool found = false;
            for (auto& id : array_IDs) {
                if (leftnode->varName == id) {
                    found = true;
                }
            }

            if (found) {
                leftnode->type = ARRAY_DECL_TYPE;
                left_child_of_root->type = ARRAY_TYPE;
            } else {
                leftnode->type = ERROR_TYPE;
                left_child_of_root->type = ERROR_TYPE;
            }


            left_child_of_root->opType = WHOLE_ARRAY_OP;
            left_child_of_root->varName = "[.]"; 

            left_child_of_root->left = leftnode;


            leftnode->parent = left_child_of_root;

            return left_child_of_root;
        } else if (t.token_type == LPAREN || t.token_type == ID || t.token_type == NUM) {
            //          []
            //       ____|____
            //       |       |
            //      ID      expr
            rightnode = parse_expr();

            expect(RBRAC);

            exprNode* left_child_of_root = new exprNode();

            bool found = false;
            for (auto& id : array_IDs) {
                if (leftnode->varName == id) {
                    found = true;
                }
            }

            if (found) {
                leftnode->type = ARRAY_DECL_TYPE;
                if (rightnode->type == SCALAR_TYPE) {
                    left_child_of_root->type = ARRAY_TYPE;
                } else {
                    left_child_of_root->type = ERROR_TYPE;
                }
            } else {
                leftnode->type = ERROR_TYPE;
                left_child_of_root->type = ERROR_TYPE;
            }

            left_child_of_root->opType = ARRAY_ELEM_OP;
            left_child_of_root->varName = "[]";

            left_child_of_root->left = leftnode;
            left_child_of_root->right = rightnode;


            rightnode->parent = left_child_of_root;
            leftnode->parent = left_child_of_root;


            return left_child_of_root;
        }
    } else if (t.token_type == EQUAL) {
        // Return only leftnode as all we have on LHS is an ID
        bool found = false;
        for (auto& id : scalar_IDs) {
            if (leftnode->varName == id) {
                found = true;
            }
        }

        if (found) {
            leftnode->type = SCALAR_TYPE;
        } else {
            leftnode->type = ERROR_TYPE;
        }
        return leftnode;
    }
}

exprNode* Parser::parse_expr()
{
    while (!stack.empty()) {
        stack.pop_back();
    }
    stack.push_back(stackNode{TERM, nullptr, Token{"", END_OF_FILE, -1}});
    int i = 0;
    while (1) {
        i++;

        Token inputToken = peek_symbol();
    
        Token stackToken = terminal_peek().term;
    
        int inputTerminalKey = 0;
        int stackTerminalKey = 0;

        inputTerminalKey = getPrecedenceKey(inputToken);
        stackTerminalKey  = getPrecedenceKey(stackToken);

        //exit(1);
        char precedence = table[inputTerminalKey][stackTerminalKey];

        if (precedence == 'a') {
            return stack[1].expr;
        } else if (precedence == '<' || precedence == '=') {
            shift();
        } else if (precedence == '>') {
            reduce();
        } else if (precedence == 'e'){
            syntax_error();
        }
    }
}
 
void Parser::printTree() {
    exprNode* root = treeRoots[0];
    int h = height(root);

    for (int i = 1; i <= h; i++) {
        traverse_and_print_level(root, i);
    }
}

void Parser::traverse_and_print_level(exprNode* root, int level) {
    if (root == nullptr) {
        return;
    }
    if (level == 1) {
        if (root->opType == ID_OP) {
            if (!start_spaces) {
                std::cout << "ID \"" << root->varName << "\"";
            } else {
                std::cout << " " << "ID \"" << root->varName << "\"";
            }
        } else if (root->opType == NUM_OP) {
            if (!start_spaces) {
                std::cout << "NUM \"" << root->varName << "\"";
            } else {
                std::cout << " " << "NUM \"" << root->varName << "\"";
            }
        } else {
            if (!start_spaces) {
                std::cout << root->varName;
                start_spaces = true;
            } else {
                std::cout << " " << root->varName;
            }
        }
    } else if (level > 1) {
        traverse_and_print_level(root->left, level - 1);
        traverse_and_print_level(root->right, level - 1);
    }
}

int Parser::height(exprNode* node) {

    if (node == nullptr) {
        return 0;
    } else {

        int lheight = height(node->left);
        int rheight = height(node->right);

        if (lheight < rheight) {
            return rheight + 1;
        } else {
            return lheight + 1;
        }
    }
} 

void Parser::type_error_in_tree(exprNode* node) {
    if (node) {
        if (node->type == ERROR) {
            tree_error = true;
        }
        return type_error_in_tree(node->left);
        return type_error_in_tree(node->right);
    }
}


void Parser::type_check() {

    for (auto& tree : treeRoots) {
        int line_number = tree->line_no;
        bool has_error = false;

        if (!(tree->left->type == ARRAY_TYPE || tree->right->type == SCALAR_TYPE)) {
            line_numbers.push_back(line_number);
        } else {
            type_error_in_tree(tree);
            if (tree_error) {
                line_numbers.push_back(line_number);
                tree_error = false;
            }
        }
    }
}

void Parser::print_typecheck_statement() {
    if (line_numbers.empty()) {
        std::cout << "Amazing! No type errors here :)";
    } else {
        std::cout << "Amazing! No type errors here :)" << std::endl << std::endl;
        for (auto& line_number : line_numbers) {
            std::cout << "Line " << line_number << std::endl;
        }
    }
}