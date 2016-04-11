#include "bison/types.h"
#include "bison/step_two_visitor.h"

extern int yyparse();

int main (int argc, char *argv[])
{
  StepTwoVisitor step_two_visitor;
  extern Program *program;

  int rc = yyparse ();
  std::cout << program->ToString() << std::endl;

  program->Accept(&step_two_visitor);

  std::cout << program->ToString() << std::endl;

  return rc;
}
