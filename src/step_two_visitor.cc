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
  BlockScope* bs = compound_statement->block_scope_;
  bs->Accept(this);

  for (auto& i : bs->list_) {
    if (auto* stmt = dynamic_cast<Statement*>(i)) {
      compound_statement->read_set_.insert(stmt->read_set_.begin(),
                                           stmt->read_set_.end());
      compound_statement->write_set_.insert(stmt->write_set_.begin(),
                                            stmt->write_set_.end());
    }
  }
}

// If node is a @symbol_table reference, then return the Symbol object it points
// to, otherwise return the node itself.
Node* StepTwoVisitor::Lookup(Node* node) {
  if (auto* symbol_ref = dynamic_cast<PrimarySymbolTableReference*>(node)) {
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
    if (auto* constant_var = dynamic_cast<ConstantVar*>(i))
      constant_var->node_ = Lookup(constant_var->node_);
    else if (ArrayAccess* ae = dynamic_cast<ArrayAccess*>(i)) {
      auto* constant_var = dynamic_cast<ConstantVar*>(ae->constant_var_);
      constant_var->node_ = Lookup(constant_var->node_);
      // TODO: replace @symbol_table references in arguments_.
      assert(ae->arguments_);
    }
  }

  // The first address in all instructions is in the write set.
  auto it = nodes.begin();
  single_statement->write_set_.insert(*it);

  // The rest is in the read set.
  while (++it != nodes.end())
    single_statement->read_set_.insert(*it);

  // If the statement contains an array expression, it may absorb the arguments_
  // into ArrayAccess object (see SingleStatement constructor in types.h) and
  // set arguments_ to NULL, therefore, check if it is null before adding it to
  // read set.
  if (single_statement->arguments_)
    single_statement->read_set_.insert(single_statement->arguments_);
}
