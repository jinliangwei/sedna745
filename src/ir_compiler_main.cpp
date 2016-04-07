#include "bison/types.h"

extern int yyparse();

int main (int argc, char *argv[])
{
  extern Program *program;
  
  int rc = yyparse ();
  program->Print();

  return rc;
}
