#include "tree.h"
#include "operators.h"

Error new_node (Types type, double value, char* name, Node** adres)
{
    if (!adres)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of adress.");

    *adres = (Node*) calloc (1, sizeof (Node));

    if (!(*adres))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory of node.");

    Error error = node_ctor (type, value, name, *adres);
    return error;
}

Error node_ctor (Types type, double value, char* name, Node* node)
{
    node->left  = NULL;
    node->right = NULL;
    node->type  = type;
    node->value = value;
    if (name)
    {
        node->name  = strdup (name);
        if (!node->name)
            RETURN_ERROR(MEM_ALLOC, "Error of allocation memory for name of var");
    }
    RETURN_ERROR(CORRECT, "");
}

Error tree_ctor (Tree* tree, const char* name, const char* file, const char* func, int line)
{
    if (!tree)
        RETURN_ERROR(NULL_POINTER, "Null pointer of tree.");

    if (!name || !file || !func || line < 0)
        RETURN_ERROR(INCOR_PARAMS, "Incorrect parameters in constructor");

    tree->size = 0;
    tree->root = NULL;
    tree->name = name;
    tree->file = file;
    tree->func = func;
    tree->line = line;

    RETURN_ERROR(CORRECT, "");
}

Error tree_dtor (Tree* tree)
{
    if (!tree)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of tree.");

    nodes_dtor (tree->root);
    tree->size = 0;
    tree->root = NULL;
    RETURN_ERROR(CORRECT, "");
}

void nodes_dtor (Node* node)
{
    if (!node)
        return;
    if (node->left)
        nodes_dtor (node->left);
    if (node->right)
        nodes_dtor (node->right);
    if (node->name)
    {
        free (node->name);
        node->name = NULL;
    }
    node->value = 0;
    free (node);
    node = NULL;
}

Error tree_verify (const Tree* tree)
{
    if (!tree)                                      RETURN_ERROR(NULL_POINTER,      "Null pointer of tree.");
    if (!tree->file || !tree->func || !tree->name)  RETURN_ERROR(INCOR_PARAMS,      "Null pointer of parameters of tree.");
    if (is_cycles (tree->root))                     RETURN_ERROR(CYCLES,            "There are cycles in tree.");
    RETURN_ERROR(CORRECT, "");
}

Error nodes_print (const Node* node, FILE* file)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    if (!node)
        RETURN_ERROR(CORRECT, "");

    char str[MAX_SIZE] = "";
    val_to_str (node, str);
    fprintf (file, "%s (", str);

    nodes_print (node->left, file);
    nodes_print (node->right, file);

    fprintf (file, ") ");

    RETURN_ERROR(CORRECT, "");
}

void val_to_str (const Node* node, char* str)
{
    if (node->type == NUM)
    {
        sprintf (str, "%.3lf", node->value);
        return;
    }

    if (node->type == VAR || node->type == FUNC)
    {
        sprintf (str, "%s", node->name);
        return;
    }

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        Operator oper = OPERATORS[i];
        if (node->type == oper.type && (int) node->value == oper.id)
        {
            strcpy (str, oper.name);
            return;
        }
    }
}

bool found_var (Elements* elems, char* name)
{
    for (size_t i = 0; i < elems->num_elems; i++)
        if (strcmp (name, elems->elems[i].name) == 0 && elems->elems[i].type == VAR)
            return true;

    return false;
}

bool found_func (Elements* elems, char* name)
{
    for (size_t i = 0; i < elems->num_elems; i++)
        if (strcmp (name, elems->elems[i].name) == 0 && elems->elems[i].type == FUNC)
            return true;

    return false;
}

Error append_var (Elements* elems, char* name, double value)
{
    if (elems->num_elems == MAX_NUM_ELEMS)
        RETURN_ERROR(VARS_OVERFLOW, "Number of variables is more than max number of variables");

    elems->elems[elems->num_elems].type = VAR;
    elems->elems[elems->num_elems].value = value;
    elems->elems[elems->num_elems].id = (int) elems->num_vars;
    elems->elems[elems->num_elems].name = strdup (name);
    if (!(elems->elems[elems->num_elems].name))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory for name of var");

    elems->num_vars++;
    elems->num_elems++;
    RETURN_ERROR(CORRECT, "");
}

