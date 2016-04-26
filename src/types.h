#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <list>
#include <map>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <set>

#include "visitor.h"

struct Node {
  virtual std::string ToString() = 0;
  virtual void Accept(Visitor *v) = 0;
  virtual ~Node() {};
};

using NodeSet = std::set<Node*>;

struct Value : public Node {
  virtual std::string ToString() = 0;
  virtual void Accept(Visitor *v) = 0;
};

struct Int : public Value {
  Int(int value) : value_(value) {}

  std::string ToString() { return std::to_string(value_); }
  void Accept(Visitor *v) { v->Visit(this); }

  int value_;
};

struct Float : public Value {
  Float(float value) : value_(value) {}

  std::string ToString() { return std::to_string(value_); }
  void Accept(Visitor *v) { v->Visit(this); }

  float value_;
};

struct String : public Value {
  String(std::string value) : value_(value) {}

  std::string ToString() { return value_; }
  void Accept(Visitor *v) { v->Visit(this); }

  std::string value_;
};

struct Identifier : public Value {
  Identifier(std::string value) : value_(value) {}

  std::string ToString() { return value_; }
  void Accept(Visitor *v) { v->Visit(this); }

  std::string value_;
};

struct NodeList : public Node {
  std::string ToString() { return ""; }

  void Accept(Visitor *v) {}

  void push_front(Node* node) {
    list_.push_front(node);
  }
  void push_back(Node* node) {
    list_.push_back(node);
  }

  std::list<Node*> list_;
};

struct IntList : public Node {
  std::string ToString() {
    std::string r;

    int i = list_.size();
    for (auto j : list_) {
      r += j->ToString();
      if (--i)
        r += " ";
    }

    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  void push_front(Int* int_value) {
    list_.push_front(int_value);
  }

  std::list<Int*> list_;
};

struct Type : public Node {};

struct PrimitiveType : public Type {
  PrimitiveType(Value* type) : type_(type) {}

  std::string ToString() {
    return type_->ToString();
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Value* type_;
};

struct NdArrayType : public Type {
  NdArrayType(Int* dimension, Type* type)
      : dimension_(dimension), type_(type) {}

  std::string ToString() {
    std::string r = "ndarray<";

    r += dimension_->ToString();
    r += ", ";
    r += type_->ToString();
    r += ">";

    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Int* dimension_;
  Type* type_;
};

struct NdArrayDomainType : public Type {
  NdArrayDomainType(Type* ndarray_type,
                    String* domain_op,
                    Identifier* identifier)
      : ndarray_type_(ndarray_type),
        domain_op_(domain_op),
        identifier_(identifier) {}

  std::string ToString() { return ""; }

  void Accept(Visitor *v) { v->Visit(this); }

  Type* ndarray_type_;
  String* domain_op_;
  Identifier* identifier_;
};

struct ScalarArrayType : public Value {
  ScalarArrayType(Type* type, Int* dimension)
      : type_(type), dimension_(dimension) {}

  std::string ToString() {
    std::string r = type_->ToString();

    if (dimension_->value_ == -1)
      return r;
    if (dimension_->value_ == -2)
      r += "[]";
    else {
      r += "[";
      r += dimension_->ToString();
      r += "]";
    }

    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Type* type_;
  Int* dimension_;
};

struct PrimarySymbolTableReference : public Node {
  PrimarySymbolTableReference(String* symbol) : symbol_(symbol) {}

  std::string ToString() {
    std::string r = "@symbol_table[";
    r += symbol_->ToString();
    r += "]";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  String* symbol_;
};

struct SymbolTableReference : public Value {
  std::string ToString() {
    std::string r;
    for (auto i : list_.list_) {
      if (IntList* t = dynamic_cast<IntList*>(i)) {
        r += "[";
        r += t->ToString();
        r += "]";
      } else if (String* t = dynamic_cast<String*>(i)) {
        r += ".";
        r += t->ToString();
      } else {
        r += i->ToString();
      }
    }
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  NodeList list_;
};

struct ValueList : public Value {
  std::string ToString() {
    std::string r;
    for (auto i : list_.list_)
      r += i->ToString();
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  NodeList list_;
};

struct ArrayValue : public Value {
  ArrayValue(ValueList* value_list) : value_list_(value_list) {}

  std::string ToString() {
    std::string r = "[";
    if (value_list_)
      r += value_list_->ToString();
    r += "]";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  ValueList* value_list_;
};

struct KeyValue : public Node {
  KeyValue(Identifier* key, Value* value) : key_(key), value_(value) {}

  std::string ToString() {
    std::string r = key_->ToString();
    r += ": ";
    r += value_->ToString();
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Identifier* key_;
  Value* value_;
};

struct KeyValueList : public Node {
  std::string ToString() {
    std::string r;
    int i = list_.size();
    for (auto j : list_) {
      r += j->ToString();
      if (--i)
        r += ",";
      r += "\n";
    }
    return r;
  }

  void Insert(KeyValue *kv) {
    assert(map_.find(kv->key_->value_) == map_.end());
    map_[kv->key_->value_] = kv->value_;
    list_.push_front(kv);
    delete kv;
  }

