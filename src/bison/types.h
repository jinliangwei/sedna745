#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <list>
#include <iostream>
#include <cassert>
#include <algorithm>

struct Node {
  virtual void Print() = 0;
  virtual ~Node() {};
};

struct Value : public Node {
  virtual void Print() = 0;
};

struct Int : public Value {
  Int(int value) : value_(value) {}
  void Print() {
    std::cout << value_;
  }
  
  int value_;
};
  
struct Float : public Value {
  Float(float value) : value_(value) {}
  void Print() {}

  float value_;
};
  
struct String : public Value {
  String(std::string value) : value_(value) {}
  void Print() {
    std::cout << value_;
  }

  std::string value_;
};

struct NodeList : public Node {
  void Print() {}
  void push_front(Node *node) {
    list_.push_front(node);
  }
  void push_back(Node *node) {
    list_.push_back(node);
  }

  std::list<Node *> list_;
};
  
struct IntList : public NodeList {
  void Print() {
    int i = list_.size();
    for (auto j : list_) {
      j->Print();
      if (--i)
        std::cout << " ";
    }
  }
};
  
struct Type : public Node {};

struct PrimitiveType : public Type {
  PrimitiveType(String *type) : type_(type) {}
  void Print() {
    type_->Print();
  }

  String *type_;
};

struct NdArrayType : public Type {
  NdArrayType(Int *dimension, Type *type)
      : dimension_(dimension), type_(type) {}
  void Print() {
    std::cout << "ndarray<";
    dimension_->Print();
    std::cout << ", ";
    type_->Print();
    std::cout << ">";
  }

  Int *dimension_;
  Type *type_;
};

struct NdArrayDomainType : public Type {
  NdArrayDomainType(Type *ndarray_type,
                    String *domain_op,
                    String *identifier)
      : ndarray_type_(ndarray_type),
        domain_op_(domain_op),
        identifier_(identifier) {}
  void Print() {}

  Type *ndarray_type_;
  String *domain_op_;
  String *identifier_;
};

struct ScalarArrayType : public Value {
  ScalarArrayType(Type *type, Int *dimension) : type_(type), dimension_(dimension) {}
  void Print() {
    type_->Print();
    if (dimension_->value_ == -1)
      return;
    if (dimension_->value_ == -2)
      std::cout << "[]";
    else {
      std::cout << "[";
      dimension_->Print();
      std::cout << "]";
    }
  }

  Type *type_;
  Int *dimension_;
};

struct PrimarySymbolTableReference : public Node {
  PrimarySymbolTableReference(String *symbol) : symbol_(symbol) {}
  void Print() {
    std::cout << "@symbol_table[";
    symbol_->Print();
    std::cout << "]";
  }

  String *symbol_;
};
      
struct SymbolTableReference : public Value {
  void Print() {
    for (auto i : list_.list_) {
      if (IntList* t = dynamic_cast<IntList*>(i)) {
        std::cout << "[";
        t->Print();
        std::cout << "]";
      } else if (String* t = dynamic_cast<String*>(i)) {
        std::cout << ".";
        t->Print();
      } else {
        i->Print();
      }
    }
  }

  NodeList list_;
};

struct ValueList : public Value {
  void Print() {
    for (auto i : list_.list_)
      i->Print();
  }

  NodeList list_;
};

struct ArrayValue : public Value {
  ArrayValue(ValueList *value_list) : value_list_(value_list) {}
  void Print() {
    std::cout << "[";
    if (value_list_)
      value_list_->Print();
    std::cout << "]";
  }

  ValueList *value_list_;
};
  
struct KeyValue : public Node {
  KeyValue(String *key, Value *value) : key_(key), value_(value) {}
  void Print() {
    key_->Print();
    std::cout << " : ";
    value_->Print();
  }

  String *key_;
  Value *value_;
};

struct KeyValueList : public NodeList {
  void Print() {
    int i = list_.size();
    for (auto j : list_) {
      j->Print();
      if (--i)
        std::cout << ",";
      std::cout << "\n";
    }
  }
};

struct Attributes : public Node {
  Attributes(KeyValueList *key_value_list) : key_value_list_(key_value_list) {}
  void Print() {}

  KeyValueList *key_value_list_;
};

struct StatementHeader : public Node {
  StatementHeader(String *header) : header_(header) {}
  void Print() {
    std::cout << "@";
    header_->Print();
  }
  
  String *header_;
};
  
struct Symbol : public Node {
  Symbol(KeyValueList *key_value_list) : key_value_list_(key_value_list) {}
  void Print() {
    std::cout << "%symbol {\n";
    key_value_list_->Print();
    std::cout << "}";
  }

  KeyValueList *key_value_list_;
};

struct Program : public NodeList {
  void Print() {
    for (auto i : list_) {
      i->Print();
      std::cout << "\n\n";
    }
  }
};

struct BlockScope : public NodeList {
  void Print() {}
};

struct Function : public Node {
  Function(Attributes* attributes, BlockScope *block_scope)
      : attributes_(attributes), block_scope_(block_scope) {}
  void Print() {}

  Attributes *attributes_;
  BlockScope *block_scope_;
};

struct ConstantVar : public Node {
  ConstantVar(Node *node) : node_(node) {}
  void Print() {}

  Node *node_;
};

struct ConstantVarList : public NodeList {
  void Print() {}
};

struct Statement : public Node {};

struct CompoundStatement : public Statement {
  CompoundStatement(StatementHeader *statement_header,
                    Attributes *attributes,
                    BlockScope *block_scope)
      : statement_header_(statement_header),
        attributes_(attributes),
        block_scope_(block_scope) {}
  void Print() {}

  StatementHeader *statement_header_;
  Attributes *attributes_;
  BlockScope *block_scope_;
};

struct ArgumentList : public NodeList {
  void Print() {}
};

struct Arguments : public NodeList {
  void Print() {}
};

struct Argument : public Node {
  Argument(ArgumentList *argument_list) : argument_list_(argument_list) {}
  void Print() {}

  ArgumentList *argument_list_;
};

struct SingleStatement : public Statement {
  SingleStatement(StatementHeader *statement_header,
                  ConstantVarList *constant_var_list,
                  Arguments *arguments)
      : statement_header_(statement_header),
        constant_var_list_(constant_var_list),
        arguments_(arguments) {}
  void Print() {}

  StatementHeader *statement_header_;
  ConstantVarList *constant_var_list_;
  Arguments *arguments_;
};

#endif
