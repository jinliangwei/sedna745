%{
#include <iostream>
#include <string>
#include "../types.h"

int yylex (void);
void yyerror (char const *);
Program *program;
%}

%union {
  Identifier *id_value;
  String *str_value;
  Int *int_value;
  Float *float_value;
  PrimitiveType *primitive_type;
  Program *program;
  Function *function;
  Type *type;
  ConstantVar *constant_var;
  Value *value;
  Symbol *symbol;
  BlockScope *block_scope;
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
  ConstantVarList *constant_var_list;
  Statement *statement;
  Argument *argument;
  Arguments *arguments;
  ArgumentList *argument_list;
  CompoundStatement *compound_statement;
}

%token SYMBOL FUNCTION ATTRIBUTES
%token NDARRAY SYMBOL_TABLE TEMP_VAR STATEMENT
%token <float_value> FLOAT_CONSTANT
%token <int_value> INT_CONSTANT
%token <str_value> STRING_LITERAL CHAR BOOLEAN STRING INT8 INT16 INT32 INT64 UINT8 UINT16 UINT32 UINT64 FLOAT DOUBLE DOMAIN_OP
%token <id_value> IDENTIFIER

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
%type <symbol> symbol;
%type <program> program;
%type <function> function;
%type <block_scope> block_scope;
%type <constant_var> constant_var;
%type <constant_var_list> constant_var_list;
%type <statement> statement;
%type <argument> argument;
%type <arguments> arguments;
%type <argument_list> argument_list;
%type <compound_statement> compound_statement;
%start program

%%
program:
%empty { $$ = new Program(); program = $$; }
| symbol program { $$ = $2; $$->list_.push_front($1); }
| function program { $$ = $2; $$->list_.push_front($1); }
;

symbol:
SYMBOL '{' key_value_list '}' { $$ = new Symbol($3); }
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
value { $$ = new ValueList(); $$->list_.push_front($1); }
| value ',' value_list { $$ = $3; $$->list_.push_front($1); }
;

key_value:
IDENTIFIER ':' value { $$ = new KeyValue($1, $3); }
;

key_value_list:
key_value { $$ = new KeyValueList(); $$->list_.push_front($1); }
| key_value ',' key_value_list { $$ = $3; $$->list_.push_front($1); }
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
primary_symbol_table_reference { $$ = new SymbolTableReference(); $$->list_.push_front($1); }
| symbol_table_reference '[' int_list ']' { $$ = $1; $$->list_.push_back($3); }
| symbol_table_reference '.' IDENTIFIER { $$ = $1; $$->list_.push_back($3); }
;

int_list:
INT_CONSTANT { $$ = new IntList(); $$->push_front($1); }
| INT_CONSTANT int_list {$$->push_front($1); }
;

function:
FUNCTION '{' attributes block_scope '}' { $$ = new Function($3, $4); }
;

attributes:
ATTRIBUTES '{' key_value_list '}' { $$ = new Attributes($3); }
;

block_scope:
%empty { $$ = new BlockScope(); }
| symbol block_scope { $$ = $2; $$->list_.push_front($1); }
| statement block_scope { $$ = $2; $$->list_.push_front($1); }
;

statement:
statement_header constant_var_list arguments { $$ = new SingleStatement($1, $2, $3); }
| compound_statement { $$ = $1; }
;

statement_header:
'@' IDENTIFIER { $$ = new StatementHeader($2); }
;

arguments:
%empty { $$ = new Arguments(); }
| argument arguments { $$ = $2; $$->list_.push_front($1); }
;

argument:
'(' argument_list ')' { $$ = new Argument($2); }
;

argument_list:
%empty { $$ = new ArgumentList(); }
| constant_var { $$ = new ArgumentList(); $$->list_.push_front($1); }
| argument { $$ = new ArgumentList(); $$->list_.push_front($1); }
| constant_var ',' argument_list { $$ = $3; $$->list_.push_front($1); }
| argument ',' argument_list { $$ = $3; $$->list_.push_front($1); }
;

constant_var:
IDENTIFIER { $$ = new ConstantVar($1); }
| primary_symbol_table_reference { $$ = new ConstantVar($1); }
| INT_CONSTANT { $$ = new ConstantVar($1); }
| FLOAT_CONSTANT { $$ = new ConstantVar($1); }
| STRING_LITERAL { $$ = new ConstantVar($1); }
;

constant_var_list:
constant_var { $$ = new ConstantVarList(); $$->list_.push_front($1); }
| constant_var constant_var_list { $$ = $2; $$->list_.push_front($1); }
;

compound_statement:
statement_header '{' attributes block_scope '}' { $$ = new CompoundStatement($1, $3, $4); }
;

%%
/* Called by yyparse on error.  */
void
yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}
