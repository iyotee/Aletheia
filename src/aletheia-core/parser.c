/*
 * ALETHEIA-Core: Simplified Parser Implementation
 */

#include "parser.h"

/* Create parser */
Parser* create_parser(Lexer* lexer) {
    Parser* parser = core_malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = next_token(lexer);
    return parser;
}

/* Free parser */
void free_parser(Parser* parser) {
    free_token(parser->current_token);
    core_free(parser);
}

/* Get current token */
Token* current_token(Parser* parser) {
    return parser->current_token;
}

/* Advance to next token */
void advance(Parser* parser) {
    free_token(parser->current_token);
    parser->current_token = next_token(parser->lexer);
}

/* Check if current token matches type */
bool match(Parser* parser, TokenType type) {
    return parser->current_token->type == type;
}

/* Expect token and advance */
bool expect(Parser* parser, TokenType type) {
    if (match(parser, type)) {
        advance(parser);
        return true;
    }
    return false;
}

/* Parse type */
ASTNode* parse_type(Parser* parser) {
    /* For now, only support int and char */
    if (match(parser, TOK_INT)) {
        advance(parser);
        return create_type(TYPE_INT);
    } else if (match(parser, TOK_CHAR)) {
        advance(parser);
        return create_type(TYPE_CHAR);
    } else if (match(parser, TOK_VOID)) {
        advance(parser);
        return create_type(TYPE_VOID);
    }

    return 0; /* Error */
}

/* Parse primary expression */
ASTNode* parse_primary(Parser* parser) {
    if (match(parser, TOK_NUM)) {
        ASTNode* node = create_ast_node(AST_INTEGER_LITERAL);
        node->data.int_value = 0; /* Simplified - no atoi */
        char* val = parser->current_token->value;
        int num = 0;
        while (*val) {
            num = num * 10 + (*val - '0');
            val++;
        }
        node->data.int_value = num;
        node->node_type = create_type(TYPE_INT);
        advance(parser);
        return node;
    }

    if (match(parser, TOK_STR)) {
        ASTNode* node = create_ast_node(AST_STRING_LITERAL);
        node->data.str_value = core_strdup(parser->current_token->value);
        node->node_type = create_pointer_type(create_type(TYPE_CHAR));
        advance(parser);
        return node;
    }

    if (match(parser, TOK_IDENT)) {
        ASTNode* node = create_ast_node(AST_IDENTIFIER);
        node->data.identifier = core_strdup(parser->current_token->value);
        /* Type will be resolved later */
        advance(parser);

        /* Check for function call */
        if (match(parser, TOK_LPAREN)) {
            advance(parser); /* consume ( */

            ASTNode* call = create_ast_node(AST_FUNCTION_CALL);
            call->data.call.name = core_strdup(node->data.identifier);
            call->data.call.args = 0;
            call->data.call.arg_count = 0;

            /* For simplicity, no arguments for now */
            if (!expect(parser, TOK_RPAREN)) {
                free_ast_node(call);
                return 0;
            }

            free_ast_node(node);
            return call;
        }

        return node;
    }

    if (match(parser, TOK_LPAREN)) {
        advance(parser);
        ASTNode* expr = parse_expression(parser);
        if (!expect(parser, TOK_RPAREN)) {
            free_ast_node(expr);
            return 0;
        }
        return expr;
    }

    return 0; /* Error */
}

/* Parse unary expression */
ASTNode* parse_unary(Parser* parser) {
    if (match(parser, TOK_STAR)) {
        advance(parser);
        ASTNode* operand = parse_unary(parser);
        if (!operand) return 0;

        ASTNode* unary = create_ast_node(AST_UNARY_EXPR);
        unary->data.unary.op = '*';
        unary->data.unary.operand = operand;
        if (operand->node_type && operand->node_type->kind == TYPE_PTR) {
            unary->node_type = operand->node_type->base;
        }
        return unary;
    }

    return parse_primary(parser);
}

