%{
#include <iostream>
#include <string>
#include "types.h"

int yylex (void);
void yyerror (char const *);
%}

%union {
  String *str_val;
  Int *int_val;
  Float *float_val;
  PrimitiveType *primitive_type;
  Type *type;
  Value *value;
  NdArrayType *ndarray_type;
  NdArrayDomainType *ndarray_domain_type;
  ScalarArrayType *scalar_array_type;
  IntList *int_list;
  PrimarySymbolTableReference *primary_symbol_table_reference;
  SymbolTableReference *symbol_table_reference;
  ValueList *value_list;
  KeyValue *key_value;
  KeyValueList *key_value_list;
  Attributes *attributes;
  StatementHeader *statement_header;
  ArrayValue *array_value;
}

%token SYMBOL FUNCTION ATTRIBUTES
%token NDARRAY SYMBOL_TABLE TEMP_VAR STATEMENT
%token <float_val> FLOAT_CONSTANT
%token <int_val> INT_CONSTANT
%token <str_val> STRING_LITERAL IDENTIFIER CHAR BOOLEAN STRING INT8 INT16 INT32 INT64 UINT8 UINT16 UINT32 UINT64 FLOAT DOUBLE DOMAIN_OP

%type <primitive_type> primitive_type
%type <type> type;
%type <value> value;
%type <ndarray_type> ndarray_type;
%type <ndarray_domain_type> ndarray_domain_type;
%type <scalar_array_type> scalar_array_type;
%type <int_list> int_list;
%type <primary_symbol_table_reference> primary_symbol_table_reference;
%type <symbol_table_reference> symbol_table_reference;
%type <value_list> value_list;
%type <array_value> array_value;
%type <key_value> key_value;
%type <key_value_list> key_value_list;
%type <attributes> attributes;
%type <statement_header> statement_header;

%start program

%%
program:
%empty
| symbol program
| function program
;

symbol:
SYMBOL '{' key_value_list '}' 
;

value:
scalar_array_type { $$ = $1; }
| INT_CONSTANT { $$ = $1; }
| FLOAT_CONSTANT { $$ = $1; }
| STRING_LITERAL { $$ = $1; }
| symbol_table_reference { $$ = $1; }
| array_value { $$ = $1; }
;

array_value:
'[' ']' { $$ = new ArrayValue(0); }
| '[' value_list ']' { $$ = new ArrayValue($2); }
;

value_list:
value { $$ = new ValueList(); $$->list_.push_back($1); }
| value ',' value_list { $$->list_.push_back($1); }
;

key_value:
IDENTIFIER ':' value { $$ = new KeyValue($1, $3); }
;

key_value_list:
key_value { $$ = new KeyValueList(); $$->push_back($1); }
| key_value ',' key_value_list { $$->push_back($1); }
;

primitive_type:
CHAR { $$ = new PrimitiveType($1); }
| BOOLEAN { $$ = new PrimitiveType($1); }
| STRING { $$ = new PrimitiveType($1); }
| INT8 { $$ = new PrimitiveType($1); }
| INT16 { $$ = new PrimitiveType($1); }
| INT32 { $$ = new PrimitiveType($1); }
| INT64 { $$ = new PrimitiveType($1); }
| UINT8 { $$ = new PrimitiveType($1); }
| UINT16 { $$ = new PrimitiveType($1); }
| UINT32 { $$ = new PrimitiveType($1); }
| UINT64 { $$ = new PrimitiveType($1); }
| FLOAT { $$ = new PrimitiveType($1); }
| DOUBLE { $$ = new PrimitiveType($1); }
| IDENTIFIER { $$ = new PrimitiveType($1); }
;

ndarray_type:
NDARRAY '<' INT_CONSTANT ',' type '>' { $$ = new NdArrayType($3, $5); }
;

ndarray_domain_type:
ndarray_type DOMAIN_OP IDENTIFIER { $$ = new NdArrayDomainType($1, $2, $3); }
;

type:
primitive_type { $$ = $1; }
| ndarray_type { $$ = $1; }
| ndarray_domain_type { $$ = $1; }
;

scalar_array_type:
type { $$ = new ScalarArrayType($1, new Int(-1)); }
| type '[' ']' { $$ = new ScalarArrayType($1, new Int(-2)); }
| type '[' INT_CONSTANT ']' { $$ = new ScalarArrayType($1, $3); }
;


primary_symbol_table_reference:
SYMBOL_TABLE '[' STRING_LITERAL ']' { $$ = new PrimarySymbolTableReference($3); }
;

symbol_table_reference:
primary_symbol_table_reference { $$ = new SymbolTableReference(); $$->list_.push_back($1); }
| symbol_table_reference '[' int_list ']' { $$->list_.push_back($3); }
| symbol_table_reference '.' IDENTIFIER { $$->list_.push_back($3); }
;

int_list:
INT_CONSTANT { $$ = new IntList(); $$->push_back($1); }
| INT_CONSTANT int_list {$$->push_back($1); }
;

function:
FUNCTION '{' attributes block_scope '}'
;

attributes:
ATTRIBUTES '{' key_value_list '}' { $$ = new Attributes($3); }
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
'@' IDENTIFIER { $$ = new StatementHeader($2); }
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
