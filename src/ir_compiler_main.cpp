#include "bison/types.h"
#include "bison/step_two_visitor.h"

extern int yyparse();

int main (int argc, char *argv[])
{
  StatementList statements;
  StepTwoVisitor step_two_visitor(&statements);
  extern Program *program;

  int rc = yyparse ();
  std::cout << program->ToString() << std::endl;

  program->Accept(&step_two_visitor);

  return rc;
}
