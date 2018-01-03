#pragma once
#include <stdio.h>
#include <string.h>
#if defined(__MACH__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

//-a.b + c * d
/*
  program                  = decl_list
  decl_list                = function | type_declaration | function decl_list | type_declaration decl_list
  function                 = single_return_function | multi_return_function
  single_return_function   = identifier ( identifier_list ) : ( type_list ) -> type block
  multi_return_function    = identifier ( identifier_list ) : ( type_list ) -> ( type_list ) block
  type_declaration         = def identifier { type_specifier_list }
  type_specifier_list      = type_specifier | type_specifier type_specifier_list
  type_specifier           = identifier : type ; | identifier : type -> expr ;
  identifier_list          = identifier | identifier , identifier_list
  literal                  = int_literal | real_literal | complex_literal | string_literal | true | false
  type_list                = type | type , type_list
  type                     = r32 | r64 | i8 | i16 | i32 | i64 | u8 | u16 | u32 | u64 | string | bool | complex | identifier
  block                    = statement . | { semicolon_statement_list . }
  semicolon_statement_list = statement | statement ; semicolon_statement_list
  comma_statement_list     = statement | statement , comma_statement_list
  prn_comma_statement_list = ( comma_statement_list )
  statement                = block | conditional | declaration | expr | prn_comma_statement_list
  conditional              = when expr -> block | when expr -> block else -> block
  declaration              = single_decl | mult_decl
  single_decl              = let identifier : type -> block
  mult_decl                = let ( identifier_list ) : ( type_list ) -> block
  expr                     = expr1 operator1 expr1 | expr1
  operator1                = <vertical bar> | ^ | &
  expr1                    = expr2 operator2 expr2 | expr2
  operator2                = < | > | <= | >= | = | !=
  expr2                    = expr3 operator3 expr3 | expr3
  operator3                = << | >>
  expr3                    = expr4 operator4 expr4 | expr4
  operator4                = + | -
  expr4                    = expr5 operator5 expr5 | expr5
  operator5                = * | / | %
  expr5                    = unary_operator expr6 | expr6
  unary_operator           = ~ | ! | cast | * | &
  cast                     = ( type )
  expr6                    = function_call | array_lookup | struct_lookup | identifier | literal | ( expr )
  function_call            = expr7 ( argument_list ) 
  array_lookup             = expr7 [ argument_list ]
  struct_lookup            = expr7 . identifier
  argument_list            = <epsilon> | expr | expr argument_list
 */

typedef enum
{
    terminal,
    program,
    decl_list,
    function,
    single_return_function,
    mult_return_function,
    type_declaration,
    type_specifier_list,
    type_specifier,
    identifier_list,
    literal,
    type_list,
    type,
    block,
    semicolon_statement_list,
    comma_statement_list,
    prn_comma_statement_list,
    statement,
    conditional,
    declaration,
    single_decl,
    mult_decl,
    expr,
    operator1,
    expr1,
    operator2,
    expr2,
    operator3,
    expr3,
    operator4,
    expr4,
    operator5,
    expr5,
    unary_operator,
    cast,
    expr6,
    function_call,
    array_lookup,
    struct_lookup,
    argument_list,
    error,
    eof,
    nonterminal_count,
} nonterminals;

typedef struct
{
    token *curr_tok;
    token *curr;
} parser;

typedef struct ast_node
{
    nonterminals type;
    token *terminal;
    struct ast_node *production[16];
} ast_node;

void next_tok_(parser *parse)
{
    parse->curr_tok = parse->curr;
    parse->curr++;
}

void consume_comments(parser *parse)
{
    while(parse->curr_tok->type == token_comment)
	next_tok_(parse);
}

void next_tok(parser *parse)
{
    consume_comments(parse);
    next_tok_(parse);
}

token *peek_tok(parser *parse)
{
    return(parse->curr_tok + 1);
}

void prev_token(parser *parse)
{
    parse->curr--;
    parse->curr_tok = (parse->curr - 1);
}

ast_node *make_node(nonterminals nonterminal)
{
    ast_node *result = malloc(sizeof(ast_node));
    memset(result->production, 0, sizeof(result->production) / sizeof(result->production[0]));
    result->type = nonterminal;
    return(result);
}

ast_node *make_terminal(parser *parse)
{
    ast_node *result = make_node(terminal);
    result->terminal = parse->curr_tok;
    return(result);
}

//TODO(sasha): make parser keep track of line number
ast_node *parser_error(parser *parse, const char *error_string)
{
    printf("Parser error in %s:%i:%i: %s\n", parse->curr_tok->file, parse->curr_tok->line, parse->curr_tok->column, error_string);
    return make_node(error);
}

