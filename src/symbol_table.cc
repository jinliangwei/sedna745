#include "symbol_table.h"

void SymbolTable::AddScope() {
  scopes_.push_front(ScopeMap());
}

void SymbolTable::AddSymbol(Symbol *symbol) {
  std::string symbol_name;

  for (auto kv : symbol->key_value_list_->list_)
    if (kv->key_->ToString() == "identifier")
      symbol_name = kv->value_->ToString();

  // There must be an "identifier" within %symbol definition.
  assert(symbol_name != "");

  auto t = scopes_.front().insert(std::make_pair(symbol_name, symbol));

  // A symbol name can be used only once in a given scope.
  assert(t.second);
}

Symbol* SymbolTable::Lookup(std::string symbol) {
  for (const auto& scope : scopes_) {
    auto i = scope.find(symbol);
    if (i != scope.end())
      return i->second;
  }
  return nullptr;
}