/* Parse binary expression (simplified precedence) */
ASTNode* parse_expression(Parser* parser) {
    ASTNode* left = parse_unary(parser);
    if (!left) return 0;

    while (match(parser, TOK_PLUS) || match(parser, TOK_MINUS) ||
           match(parser, TOK_STAR) || match(parser, TOK_SLASH) ||
           match(parser, TOK_EQ) || match(parser, TOK_LT) || match(parser, TOK_GT)) {

        char op = 0;
        switch (parser->current_token->type) {
            case TOK_PLUS: op = '+'; break;
            case TOK_MINUS: op = '-'; break;
            case TOK_STAR: op = '*'; break;
            case TOK_SLASH: op = '/'; break;
            case TOK_EQ: op = '='; break; /* Note: simplified */
            case TOK_LT: op = '<'; break;
            case TOK_GT: op = '>'; break;
        }

        advance(parser);
        ASTNode* right = parse_unary(parser);
        if (!right) {
            free_ast_node(left);
            return 0;
        }

        ASTNode* binary = create_ast_node(AST_BINARY_EXPR);
        binary->data.binary.op = op;
        binary->data.binary.left = left;
        binary->data.binary.right = right;
        binary->node_type = create_type(TYPE_INT); /* Assume int result */

        left = binary;
    }

    return left;
}

/* Parse assignment */
ASTNode* parse_assignment(Parser* parser) {
    ASTNode* target = parse_unary(parser);
    if (!target) return 0;

    if (match(parser, TOK_ASSIGN)) {
        advance(parser);
        ASTNode* value = parse_expression(parser);
        if (!value) {
            free_ast_node(target);
            return 0;
        }

        ASTNode* assign = create_ast_node(AST_ASSIGN_EXPR);
        assign->data.assign.target = target;
        assign->data.assign.value = value;
        return assign;
    }

    return target;
}

/* Parse statement */
ASTNode* parse_statement(Parser* parser) {
    if (match(parser, TOK_INT) || match(parser, TOK_CHAR)) {
        return parse_variable_declaration(parser);
    }

    if (match(parser, TOK_RETURN)) {
        advance(parser);
        ASTNode* expr = parse_expression(parser);
        if (!expect(parser, TOK_SEMI)) {
            free_ast_node(expr);
            return 0;
        }

        ASTNode* ret = create_ast_node(AST_RETURN_STMT);
        ret->data.return_expr = expr;
        return ret;
    }

    if (match(parser, TOK_IF)) {
        advance(parser);
        if (!expect(parser, TOK_LPAREN)) return 0;

        ASTNode* condition = parse_expression(parser);
        if (!condition) return 0;

        if (!expect(parser, TOK_RPAREN)) {
            free_ast_node(condition);
            return 0;
        }

        ASTNode* then_branch = parse_statement(parser);
        if (!then_branch) {
            free_ast_node(condition);
            return 0;
        }

        ASTNode* else_branch = 0;
        if (match(parser, TOK_ELSE)) {
            advance(parser);
            else_branch = parse_statement(parser);
        }

        ASTNode* if_stmt = create_ast_node(AST_IF_STMT);
        if_stmt->data.if_stmt.condition = condition;
        if_stmt->data.if_stmt.then_branch = then_branch;
        if_stmt->data.if_stmt.else_branch = else_branch;
        return if_stmt;
    }

    if (match(parser, TOK_WHILE)) {
        advance(parser);
        if (!expect(parser, TOK_LPAREN)) return 0;

        ASTNode* condition = parse_expression(parser);
        if (!condition) return 0;

        if (!expect(parser, TOK_RPAREN)) {
            free_ast_node(condition);
            return 0;
        }

        ASTNode* body = parse_statement(parser);
        if (!body) {
            free_ast_node(condition);
            return 0;
        }

        ASTNode* while_stmt = create_ast_node(AST_WHILE_STMT);
        while_stmt->data.while_stmt.condition = condition;
        while_stmt->data.while_stmt.body = body;
        return while_stmt;
    }

    if (match(parser, TOK_LBRACE)) {
        advance(parser);

        /* Simple block - for now, just one statement */
        ASTNode* stmt = parse_statement(parser);
        if (!expect(parser, TOK_RBRACE)) {
            free_ast_node(stmt);
            return 0;
        }

        ASTNode* block = create_ast_node(AST_BLOCK);
        block->data.block.statements = core_malloc(sizeof(ASTNode*));
        block->data.block.statements[0] = stmt;
        block->data.block.stmt_count = 1;
        return block;
    }

    /* Expression statement */
    ASTNode* expr = parse_assignment(parser);
    if (expect(parser, TOK_SEMI)) {
        return expr;
    }

    free_ast_node(expr);
    return 0;
}

