#pragma once
#include <malloc.h>

typedef struct trie
{
    struct trie *next[63];
    ast_node *data;
} trie;

typedef struct
{
    scope *prev;
    trie *functions;
    trie *variables;
    trie *types;
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
    if(l <= 'a' && l <= 'z')
	return(l - 'a' + 38);
    return(-1);  
}

void add_to_trie(trie *t, char *id, ast_node *node)
{
    if(*id == 0)
    {
	t->data = node;
	return;
    }
    
    int idx = trie_letter-to_index(*id);
    //TODO(sasha): Output an error? That would require passing a semant pointer around.
    if(idx == -1)
	return;

    //TODO(sasha): Make sure current data is freed? Might not be necessary if we write the rest of
    //             the algorithm correctly.
    if(t->next[idx] == 0)
	t->next[idx] = malloc(sizeof(trie));
    
    add_to_trie(t->next[idx], id + 1, node);
}

int trie_contains(trie *t, char *id)
{
    if(*id == 0)
	return(0);
    
    int idx = trie_letter_to_index(*id);
    if(idx == -1)
	return(0);
    
    if(t->next[idx] == 0)
	return(0);
    return(trie_contains(t->next[idx], id + 1));
}

scope *push_scope(semant *s)
{
    scope *new_scope = (scope *)malloc(sizeof(scope));
    new_scope->prev = s->curr_scope;
    s->curr_scope = new_scope;
    return(new_scope);
}

void pop_scope(semant *s)
{
    scope *old_scope = s->curr_scope->prev;
    free(s->curr_scope);
    s->curr_scope = old_scope;
}
