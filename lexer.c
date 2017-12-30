#pragma once
#include "lexer.h"
#include <string.h>
#if defined(__MACH__)
#include <stdlib.h>
#else 
#include <malloc.h>
#endif

void next_char(lexer *lex)
{
    lex->curr_char = *lex->curr;    
    lex->curr++;
    lex->column++;
    if(lex->curr_char == '\n')
    {
	lex->line++;
	lex->column = 0;
    }
    if(lex->curr_char == '\t')
	lex->column += 3;
}

void prev_char(lexer *lex)
{
    lex->curr--;
    lex->curr_char = (lex->curr[-1]);
    lex->column--;
    if(lex->curr_char == '\n')
	lex->line--;
    if(lex->curr_char == '\t')
	lex->column -= 3;
}

void consume_whitespace(lexer *lex)
{
    while(lex->curr_char == ' ' ||
	  lex->curr_char == '\t' ||
	  lex->curr_char == '\n')
	next_char(lex);
}

void lexer_error(lexer *lex, const char *msg)
{
    printf("Lexer error in %s:%i:%i: %s\n", lex->file, lex->line, lex->column, msg);
}

int strcmp(const char *a, const char *b)
{
    while(*a && (*a == *b))
    {
	a++;
	b++;
    }
    return(*a - *b);
}

int is_valid_id(lexer *lex)
{
    return(('A' <= lex->curr_char &&
	    lex->curr_char <= 'Z') ||
	('a' <= lex->curr_char &&
	 lex->curr_char <= 'z') ||
	('0' <= lex->curr_char &&
	 lex->curr_char <= '9') ||
	   lex->curr_char == '_');
}

long lex_int(lexer *lex)
{
    long result = lex->curr_char - '0';
    while('0' <= lex->curr_char &&
	  lex->curr_char <= '9')
    {
	next_char(lex);
	consume_whitespace(lex);
	result = result * 10 + (lex->curr_char - '0');
    }
    prev_char(lex);
    return(result);
}

void next_token(lexer *lex, token *tok)
{
    switch(lex->curr_char)
    {
    case '/':
	next_char(lex);
	if(lex->curr_char == '/')
	{
	    do
	    {
		next_char(lex);
		if(lex->curr_char == '\0')
		{
		    tok->type = token_eof;
		    return;
		}
	    }
	    while(lex->curr_char != '\n');
	    tok->type = token_comment;
	    return;
	}
	if(lex->curr_char == '*')
	{
	    int nested_comments = 1;
	    for(nested_comments = 1; nested_comments > 0;)
	    {
		next_char(lex);
		if(lex->curr_char == '\0')
		{
		    lexer_error(lex, "End of file in comment");
		    tok->type = token_eof;
		    return;
		}
		if(lex->curr_char == '/')
		{
		    next_char(lex);
		    if(lex->curr_char == '*')
			nested_comments++;
		    else
			prev_char(lex);
		}
		if(lex->curr_char == '*')
		{
		    next_char(lex);
		    if(lex->curr_char == '/')
			nested_comments--;
		    else
			prev_char(lex);
		}
	    }
	    tok->type = token_comment;
	    return;
	}
	else
	    prev_char(lex);
    case '-':
	next_char(lex);
	if(lex->curr_char == '>')
	{
	    tok->type = token_arrow;
	    return;
	}
	prev_char(lex);
    case '>':
	next_char(lex);
	if(lex->curr_char == '=')
	{
	    tok->type = token_ge;
	    return;
	}
	else if(lex->curr_char == '>')
	{
	    tok->type = token_rshift;
	    return;
	}
	prev_char(lex);
    case '<':
	next_char(lex);
	if(lex->curr_char == '=')
	{
	    tok->type = token_le;
	    return;
	}
	else if(lex->curr_char == '<')
	{
	    tok->type = token_lshift;
	    return;
	}
	prev_char(lex);
    case '!':
	next_char(lex);
	if(lex->curr_char == '=')
	{
	    tok->type = token_nequal;
	    return;
	}
	prev_char(lex);
    case '\0':
    case '@':
    case '#':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
    case '(':
    case ')':
    case '=':
    case '+':
    case '?':
    case '.':
    case ',':
    case '[':
    case ']':
    case '{':
    case '}':
    case '|':
    case ':':
    case ';':
    case '~':
	tok->type = lex->curr_char;
	return;
    }
    
    if(('A' <= lex->curr_char &&
	lex->curr_char <= 'Z') ||
       ('a' <= lex->curr_char &&
	lex->curr_char <= 'z') ||
       lex->curr_char == '_')
    {
	int size = 0;
	char *beg = lex->curr - 1;
	while(is_valid_id(lex))
	{
	    next_char(lex);
	    size++;
	}
	tok->value = malloc(sizeof(char) * size + 1);
	for(int i = 0; i < size; i++)
	{
	    ((char *)tok->value)[i] = beg[i];
	}
	
	prev_char(lex);
	
	((char *)tok->value)[size] = 0;
	if(strcmp((char *)tok->value, "when") == 0)
	    tok->type = token_when;
	else if(strcmp((char *)tok->value, "else") == 0)
	    tok->type = token_else;
	else if(strcmp((char *)tok->value, "for") == 0)
	    tok->type = token_for;
	else if(strcmp((char *)tok->value, "while") == 0)
	    tok->type = token_while;
	else if(strcmp((char *)tok->value, "let") == 0)
	    tok->type = token_let;
	else if(strcmp((char *)tok->value, "r32") == 0)
	    tok->type = token_r32;
	else if(strcmp((char *)tok->value, "r64") == 0)
	    tok->type = token_r64;
	else if(strcmp((char *)tok->value, "i8") == 0)
	    tok->type = token_i8;
	else if(strcmp((char *)tok->value, "i16") == 0)
	    tok->type = token_i16;
	else if(strcmp((char *)tok->value, "i32") == 0)
	    tok->type = token_i32;
	else if(strcmp((char *)tok->value, "i64") == 0)
	    tok->type = token_i64;
	else if(strcmp((char *)tok->value, "u8") == 0)
	    tok->type = token_u8;
	else if(strcmp((char *)tok->value, "u16") == 0)
	    tok->type = token_u16;
	else if(strcmp((char *)tok->value, "u32") == 0)
	    tok->type = token_u32;
	else if(strcmp((char *)tok->value, "u64") == 0)
	    tok->type = token_u64;
	else if(strcmp((char *)tok->value, "string") == 0)
	    tok->type = token_string;
	else if(strcmp((char *)tok->value, "bool") == 0)
	    tok->type = token_bool;
	else if(strcmp((char *)tok->value, "complex") == 0)
	    tok->type = token_complex;
	else if(strcmp((char *)tok->value, "true") == 0)
	    tok->type = token_true;
	else if(strcmp((char *)tok->value, "false") == 0)
	    tok->type = token_false;
	else if(strcmp((char *)tok->value, "def") == 0)
	    tok->type = token_def;
	else
	    tok->type = token_id;
    }
    else if('0' <= lex->curr_char &&
	    lex->curr_char <= '9')
    {
	long a = lex_int(lex);
	next_char(lex);
	consume_whitespace(lex);
	if(lex->curr_char == '.')
	{
	    next_char(lex);
	    char *b = lex->curr - 1;
	    consume_whitespace(lex);
	    if('0' <= lex->curr_char &&
	       lex->curr_char <= '9')
	    {
		double b = (double)lex_int(lex);
		while(b > 1.0)
		    b /= 10.0;
		tok->value = malloc(sizeof(double));
		*(double *)tok->value = a + b;
		tok->type = token_real_literal;
	    }
	    else
	    {
		tok->value = malloc(sizeof(long));
		*(long *)tok->value = a;
		tok->type = token_int_literal;
		lex->curr = b;
		prev_char(lex);
	    }
	}
	else if(lex->curr_char == 'i' || lex->curr_char == 'I')
	{
	    tok->value = malloc(sizeof(long));
	    *(long *)tok->value = a;
	    tok->type = token_complex_literal;
	}
	else
	{
	    tok->value = malloc(sizeof(long));
	    *(long *)tok->value = a;
	    tok->type = token_int_literal;
	    prev_char(lex);
	}
    }
}