/* Parse variable declaration */
ASTNode* parse_variable_declaration(Parser* parser) {
    /* Parse type */
    TypeInfo* var_type = 0;
    if (match(parser, TOK_INT)) {
        advance(parser);
        var_type = create_type(TYPE_INT);
    } else if (match(parser, TOK_CHAR)) {
        advance(parser);
        var_type = create_type(TYPE_CHAR);
    } else {
        return 0;
    }

    /* Variable name */
    if (!match(parser, TOK_IDENT)) {
        free_type(var_type);
        return 0;
    }

    char* name = core_strdup(parser->current_token->value);
    advance(parser);

    /* Check for pointer */
    if (match(parser, TOK_STAR)) {
        advance(parser);
        var_type = create_pointer_type(var_type);
    }

    /* Initializer */
    ASTNode* init = 0;
    if (match(parser, TOK_ASSIGN)) {
        advance(parser);
        init = parse_expression(parser);
    }

    if (!expect(parser, TOK_SEMI)) {
        free_type(var_type);
        core_free(name);
        free_ast_node(init);
        return 0;
    }

    ASTNode* decl = create_ast_node(AST_VAR_DECL);
    decl->data.var_decl.name = name;
    decl->data.var_decl.var_type = var_type;
    decl->data.var_decl.initializer = init;
    return decl;
}

/* Parse function definition */
ASTNode* parse_function_definition(Parser* parser) {
    /* Return type */
    TypeInfo* return_type = 0;
    if (match(parser, TOK_INT)) {
        advance(parser);
        return_type = create_type(TYPE_INT);
    } else if (match(parser, TOK_CHAR)) {
        advance(parser);
        return_type = create_type(TYPE_CHAR);
    } else if (match(parser, TOK_VOID)) {
        advance(parser);
        return_type = create_type(TYPE_VOID);
    } else {
        return 0;
    }

    /* Function name */
    if (!match(parser, TOK_IDENT)) {
        free_type(return_type);
        return 0;
    }

    char* name = core_strdup(parser->current_token->value);
    advance(parser);

    /* Parameters (simplified - no params for now) */
    if (!expect(parser, TOK_LPAREN) || !expect(parser, TOK_RPAREN)) {
        free_type(return_type);
        core_free(name);
        return 0;
    }

    /* Body */
    ASTNode* body = parse_statement(parser);
    if (!body) {
        free_type(return_type);
        core_free(name);
        return 0;
    }

    ASTNode* func = create_ast_node(AST_FUNCTION_DEF);
    func->data.func_def.name = name;
    func->data.func_def.params = 0;
    func->data.func_def.param_count = 0;
    func->data.func_def.return_type = return_type;
    func->data.func_def.body = body;
    return func;
}

/* Parse program */
ASTNode* parse_program(Parser* parser) {
    ASTNode* program = create_ast_node(AST_PROGRAM);
    program->data.program.declarations = 0;
    program->data.program.decl_count = 0;

    /* For now, just parse one function */
    ASTNode* func = parse_function_definition(parser);
    if (func) {
        program->data.program.declarations = core_malloc(sizeof(ASTNode*));
        program->data.program.declarations[0] = func;
        program->data.program.decl_count = 1;
    }

    return program;
}

