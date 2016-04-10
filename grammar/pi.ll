%{
#include <string>
#include "types.h"

#include "pi.tab.hh"
%}

D	[0-9]
L	[a-zA-Z_]
H	[a-fA-F0-9]
E	[Ee][+-]?{D}+
FS	(f|F|l|L)
IS	(u|U|l|L)*

%option noyywrap

%%


"%symbol"		{ return SYMBOL; }
"%function"		{ return FUNCTION; }
"%attributes"		{ return ATTRIBUTES; }
"@symbol_table"		{ return SYMBOL_TABLE; }

"char"		{ yylval.str_value = new String(yytext); return CHAR; }
"bool"		{ yylval.str_value = new String(yytext); return BOOLEAN; }
"string"	{ yylval.str_value = new String(yytext); return STRING; }
"int8"		{ yylval.str_value = new String(yytext); return INT8; }
"int16"		{ yylval.str_value = new String(yytext); return INT16; }
"int32"		{ yylval.str_value = new String(yytext); return INT32; }
"int64"		{ yylval.str_value = new String(yytext); return INT64; }
"uint8"		{ yylval.str_value = new String(yytext); return UINT8; }
"uint16"	{ yylval.str_value = new String(yytext); return UINT16; }
"uint32"	{ yylval.str_value = new String(yytext); return UINT32; }
"uint64"	{ yylval.str_value = new String(yytext); return UINT64; }
"float"		{ yylval.str_value = new String(yytext); return FLOAT; }
"double"	{ yylval.str_value = new String(yytext); return DOUBLE; }
"ndarray"	{ yylval.str_value = new String(yytext); return NDARRAY; }

{L}({L}|{D})* 		{ yylval.str_value = new String(yytext); return IDENTIFIER; }

("+"|"-")?0[xX]{H}+{IS}?  	 { yylval.int_value = new Int(atoi(yytext)); return INT_CONSTANT; }
("+"|"-")?0{D}+{IS}?		 { yylval.int_value = new Int(atoi(yytext)); return INT_CONSTANT; }
("+"|"-")?{D}+{IS}?		 { yylval.int_value = new Int(atoi(yytext)); return INT_CONSTANT; }
("+"|"-")?{D}+{E}{FS}?		 { yylval.int_value = new Int(atoi(yytext)); return INT_CONSTANT; }
("+"|"-")?{D}*"."{D}+({E})?{FS}? { yylval.float_value = new Float(atof(yytext)); return FLOAT_CONSTANT; }
("+"|"-")?{D}+"."{D}*({E})?{FS}? { yylval.float_value = new Float(atof(yytext)); return FLOAT_CONSTANT; }
L?'(\\.|[^\\'])+'	 { yylval.int_value = new Int(atoi(yytext)); return INT_CONSTANT; }
L?\"(\\.|[^\\"])*\"	 { yylval.str_value = new String(yytext); return STRING_LITERAL; }

"{"			 { return '{'; }
"}"			 { return '}'; }
"("			 { return '('; }
")"			 { return ')'; }
"["			 { return '['; }
"]"			 { return ']'; }
">"			 { return '>'; }
"<"			 { return '<'; }
","			 { return ','; }
"."			 { return '.'; }
"\n"			 { }
":"			 { return ':'; }
"::"			 { return DOMAIN_OP; }
"@"			 { return '@'; }
. 			 { }

%%
