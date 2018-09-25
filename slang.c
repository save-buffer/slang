#include <stdio.h>

#include "lexer.c"
#include "parser.c"
#include "semant.c"

int main(int argc, char **argv)
{
    if(argc > 1)
    {
	FILE *src = fopen(argv[1], "r");
	fseek(src, 0, SEEK_END);
	int size = ftell(src);
	fseek(src, 0, SEEK_SET);
	printf("size: %d bytes\n", size);
	char buff[size + 1];
	fread(buff, 1, size, src);
	fclose(src);
	buff[size] = 0;
	printf("%s\n", buff);

	int token_count;
	token tokenized_src[size];
	lex(buff, argv[1], tokenized_src, &token_count);
	print_token_stream(tokenized_src, token_count);
	ast_node *root = parse(tokenized_src);
	render_ast("ast.dot", root);

	semantic_analysis(root);
    }
    else
    {
	printf("Usage: ./slang [filename]\n");
    }
    // Why free things when the OS will do it for us?
    return(0);
}
