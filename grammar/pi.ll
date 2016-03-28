%{
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

"char"		{ return CHAR; }
"bool"		{ return BOOLEAN; }
"string"	{ return STRING; }
"int8"		{ return INT8; }
"int16"		{ return INT16; }
"int32"		{ return INT32; }
"int64"		{ return INT64; }
"uint8"		{ return UINT8; }
"uint16"	{ return UINT16; }
"uint32"	{ return UINT32; }
"uint64"	{ return UINT64; }
"float"		{ return FLOAT; }
"double"	{ return DOUBLE; }
"ndarray"	{ return NDARRAY; }

{L}({L}|{D})* 		{ return IDENTIFIER; }

("+"|"-")?0[xX]{H}+{IS}?  	 { return INT_CONSTANT; }
("+"|"-")?0{D}+{IS}?		 { return INT_CONSTANT; }
("+"|"-")?{D}+{IS}?		 { return INT_CONSTANT; }
("+"|"-")?{D}+{E}{FS}?		 { return INT_CONSTANT; }
("+"|"-")?{D}*"."{D}+({E})?{FS}? { return FLOAT_CONSTANT; }
("+"|"-")?{D}+"."{D}*({E})?{FS}? { return FLOAT_CONSTANT; }
L?'(\\.|[^\\'])+'	 { return INT_CONSTANT; }
L?\"(\\.|[^\\"])*\"	 { return STRING_LITERAL; }

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