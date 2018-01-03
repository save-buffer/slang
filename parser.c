#pragma once
#include "parser.h"

ast_node *parse_type(parser *parse);
ast_node *parse_statement(parser *parse);
ast_node *parse_expr(parser *parse);
ast_node *parse_block(parser *parse);
ast_node *parse_semicolon_statement_list(parser *parse);
ast_node *parse_declaration(parser *parse);
ast_node *parse_identifier_list(parser *parse);
ast_node *parse_type_list(parser *parse);


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
	break;
    case '(':
	new->production[0] = parse_cast(parse);
	break;
    default:
	new->production[0] = parser_error(parse, "Error parsing unary operator");
	break;
    }
    return(new);
}

ast_node *parse_expr2(parser *parse)
{
    ast_node *new = make_node(expr2);
    new->production[0] = parse_expr3(parse);
    switch(peek_tok(parse)->type)
    {
    case '<':
    case '>':
    case '=':
    case token_nequal:
    case token_ge:
    case token_le:
	next_tok(parse);
	new->production[1] = parse_operator_2(parse);
	next_tok(parse);
	new->production[2] = parse_expr2(parse);
	break;
    default:
	break;
    }
    return(new);
}

ast_node *parse_operator2(parser *parse)
{
    switch(parse->curr_tok->type)
    {
    case '<':
    case '>':
    case '=':
    case token_nequal:
    case token_ge:
    case token_le:
	return(make_terminal(parse));
    default:
	return(make_node(error));
    }
}

