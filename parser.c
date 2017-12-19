#pragma once
#include <malloc.h>
/*
  program                  = decl_list
  decl_list                = function | type_declaration | function decl_list | type_declaration decl_list
  function                 = identifier ( parameter_list ) : ( type_list ) -> type block
                             identifier ( parameter_list ) : ( type_list ) -> ( type_list ) block
  type_declaration         = def identifier { type_specifier_list }
  type_specifier_list      = type_specifier | type_specifier type_specifier_list
  type_specifier           = identifier : type ; | identifier : type -> expression
  parameter_list           = parameter | parameter , parameter_list
  parameter_name           = identifier
  literal                  = int_literal | real_literal | complex_literal | string_literal | true | false
  type_list                = type | type , type_list
  type                     = r32 | r64 | i8 | i16 | i32 | i64 | u8 | u16 | u32 | u64 | string | bool | complex | identifier
  block                    = statement . | { semicolon_statement_list statement . } | { statement . }
  semicolon_statement_list = statement ; | statement ; semicolon_statement_list
  statement                = block | conditional | declaration | expr
  conditional              = when expr -> block | when expr -> block else block
  declaration              = let identifier : type  -> block
  expr                     = ( expr ) | expr1
  expr1                    = expr operator1 expr | expr2
  operator1                = <vertical bar> | ^ | &
  expr2                    = expr operator2 expr | expr3
  operator2                = < | > | <= | >= | = | !=
  expr3                    = expr operator3 expr | expr4
  operator3                = << | >>
  expr4                    = expr operator4 expr | expr5
  operator4                = + | -
  expr5                    = expr operator5 expr | expr6
  operator5                = * | / | %
  expr6                    = unary_operator expr | expr7
  unary_operator           = ~ | ! | ( type ) | * | &
  expr7                    = expr ( argument_list ) | expr [ argument_list ] | expr . identifier | identifier | literal
  argument_list            = <epsilon> | expr | expr argument_list
 */

typedef enum
{
    terminal,
    program,
    decl_list,
    function,
    type_declaration,
    type_specifier_list,
    type_specifier,
    parameter_list,
    parameter_name,
    literal,
    type_list,
    type,
    block,
    semicolon_statement_list,
    statement,
    conditional,
    declaration,
    expr,
    expr1,
    operator1,
    expr2,
    operator2,
    expr3,
    operator3,
    expr4,
    operator4,
    expr5,
    operator5,
    expr6,
    unary_operator,
    expr7,
    argument_list,
    nonterminal_count,
} nonterminals;

typedef struct
{
    token *curr;
} parser;

typedef struct ast_node
{
    nonterminals type;
    token terminal;
    struct ast_node *productions[11];
} ast_node;

typedef struct ast_node_stack
{
    struct ast_node_stack *prev;
    ast_node *curr;
} ast_node_stack;

void push(ast_node_stack **stack, ast_node *element)
{
    ast_node_stack *new_node = malloc(sizeof(ast_node_stack));
    new_node->prev = *stack;
    new_node->curr = element;
    *stack = new_node;
}

ast_node *pop(ast_node_stack **stack)
{
    ast_node *popped = (*stack)->curr;
    ast_node_stack *to_free = *stack;
    *stack = (*stack)->prev;
    free(to_free);
    return(popped);
}

ast_node *parse(token *tokens)
{
    parser parse;
    parse.curr = tokens;
    
}
