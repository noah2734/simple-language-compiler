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
        if (isVariableAccess) {
            return Token{"", END_OF_FILE, -1};
        } else syntax_error(); return Token{};
        
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
        default:
            syntax_error();
            return -1;
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
            bool inArray = false;
            bool inScalar = false;

            for (auto& id : array_IDs) {
                if (id_lexeme == id) {
                    inArray = true;
                }
            }

            for (auto& id : scalar_IDs) {
                if (id_lexeme == id) {
                    inScalar = true;
                }
            }

            if (inArray && inScalar) {
                exprRoot->type = ERROR_TYPE;
            } else if (inArray) {
                exprRoot->type = ARRAY_DECL_TYPE;
            } else if (inScalar) {
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
        return;
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
                default:
                    syntax_error();
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
        if (exprRoot == nullptr) {
            syntax_error();
        }
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

        //exprRoot->left = tempStack[0].expr;
        

        exprRoot->left = tempStack[0].expr;
       

        tempStack[0].expr->parent = exprRoot;

        if (tempStack[0].expr->type == SCALAR_TYPE || tempStack[0].expr->type == ARRAY_DECL_TYPE) {
            exprRoot->type = ARRAY_TYPE;
        } else {
            exprRoot->type = ERROR_TYPE;
        }

        stack.push_back(stackNode{EXPR, exprRoot});
        return;
    } else {
        syntax_error();
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

    mem.declarScalarVariable(t.lexeme);

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
    Token t = lexer.peek(1);
    if (t.token_type == ID || t.token_type == OUTPUT || t.token_type == NUM) {
        parse_stmt_list();
    } else syntax_error();
    expect(RBRACE);

}

void Parser::parse_stmt_list()
{
    parse_stmt();
    Token t = lexer.peek(1);
    if (t.token_type == ID || t.token_type == OUTPUT || t.token_type == NUM) {
        parse_stmt_list();
    } else {
        return;
    }
}

void Parser::parse_stmt()
{
    Token t = lexer.peek(1);

    if (t.token_type == ID) {
        parse_assign_stmt();
    } else if (t.token_type == OUTPUT) {
        parse_output_stmt();
    } else {
        syntax_error();
    }
}

void Parser::parse_assign_stmt()
{

    exprNode* root = new exprNode();
    root->opType = EQUAL_OP;
    root->varName = "=";
    isOutput = false;
    exprNode* left_child_of_root = parse_variable_access();

    Token t = expect(EQUAL);

    root->left = left_child_of_root;
    root->line_no = t.line_no;
    
    isVariableAccess = false;
    exprNode* right_child_of_root = parse_expr();

    if (right_child_of_root->type == ARRAY_DECL_TYPE) {
        right_child_of_root->type = ERROR_TYPE;
    }

    else if (! (left_child_of_root->type == ARRAY_TYPE || right_child_of_root->type == SCALAR_TYPE)) {
        assignment_error_line_numbers.push_back(t.line_no);
    }

    root->right = right_child_of_root;
    right_child_of_root->parent = root;

    treeRoots.push_back(root);

    expect(SEMICOLON);
}