//NOTE(sasha): This function allocates a new string on the heap to keep track of the file name.
//             Is this necessary?
void lex(char *src, char *file, token *tokens, int *token_count)
{
    lexer lex = {};
    lex.file = file;
    lex.curr = src;
    lex.line = 1;
    lex.column = 1;
    *token_count = 0;
    do
    {
	next_char(&lex);
	consume_whitespace(&lex);
	tokens->file = lex.file;
	tokens->line = lex.line;
	tokens->column = lex.column;
	++*token_count;
	next_token(&lex, tokens);
    } while(tokens++->type != token_eof);
}

const char *token_to_string(token *tok)
{
    switch(tok->type)
    {
    case token_eof:
	return("eof ");
	break;
    case token_comment:
	return("comment ");
	break;
    case '!':
	return("!");
    case '@':
	return("@");
    case '#':
	return("#");
    case '$':
	return("$");
    case '%':
	return("%");	
    case '^':
	return("^");
    case '&':
	return("&");
    case '*':
	return("*");
    case '(':
	return("(");
    case ')':
	return(")");
    case '-':
	return("-");
    case '=':
	return("=");
    case '+':
	return("+");
    case '?':
	return("?");
    case '.':
	return(".");
    case ',':
	return(",");
    case '>':
	return(">");
    case '<':
	return("<");
    case '[':
	return("[");
    case ']':
	return("]");
    case '{':
	return("{");
    case '}':
	return("}");
    case '|':
	return("|");
    case ':':
	return(":");
    case ';':
	return(";");
    case '~':
	return("~");
    case token_r32:
	return("r32");
    case token_r64:
	return("r64");
    case token_i8:
	return("i8");
    case token_i16:
	return("i16");
    case token_i32:
	return("i32");
    case token_i64:
	return("i64");
    case token_u8:
	return("u8");
    case token_u16:
	return("u16");
    case token_u32:
	return("u32");
    case token_u64:
	return("u64");
    case token_string:
	return("string");
    case token_bool:
	return("bool");
    case token_complex:
	return("complex");
    case token_int_literal:
	return("int_literal");
    case token_real_literal:
	return("real_literal");
    case token_complex_literal:
	return("complex_literal");
    case token_string_literal:
	return("string_literal");
    case token_when:
	return("when");
    case token_else:
	return("else");
    case token_while:
	return("while");
    case token_for:
	return("for");
    case token_arrow:
	return("->");
    case token_id:
	return("identifier");
    case token_let:
	return("let");
    case token_true:
	return("true");
    case token_false:
	return("false");
    case token_def:
	return("def");
    case token_ge:
	return(">=");
    case token_le:
	return("<=");
    case token_nequal:
	return("!=");
    default:
	return("error");
    }
}

void print_token_stream(token *tok, int token_count)
{
    printf("token count: %d\n", token_count);
    for(int i = 0; i < token_count; i++)
	printf("%s ", token_to_string(tok++));
    printf("\n");
}

