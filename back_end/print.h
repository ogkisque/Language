#ifndef PRINT_HEADER
#define PRINT_HEADER

#include <stdio.h>

#include "tree.h"
#include "error.h"

Error print_trees                   (const Functions* funcs, FILE* file);
Error print_tree                    (const Function* func, IfWhileId* if_while_id, FILE* file);
Error print_args                    (const Node* node, FILE* file);
Error print_commands                (const Node* node, IfWhileId* if_while_id, FILE* file);
Error print_input_output_return     (const Node* node, bool* is_print, FILE* file);
Error print_assign                  (const Node* node, bool* is_print, FILE* file);
Error print_expression              (const Node* node, FILE* file);
Error print_func_call               (const Node* node, FILE* file);
Error print_args_call               (const Node* node, FILE* file);
Error print_if                      (const Node* node, IfWhileId* if_while_id, bool* is_print, FILE* file);
Error print_while                   (const Node* node, IfWhileId* if_while_id, bool* is_print, FILE* file);

#endif //PRINT_HEADER
