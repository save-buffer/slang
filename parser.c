/*
  program                  = function_list
  decl_list                = function | type_declaration | type_declaration decl_list | function decl_list
  function                 = identifier ( parameter_list ) : ( type_list ) -> type block
                             identifier ( parameter_list ) : ( type_list ) -> ( type_list ) block
  type_declaration         = def identifier { type_specifier_list }
  type_specifier_list      = type_specifier | type_specifier type_specifier_list
  type_specifier           = identifier : type ; | identifier : type -> expression
  parameter_list           = parameter | parameter , parameter_list
  parameter                = identifier | literal
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
  argument_list            = <empty> | expr | expr argument_list
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
 */
