#pragma once
#if defined(__MACH__)
#include <stdlib.h>
#else 
#include <malloc.h>
#endif

typedef enum
{
    token_invalid,
    token_eof = '\0',

    token_when = 128,
    token_else = 129,
    token_for = 130,
    token_while = 131,
    token_let = 132,
    token_true = 133,
    token_false = 134,   
    token_comment = 135,
    token_r32 = 136,
    token_r64 = 137,
    token_i8 = 138,
    token_i16 = 139,
    token_i32 = 140,
    token_i64 = 141,
    token_u8 = 142,
    token_u16 = 143,
    token_u32 = 144,
    token_u64 = 145,
    token_string = 146,
    token_bool = 147,
    token_complex = 148,
    token_def = 149,

    token_int_literal = 150,
    token_real_literal = 151,
    token_complex_literal = 152,
    token_string_literal = 153,
    
    token_id = 154,
    token_ge = 155,
    token_le = 156,
    token_arrow = 157,
    token_lshift = 158,
    token_rshift = 159,
    token_nequal = 160,
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
    char *file;
    int line;
    int column;
    tokens type;
    void *value;
} token;

typedef struct
{
    char *file;
    char *curr;
    char curr_char;
    int line;
    int column;
} lexer;


void next_char(lexer *lex);
void prev_char(lexer *lex);
void consume_whitespace(lexer *lex);
void lexer_error(lexer *lex, const char *msg);
int strcmp(const char *a, const char *b);
int is_valid_id(lexer *lex);
long lex_int(lexer *lex);
void next_token(lexer *lex, token *tok);
void lex(char *src, char *file, token *tokens, int *token_count);
const char *token_to_string(token *tok);
void print_token_stream(token *tok, int token_count);