void Parser::parse_output_stmt()
{
    expect(OUTPUT);
    isOutput = true;
    exprNode* root = parse_variable_access();
    outputRoots.push_back(root);
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

    if (!isOutput) {
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

               /* bool found = false;
                for (auto& id : array_IDs) {
                    if (leftnode->varName == id) {
                        found = true;
                    }
                }

                if (found) {
                    leftnode->type = ARRAY_DECL_TYPE;
                } else {
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
                }

                if (leftnode->type == SCALAR_TYPE) {
                    left_child_of_root->type = ERROR_TYPE;
                } else if (leftnode->type == ARRAY_DECL_TYPE) {
                    left_child_of_root->type = ARRAY_TYPE;
                } else {
                    left_child_of_root->type = ERROR_TYPE;
                }*/

                bool inArray = false;
                bool inScalar = false;

                for (auto& id : array_IDs) {
                    if (leftnode->varName == id) {
                        inArray = true;
                    }
                }

                for (auto& id : scalar_IDs) {
                    if (leftnode->varName == id) {
                        inScalar = true;
                    }
                }

                if (inArray && inScalar) {
                    leftnode->type = ERROR_TYPE;
                } else if (inScalar) {
                    leftnode->type = SCALAR_TYPE;
                } else if (inArray) {
                    leftnode->type = ARRAY_DECL_TYPE;
                } else {
                    leftnode->type = ERROR_TYPE;
                }

                if (leftnode->type == SCALAR_TYPE) {
                    left_child_of_root->type = ERROR_TYPE;
                }
                else if (leftnode->type == ARRAY_DECL_TYPE) {
                    left_child_of_root->type = ARRAY_TYPE;
                } else {
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

                bool inArray = false;
                bool inScalar = false;

                for (auto& id : array_IDs) {
                    if (leftnode->varName == id) {
                        inArray = true;
                    }
                }

                for (auto& id : scalar_IDs) {
                    if (leftnode->varName == id) {
                        inScalar = true;
                    }
                }

                if (inArray && inScalar) {
                    leftnode->type = ERROR_TYPE;
                } else if (inScalar) {
                    leftnode->type = SCALAR_TYPE;
                } else if (inArray) {
                    leftnode->type = ARRAY_DECL_TYPE;
                } else {
                    leftnode->type = ERROR_TYPE;
                }

                if (leftnode->type == ARRAY_DECL_TYPE && rightnode->type == SCALAR_TYPE) {
                    left_child_of_root->type = SCALAR_TYPE;
                } else {
                    left_child_of_root->type = ERROR_TYPE;
                }

                left_child_of_root->opType = ARRAY_ELEM_OP;
                left_child_of_root->varName = "[]";

                left_child_of_root->left = leftnode;
                left_child_of_root->right = rightnode;


                rightnode->parent = left_child_of_root;
                leftnode->parent = left_child_of_root;


                return left_child_of_root;
            } else {
                syntax_error();
                return nullptr;
            }
        } else if (t.token_type == EQUAL) {
            // Return only leftnode as all we have on LHS is an ID
            bool inScalar = false;
            bool inArray = false;
            for (auto& id : scalar_IDs) {
                if (leftnode->varName == id) {
                    inScalar = true;
                }
            }

            for (auto& id : array_IDs) {
                if (leftnode->varName == id) {
                    inArray = true;
                }
            }

            if (inScalar && inArray) {
                leftnode->type = ERROR_TYPE;
            } else if (inScalar) {
               leftnode->type = SCALAR_TYPE;
            } else if (inArray) {
                leftnode->type = ERROR_TYPE;
            } else {
                leftnode->type = ERROR_TYPE;
            }
            return leftnode;
        } else {
            syntax_error();
            return nullptr;
        }
    } else {
        if (t.token_type == LBRAC) {
            expect(LBRAC);

            exprNode* rightnode = new exprNode();

            t = lexer.peek(1);

            if (t.token_type == DOT) {
                expect(DOT);

                expect(RBRAC);

                exprNode* root = new exprNode();

                bool found = false;
                for (auto& id : array_IDs) {
                    if (leftnode->varName == id) {
                        found = true;
                    }
                }

                if (found) {
                    leftnode->type = ARRAY_DECL_TYPE;
                } else {
                    bool found = false;
                    for (auto& id : array_IDs) {
                        if (leftnode->varName == id) {
                            found = true;
                        }
                    }
                    if (found) {
                    leftnode->type = SCALAR_TYPE;
                    } else {
                        leftnode->type = ERROR_TYPE;
                    }
                }

                if (leftnode->type == SCALAR_TYPE) {
                    root->type = ARRAY_TYPE;
                } else if (leftnode->type == ARRAY_DECL_TYPE) {
                    root->type = ARRAY_TYPE;
                } else {
                    root->type = ERROR_TYPE;
                }


                root->opType = WHOLE_ARRAY_OP;
                root->varName = "[.]"; 

                root->left = leftnode;


                leftnode->parent = root;
                root->line_no = t.line_no;
                return root;
            } else if (t.token_type == LPAREN || t.token_type == ID || t.token_type == NUM) {
                
                rightnode = parse_expr();

                expect(RBRAC);

                exprNode* root = new exprNode();

                bool found = false;
                for (auto& id : array_IDs) {
                    if (leftnode->varName == id) {
                        found = true;
                    }
                }

                if (found) {
                    leftnode->type = ARRAY_DECL_TYPE;
                } else {
                    leftnode->type = ERROR_TYPE;
                }

                if (leftnode->type == ARRAY_DECL_TYPE && rightnode->type == SCALAR_TYPE) {
                    root->type = SCALAR_TYPE;
                } else {
                    root->type = ERROR_TYPE;
                }

                root->opType = ARRAY_ELEM_OP;
                root->varName = "[]";

                root->left = leftnode;
                root->right = rightnode;


                rightnode->parent = root;
                leftnode->parent = root;

                root->line_no = t.line_no;
                return root;
            } else {
                syntax_error();
                return nullptr;
            }
        } else if (t.token_type == SEMICOLON) {
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
                leftnode->line_no = t.line_no;
                return leftnode;
            
        } else {
            syntax_error();
            return nullptr;
        }
    }
}

