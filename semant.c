#include "semant.h"

void add_basic_types(trie *t)
{
    ast_node *dummy_node = malloc(sizeof(ast_node));
    dummy_node->type = basic_type;
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
    if(ast->production[0]->type == type_declaration)
    {
	add_to_trie(global_scope->ids, (char *)(ast->production[0]->production[1]->terminal->value), ast->production[0]);
    }
    else
    {
	add_to_trie(global_scope->ids, (char *)(ast->production[0]->production[0]->terminal->value), ast->production[0]);
    }
    if(ast->production[1]->type != eof)
	add_functions_and_types(global_scope, ast->production[1]);
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
