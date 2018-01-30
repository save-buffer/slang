#pragma once
#include <malloc.h>


typedef struct
{
    scope *prev;
} scope;

typedef struct
{
    scope *curr_scope;
} semant;

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
