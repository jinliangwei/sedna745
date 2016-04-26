#include "dependence_analysis.hpp"
#include <iostream>
#include <typeinfo>
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
      if (st->statement_header_->header_->value_ == "foreach")
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

Identifier*
DependenceAnalysis::FindLoopIndexTmpVar(NodeList *stmts) {
  for (auto st : stmts->list_) {
    if (auto stmt = dynamic_cast<SingleStatement*>(st)) {
      if (stmt->statement_header_->header_->value_
          == "member_read") {
        std::cout << "found member read "
                  << std::endl;
        // HACK!!!! assuimg the first member_read to idx in a loop is reading the
        // loop index
        /* auto read_set = stmt->read_set_;
           assert(read_set.size() == 1);
           auto read_node = *(read_set.begin());
           if (!((auto read_symbol = dynamic_cast<Symbol*>(read_node))
           && (read_symbol == loop_var))) {
           continue;
           }*/
        auto argument = dynamic_cast<Argument*>(*(stmt->arguments_->list_.begin()));
        assert(argument);
        auto argument_iter = argument->argument_list_->list_.begin();
        assert(argument_iter != argument->argument_list_->list_.end());
        auto argument_obj = *argument_iter;
        auto member_var = dynamic_cast<ConstantVar*>(argument_obj);
        assert(member_var);
        auto member_id = dynamic_cast<Identifier*>(member_var->node_);
        assert(member_id);
        if (member_id->value_ == "idx") {
          auto write_set = stmt->write_set_;
          assert(write_set.size() == 1);
          auto write_node = *(write_set.begin());
          auto write_var = dynamic_cast<ConstantVar*>(write_node);
          assert(write_var);
          auto write_id = dynamic_cast<Identifier*>(write_var->node_);
          assert(write_id);
          return write_id;
        }
      }
    }
  }
  // the assumption is the loop index variable is always accessed
  assert(0);
  return nullptr;
}

void
DependenceAnalysis::PopulateSubExprTable(NodeList *stmts) {
  for (auto stmt : stmts->list_) {
    if (auto sstmt
        = dynamic_cast<SingleStatement*>(stmt)) {
      auto write_set = sstmt->write_set_;
      assert(write_set.size() == 1);
      if (auto write_node = dynamic_cast<ConstantVar*>(*(write_set.begin()))) {
        if (auto write_id
            = dynamic_cast<Identifier*>(write_node->node_)) {
          sub_expr_table_.emplace(write_id->value_, sstmt);
          std::cout << "writes to " << write_id->value_ << std::endl;
        }
      }
    } else if (auto cstmt
        = dynamic_cast<CompoundStatement*>(stmt)) {
      PopulateSubExprTable(static_cast<NodeList*>(cstmt->block_scope_));
    }
  }
}

void
DependenceAnalysis::PopulateSymbolAccessTables(CompoundStatement *loop) {
  int loop_scope_level = loop->block_scope_->scope_level_;
  for (auto write_node : loop->write_set_) {
    if (auto array_write
        = dynamic_cast<ArrayAccess*>(write_node)) {
      auto constant_var
          = dynamic_cast<ConstantVar*>(array_write->constant_var_);
      assert(constant_var);
      if (auto write_symbol
          = dynamic_cast<Symbol*>(constant_var->node_)) {
        if (write_symbol->scope_level_ >= loop_scope_level) continue;
        if (array_write_table_.find(write_symbol) == array_write_table_.end()) {
          array_write_table_.emplace(write_symbol, std::list<ArrayAccessSubscripts>());
        }
        // Assuming 1) arguments has only one argument;
        // 2) argument has a non-nested list of
        // constant_vars
        // This should be fixed in the IR.
        auto arguments = array_write->arguments_;
        assert(arguments->list_.size() == 1);
        auto argument = dynamic_cast<Argument*>(*(arguments->list_.begin()));
        assert(argument);
        std::cout << "array write " << write_symbol->key_value_list_    \
            ->Lookup("identifier")->ToString() << std::endl;
        ArrayAccessSubscripts subscripts;
        for (auto argument_obj : argument->argument_list_->list_) {
          auto constant_var = dynamic_cast<ConstantVar*>(argument_obj);
          assert(constant_var);
          subscripts.push_back(constant_var->node_);
        }
        array_write_table_[write_symbol].push_back(subscripts);
      }
    } else {
      auto wconst_var = dynamic_cast<ConstantVar*>(write_node);
      assert(wconst_var);
      if (auto wsymbol = dynamic_cast<Symbol*>(wconst_var->node_)) {
        if (wsymbol->scope_level_ >= loop_scope_level) continue;
        scalar_write_set_.insert(wsymbol);
        std::cout << "scalar write " << wsymbol->key_value_list_ \
            ->Lookup("identifier")->ToString() << std::endl;
      }
    }
  }

  for (auto read_node : loop->read_set_) {
    if (auto array_read
        = dynamic_cast<ArrayAccess*>(read_node)) {
      auto constant_var
          = dynamic_cast<ConstantVar*>(array_read->constant_var_);
      assert(constant_var);
      if (auto read_symbol
          = dynamic_cast<Symbol*>(constant_var->node_)) {
        if (read_symbol->scope_level_ >= loop_scope_level) continue;
        if (array_read_table_.find(read_symbol) == array_read_table_.end()) {
          array_read_table_.emplace(read_symbol, std::list<ArrayAccessSubscripts>());
        }
        auto arguments = array_read->arguments_;
        assert(arguments->list_.size() == 1);
        auto argument = dynamic_cast<Argument*>(*(arguments->list_.begin()));
        assert(argument);
        std::cout << "array read " << read_symbol->key_value_list_    \
            ->Lookup("identifier")->ToString() << std::endl;
        ArrayAccessSubscripts subscripts;
        for (auto argument_obj : argument->argument_list_->list_) {
          auto constant_var = dynamic_cast<ConstantVar*>(argument_obj);
          assert(constant_var);
          subscripts.push_back(constant_var->node_);
        }
        array_read_table_[read_symbol].push_back(subscripts);
      }
    } else if(auto rconst_var = dynamic_cast<ConstantVar*>(read_node)) {
      if (auto rsymbol = dynamic_cast<Symbol*>(rconst_var->node_)) {
        if (rsymbol->scope_level_ >= loop_scope_level) continue;
        scalar_read_set_.insert(rsymbol);
        std::cout << "scalar read " << rsymbol->key_value_list_      \
            ->Lookup("identifier")->ToString() << std::endl;
      }
    }
  }
}

