/*
  program                  = function_list
  function_list            = function | function function_list
  function                 = identifier ( parameter_list ) : ( type_list ) -> type  { semicolon_statement_list statement . } |
                             identifier ( parameter_list ) : ( type_list ) -> ( type_list ) { semicolon_statement_list statement . }
  parameter_list           = parameter | parameter , parameter_list
  parameter                = identifier | int_literal | real_literal | complex_literal | string_literal | true | false
  type_list                = type | type , type_list
  type                     = r32 | r64 | i8 | i16 | i32 | i64 | u8 | u16 | u32 | u64 | string | bool | complex | identifier
  semicolon_statement_list = statement ; | statement ; semicolon_statement_list
  statement                = declaration |
  declaration              = let identifier : 
 */
