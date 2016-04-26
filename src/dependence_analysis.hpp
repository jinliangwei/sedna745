#pragma once
#include "types.h"
#include "dependence_graph.hpp"
#include <map>
#include <string>
#include <list>
#include <vector>
#include <set>

using SubExprTable = std::map<std::string, SingleStatement*>;
using ArrayAccessSubscripts = std::vector<Node*>;
using SymbolArrayAccessTable = std::map<Symbol*, std::list<ArrayAccessSubscripts>>;
using SymbolScalaAccessSet = std::set<Symbol*>;

/*
 * Requirements on IR:
 * 1. Operations are performed on either symbols (i.e. symbol table references),
 * or temporary variables. The latter is immutable, i.e. assigned only once.
 * 2. Indices to array accesses are temporary variables. They must be scalar,
 * that is, they cannot be results of array write or source of array read.
 * 3. At most one operand is symbol. Symbol may appear in arguments only if the
 * statement is a member operation.
 */

class Op {
 public:
  virtual ~Op() { }
  virtual size_t evaluate(const IterVec &iter) const = 0;
};

class IndexReadOp : public Op {
 private:
  const size_t index_;
 public:
  IndexReadOp(size_t index):
      index_(index) { }
  size_t evaluate(const IterVec &iter) const {
    return iter[index_];
  }
};

class ConstantOp : public Op {
 private:
  const size_t value_;
 public:
  ConstantOp(size_t value):
      value_(value) { }
  size_t evaluate(const IterVec &iter) const {
    return value_;
  }
};

class AddOp : public Op {
 private:
  const Op* left_, *right_;
  AddOp(const Op *left, const Op *right):
      left_(left),
      right_(right) { }

  size_t evaluate(const IterVec &iter) const {
    return left_->evaluate(iter) + right_->evaluate(iter);
  }
};

class MultiplyOp : public Op {
 private:
  const Op* left_, *right_;
  MultiplyOp(const Op *left, const Op *right):
      left_(left),
      right_(right) { }

  size_t evaluate(const IterVec &iter) const {
    return left_->evaluate(iter) * right_->evaluate(iter);
  }
};

class AccessDependenceChecker {
 private:
  Symbol *symbol_;
  ArrayAccessSubscripts access1_;
  ArrayAccessSubscripts access2_;
  std::vector<Op*> op1_, op2_;
 public:
  AccessDependenceChecker(
      Symbol *symbol,
      const ArrayAccessSubscripts &access1,
      const ArrayAccessSubscripts &access2):
      symbol_(symbol),
      access1_(access1),
      access2_(access2) { }
  void Initialize(const SubExprTable &sub_expr_table,
                  Identifier *loop_index_tmp);
  bool Evaluate(const IterVec & iter1, const IterVec &iter2);
};

class IterationDependenceChecker {
 private:
  std::vector<AccessDependenceChecker> access_dependence_;
  bool always_dependent_ { false };
 public:
  IterationDependenceChecker() { }

  void AddAccessPair(
      Symbol *symbol,
      const ArrayAccessSubscripts &access1,
      const ArrayAccessSubscripts &access2,
      const SubExprTable &sub_expr_table,
      Identifier *loop_index_tmp) {
    access_dependence_.emplace_back(symbol, access1, access2);
    access_dependence_.back().Initialize(sub_expr_table, loop_index_tmp);
  }

  void SetAlwaysDependent() {
    always_dependent_ = true;
  }

  bool Evaluate(const IterVec & iter1, const IterVec &iter2) {
    if (always_dependent_) return true;
    for (auto access_pair : access_dependence_) {
      if (access_pair.Evaluate(iter1, iter2))
        return true;
    }
    return false;
  }
};

class DependenceAnalysis {
 private:
  SubExprTable sub_expr_table_;
  Program* program_;
  CompoundStatement *loop_;
  Identifier *loop_index_tmp_; // assuming there is only one tmp holding loop index
  SymbolArrayAccessTable array_read_table_;
  SymbolArrayAccessTable array_write_table_;
  SymbolScalaAccessSet scalar_read_set_;
  SymbolScalaAccessSet scalar_write_set_;
  IterationDependenceChecker dependence_checker_;

  Function *FindMainFunction(Program *program);
  // find first foreach loop by DFS
  CompoundStatement *FindFirstForEachLoop(NodeList *node_list);
  void PopulateSubExprTable(NodeList *stmts);
  Identifier* FindLoopIndexTmpVar(NodeList *stmts);
  void PopulateSymbolAccessTables(CompoundStatement *loop);
  void GetDependentAccessPairs();
 public:
  DependenceAnalysis(Program* program):
      program_(program) { }

  bool Initialize();
  DependenceGraph ComputeDependenceGraph(const IterationSpace &iteration_space);
};
