#include "parser.h"

void Parser::syntax_error() {
    std::cout << "SNYATX EORRR !!!";
    exit(1);
}

stackNode Parser::terminal_peek() {
    for (int i = stack.size() - 1; i >= 0; i--) {
        if (stack[i].type == TERM) {
            return stack[i];
        }
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
    A_Node* left_child_of_root = parse_variable_access();

    Token t = expect(EQUAL);

    tree.root->value = std::make_pair(t.token_type, t.lexeme);

    tree.root->left_child = left_child_of_root;
    left_child_of_root->parent = tree.root;
    
    A_Node* right_child_of_root = parse_expr();

    tree.root->right_child = right_child_of_root;
    right_child_of_root->parent = tree.root;

    expect(SEMICOLON);
}

void Parser::parse_output_stmt()
{
    expect(OUTPUT);

    parse_variable_access();

    expect(SEMICOLON);
}

A_Node* Parser::parse_variable_access()
{
    A_Node* node;

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

A_Node* Parser::parse_expr()
{
    
}