Error append_func (Elements* elems, char* name)
{
    if (elems->num_elems == MAX_NUM_ELEMS)
        RETURN_ERROR(VARS_OVERFLOW, "Number of variables is more than max number of variables");

    elems->elems[elems->num_elems].type = FUNC;
    elems->elems[elems->num_elems].value = FUNC_DEF_VAL;
    elems->elems[elems->num_elems].id = (int) elems->num_funcs;
    elems->elems[elems->num_elems].name = strdup (name);
    if (!(elems->elems[elems->num_elems].name))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory for name of var");

    elems->num_funcs++;
    elems->num_elems++;
    RETURN_ERROR(CORRECT, "");
}

Error read_file (FILE* file, ReadStr* str)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    str->size = fread (str->str, sizeof (char), MAX_STR_SIZE, file);
    str->pos = 0;
    str->str[str->size - 1] = '\0';
    RETURN_ERROR (CORRECT, "");
}

bool is_cycles (Node* node)
{
    Node* points[MAX_SIZE] = {};
    get_points (node, points, 0);

    qsort (points, MAX_SIZE, sizeof (Node*), comparator);
    for (int i = 1; points[i]; i++)
        if (points[i - 1] == points[i])
            return true;

    return false;
}

void get_points (Node* node, Node* points[], size_t pos)
{
    if (!node)
        return;

    points[pos] = node;
    get_points (node->left, points, pos * 2 + 1);
    get_points (node->right, points, pos * 2 + 2);
}

int comparator (const void* p1, const void* p2)
{
    if ((const Node*) p1 > (const Node*) p2)
        return 1;
    else if ((const Node*) p1 < (const Node*) p2)
        return -1;
    return 0;
}

bool is_zero (double x)
{
    return (abs (x) < EPS);
}

Error elems_ctor (Elements* elems)
{
    if (!elems)
        RETURN_ERROR(NULL_POINTER, "Null pointer of struct elems");

    elems->elems = (Element*) calloc (MAX_NUM_ELEMS, sizeof (Element));

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        Operator oper = OPERATORS[i];
        elems->elems[i].type  = oper.type;
        elems->elems[i].id    = oper.id;
        elems->elems[i].name  = oper.name;
        elems->elems[i].value = OPER_DEF_VAL;
    }
    elems->num_elems = NUM_OPERS;
    elems->num_vars  = 0;

    RETURN_ERROR(CORRECT, "");
}

Node* create_node (Types type, double value, char* name, Node* left, Node* right)
{
    Node* node = NULL;
    new_node (type, value, name, &node);
    node->left = left;
    node->right = right;
    return node;
}

void tokens_dtor (Tokens* tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        if (tokens->tokens[i])
        {
            if (tokens->tokens[i]->name)
            {
                free (tokens->tokens[i]->name);
                tokens->tokens[i]->name = NULL;
            }
            free (tokens->tokens[i]);
        }
    }
}

void print_funcs (const Functions* funcs, const Function* main, FILE* file)
{
    for (size_t i = 0; i < funcs->num_funcs; i++)
        print_func (funcs->funcs[i].root, &(funcs->funcs[i].elems), file);

    print_func (main->root, &(main->elems), file);
}

void print_func (const Node* node, const Elements* elems, FILE* file)
{
    if (!node)
        return;

    char text[MAX_TEXT_SIZE] = "";
    val_to_str_print (text, node, elems);
    //printf ("type %d val %lg name %s text %s\n", node->type, node->value, node->name, text);

    fprintf (file, "%s ( ", text);
    print_func (node->left, elems, file);
    fprintf (file, ") ");

    fprintf (file, "( ");
    print_func (node->right, elems, file);
    fprintf (file, ") ");

}

void val_to_str_print (char* text, const Node* node, const Elements* elems)
{
    if (node->type == NUM)
    {
        sprintf (text, "%lg", node->value);
        return;
    }

    size_t num_in_elems = 0;
    for (size_t i = 0; i < elems->num_elems; i++)
    {
        if (node->type == elems->elems[i].type)
        {
            //printf ("type %d id %d name %s\n", elems->elems[i].type, elems->elems[i].id, elems->elems[i].name);
            if (node->type == OPER)
            {
                if ((int) node->value == elems->elems[i].id)
                {
                    num_in_elems = i;
                    break;
                }
            }
            else
            {
                if (strcmp (node->name, elems->elems[i].name) == 0)
                {
                    //printf ("type %d name %s var%d %s\n", node->type, node->name, elems->elems[i].id, elems->elems[i].name);
                    num_in_elems = i;
                    break;
                }
            }
        }
    }

    if (node->type == OPER)
        sprintf (text, "%s", OPERATORS[num_in_elems].name_to_print);
    else if (node->type == VAR)
        sprintf (text, "var%d", elems->elems[num_in_elems].id);
    else if (node->type == FUNC)
        sprintf (text, "func%d", elems->elems[num_in_elems].id);
}