ast_node *parse_expr1(parser *parse)
{
    ast_node *new = make_node(expr1);
    new->production[0] = parse_expr2(parse);
    switch(peek_tok(parse)->type)
    {
    case '<':
    case '>':
    case '=':
    case token_nequal:
    case token_ge:
    case token_le:
	next_tok(parse);
	new->production[1] = parse_operator_2(parse);
	next_tok(parse);
	new->production[2] = parse_expr2(parse);
	break;
    default:
	break;
    }
    return(new);
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

ast_node *parse_expr(parser *parse)
{
    ast_node *new = make_node(expr);
    new->production[0] = parse_expr1(parse);
    switch(peek_tok(parse)->type)
    {
    case '|':
    case '^':
    case '&':
	next_tok(parse);
	new->production[1] = parse_operator_1(parse);
	next_tok(parse);
	new->production[2] = parse_expr1(parse);
	break;
    default:
	break;
    }
    return(new);
}

ast_node *parse_single_or_mult_decl(parser *parse)
{
    ast_node *new = make_node(single_decl);
    int i = 0;
    if(parse->curr_tok->type == token_let)
    {
	new->production[i++] = make_terminal(parse);
	next_tok(parse);
	if(parse->curr_tok->type == '(')
	{
	    new->type = mult_decl;
	    new->production[i++] = make_terminal(parse);
	    next_tok(parse);
	    new->production[i++] = parse_identifier_list(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == ')')
	    {
		new->production[i++] = make_terminal(parse);
		next_tok(parse);
		if(parse->curr_tok->type == ':')
		{
		    new->production[i++] = make_terminal(parse);
		    next_tok(parse);
		    if(parse->curr_tok->type == '(')
		    {
			new->production[i++] = make_terminal(parse);
			next_tok(parse);
			new->production[i++] = parse_type_list(parse);
			next_tok(parse);
			if(parse->curr_tok->type == ')')
			    new->production[i++] = make_terminal(parse);
			else
			    new->production[i] = parser_error(parse, ") expected");
		    }
		    else
			new->production[i] = parser_error(parse, "( expected");
		}
		else
		    new->production[i] = parser_error(parse, ": expected");
	    }
	    else
		new->production[i] = parser_error(parse, ") expeted");
	}
	else if(parse->curr_tok->type == token_id)
	{
	    new->production[i++] = make_terminal(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == ':')
	    {
		new->production[i++] = make_terminal(parse);
		next_tok(parse);
		new->production[i++] = parse_type(parse);
	    }
	    else
		new->production[i] = parser_error(parse, ": expected");
	}
	else
	{
	    new->production[i] = parser_error(parse, "identifier expected");
	}
	if(new->production[i]->type == error)
	    return(new);
	next_tok(parse);
	if(parse->curr_tok->type == token_arrow)
	{
	    new->production[i++] = make_terminal(parse);
	    next_tok(parse);
	    new->production[i++] = parse_block(parse);
	}
	else
	    new->production[i] = parser_error(parse, "-> expected");
    }
    return(new);
}

ast_node *parse_declaration(parser *parse)
{
    ast_node *new = make_node(declaration);
    new->production[0] = parse_single_or_mult_decl(parse);
    return(new);
}

ast_node *parse_conditional(parser *parse)
{
    ast_node *new = make_node(conditional);
    int i = 0;
    if(parse->curr_tok->type == token_when)
    {
	new->production[i++] = make_terminal(parse);
	next_tok(parse);
	new->production[i++] = parse_expr(parse);
	next_tok(parse);
	if(parse->curr_tok->type == token_arrow)
	{
	    new->production[i++] = make_terminal(parse);
	    next_tok(parse);
	    new->production[i++] = parse_block(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == token_else)
	    {		
		new->production[i++] = make_terminal(parse);
		next_tok(parse);
		if(parse->curr_tok->type == token_arrow)
		{
		    new->production[i++] = make_terminal(parse);
		    next_tok(parse);
		    new->production[i++] = parse_block(parse);
		}
		else
		{
		    new->production[--i] = parser_error(parse, "-> expected");
		}
	    }
	}
	else
	{
	    new->production[--i] = parser_error(parse, "-> expected");
	}
    }
    else
    {
	new->production[--i] = parser_error(parse, "when expected in conditional");
    }
    return(new);
}

ast_node *parse_comma_statement_list(parser *parse)
{
    ast_node *new = make_node(comma_statement_list);
    int i = 0;
    new->production[i++] = parse_statement(parse);
    if(peek_tok(parse)->type == ')')
	return(new);
    next_tok(parse);
    if(parse->curr_tok->type == ',')
    {
	new->production[i++] = make_terminal(parse);
	next_tok(parse);
	new->production[i++] = parse_semicolon_statement_list(parse);
    }
    else
    {
	new->production[i] = parser_error(parse, ", expected");
    }
    return(new);
}

ast_node *parse_prn_comma_statement_list(parser *parse)
{
    ast_node *new = make_node(prn_comma_statement_list);
    int i = 0;
    if(parse->curr_tok->type == '(')
    {
	new->production[i++] = make_terminal(parse);
	next_tok(parse);
	new->production[i++] = parse_comma_statement_list(parse);
	next_tok(parse);
	if(parse->curr_tok->type == ')')
	    new->production[i++] = make_terminal(parse);
	else
	    new->production[i] = parser_error(parse, ") expected");
    }
    return(new);
}

ast_node *parse_statement(parser *parse)
{
    ast_node *new = make_node(statement);
    switch(parse->curr_tok->type)
    {
    case token_when:
	new->production[0] = parse_conditional(parse);
	break;
    case '{':
	new->production[0] = parse_block(parse);
	break;
    case token_let:
	new->production[0] = parse_declaration(parse);
	break;
    case '(':
	new->production[0] = parse_prn_comma_statement_list(parse);
	break;
    default:
	new->production[0] = parse_expr(parse);
	break;
    }
    return(new);
}

ast_node *parse_semicolon_statement_list(parser *parse)
{
    ast_node *new = make_node(semicolon_statement_list);
    new->production[0] = parse_statement(parse);
    if(peek_tok(parse)->type == '.')
	return(new);
    next_tok(parse);
    if(parse->curr_tok->type == ';')
    {
	new->production[1] = make_terminal(parse);
	next_tok(parse);
	new->production[2] = parse_semicolon_statement_list(parse);
    }
    else
    {
	new->production[1] = parser_error(parse, "; expected");
    }
    return(new);
}

ast_node *parse_block(parser *parse)
{
    ast_node *new = make_node(block);
    int i = 0;
    if(parse->curr_tok->type == '{')
    {
	new->production[i++] = make_terminal(parse);
	next_tok(parse);
	new->production[i++] = parse_semicolon_statement_list(parse);
	next_tok(parse);
	if(parse->curr_tok->type == '.')
	{
	    new->production[i++] = make_terminal(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == '}')
	    {
		new->production[i++] = make_terminal(parse);
	    }
	    else
	    {
		new->production[i] = parser_error(parse, "} expected");
	    }
	}
	else
	{
	    new->production[i] = parser_error(parse, ". expected");
	}
    }
    else
    {
	new->production[i++] = parse_statement(parse);
	next_tok(parse);
	if(parse->curr_tok->type == '.')
	{
	    new->production[i++] = make_terminal(parse);
	}
	else
	{
	    new->production[i] = parser_error(parse, ". expected");
	}
    }
    return(new);
}

ast_node *parse_type_list(parser *parse)
{
    ast_node *new = make_node(type_list);
    new->production[0] = parse_type(parse);
    if(peek_tok(parse)->type == ')')
	return(new);
    next_tok(parse);
    if(parse->curr_tok->type == ',')
    {
	new->production[1] = make_terminal(parse);
	next_tok(parse);
	new->production[2] = parse_type_list(parse);
    }
    else
    {
	new->production[1] = parser_error(parse, ", expected");
    }
    return(new);
}

ast_node *parse_identifier_list(parser *parse)
{
    ast_node *new = make_node(identifier_list);
    if(parse->curr_tok->type == token_id)
    {
	new->production[0] = make_terminal(parse);
	if(peek_tok(parse)->type == ')')
	    return(new);
	next_tok(parse);
	if(parse->curr_tok->type == ',')
	{
	    new->production[1] = make_terminal(parse);
	    next_tok(parse);
	    new->production[2] = parse_identifier_list(parse);
	}
	else
	{
	    new->production[1] = parser_error(parse, ", expected");
	}
    }
    else
    {
	new->production[0] = parser_error(parse, "identifier expected");
    }
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

ast_node *parse_single_or_mult_return_function(parser *parse)
{
    ast_node *new = make_node(single_return_function);
    int i = 0;
    if(parse->curr_tok->type == token_id)
    {
	new->production[i++] = make_terminal(parse);
	next_tok(parse);
	if(parse->curr_tok->type == '(')
	{
	    new->production[i++] = make_terminal(parse);
	    next_tok(parse);
	    new->production[i++] = parse_identifier_list(parse);
	    next_tok(parse);
	    if(parse->curr_tok->type == ')')
	    {
		new->production[i++] = make_terminal(parse);
		next_tok(parse);
		if(parse->curr_tok->type == ':')
		{
		    new->production[i++] = make_terminal(parse);
		    next_tok(parse);
		    if(parse->curr_tok->type == '(')
		    {
			new->production[i++] = make_terminal(parse);
			next_tok(parse);
			new->production[i++] = parse_type_list(parse);
			next_tok(parse);
			if(parse->curr_tok->type == ')')
			{
			    new->production[i++] = make_terminal(parse);
			    next_tok(parse);
			    if(parse->curr_tok->type == token_arrow)
			    {
				new->production[i++] = make_terminal(parse);
				next_tok(parse);
				if(parse->curr_tok->type == '(')
				{
				    new->type = mult_return_function;
				    new->production[i++] = make_terminal(parse);
				    next_tok(parse);
				    new->production[i++] = parse_type_list(parse);
				    next_tok(parse);
				    if(parse->curr_tok->type == ')')
				    {
					new->production[i++] = make_terminal(parse);
				    }
				    else
				    {
					new->production[i++] = parser_error(parse, ") expected");
				    }
				}
				else
				{
				    new->production[i++] = parse_type(parse);
				}
				next_tok(parse);
				new->production[i++] = parse_block(parse);
			    }
			    else
			    {
				new->production[i] = parser_error(parse, "-> expected");
			    }
			}
			else
			{
			    new->production[i] = parser_error(parse, ") expected");
			}
		    }
		    else
		    {
			new->production[i] = parser_error(parse, "( expected");
		    }
		}
		else
		{
		    new->production[i] = parser_error(parse, ": expected");
		}
	    }
	    else
	    {
		new->production[i] = parser_error(parse, ") expected");
	    }
	}
	else
	{
	    new->production[i] = parser_error(parse, "( expected");
	}
    }
    else
    {
	new->production[i] = parser_error(parse, "Identifier expected in function declaration");
    }
    return(new);
}

ast_node *parse_function(parser *parse)
{
    ast_node *new = make_node(function);
    new->production[0] = parse_single_or_mult_return_function(parse);
    next_tok(parse);
    return(new);
}

ast_node *parse_type_specifier(parser *parse)
{
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
		    new->production[6] = parser_error(parse, "; expected");
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
    consume_comments(parse);
    ast_node *new = make_node(decl_list);
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
    case function:
	return("function");
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
    case identifier_list:
	return("identifier_list");
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
    case comma_statement_list:
	return("comma_statement_list");
    case prn_comma_statement_list:
	return("prn_comma_statement_list");
    case statement:
	return("statement");
    case conditional:
	return("conditional");
    case declaration:
	return("declaration");
    case single_decl:
	return("single_decl");
    case mult_decl:
	return("mult_decl");
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
    case error:
    case nonterminal_count:
	//NOTE(sasha): no default because we want to handle all cases explicitly.
	//NOTE(sasha): case nonterminal_count should never happen. If it does, there's an error.
	return("error");
    }	 
}

static void render_ast_(FILE *diagram, ast_node *cur, int parent)
{
    static int count = 1;

    int this = count;
    if(parent != 0)
	fprintf(diagram, "\tnode%i -> node%i;\n", parent, this);
    fprintf(diagram, "\tnode%i [label=\"%s\"];\n", this, ast_node_to_string(cur));
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
