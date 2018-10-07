#include "semant.h"

// TODO(sasha): Switch to global type table that maps int -> string
//              so we can just pass ints around instead of doing a string
//              compare every time we type check something.

void add_basic_types(trie *t)
{
    ast_node *dummy_node = malloc(sizeof(ast_node));
    dummy_node->type = basic_type;
    dummy_node->is_typename = 1;
    add_to_trie(t, "r32", dummy_node);
    add_to_trie(t, "r64", dummy_node);
    add_to_trie(t, "i8", dummy_node);
    add_to_trie(t, "i16", dummy_node);
    add_to_trie(t, "i32", dummy_node);
    add_to_trie(t, "i64", dummy_node);
    add_to_trie(t, "u8", dummy_node);
    add_to_trie(t, "u16", dummy_node);
    add_to_trie(t, "u32", dummy_node);
    add_to_trie(t, "u64", dummy_node);
    add_to_trie(t, "string", dummy_node);
    add_to_trie(t, "bool", dummy_node);
    add_to_trie(t, "complex", dummy_node);
    add_to_trie(t, "atom", dummy_node);
}

void add_functions_and_types(scope *global_scope, ast_node *ast)
{
    char *id = 0;
    
    if(ast->production[0]->type == type_declaration)
        id = ast->production[0]->production[1]->terminal->value;
    else if(ast->production[0]->type == function)
        id = (char *)(ast->production[0]->production[0]->production[0]->terminal->value);

    ast_node *existing_node = 0;
    if((existing_node = search_trie(global_scope->ids, id)))
    {
        char buff[512];
        snprintf(buff,
		 sizeof(buff),
		 "Identifier %s already exists. Previous definition at %s:%i:%i",
		 id,
		 existing_node->file,
		 existing_node->line,
		 existing_node->column);
	return;
    }
        
    add_to_trie(global_scope->ids,
		id,
                ast->production[0]);
    
    if(ast->production[1]->type != eof)
        add_functions_and_types(global_scope, ast->production[1]);
}

int add_params(semant *s, ast_node *param_list)
{
    if(!param_list)
        return(0);
    param_list->production[0]->is_typename = 0;
    add_to_trie(s->curr_scope->ids, (char *)param_list->production[0]->terminal->value, param_list->production[0]);
    return(1 + add_params(s, param_list->production[2]));
}

int add_param_types(semant *s, ast_node *type_list)
{
    if(!type_list)
        return(0);
    type_list->production[0]->is_typename = 1;
    add_to_trie(s->curr_scope->ids, (char *)type_list->production[0]->terminal->value, type_list->production[0]);
    return(1 + add_params(s, type_list->production[2]));
}

void add_return_types(semant *s, ast_node *fun, ast_node *return_list)
{
    if(!return_list)
        return;
    return_list->production[0]->is_typename = 1;
    fun->return_types[fun->num_return_types++] = (char *)return_list->production[0]->terminal->value;

    add_return_types(s, fun, return_list->production[2]);
}

void typecheck_function(semant *s, ast_node *ast)
{
    push_scope(s);
    ast_node *fun = ast->production[0]->production[0];
    int num_param_ids = add_params(s, fun->production[2]);
    int num_param_types = add_param_types(s, fun->production[6]);
    
    if(num_param_ids != num_param_types)
	semant_error(fun->production[2], "Number of parameters must match number of types");
    ast_node *return_list = fun->production[10];

    add_return_types(s, fun, return_list);
}

void semantic_analysis(ast_node *ast)
{
    semant sem;
    scope global_scope = {};
    trie ids = {};

    global_scope.ids = &ids;
    
    add_basic_types(global_scope.ids);
    sem.curr_scope = &global_scope;
    add_functions_and_types(&global_scope, ast->production[0]);
}
