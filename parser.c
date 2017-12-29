#pragma once
#include "parser.h"

ast_node *parse_type(parser *parse);

ast_node *parse_type_list(parser *parse)
{
    return(NULL);
}

ast_node *parse_parameter_list(parser *parse)
{
    return(NULL);
}

ast_node *parse_cast(parser *parse)
{
    ast_node *new = make_node(cast);
    if(parse->curr_tok->type == '(')
    {
	new->production[0] = make_terminal(parse);
	next_tok(parse);
	new->production[1] = parse_type(parse);
	next_tok(parse);
	if(parse->curr_tok->type == ')')
	    new->production[2] = make_terminal(parse);
	else
	    new->production[2] = make_node(error);
    }
    else
	new->type = error;
    return(new);
}

ast_node *parse_unary_operator(parser *parse)
{
    ast_node *new = make_node(unary_operator);
    switch(parse->curr_tok->type)
    {
    case '~':
    case '!':
    case '*':
    case '&':
	new->production[0] = make_terminal(parse);
	return(new);
    default:
	free(new);
	return(parser_error(parse, "Error parsing unary operator"));
    }
}

ast_node *parse_operator1(parser *parse)
{
    switch(parse->curr_tok->type)
    {
    case '|':
    case '^':
    case '&':
	return(make_terminal(parse));
    default:
	return(make_node(error));
    }
}

ast_node *parse_expr1(parser *parse)
{
    return(NULL);
}

ast_node *parse_expr(parser *parse)
{
    ast_node *new = make_node(expr);
    if(parse->curr_tok->type == '(')
    {
	new->production[0] = make_terminal(parse);
	next_tok(parse);
	new->production[1] = parse_expr(parse);
	next_tok(parse);
	if(parse->curr_tok->type == ')')
	    new->production[2] = make_terminal(parse);
	else
	    new->production[2] = make_node(error);
	
	return(new);
    }
    new->production[0] = parse_expr1(parse);
    return(new);
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
	return(make_terminal(parse));
    default:
	return(parser_error(parse, "Typename expected"));
    }
}

ast_node *parse_function(parser *parse)
{
    //TODO(sasha): Make this handle invalid tokens
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
			new->production[5] = make_terminal(parse);
			next_tok(parse);
			new->production[6] = parse_type_list(parse);
			next_tok(parse);
		    }
		    else
		    {
			new->production[5] = make_node(error);
		    }
		}
		else
		{
		    new->production[4] = make_node(error);
		}
	    }
	    else
	    {
		new->production[3] = make_node(error);
	    }
	}
	else
	{
	    new->production[2] = make_node(error);
	}
    }
    else
    {
	new->production[0] = make_node(error);
    }
    return(new);
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
		    new->production[6] = parser_error(parse, "; expected");

		}
		return(new);
	    default:
		new->production[3] = parser_error(parse, "Expression or ; expected");
		return(new);
	    }
	}
	else
	{
	    new->production[1] = parser_error(parse, ": expected");
	}
    }
    else
    {
	new->production[0] = parser_error(parse, "Identifier expected");	
    }
    return(new);
}

ast_node *parse_type_specifier_list(parser *parse)
{
    ast_node *new = make_node(type_specifier_list);
    new->production[0] = parse_type_specifier(parse);
    if(peek_tok(parse)->type == '}')
	return(new);
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
		    new->production[4] = parser_error(parse, "} expected");
		}
	    }
	    else
	    {
		new->production[2] = parser_error(parse, "{ expected");
	    }
	}
	else
	{

	    new->production[1] = parser_error(parse, "identifier expected");
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
	return(make_node(eof));
    case token_def:
	new->production[0] = parse_type_declaration(parse);
	break;
    case token_id:
	new->production[0] = parse_function(parse);
	break;
    default:
	free(new);
	return(parser_error(parse, "Function or type declaration expected"));
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
    parse.curr_tok = tokens;
    parse.curr = tokens + 1;
    consume_comments(&parse);
    return(parse_program(&parse));
}

const char *ast_node_to_string(ast_node *node)
{
    if(node == NULL)
	return("not implemented");
    switch(node->type)
    {
    case terminal:
	return(token_to_string(node->terminal));
    case program:
	return("program");
    case decl_list:
	return("decl_list");
    case single_return_function:
	return("single_return_function");
    case mult_return_function:
	return("mult_return_function");
    case type_declaration:
	return("type_declaration");
    case type_specifier:
	return("type_specifier");
    case type_specifier_list:
	return("type_specifier_list");
    case parameter_list:
	return("parameter_list");
    case parameter_name:
	return("parameter_name");
    case literal:
	return("literal");
    case type_list:
	return("type_list");
    case type:
	return("type");
    case block:
	return("block");
    case semicolon_statement_list:
	return("semicolon_statement_list");
    case statement:
	return("statement");
    case conditional:
	return("conditional");
    case declaration:
	return("declaration");
    case expr:
	return("expr");
    case expr1:
	return("expr1");
    case expr2:
	return("expr2");
    case expr3:
	return("expr3");
    case expr4:
	return("expr4");
    case expr5:
	return("expr5");
    case expr6:
	return("expr6");
    case expr7:
	return("expr7");
    case operator1:
	return("operator1");
    case operator2:
	return("operator2");
    case operator3:
	return("operator3");
    case operator4:
	return("operator4");
    case operator5:
	return("operator5");
    case unary_operator:
	return("unary_operator");
    case cast:
	return("cast");
    case argument_list:
	return("argument_list");
    case eof:
	return("eof");
    default:
	return("error");
    }	 
}

static void render_ast_(FILE *diagram, ast_node *cur, int parent)
{
    static int count = 1;

    int this = count;
    if(parent != 0)
	fprintf(diagram, "node%i -> node%i;\n", parent, this);
    fprintf(diagram, "node%i [label=\"%s\"];\n", this, ast_node_to_string(cur));
    count++;
    for(int i = 0; cur->production[i] != 0; i++)
    {
	render_ast_(diagram, cur->production[i], this);
    }
}

void render_ast(const char *path, ast_node *root)
{
    FILE *diagram = fopen(path, "w");
    fprintf(diagram, "digraph ast\n{\n");
    render_ast_(diagram, root, 0);
    fprintf(diagram, "}\n");
    fclose(diagram);
}
