#include "print.h"
#include "operators.h"

Error print_trees (const Functions* funcs, FILE* file)
{
    Error error = {};
    IfWhileId if_while_id = {};

    fprintf (file, "jmp main\n\n");
    for (size_t i = 0; i < funcs->num_funcs; i++)
    {
        error = print_tree (funcs->funcs[i], &if_while_id, file);
        PARSE_ERROR_WITHOUT_TREE(error);
        fprintf (file, "\n");
    }

    fprintf (file, "hlt");
    RETURN_ERROR(CORRECT, "");
}

Error print_tree (const Function* func, IfWhileId* if_while_id, FILE* file)
{
    Error error = {};
    fprintf (file, ":%s\n", func->root->name);

    error = print_args (func->root->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = print_commands (func->root->right, if_while_id, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    RETURN_ERROR(CORRECT, "");
}

Error print_args (const Node* node, FILE* file)
{
    Error error = {};
    if (!node)
        RETURN_ERROR(CORRECT, "");

    error = print_args (node->right, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    fprintf (file, "pop [%d]\n", (int) node->left->value);

    RETURN_ERROR(CORRECT, "");
}

Error print_commands (const Node* node, IfWhileId* if_while_id, FILE* file)
{
    if (!node)
        RETURN_ERROR(CORRECT, "");

    Error error = {};
    bool is_print = false;
    if (node->type == OPER)
    {
        if ((int) node->value == END_STR)
        {
            error = print_commands (node->left, if_while_id, file);
            PARSE_ERROR_WITHOUT_TREE(error);

            error = print_commands (node->right, if_while_id, file);
            PARSE_ERROR_WITHOUT_TREE(error);

            RETURN_ERROR(CORRECT, "");
        }

        error = print_input_output_return (node, &is_print, file);
        PARSE_ERROR_WITHOUT_TREE(error);
        if (is_print)
            RETURN_ERROR(CORRECT, "");

        error = print_assign (node, &is_print, file);
        PARSE_ERROR_WITHOUT_TREE(error);
        if (is_print)
            RETURN_ERROR(CORRECT, "");

        error = print_if (node, if_while_id, &is_print, file);
        PARSE_ERROR_WITHOUT_TREE(error);
        if (is_print)
            RETURN_ERROR(CORRECT, "");

        error = print_while (node, if_while_id, &is_print, file);
        PARSE_ERROR_WITHOUT_TREE(error);
        if (is_print)
            RETURN_ERROR(CORRECT, "");
    }

    RETURN_ERROR(SYNTAX_ERR, "Unknown operator");
}

Error print_input_output_return (const Node* node, bool* is_print, FILE* file)
{
    *is_print = false;

    if ((int) node->value == INPUT_VAR)
    {
        fprintf (file, "in\n");
        fprintf (file, "pop [%d]\n", (int) node->right->value);
        *is_print = true;
    }

    if ((int) node->value == PRINT)
    {
        fprintf (file, "push [%d]\n", (int) node->right->value);
        fprintf (file, "out\n");
        *is_print = true;
    }

    if ((int) node->value == RET)
    {
        fprintf (file, "push [%d]\n", (int) node->right->value);
        fprintf (file, "ret\n");
        *is_print = true;
    }

    RETURN_ERROR(CORRECT, "");
}

Error print_assign (const Node* node, bool* is_print, FILE* file)
{
    if ((int) node->value != ASSIGN)
    {
        *is_print = false;
        RETURN_ERROR(CORRECT, "");
    }

    *is_print = true;
    Error error = {};

    error = print_expression (node->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);
    fprintf (file, "pop [%d]\n", (int) node->right->value);

    RETURN_ERROR(CORRECT, "");
}

Error print_expression (const Node* node, FILE* file)
{
    if (!node)
        RETURN_ERROR(CORRECT, "");

    Error error = {};
    if (node->type == VAR)
    {
        fprintf (file, "push [%d]\n", (int) node->value);
        RETURN_ERROR(CORRECT, "");
    }
    else if (node->type == NUM)
    {
        fprintf (file, "push %lg\n", node->value);
        RETURN_ERROR(CORRECT, "");
    }
    else if (node->type == FUNC)
    {
        error = print_func_call (node, file);
        PARSE_ERROR_WITHOUT_TREE(error);
        RETURN_ERROR(CORRECT, "");
    }

    error = print_expression (node->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);
    error = print_expression (node->right, file);
    PARSE_ERROR_WITHOUT_TREE(error);


    for (size_t i = 0; i < NUM_OPERS; i++)
        if ((int) node->value == OPERATORS[i].id)
        {
            if (OPERATORS[i].name_to_print_asm)
                fprintf (file, "%s\n", OPERATORS[i].name_to_print_asm);
            else
                RETURN_ERROR(SYNTAX_ERR, "Assembler don't know this operator");
        }

    RETURN_ERROR(CORRECT, "");
}

Error print_func_call (const Node* node, FILE* file)
{
    Error error = {};

    error = print_args_call (node->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);
    fprintf (file, "call %s\n", node->name);

    RETURN_ERROR(CORRECT, "");
}

Error print_args_call (const Node* node, FILE* file)
{
    Error error = {};
    if (!node)
        RETURN_ERROR(CORRECT, "");

    error = print_expression (node->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = print_args_call (node->right, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    RETURN_ERROR(CORRECT, "");
}

Error print_if (const Node* node, IfWhileId* if_while_id, bool* is_print, FILE* file)
{
    if ((int) node->value != IF)
    {
        *is_print = false;
        RETURN_ERROR(CORRECT, "");
    }

    *is_print = true;
    Error error = {};

    error = print_expression (node->left->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = print_expression (node->left->right, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    for (size_t i = 0; i < NUM_OPERS; i++)
        if ((int) node->left->value == OPERATORS[i].id)
            fprintf (file, "%s ", OPERATORS[i].name_to_print_asm);

    size_t if_id = if_while_id->num_if;
    if_while_id->num_if++;
    fprintf (file, "if%lld\n", if_id);

    error = print_commands (node->right, if_while_id, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    fprintf (file, ":if%lld\n", if_id);
    RETURN_ERROR(CORRECT, "");
}

Error print_while (const Node* node, IfWhileId* if_while_id, bool* is_print, FILE* file)
{
    if ((int) node->value != WHILE)
    {
        *is_print = false;
        RETURN_ERROR(CORRECT, "");
    }

    *is_print = true;
    Error error = {};
    size_t while_id = if_while_id->num_while;
    if_while_id->num_while++;

    fprintf (file, ":while%lld\n", while_id);

    error = print_expression (node->left->left, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = print_expression (node->left->right, file);
    PARSE_ERROR_WITHOUT_TREE(error);

    for (size_t i = 0; i < NUM_OPERS; i++)
        if ((int) node->left->value == OPERATORS[i].id)
            fprintf (file, "%s ", OPERATORS[i].name_to_print_asm);

    fprintf (file, "end_while%lld\n", while_id);

    error = print_commands (node->right, if_while_id, file);
    PARSE_ERROR_WITHOUT_TREE(error);
    fprintf (file, "jmp while%lld\n", while_id);

    fprintf (file, ":end_while%lld\n", while_id);
    RETURN_ERROR(CORRECT, "");
}
