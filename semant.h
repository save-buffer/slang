A#pragma once
#include <stdio.h>
#include <string.h>
#if defined(__MACH__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

typedef struct trie
{
    struct trie *next[63];
    ast_node *data;
} trie;

typedef struct scope
{
    struct scope *prev;
    trie *ids;
} scope;

typedef struct
{
    scope *curr_scope;
} semant;

int trie_letter_to_index(char l)
{
    //48-57  0-9
    //65-90  A-Z
    //95     _
    //97-122 a-z
    if('0' <= l && l <= '9')
	return(l - '0');
    if('A' <= l && l <= 'Z')
	return(l - 'A' + 10);
    if(l == '_')
	return(37);
    if('a' <= l && l <= 'z')
	return(l - 'a' + 38);
    return(-1);  
}

trie *add_to_trie(trie *t, char *id, ast_node *node)
{
    if(*id == 0)
    {
	t->data = node;
	return(t);
    }
    
    int idx = trie_letter_to_index(*id);
    //TODO(sasha): Output an error? That would require passing a semant pointer around.
    if(idx == -1)
	return(0);

    //TODO(sasha): Make sure current data is freed? Might not be necessary if we write the rest of
    //             the algorithm correctly.
    if(t->next[idx] == 0)
    {
	t->next[idx] = malloc(sizeof(trie));
	memset(t->next[idx], 0, sizeof(trie));
    }
    
    return(add_to_trie(t->next[idx], id + 1, node));
}

ast_node *search_trie(trie *t, char *id)
{
    if(*id == 0)
	return(t->data);
        
    int idx = trie_letter_to_index(*id);
    if(idx == -1)
	return(0);
    
    if(t->next[idx] == 0)
	return(0);
    
    return(search_trie(t->next[idx], id + 1));
}

int trie_contains(trie *t, char *id)
{
    return(search_trie(t, id) != 0);
}

trie *copy_trie(trie *to_copy)
{
    if(to_copy == 0)
	return(0);
    
    trie *result = malloc(sizeof(trie));
    result->data = to_copy->data;
    for(int i = 0; i < sizeof(result->next) / sizeof(result->next[0]); i++)
	result->next[i] = copy_trie(to_copy->next[i]);
    return(result);
}

void free_trie(trie *to_free)
{
    if(to_free == 0)
	return;
    
    for(int i = 0; i < sizeof(to_free->next) / sizeof(to_free->next[0]); i++)
	free_trie(to_free->next[i]);
    free(to_free);    
}

scope *push_scope(semant *s)
{
    scope *new_scope = (scope *)malloc(sizeof(scope));
    new_scope->ids = copy_trie(s->curr_scope->ids);
    new_scope->prev = s->curr_scope;
    s->curr_scope = new_scope;
    return(new_scope);
}

void pop_scope(semant *s)
{
    scope *old_scope = s->curr_scope->prev;
    free_trie(s->curr_scope->ids);
    free(s->curr_scope);
    s->curr_scope = old_scope;
}

void semant_error(ast_node *ast, const char *error_string)
{
    printf("Semantic error in %s:%i:%i: %s\n", ast->file, ast->line, ast->column, error_string);
}
