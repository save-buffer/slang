#pragma once
#include <malloc.h>

typedef enum
{
    token_invalid,
    token_eof = '\0',

    token_when = 1,
    token_else = 2,
    token_for = 3,
    token_while = 4,
    token_let = 5,
    token_true = 6,
    token_false = 7,   
    token_comment = 8,
    token_r32 = 9,
    token_r64 = 10,
    token_i8 = 11,
    token_i16 = 12,
    token_i32 = 13,
    token_i64 = 14,
    token_u8 = 15,
    token_u16 = 16,
    token_u32 = 17,
    token_u64 = 18,
    token_string = 19,
    token_bool = 20,
    token_complex = 21,
    token_def = 22,

    token_int_literal = 23,
    token_real_literal = 24,
    token_complex_literal = 25,
    token_string_literal = 26,
    
    token_id = 27,
    token_ge = 28,
    token_le = 29,
    token_arrow = 30,
    token_lshift = 31,
    token_rshift = 32,
    token_nequal = 33,
    token_colon = ':',
    token_semicolon = ';',
    token_exclam = '!',
    token_at = '@',
    token_pound = '#',
    token_dollar = '$',
    token_percent = '%',
    token_carrot = '^',
    token_and = '&',
    token_star = '*',
    token_left_paren = '(',
    token_right_paren = ')',
    token_minus = '-',
    token_equals = '=',
    token_plus = '+',
    token_question = '?',
    token_dot = '.',
    token_comma = ',',
    token_greater = '>',
    token_less = '<',
    token_slash = '/',
    token_tilde = '~',

    token_left_bracket = '[',
    token_right_bracket = ']',
    token_left_curly = '{',
    token_right_curly = '}',
    token_bar = '|',
    
} tokens;

typedef struct
{
    tokens type;
    void *value;
} token;

typedef struct
{
    char *curr;
    char curr_char;
    int line;
    int column;
} lexer;


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
    for(;;)
    {
	switch(lex->curr_char)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	    break;
	default:
	    return;
	}
	next_char(lex);
    }
}

void lexer_error(lexer *lex, const char *msg)
{
    printf("Lexer error at line %d column %d: %s\n", lex->line, lex->column, msg);
}

int strcmp(char *a, char *b)
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

void lex(char *src, token *tokens, int *token_count)
{
    lexer lex = {};
    lex.curr = src;
    *token_count = 0;
    do
    {
	next_char(&lex);
	consume_whitespace(&lex);
	++*token_count;
	next_token(&lex, tokens);
    } while(tokens++->type != token_eof);
}

void print_token(token *tok)
{
    switch(tok->type)
    {
    case token_eof:
	printf("eof ");
	break;
    case token_comment:
	printf("comment ");
	break;
    case '!':
    case '@':
    case '#':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
    case '(':
    case ')':
    case '-':
    case '=':
    case '+':
    case '?':
    case '.':
    case ',':
    case '>':
    case '<':
    case '[':
    case ']':
    case '{':
    case '}':
    case '|':
    case ':':
    case ';':
    case '~':
	printf("%c ", tok->type);
	break;
    case token_r32:
	printf("r32 ");
	break;
    case token_r64:
	printf("r64 ");
	break;
    case token_i8:
	printf("i8 ");
	break;
    case token_i16:
	printf("i16 ");
	break;
    case token_i32:
	printf("i32 ");
	break;
    case token_i64:
	printf("i64 ");
	break;
    case token_u8:
	printf("u8 ");
	break;
    case token_u16:
	printf("u16 ");
	break;
    case token_u32:
	printf("u32 ");
	break;
    case token_u64:
	printf("u64 ");
	break;
    case token_string:
	printf("string ");
	break;
    case token_bool:
	printf("bool ");
	break;
    case token_complex:
	printf("complex ");
	break;
    case token_int_literal:
	printf("int_literal ");
	break;
    case token_real_literal:
	printf("real_literal ");
	break;
    case token_complex_literal:
	printf("complex_literal ");
	break;
    case token_string_literal:
	printf("string_literal ");
	break;
    case token_when:
	printf("when ");
	break;
    case token_else:
	printf("else ");
	break;
    case token_while:
	printf("while ");
	break;
    case token_for:
	printf("for ");
	break;
    case token_arrow:
	printf("-> ");
	break;
    case token_id:
	printf("identifier ");
	break;
    case token_let:
	printf("let ");
	break;
    case token_true:
	printf("true ");
	break;
    case token_false:
	printf("false ");
	break;
    case token_def:
	printf("def ");
	break;
    case token_ge:
	printf(">= ");
	return;
    case token_le:
	printf("<= ");
	break;
    case token_nequal:
	printf("!= ");
	break;
    default:
	printf("some other one %d ", tok->type);
	break;
    }
}

void print_token_stream(token *tok, int token_count)
{
    printf("token count: %d\n", token_count);
    for(int i = 0; i < token_count; i++)
	print_token(tok++);
    printf("\n");
}

