#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <map>
#include <string>
#include <deque>

#include "types.h"

using ScopeMap = std::map<std::string, Symbol*>;

class SymbolTable {
 public:
  void AddScope();
  void AddSymbol(Symbol *);
  Symbol* Lookup(std::string symbol);
  
 private:
  std::deque<ScopeMap> scopes_;
};

#endif
