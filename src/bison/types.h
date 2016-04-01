#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <vector>

struct Node;

struct Node {
  virtual ~Node() {};
};

struct Value : public Node {};

struct Int : public Value {
  Int(int value) : value_(value) {}
  
  int value_;
};
  
struct Float : public Value {
  Float(float value) : value_(value) {}

  float value_;
};
  
struct String : public Value {
  String(std::string value) : value_(value) {}

  std::string value_;
};

struct NodeList : public Node {
  void push_back(Node *node) {
    list_.push_back(node);
  }

  std::vector<Node *> list_;
};
  
struct IntList : public NodeList {};
  
struct Identifier : public Node {
  Identifier(std::string value) : value_(value) {}

  std::string value_;
};

struct Type : public Node {};

struct PrimitiveType : public Type {
  PrimitiveType(String *type) : type_(type) {}

  String *type_;
};

struct NdArrayType : public Type {
  NdArrayType(Int *dimension, Type *type) : dimension_(dimension), type_(type) {}

  Int *dimension_;
  Type *type_;
};

struct NdArrayDomainType : public Type {
  NdArrayDomainType(Type *ndarray_type,
                    String *domain_op,
                    String *identifier) :
      ndarray_type_(ndarray_type),
      domain_op_(domain_op),
      identifier_(identifier) {}

  Type *ndarray_type_;
  String *domain_op_;
  String *identifier_;
};

struct ScalarArrayType : public Value {
  ScalarArrayType(Type *type, Int *dimension) : type_(type), dimension_(dimension) {}

  Type *type_;
  Int *dimension_;
};

struct PrimarySymbolTableReference : public Node {
  PrimarySymbolTableReference(String *symbol) : symbol_(symbol) {}

  String *symbol_;
};
      
struct SymbolTableReference : public Value {
  NodeList list_;
};

struct ValueList : public Value {
  NodeList list_;
};

struct ArrayValue : public Value {
  ArrayValue(ValueList *value_list) : value_list_(value_list) {}

  ValueList *value_list_;
};
  
struct KeyValue : public Node {
  KeyValue(String *key, Value *value) : key_(key), value_(value) {}

  String *key_;
  Value *value_;
};

struct KeyValueList : public NodeList {};

struct Attributes : public Node {
  Attributes(KeyValueList *key_value_list) : key_value_list_(key_value_list) {}

  KeyValueList *key_value_list_;
};

struct StatementHeader : public Node {
  StatementHeader(String *header) : header_(header) {}
  
  String *header_;
};
  
#endif
