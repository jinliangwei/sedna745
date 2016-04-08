#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <list>
#include <iostream>
#include <cassert>
#include <algorithm>

struct Node {
  virtual std::string ToString() = 0;
  virtual ~Node() {};
};

struct Value : public Node {
  virtual std::string ToString() = 0;
};

struct Int : public Value {
  Int(int value) : value_(value) {}

  std::string ToString() { return std::to_string(value_); }
  
  int value_;
};
  
struct Float : public Value {
  Float(float value) : value_(value) {}

  std::string ToString() { return std::to_string(value_); }

  float value_;
};
  
struct String : public Value {
  String(std::string value) : value_(value) {}

  std::string ToString() { return value_; }

  std::string value_;
};

struct NodeList : public Node {
  std::string ToString() { return ""; }

  void push_front(Node* node) {
    list_.push_front(node);
  }
  void push_back(Node* node) {
    list_.push_back(node);
  }

  std::list<Node*> list_;
};
  
struct IntList : public NodeList {
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
};
  
struct Type : public Node {};

struct PrimitiveType : public Type {
  PrimitiveType(String* type) : type_(type) {}

  std::string ToString() {
    return type_->ToString();
  }

  String* type_;
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

  Int* dimension_;
  Type* type_;
};

struct NdArrayDomainType : public Type {
  NdArrayDomainType(Type* ndarray_type,
                    String* domain_op,
                    String* identifier)
      : ndarray_type_(ndarray_type),
        domain_op_(domain_op),
        identifier_(identifier) {}

  std::string ToString() { return ""; }

  Type* ndarray_type_;
  String* domain_op_;
  String* identifier_;
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

  NodeList list_;
};

struct ValueList : public Value {
  std::string ToString() {
    std::string r;
    for (auto i : list_.list_)
      r += i->ToString();
    return r;
  }

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

  ValueList* value_list_;
};
  
struct KeyValue : public Node {
  KeyValue(String* key, Value* value) : key_(key), value_(value) {}

  std::string ToString() {
    std::string r = key_->ToString();
    r += ": ";
    r += value_->ToString();
    return r;
  }

  String* key_;
  Value* value_;
};

struct KeyValueList : public NodeList {
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
};

struct Attributes : public Node {
  Attributes(KeyValueList* key_value_list) : key_value_list_(key_value_list) {}

  std::string ToString() {
    std::string r = "%attributes {\n";
    r += key_value_list_->ToString();
    r += "}";
    return r;
  }

  KeyValueList* key_value_list_;
};

struct StatementHeader : public Node {
  StatementHeader(String* header) : header_(header) {}

  std::string ToString() {
    std::string r = "@";
    r += header_->ToString();
    return r;
  }
  
  String* header_;
};
  
struct Symbol : public Node {
  Symbol(KeyValueList* key_value_list) : key_value_list_(key_value_list) {}

  std::string ToString() {
    std::string r = "%symbol {\n";
    r += key_value_list_->ToString();
    r += "}";
    return r;
  }

  KeyValueList* key_value_list_;
};

struct Program : public NodeList {
  std::string ToString() {
    std::string r;
    for (auto i : list_) {
      r += i->ToString();
      r += "\n\n";
    }
    return r;
  }
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
};

struct Function : public Node {
  Function(Attributes* attributes, BlockScope* block_scope)
      : attributes_(attributes), block_scope_(block_scope) {}

  std::string ToString() {
    std::string r = "%function {\n";
    r += attributes_->ToString();
    r += "\n\n";
    r += block_scope_->ToString();
    r += "}\n";
    return r;
  }

  Attributes* attributes_;
  BlockScope* block_scope_;
};

struct ConstantVar : public Node {
  ConstantVar(Node* node) : node_(node) {}

  std::string ToString() {
    return node_->ToString();
  }

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
};

struct Statement : public Node {};

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
    r += "}\n";
    return r;
  }

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
};

struct Arguments : public NodeList {
  std::string ToString() {
    std::string r;
    for (auto i : list_)
      r += i->ToString();
    return r;
  }
};

struct Argument : public Node {
  Argument(ArgumentList* argument_list) : argument_list_(argument_list) {}

  std::string ToString() {
    std::string r = "(";
    r += argument_list_->ToString();
    r += ")";
    return r;
  }

  ArgumentList* argument_list_;
};

struct SingleStatement : public Statement {
  SingleStatement(StatementHeader* statement_header,
                  ConstantVarList* constant_var_list,
                  Arguments* arguments)
      : statement_header_(statement_header),
        constant_var_list_(constant_var_list),
        arguments_(arguments) {}

  std::string ToString() {
    std::string r = statement_header_->ToString();
    r += " ";
    r += constant_var_list_->ToString();
    r += " ";
    r += arguments_->ToString();
    return r;
  }

  StatementHeader* statement_header_;
  ConstantVarList* constant_var_list_;
  Arguments* arguments_;
};

#endif
