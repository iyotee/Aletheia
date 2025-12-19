/*
 * ALETHEIA-Core: Simplified Parser
 */

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

/* Parser state */
typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

/* Functions */
Parser* create_parser(Lexer* lexer);
void free_parser(Parser* parser);

ASTNode* parse_program(Parser* parser);
ASTNode* parse_function_definition(Parser* parser);
ASTNode* parse_variable_declaration(Parser* parser);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_type(Parser* parser);

/* Helper functions */
Token* current_token(Parser* parser);
void advance(Parser* parser);
bool match(Parser* parser, TokenType type);
bool expect(Parser* parser, TokenType type);

#endif /* PARSER_H */

