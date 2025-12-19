/*
 * ALETHEIA-Core: AST Implementation
 */

#include "ast.h"

/* Create AST node */
ASTNode* create_ast_node(ASTNodeType type) {
    ASTNode* node = core_malloc(sizeof(ASTNode));
    node->type = type;
    node->node_type = 0;
    return node;
}

/* Free AST node (recursive) */
void free_ast_node(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.decl_count; i++) {
                free_ast_node(node->data.program.declarations[i]);
            }
            core_free(node->data.program.declarations);
            break;

        case AST_FUNCTION_DEF:
            core_free(node->data.func_def.name);
            for (int i = 0; i < node->data.func_def.param_count; i++) {
                free_ast_node(node->data.func_def.params[i]);
            }
            core_free(node->data.func_def.params);
            free_type(node->data.func_def.return_type);
            free_ast_node(node->data.func_def.body);
            break;

        case AST_VAR_DECL:
            core_free(node->data.var_decl.name);
            free_type(node->data.var_decl.var_type);
            free_ast_node(node->data.var_decl.initializer);
            break;

        case AST_RETURN_STMT:
            free_ast_node(node->data.return_expr);
            break;

        case AST_IF_STMT:
            free_ast_node(node->data.if_stmt.condition);
            free_ast_node(node->data.if_stmt.then_branch);
            free_ast_node(node->data.if_stmt.else_branch);
            break;

        case AST_WHILE_STMT:
            free_ast_node(node->data.while_stmt.condition);
            free_ast_node(node->data.while_stmt.body);
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                free_ast_node(node->data.block.statements[i]);
            }
            core_free(node->data.block.statements);
            break;

        case AST_BINARY_EXPR:
            free_ast_node(node->data.binary.left);
            free_ast_node(node->data.binary.right);
            break;

        case AST_UNARY_EXPR:
            free_ast_node(node->data.unary.operand);
            break;

        case AST_ASSIGN_EXPR:
            free_ast_node(node->data.assign.target);
            free_ast_node(node->data.assign.value);
            break;

        case AST_FUNCTION_CALL:
            core_free(node->data.call.name);
            for (int i = 0; i < node->data.call.arg_count; i++) {
                free_ast_node(node->data.call.args[i]);
            }
            core_free(node->data.call.args);
            break;

        case AST_IDENTIFIER:
            core_free(node->data.identifier);
            break;

        case AST_INTEGER_LITERAL:
            /* Nothing to free */
            break;

        case AST_STRING_LITERAL:
            core_free(node->data.str_value);
            break;
    }

    if (node->node_type) {
        free_type(node->node_type);
    }

    core_free(node);
}

/* Create type */
TypeInfo* create_type(TypeKind kind) {
    TypeInfo* type = core_malloc(sizeof(TypeInfo));
    type->kind = kind;
    type->base = 0;

    switch (kind) {
        case TYPE_VOID: type->size = 0; break;
        case TYPE_CHAR: type->size = 1; break;
        case TYPE_INT: type->size = 4; break;
        case TYPE_PTR: type->size = 8; break; /* 64-bit pointers */
    }

    return type;
}

/* Create pointer type */
TypeInfo* create_pointer_type(TypeInfo* base) {
    TypeInfo* ptr_type = create_type(TYPE_PTR);
    ptr_type->base = base;
    return ptr_type;
}

/* Free type */
void free_type(TypeInfo* type) {
    if (!type) return;
    if (type->base) {
        free_type(type->base);
    }
    core_free(type);
}

