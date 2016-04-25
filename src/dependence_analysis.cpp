#include "dependence_analysis.hpp"
#include <iostream>

Function*
DependenceAnalysis::FindMainFunction(Program *program) {
  Function *func = nullptr;
  for (auto node : program_->list_) {
    std::cout << "node " << (void *) node << std::endl;
    if (auto f = dynamic_cast<Function*>(node))
      func = f;
  }
  assert(func);
  return func;
}

CompoundStatement*
DependenceAnalysis::FindFirstForEachLoop(NodeList *node_list) {
  for (auto node : node_list->list_) {
    if (auto st = dynamic_cast<CompoundStatement*>(node)) {
      if (st->statement_header_->header_->value_.compare("foreach") == 0)
        return st;
      else {
        CompoundStatement* cst = FindFirstForEachLoop(
            static_cast<NodeList*>(st->block_scope_));
        if (cst) return cst;
      }
    }
  }
  return nullptr;
}

void
DependenceAnalysis::PopulateSubExprTable(NodeList *stmts) {
  for (auto stmt : stmts->list_) {
    if (auto sstmt
        = dynamic_cast<SingleStatement*>(stmt)) {
      auto write_set = sstmt->write_set_;
      assert(write_set.size() == 1);
      auto write_node = dynamic_cast<ConstantVar*>(*(write_set.begin()));
      assert(write_node);
      if (auto write_id
          = dynamic_cast<Identifier*>(write_node->node_)) {
        sub_expr_table_.emplace(write_id->value_, sstmt);
        std::cout << "writes to " << write_id->value_ << std::endl;
      }
    } else if (auto cstmt
        = dynamic_cast<CompoundStatement*>(stmt)) {
      PopulateSubExprTable(static_cast<NodeList*>(cstmt->block_scope_));
    }
  }
}

void
DependenceAnalysis::PopulateSymbolAccessTables(CompoundStatement *loop) {
  for (auto write_node : loop->write_set_) {
    if (auto array_write
        = dynamic_cast<ArrayAccess*>(write_node)) {
      std::cout << "array write " << (void*) array_write->constant_var_;
    } else {
      if (auto wconst_var = dynamic_cast<ConstantVar*>(write_node)) {
        if (dynamic_cast<Symbol*>(wconst_var->node_)) {
          std::cout << "write symbol " << std::endl;
        } else {
          std::cout << "write const var" << std::endl;
        }
      } else {
        std::cout << "write set " << (void*) write_node << std::endl;
      }
    }
  }

  for (auto read_node : loop->read_set_) {
    if (auto array_read
        = dynamic_cast<ArrayAccess*>(read_node)) {
      std::cout << "array read " << (void*) array_read->constant_var_;
    } else {
      std::cout << "read set " << (void*) read_node << std::endl;
    }
  }
}

bool
DependenceAnalysis::Initialize() {
  Function *main_func = FindMainFunction(program_);
  std::cout << "found main function " << (void*) main_func << std::endl;

  loop_ = FindFirstForEachLoop(
      static_cast<NodeList*>(main_func->block_scope_));
  if (!loop_) return false;
  std::cout << "found foreach loop " << (void *) loop_ << std::endl;
  PopulateSubExprTable(static_cast<NodeList*>(main_func->block_scope_));

  PopulateSymbolAccessTables(loop_);
  return true;
}

DependenceGraph
DependenceAnalysis::ComputeDependenceGraph() {
  return DependenceGraph();
}
