#pragma once
#include "types.h"
#include "dependence_graph.hpp"
#include <map>
#include <string>
#include <list>
#include <vector>
#include <set>

using SubExprTable = std::map<std::string, SingleStatement*>;
using ArrayAccessSubscripts = std::vector<std::string>;
using SymbolArrayAccessTable = std::map<Symbol*, std::list<ArrayAccessSubscripts>>;
using SymbolScalaAccessSet = std::set<Symbol*>;

class DependenceAnalysis {
 private:
  SubExprTable sub_expr_table_;
  Program* program_;
  CompoundStatement *loop_;
  SymbolArrayAccessTable array_read_table_;
  SymbolArrayAccessTable array_write_table_;
  SymbolScalaAccessSet scalar_read_set_;
  SymbolScalaAccessSet scalar_write_set_;

  Function *FindMainFunction(Program *program);
  // find first foreach loop by DFS
  CompoundStatement *FindFirstForEachLoop(NodeList *node_list);
  void PopulateSubExprTable(NodeList *stmts);
  void PopulateSymbolAccessTables(CompoundStatement *loop);
 public:
  DependenceAnalysis(Program* program):
      program_(program) { }

  bool Initialize();
  DependenceGraph ComputeDependenceGraph();
};
