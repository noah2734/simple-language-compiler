#ifndef __PARSER_H__
#define __PARSER_H__

#include <iostream>
#include <vector>
#include "lexer.h"


typedef enum {
    EXPR,
    TERM
} snodeType;

typedef enum {
    EQUAL_OP,
    ID_OP,
    NUM_OP,
    MINUS_OP,
    PLUS_OP,
    MULT_OP,
    DIV_OP,
    ARRAY_ELEM_OP,
    WHOLE_ARRAY_OP
} operatorType;

typedef enum {
    SCALAR_TYPE,
    ARRAY_TYPE,
    ARRAY_DECL_TYPE,
    ERROR_TYPE
} exprType;

struct exprNode {
    operatorType opType;

    exprType type;

    
    std::string varName;
    int line_no;
    

    
    struct exprNode* left;
    struct exprNode* right;
    struct exprNode* parent;

    struct array {
        struct exprNode *arrayexpr;
        int line_no;
    };

};

struct stackNode {
    snodeType type;
    exprNode *expr;
    Token term;
};


class Parser {

private:
    const std::vector<std::string> expr_rhs = {"E-E", "E+E", "E*E", "E/E", "(E)","E[E]", "E[.]"};
    const char table[12][12] = {
 // stack -> |+|  |-|  |*|  |/|  |(|  |)|  |[|  |.|  |]|  |num||id| |$|
// input      0    1    2    3    4    5    6    7    8    9    10   11
/* |+| */   {'>', '>', '>', '>', '<', '>', '<', 'e', '>', '>', '>', '<'},// 0
/* |-| */   {'>', '>', '>', '>', '<', '>', '<', 'e', '>', '>', '>', '<'},// 1
/* |*| */   {'<', '<', '>', '>', '<', '>', '<', 'e', '>', '>', '>', '<'},// 2
/* |/| */   {'<', '<', '>', '>', '<', '>', '<', 'e', '>', '>', '>', '<'},// 3
/* |(| */   {'<', '<', '<', '<', '<', 'e', '<', 'e', 'e', 'e', 'e', '<'},// 4
/* |)| */   {'>', '>', '>', '>', '=', '>', '<', 'e', '>', '>', '>', 'e'},// 5
/* |[| */   {'<', '<', '<', '<', '<', '>', '<', 'e', '>', '>', '>', '<'},// 6
/* |.| */   {'e', 'e', 'e', 'e', 'e', 'e', '=', 'e', 'e', 'e', 'e', 'e'},// 7
/* |]| */   {'>', '>', '>', '>', '<', '>', '=', '=', '>', '>', '>', 'e'},// 8
/* |num| */ {'<', '<', '<', '<', '<', 'e', '<', 'e', 'e', 'e', 'e', '<'},// 9
/* |id| */  {'<', '<', '<', '<', '<', 'e', '<', 'e', 'e', 'e', 'e', '<'},// 10
/* |$| */   {'>', '>', '>', '>', 'e', '>', 'e', 'e', '>', '>', '>', 'a'},// 11
    };
    LexicalAnalyzer lexer;
    std::vector<exprNode*> treeRoots;
    std::vector<exprNode*> outputRoots;
    std::vector<stackNode> stack;
    std::vector<std::string> scalar_IDs;
    std::vector<std::string> array_IDs;
    std::vector<int> expr_error_line_numbers; 
    std::vector<int> assignment_error_line_numbers;
    bool isVariableAccess;
    bool isOutput = false;
    bool start_spaces = false;
    bool tree_error = false;

public:

    Token expect(TokenType expected_type);

    void syntax_error();

    void parse_program();

    void parse_decl_section();

    void parse_scalar_decl_section();

    void parse_scalar_id_list();

    void parse_array_id_list();

    void parse_array_decl_section();

    void parse_block();

    void parse_stmt_list();

    void parse_stmt();

    void parse_assign_stmt();

    void parse_output_stmt();

    exprNode* parse_variable_access();

    exprNode* parse_expr();

    Token peek_symbol();

    stackNode terminal_peek();

    int getPrecedenceKey(Token token);

    void shift();   

    void reduce();

    void traverse_and_print(exprNode* node);

    void printTree();

    void printStack();

    void traverse_and_print_level(exprNode* node, int level);

    int height(exprNode* node);

    void type_check();

    void type_error_in_tree(exprNode* node);

    void print_typecheck_statement();
};

#endif  