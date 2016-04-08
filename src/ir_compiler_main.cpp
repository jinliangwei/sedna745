#include "bison/types.h"

extern int yyparse();

int main (int argc, char *argv[])
{
  extern Program *program;
  
  int rc = yyparse ();
  std::cout << program->ToString() << std::endl;

  return rc;
}
