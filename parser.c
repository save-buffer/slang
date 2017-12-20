#pragma once
#include "parser.h"
//TODO(sasha): make parser keep track of line number
void parser_error(parser *parse, const char *error)
{
    printf("%s", error);
}

ast_node *parse_type_declaration(parser *parse)
{
    ast_node *new = make_node(type_declaration);
    if(parse->curr_tok.type == token_def)
    {
	new->productions[0] = make_terminal(parse);
	next_tok(parse);
	if(parse->curr_tok.type == tok_id)
	{
	    new->productions[1] = make_terminal(parse);
	    next_tok(parse);
	    if(parse->curr_tok == '{')
	    {
		new->productions[2] = make_terminal(parse);
		new->productions[3] = parse_type_specifier_list(parse);
	    }
	}
    }
}

ast_node *parse_decl_list(parser *parse)
{
    ast_node *new = make_node(decl_list);
    new->productions[0] = parse_function(parse);
    if(new->productions[0].type == error)
    {
	free(new->productions[0]);
	new->productions[0] = parse_type_declaration(parse);
	if(new->productions[0].type == error)
	{
	    //TODO(sasha): should errors only happen at the terminal level?
	    parser_error(parse, "Parser error!");
	}	
    }    
    new->productions[1] = parse_decl_list(parse);
    return(new);
}

ast_node *parse_program(parser *parse)
{
    ast_node *new = make_node(program);
    new->productions[0] = parse_decl_list(parse);
    return(new);
}

ast_node *parse(token *tokens)
{
    parser parse;
    parse.curr = tokens;
    next_tok(&parse);
    parse_program(&parse);
}
