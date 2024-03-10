#include "parser.h"

void Parser::syntax_error() {
    std::cout << "SNYATX EORRR !!!";
    exit(1);
}

Token Parser::get_symbol() {
    if (symbolIndex >= currentSymbols.length()) {
        Token t = lexer.GetToken();
    }
    

    

}

Token Parser::peek_symbol() {

}

stackNode Parser::terminal_peek() {
    for (int i = stack.size() - 1; i >= 0; i--) {
        if (stack[i].type == TERM) {
            return stack[i];
        }
    }
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

Token Parser::expect(TokenType expected_type) 
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type) {
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

    if (lexer.peek(1).token_type == ID) {
        parse_scalar_id_list();
    } else {
        syntax_error();
    }
}

void Parser::parse_array_id_list()
{
    Token t = expect(ID);
    
    array_IDs.push_back(t.lexeme);

    if (lexer.peek(1).token_type == ID) {
        parse_array_id_list();
    } else {
        syntax_error();
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
    } else {
        syntax_error();
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
    exprNode* left_child_of_root = parse_variable_access();

    Token t = expect(EQUAL);

    root->left = left_child_of_root;
    
    
    exprNode* right_child_of_root = parse_expr();

    root->right = right_child_of_root;
    right_child_of_root->parent = root;

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
    exprNode* node;

    expect(ID);

    expect(LBRAC);

    Token t = lexer.peek(1);
    if (t.token_type == DOT) {
        expect(DOT);
    } else if (t.token_type == LPAREN || t.token_type == ID || t.token_type == NUM) {
        parse_expr();
    } else {
        syntax_error();
    }

    expect(RBRAC);
}

exprNode* Parser::parse_expr()
{
    stack.push_back(stackNode{TERM, nullptr, Token{"", END_OF_FILE, -1}});
    Token inputToken = peek_symbol();
    Token stackToken = terminal_peek().term;
    
    int inputTerminalKey = getPrecedenceKey(inputToken);
    int stackTerminalKey = getPrecedenceKey(stackToken);

    char precedence = table[inputTerminalKey][stackTerminalKey];

    if (precedence == '<' || precedence == '=') {
        //shift
    } else if (precedence == '>') {
        //reduce
    } else {
        syntax_error();
    }

}