void
DependenceAnalysis::GetDependentAccessPairs() {
  if (!scalar_write_set_.empty()) {
    dependence_checker_.SetAlwaysDependent();
    return;
  }

  for (const auto &write_access : array_write_table_) {
    Symbol *write_symbol = write_access.first;
    const auto &subscripts = write_access.second;
    for (auto subscript = subscripts.begin(); subscript != subscripts.end();
         subscript++) {
      auto subscript2 = subscript;
      for (subscript2++; subscript2 != subscripts.end();
           subscript2++) {
        dependence_checker_.AddAccessPair(write_symbol, *subscript, *subscript2,
                                          sub_expr_table_, loop_index_tmp_);
      }
    }

    for (const auto &read_access : array_read_table_) {
      Symbol *read_symbol = read_access.first;
      if (write_symbol == read_symbol) {
        const auto &read_subscripts = read_access.second;
        for (auto subscript = subscripts.begin(); subscript != subscripts.end();
         subscript++) {
          for (auto read_subscript = read_subscripts.begin();
               read_subscript != read_subscripts.end();
               read_subscript++) {
            dependence_checker_.AddAccessPair(write_symbol, *subscript,
                                              *read_subscript, sub_expr_table_,
                                              loop_index_tmp_);
          }
        }
      }
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

  loop_index_tmp_ = FindLoopIndexTmpVar(
      static_cast<NodeList*>(loop_->block_scope_));

  std::cout << "found foreach loop " << (void *) loop_ << std::endl;
  PopulateSubExprTable(static_cast<NodeList*>(main_func->block_scope_));

  PopulateSymbolAccessTables(loop_);
  GetDependentAccessPairs();
  return true;
}

DependenceGraph
DependenceAnalysis::ComputeDependenceGraph(const IterationSpace &iteration_space) {
  DependenceGraph dg(iteration_space);
  size_t num_iterations = iteration_space.GetNumberIterations();
  for (IterId i = 0; i < num_iterations; i++) {
    for (IterId j = i + 1; j < num_iterations; j++) {
      IterVec vi = iteration_space.GetIterationVector(i);
      IterVec vj = iteration_space.GetIterationVector(j);
      if (dependence_checker_.Evaluate(vi, vj)) {
        dg.AddEdge(i, j);
      }
    }
  }
  return dg;
}

Op *
GetOp(Node *subscript, const SubExprTable &sub_expr_table,
      Identifier *loop_index_tmp) {
  std::cout << __func__ << std::endl;
  if (auto int_val = dynamic_cast<Int*>(subscript)) {
    return new ConstantOp(int_val->value_);
  }

  auto id = dynamic_cast<Identifier*>(subscript);
  assert(id);
  auto stmt_iter = sub_expr_table.find(id->value_);
  assert(stmt_iter != sub_expr_table.end());
  auto stmt = stmt_iter->second;
  std::string header = stmt->statement_header_->header_->value_;
  if (header == "array_read") {
    auto read_set = stmt->read_set_;
    assert(read_set.size() == 1);
    auto read_node = *(read_set.begin());
    auto read_array_acc = dynamic_cast<ArrayAccess*>(read_node);
    assert(read_array_acc);
    auto read_var = dynamic_cast<ConstantVar*>(read_array_acc->constant_var_);
    assert(read_var);
    auto read_id = dynamic_cast<Identifier*>(read_var->node_);
    assert(read_id);
    if (read_id->value_ == loop_index_tmp->value_) {
      auto argument = dynamic_cast<Argument*>(*(read_array_acc->arguments_->list_.begin()));
      assert(argument);
      auto argument_iter = argument->argument_list_->list_.begin();
      assert(argument_iter != argument->argument_list_->list_.end());
      auto argument_obj = *argument_iter;
      auto constant_var = dynamic_cast<ConstantVar*>(argument_obj);
      assert(constant_var);
      auto index = dynamic_cast<Int*>(constant_var->node_);
      assert(index);
      return new IndexReadOp(index->value_);
    } else {
      // Loop access must be affine
      assert(0);
    }
  }
  assert(0);
  return nullptr;
}

void
AccessDependenceChecker::Initialize(const SubExprTable &sub_expr_table,
                                    Identifier *loop_index_tmp) {
  for (auto subscript : access1_) {
    op1_.push_back(GetOp(subscript, sub_expr_table, loop_index_tmp));
  }

  for (auto subscript : access2_) {
    op2_.push_back(GetOp(subscript, sub_expr_table, loop_index_tmp));
  }
}

bool
AccessDependenceChecker::Evaluate(const IterVec & iter1, const IterVec &iter2) {
  for (size_t i = 0; i < op1_.size(); ++i) {
    size_t ac1 = op1_[i]->evaluate(iter1);
    size_t ac2 = op2_[i]->evaluate(iter2);
    if (ac1 != ac2) return false;
  }
  return true;
}
