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

// If node is a @symbol_table reference, then return the Symbol object it points
// to, otherwise return the node itself.
Node* StepTwoVisitor::Lookup(Node* node) {
  if (auto *symbol_ref = dynamic_cast<PrimarySymbolTableReference*>(node)) {
    Node* node = symbol_table_.Lookup(symbol_ref->symbol_->ToString());
    assert(node);  // Lookup should succeed, or it is an undefined symbol.
    return node;
  }
  return node;
}

void StepTwoVisitor::Visit(SingleStatement* single_statement) {
  auto& nodes = single_statement->constant_var_list_->list_;
  assert(!nodes.empty());

  // Replace all @symbol_table references with pointers to actual symbols.
  for (auto& i : nodes) {
    auto* constant_var = dynamic_cast<ConstantVar*>(i);
    assert(constant_var);
    constant_var->node_ = Lookup(constant_var->node_);
  }

  // If there is ArrayAccess, also replace @symbol_table reference in it.
  if (ArrayAccess* a = single_statement->array_access_) {
    auto* constant_var = dynamic_cast<ConstantVar*>(a->constant_var_);
    constant_var->node_ = Lookup(constant_var->node_);
    // TODO: replace @symbol_table references in arguments_.
    assert(a->arguments_);
  } else {
    // TODO: if arguments_ then replace @symbol_table references in arguments_.
  }

  // The first address in all instructions is in the write set.
  auto it = nodes.begin();
  single_statement->write_set_.insert(*it);

  // The rest is in the read set.
  while (++it != nodes.end())
    single_statement->read_set_.insert(*it);

  if (ArrayAccess* a = single_statement->array_access_)
    single_statement->read_set_.insert(a);
  else if (single_statement->arguments_)
    single_statement->read_set_.insert(single_statement->arguments_);
}