  Value *Lookup(const std::string &key) {
    auto it = map_.find(key);
    return it == map_.end() ? 0 : it->second;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  std::list<KeyValue*> list_;
  std::map<std::string, Value*> map_;
};

struct Attributes : public Node {
  Attributes(KeyValueList* key_value_list) : key_value_list_(key_value_list) {}

  std::string ToString() {
    std::string r = "%attributes {\n";
    r += key_value_list_->ToString();
    r += "}";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  KeyValueList* key_value_list_;
};

struct StatementHeader : public Node {
  StatementHeader(Identifier* header) : header_(header) {}

  std::string ToString() {
    std::string r = "@";
    r += header_->ToString();
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Identifier* header_;
};

struct Symbol : public Node {
  Symbol(KeyValueList* key_value_list) : key_value_list_(key_value_list) {}

  std::string ToString() {
    std::string r = "%symbol {\n";
    r += key_value_list_->ToString();
    r += "}";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  KeyValueList* key_value_list_;
  int scope_level_ { 0 };
};

struct Program : public NodeList {
  std::string ToString() {
    std::string r;
    int i = list_.size();
    for (auto j : list_) {
      r += j->ToString();
      if (--i)
        r += "\n";
    }
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }
};

struct BlockScope : public NodeList {
  std::string ToString() {
    std::string r;
    for (auto i : list_) {
      r += i->ToString();
      r += "\n";
    }
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }
  int scope_level_ {0};
};

struct Function : public Node {
  Function(Attributes* attributes, BlockScope* block_scope)
      : attributes_(attributes), block_scope_(block_scope) {}

  std::string ToString() {
    std::string r = "%function {\n";
    r += attributes_->ToString();
    r += "\n";
    r += block_scope_->ToString();
    r += "}";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Attributes* attributes_;
  BlockScope* block_scope_;
};

struct ConstantVar : public Node {
  ConstantVar(Node* node) : node_(node) {}

  std::string ToString() {
    return node_->ToString();
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Node* node_;
};

struct ConstantVarList : public NodeList {
  std::string ToString() {
    std::string r;
    int i = list_.size();
    for (auto j : list_) {
      r += j->ToString();
      if (--i)
        r += ", ";
    }
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }
};

struct Statement : public Node {
  // These start as empty, and are populated after StepTwoVisitor pass.
  NodeSet read_set_;
  NodeSet write_set_;
};

struct CompoundStatement : public Statement {
  CompoundStatement(StatementHeader* statement_header,
                    Attributes* attributes,
                    BlockScope* block_scope)
      : statement_header_(statement_header),
        attributes_(attributes),
        block_scope_(block_scope) {}

  std::string ToString() {
    std::string r = statement_header_->ToString();
    r += " {\n";
    r += attributes_->ToString();
    r += "\n";
    r += block_scope_->ToString();
    r += "}";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  StatementHeader* statement_header_;
  Attributes* attributes_;
  BlockScope* block_scope_;
};

struct ArgumentList : public NodeList {
  std::string ToString() {
    std::string r;
    int i = list_.size();
    for (auto j : list_) {
      r += j->ToString();
      if (--i)
        r += ", ";
    }
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }
};

struct Arguments : public NodeList {
  std::string ToString() {
    std::string r;
    for (auto i : list_)
      r += i->ToString();
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }
};

struct Argument : public Node {
  Argument(ArgumentList* argument_list) : argument_list_(argument_list) {}

  std::string ToString() {
    std::string r = "(";
    r += argument_list_->ToString();
    r += ")";
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  ArgumentList* argument_list_;
};

struct ArrayAccess : public Node {
  ArrayAccess(Node* constant_var,
              Arguments* arguments)
      : constant_var_(constant_var),
        arguments_(arguments) {}

  std::string ToString() {
    std::string r = constant_var_->ToString();
    r += " ";
    r += arguments_->ToString();
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  Node* constant_var_;
  Arguments* arguments_;
};

struct SingleStatement : public Statement {
  SingleStatement(StatementHeader* statement_header,
                  ConstantVarList* constant_var_list,
                  Arguments* arguments)
      : statement_header_(statement_header),
        constant_var_list_(constant_var_list),
        arguments_(arguments) {

    std::string stmt = statement_header_->ToString();

    ArrayAccess *ae = 0;
    if (stmt == "@array_read" ||
        stmt == "@ndarray_read") {
      ae = new ArrayAccess(constant_var_list->list_.back(), arguments_);
      constant_var_list_->list_.pop_back();
      constant_var_list_->list_.push_back(ae);
      arguments_ = nullptr;
    } else if (stmt == "@ndarray_store") {
      ae = new ArrayAccess(constant_var_list_->list_.front(), arguments_);
      constant_var_list_->list_.pop_front();
      constant_var_list_->list_.push_front(ae);
      arguments_ = nullptr;
    }
  }

  std::string ToString() {
    std::string r = statement_header_->ToString();
    r += " ";
    r += constant_var_list_->ToString();
    if (arguments_)
      r += " " + arguments_->ToString();
    return r;
  }

  void Accept(Visitor *v) { v->Visit(this); }

  StatementHeader* statement_header_;
  ConstantVarList* constant_var_list_;
  Arguments* arguments_;
};

#endif
