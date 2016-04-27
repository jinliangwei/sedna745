#include "types.h"
#include "step_two_visitor.h"
#include "dependence_analysis.hpp"

extern int yyparse();

int main (int argc, char *argv[])
{
  StepTwoVisitor step_two_visitor;
  extern Program *program;

  int rc = yyparse ();
  //std::cout << program->ToString() << std::endl;

  program->Accept(&step_two_visitor);

  //std::cout << program->ToString() << std::endl;

  DependenceAnalysis dependence_analysis(program);
  bool has_foreach_loop = dependence_analysis.Initialize();
  IterVec dims {4, 4};
  IterationSpace iteration_space(dims);
  if (has_foreach_loop) {
    DependenceGraph dg = dependence_analysis.ComputeDependenceGraph(iteration_space);
    dg.Print();
    dg.GreedyColor();
  }

  return rc;
}
