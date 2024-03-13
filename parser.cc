#include "parser.h"

void Parser::syntax_error() {
    std::cout << "SNYATX EORRR !!!";
    exit(1);
}

Token Parser::peek_symbol() {
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    if (t1.token_type == SEMICOLON) {
        return Token{"", END_OF_FILE, -1};
    } else if (t1.token_type == RBRAC && t2.token_type == EQUAL) {
        return Token{"", END_OF_FILE, -1};
    } else if (t1.token_type == RBRAC && t2.token_type == SEMICOLON) {
        return Token{"", END_OF_FILE, -1};
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
    Token t = lexer.GetToken();

    std::vector<exprNode*> expressions;
    std::vector<stackNode> full_expression;
    std::string expression_string = "";

    while (1)
    {
        if (stack[stack.size() - 1].type == EXPR) {
            expression_string += "E";
            expressions.insert(expressions.begin(), stack[stack.size() - 1].expr);
            full_expression.insert(full_expression.begin(), stack[stack.size() - 1]);
            stack.pop_back();
        } else {
            int inputKey = getPrecedenceKey(t);
            int stackKey = getPrecedenceKey(stack[stack.size() - 1].term);
            char prec = table[stackKey][inputKey];
            if (prec == '<') {
                if (full_expression.size() == 1) {
                    if (full_expression[0].type == TERM && full_expression[0].term.token_type == ID || (full_expression[0].type == TERM && full_expression[0].term.token_type == NUM)) {
                        stackNode new_expr;
                        new_expr.type = EXPR;
                        new_expr.expr = new exprNode();
                        new_expr.expr->opType = ID_OP;
                        new_expr.expr->varName = full_expression[0].term.lexeme;
                        new_expr.expr->line_no = full_expression[0].term.line_no;
                    } 
                }
                for (int i = 0; i < expr_rhs.size(); i++) {
                    if (expr_rhs[i] == expression_string) {
                        stackNode new_expr;
                        new_expr.type = EXPR;
                        new_expr.expr = new exprNode();
                        switch(expression_string[1]) {
                            case '[':
                                new_expr.expr->opType = ARRAY_ELEM_OP;
                                new_expr.expr->varName = "[]";
                                new_expr.expr->line_no = -1;
                                new_expr.expr->left = expressions[0];
                                new_expr.expr->left->parent = new_expr.expr;
                                new_expr.expr->right = expressions[2];
                                new_expr.expr->right->parent = new_expr.expr;
                                break;
                            case '+':
                                new_expr.expr->opType = PLUS_OP;
                                new_expr.expr->varName = "+";
                                new_expr.expr->line_no = -1;
                                new_expr.expr->left = expressions[0];
                                new_expr.expr->left->parent = new_expr.expr;
                                new_expr.expr->right = expressions[2];
                                new_expr.expr->right->parent = new_expr.expr;
                                break;
                            case '-':
                                new_expr.expr->opType = MINUS_OP;
                                new_expr.expr->varName = "-";
                                new_expr.expr->line_no = -1;
                                new_expr.expr->left = expressions[0];
                                new_expr.expr->left->parent = new_expr.expr;
                                new_expr.expr->right = expressions[2];
                                new_expr.expr->right->parent = new_expr.expr;
                                break;
                            case '/':
                                new_expr.expr->opType = DIV_OP;
                                new_expr.expr->varName = "/";
                                new_expr.expr->line_no = -1;
                                new_expr.expr->left = expressions[0];
                                new_expr.expr->left->parent = new_expr.expr;
                                new_expr.expr->right = expressions[2];
                                new_expr.expr->right->parent = new_expr.expr;
                                break;
                            case '*':
                                new_expr.expr->opType = MULT_OP;
                                new_expr.expr->varName = "*";
                                new_expr.expr->line_no = -1;
                                new_expr.expr->left = expressions[0];
                                new_expr.expr->left->parent = new_expr.expr;
                                new_expr.expr->right = expressions[2];
                                new_expr.expr->right->parent = new_expr.expr;
                                break;
                        }
                        stack.push_back(new_expr);
                    }
                }
            } else {
                full_expression.insert(full_expression.begin(), stack[stack.size() - 1]);
                stack.pop_back();
            }
        }
    }
}


Token Parser::expect(TokenType expected_type) 
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type) {
        std::cout << "@expect with expected type: " << expected_type << std::endl;
        std::cout << "the actual type is: " << t.token_type << std::endl;
        syntax_error();
    }
    return t;
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
    if (t.token_type == ID || t.token_type == OUTPUT) {
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
    exprNode* left_child_of_root = parse_variable_access();

    Token t = expect(EQUAL);

    root->left = left_child_of_root;
    
    
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
    Token t = expect(ID);

    exprNode* leftnode = new exprNode();
    leftnode->opType = ID_OP;
    leftnode->varName = t.lexeme;

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

            left_child_of_root->opType = WHOLE_ARRAY_OP;
            left_child_of_root->left = leftnode;

            rightnode->varName = ".";

            left_child_of_root->right = rightnode;

            rightnode->parent = left_child_of_root;
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

            left_child_of_root->opType = ARRAY_ELEM_OP;

            left_child_of_root->left = leftnode;
            left_child_of_root->right = rightnode;

            rightnode->parent = left_child_of_root;
            leftnode->parent = left_child_of_root;

            return left_child_of_root;
        }
    } else if (t.token_type == EQUAL) {
        // Return only leftnode as all we have on LHS is an ID
        return leftnode;
    }
}

exprNode* Parser::parse_expr()
{
    stack.push_back(stackNode{TERM, nullptr, Token{"", END_OF_FILE, -1}});
    int i = 0;
    while (1) {
        i++;
        if (i == 2) {
            for (auto& node : stack) {
            std::cout << node.type << std::endl;
            exit(1);
        }
        }
        Token inputToken = peek_symbol();
    
        Token stackToken = terminal_peek().term;
    
        int inputTerminalKey = getPrecedenceKey(inputToken);
        int stackTerminalKey = getPrecedenceKey(stackToken);

        char precedence = table[inputTerminalKey][stackTerminalKey];
        if (precedence == 'a') {
            return stack[1].expr;
        } else if (precedence == '<' || precedence == '=') {
            shift();
        } else if (precedence == '>') {
            reduce();
        } else {
            std::cout << "@parse_expr";
            syntax_error();
        }
    }
}

void Parser::traverse_and_print(exprNode* node) {
    traverse_and_print(node->left);
    std::cout << node->varName;
    traverse_and_print(node->right);
}

void Parser::printTree() {
    exprNode* root = treeRoots[0];
    traverse_and_print(root);
}

