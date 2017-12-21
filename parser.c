#pragma once
#include "parser.h"
//TODO(sasha): make parser keep track of line number
void parser_error(parser *parse, const char *error)
{
    printf("Parser error: %s\n", error);    
}

void consume_comments(parser *parse)
{
    while(parse->curr_tok->type == token_comment)
	next_tok(parse);
}

ast_node *parse_expr(parser *parse)
{
}

ast_node *parse_type(parser *parse)
{
    switch(parse->curr_tok->type)
    {
    case token_r32:
    case token_r64:
    case token_i8:
    case token_i16:
    case token_i32:
    case token_i64:
    case token_u8:
    case token_u16:
    case token_u32:
    case token_u64:
    case token_string:
    case token_bool:
    case token_complex:
    case token_id:
	return make_terminal(parse);
    default:
	parser_error(parse, "Typename expected");
	return make_node(error);
    }
}

ast_node *parse_function(parser *parse)
{
    consume_comments(parse);
    ast_node *new = make_node(single_return_function);
    if(parse->curr_tok->type == token_id)
    {
	new->production[0] = make_terminal(parse);
	next_tok(parse);
	if(parse->curr_tok->type == '(')
	{
	    new->production[1] = make_terminal(parse);
	    next_tok(parse);
	    new->production[2] = parse_parameter_list(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == ')')
	    {
		new->production[3] = make_terminal(parse);
		next_tok(parse);
		if(parse->curr_tok->type == token_arrow)
		{
		    new->production[4] = make_terminal(parse);
		    if(parse->curr_tok->type == '(')
		    {
			new->productions[5] = make_terminal(parse);
			next_tok(parse);
			new->productions[6] = parse_type_list(parse);
			next_tok(parse);
		    }
		}
	    }
	}
    }
}

ast_node *parse_type_specifier(parser *parse)
{
    consume_comments(parse);
    ast_node *new = make_node(type_specifier);
    if(parse->curr_tok->type == token_id)
    {
	new->production[0] = make_terminal(parse);
	next_tok(parse);
	if(parse->curr_tok->type == ':')
	{
	    new->production[1] = make_terminal(parse);
	    next_tok(parse);
	    new->production[2] = parse_type(parse);
	    next_tok(parse);
	    switch(parse->curr_tok->type)
	    {
	    case ';':
		new->production[3] = make_terminal(parse);
		return(new);
	    case token_arrow:
		new->production[4] = make_terminal(parse);
		next_tok(parse);
		new->production[5] = parse_expr(parse);
		next_tok(parse);
		if(parse->curr_tok->type == ';')
		    new->production[6] = make_terminal(parse);
		else
		{
		    new->production[6] = make_node(error);
		    parser_error(parse, "; expected");
		}
	    }
	}
    }
}

ast_node *parse_type_specifier_list(parser *parse)
{
    if(peek_tok(parse)->type == '}')
	return(0);
    ast_node *new = make_node(type_specifier_list);
    new->production[0] = parse_type_specifier(parse);
    next_tok(parse);
    new->production[1] = parse_type_specifier_list(parse);
    return(new);
}

ast_node *parse_type_declaration(parser *parse)
{
    consume_comments(parse);
    ast_node *new = make_node(type_declaration);
    if(parse->curr_tok->type == token_def)
    {
	new->production[0] = make_terminal(parse);
	next_tok(parse);
	if(parse->curr_tok->type == token_id)
	{
	    new->production[1] = make_terminal(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == '{')
	    {
		new->production[2] = make_terminal(parse);
		next_tok(parse);
		new->production[3] = parse_type_specifier_list(parse);
		next_tok(parse);
		if(parse->curr_tok->type == '}')
		{
		    new->production[4] = make_terminal(parse);
		    next_tok(parse);
		}
		else
		{
		    parser_error(parse, "} expected");
		    new->production[4] = make_node(error);
		}
	    }
	    else
	    {
		parser_error(parse, "{ expected");
		new->production[2] = make_node(error);
	    }
	}
	else
	{
	    parser_error(parse, "identifier expected");
	    new->production[1] = make_node(error);
	}
    }
    else
    {
	new->production[0] = make_node(error);
    }
    return(new);
}


ast_node *parse_decl_list(parser *parse)
{
    ast_node *new = make_node(decl_list);
    consume_comments(parse);
    switch(parse->curr_tok->type)
    {
    case token_eof:
	free(new);
	return make_node(eof);
    case token_def:
	new->production[0] = parse_type_declaration(parse);
	break;
    case token_id:
	new->production[0] = parse_function(parse);
	break;
    default:
	parser_error(parse, "Function or type declaration expected");
	free(new);
	return make_node(error);
    }
    new->production[1] = parse_decl_list(parse);
    return(new);
}

ast_node *parse_program(parser *parse)
{
    ast_node *new = make_node(program);
    new->production[0] = parse_decl_list(parse);
    return(new);
}

ast_node *parse(token *tokens)
{
    parser parse;
    parse.curr = tokens;
    next_tok(&parse);
    return(parse_program(&parse));
}
