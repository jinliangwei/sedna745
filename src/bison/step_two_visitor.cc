#include "step_two_visitor.h"

void StepTwoVisitor::Visit(Symbol* symbol) {
  symbol_table_.AddSymbol(symbol);
}

void StepTwoVisitor::Visit(Program* program) {
  symbol_table_.AddScope();
  for (auto i : program->list_)
    i->Accept(this);
}

void StepTwoVisitor::Visit(BlockScope* block_scope) {
  symbol_table_.AddScope();
  for (auto i : block_scope->list_)
    i->Accept(this);
}

void StepTwoVisitor::Visit(Function* function) {
  function->block_scope_->Accept(this);
}

void StepTwoVisitor::Visit(CompoundStatement* compound_statement) {
  compound_statement->block_scope_->Accept(this);
}

