%{
#include <iostream>

int yylex (void);
void yyerror (char const *);
%}

%token SYMBOL FUNCTION ATTRIBUTES
%token CHAR BOOLEAN STRING INT8 INT16 INT32 INT64 UINT8 UINT16 UINT32 UINT64 FLOAT DOUBLE
%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL DOMAIN_OP
%token NDARRAY SYMBOL_TABLE TEMP_VAR STATEMENT

%%
program:
%empty
| symbol program
| function program
;

symbol:
SYMBOL '{' key_value_list '}'
;

primary_value:
scalar_array_type
| INT_CONSTANT
| FLOAT_CONSTANT
| STRING_LITERAL
| symbol_table_reference
;

value:
primary_value
| array_value
;

array_value:
'[' ']'
| '[' value_list ']'
;

value_list:
value
| value ',' value_list
;

key_value:
IDENTIFIER ':' value
;

key_value_list:
key_value
| key_value ',' key_value_list
;

primitive_type:
CHAR
| BOOLEAN
| STRING
| INT8
| INT16
| INT32
| INT64
| UINT8
| UINT16
| UINT32
| UINT64
| FLOAT
| DOUBLE
;

struct_primitive_type:
IDENTIFIER
| primitive_type
;

ndarray_type:
NDARRAY '<' INT_CONSTANT ',' type '>'
;

ndarray_domain_type:
ndarray_type DOMAIN_OP IDENTIFIER
;

type:
struct_primitive_type
| ndarray_type
| ndarray_domain_type
;

scalar_array_type:
type
| type '[' ']'
| type '[' INT_CONSTANT ']'
;


primary_symbol_table_reference:
SYMBOL_TABLE '[' STRING_LITERAL ']'
;

symbol_table_reference:
primary_symbol_table_reference
| symbol_table_reference '[' int_list ']'
| symbol_table_reference '.' IDENTIFIER
;

int_list:
INT_CONSTANT
| INT_CONSTANT int_list
;

function:
FUNCTION '{' attributes block_scope '}'
;

attributes:
ATTRIBUTES '{' key_value_list '}'
;

block_scope:
%empty
| symbol block_scope
| statement block_scope
;

statement:
statement_header constant_var_list arguments
| compound_statement
;

statement_header:
'@' IDENTIFIER
;

arguments:
%empty
| argument arguments
;

argument:
'(' argument_list ')'
;

argument_list:
%empty
| constant_var
| argument
| constant_var ',' argument_list
| argument ',' argument_list
;

constant_var:
IDENTIFIER
| primary_symbol_table_reference
| INT_CONSTANT
| FLOAT_CONSTANT
| STRING_LITERAL
;

constant_var_list:
constant_var
| constant_var constant_var_list
;

compound_statement:
statement_header '{' attributes block_scope '}'
;

%%
/* Called by yyparse on error.  */
void
yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}