exprNode* Parser::parse_expr()
{
    while (!stack.empty()) {
        stack.pop_back();
    }
    stack.push_back(stackNode{TERM, nullptr, Token{"", END_OF_FILE, -1}});

    while (1) {

        Token inputToken = peek_symbol();
    
        Token stackToken = terminal_peek().term;
    
        int inputTerminalKey = 0;
        int stackTerminalKey = 0;

        inputTerminalKey = getPrecedenceKey(inputToken);
        stackTerminalKey  = getPrecedenceKey(stackToken);


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
        if (node->type == ERROR_TYPE) {
            tree_error = true;
        }
        type_error_in_tree(node->left);
        type_error_in_tree(node->right);
    }
}


void Parser::type_check() {

    for (auto& tree : treeRoots) {
        int line_number = tree->line_no;

        
        type_error_in_tree(tree);
        if (tree_error) {
            expr_error_line_numbers.push_back(line_number);
            tree_error = false;
        }
    }

    for (auto& tree : outputRoots) {
        int line_number = tree->line_no;


        type_error_in_tree(tree);
        if (tree_error) {
            int line_number = tree->line_no;
            expr_error_line_numbers.push_back(line_number);
            tree_error = false;
        }
    }
}

void Parser::print_typecheck_statement() {
    //std::sort(expr_error_line_numbers.begin(), expr_error_line_numbers.end());
    if (!expr_error_line_numbers.empty()) {
        std::cout << "Disappointing expression type error :(" << std::endl << std::endl;
        std::vector<int> no_dups;
        for (auto& line_number : expr_error_line_numbers) {
            bool is_dup = false;
            for (auto& num : no_dups) {
                if (line_number == num) {
                    is_dup = true;
                }
            }
            if (is_dup) {
                continue;
            } else {
                std::cout << "Line " << line_number << std::endl;
                no_dups.push_back(line_number);
            }
        }
        exit(1);
        //std::sort(expr_error_line_numbers.begin(), expr_error_line_numbers.end());
    } else if (!assignment_error_line_numbers.empty()) {
        bool is_dup = false;
        std::cout << "The following assignment(s) is/are invalid :(" << std::endl << std::endl;
        std::vector<int> no_dups;
        for (auto& line_number : assignment_error_line_numbers) {
            for (auto& num : no_dups) {
                if (line_number == num) {
                    is_dup = true;
                }
            }
            if (is_dup) {
                continue;
            } else {
                std::cout << "Line " << line_number << std::endl;
                no_dups.push_back(line_number);
            }
        }
    } else {
        std::cout << "Amazing! No type errors here :)";
    }
}