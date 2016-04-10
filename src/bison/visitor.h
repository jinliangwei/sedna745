#ifndef VISITOR_H_
#define VISITOR_H_

struct Int;
struct Value;
struct Float;
struct String;
struct NodeList;
struct IntList;
struct PrimitiveType;
struct NdArrayType;
struct NdArrayDomainType;
struct ScalarArrayType;
struct PrimarySymbolTableReference;
struct SymbolTableReference;
struct ValueList;
struct ArrayValue;
struct KeyValue;
struct KeyValueList;
struct Attributes;
struct StatementHeader;
struct Symbol;
struct Program;
struct BlockScope;
struct Function;
struct ConstantVar;
struct CompoundStatement;
struct ArgumentList;
struct Arguments;
struct Argument;
struct SingleStatement;
struct ArrayAccess;

class Visitor {
 public:
  virtual void Visit(Int* int_value) = 0;
  virtual void Visit(Value* value) = 0;
  virtual void Visit(Float* float_value) = 0;
  virtual void Visit(String* string_value) = 0;
  virtual void Visit(NodeList* node_list) = 0;
  virtual void Visit(IntList* int_list) = 0;
  virtual void Visit(PrimitiveType* primitive_type) = 0;
  virtual void Visit(NdArrayType* nd_array_type) = 0;
  virtual void Visit(NdArrayDomainType* nd_array_domain_type) = 0;
  virtual void Visit(ScalarArrayType* scalar_array_Type) = 0;
  virtual void Visit(PrimarySymbolTableReference* primary_symbol_table_reference) = 0;
  virtual void Visit(SymbolTableReference* symbol_table_reference) = 0;
  virtual void Visit(ValueList* value_list) = 0;
  virtual void Visit(ArrayValue* array_value) = 0;
  virtual void Visit(KeyValue* key_value) = 0;
  virtual void Visit(KeyValueList* key_value_list) = 0;
  virtual void Visit(Attributes* attributes) = 0;
  virtual void Visit(StatementHeader* statement_header) = 0;
  virtual void Visit(Symbol* symbol) = 0;
  virtual void Visit(Program* program) = 0;
  virtual void Visit(BlockScope* block_scope) = 0;
  virtual void Visit(Function* function) = 0;
  virtual void Visit(ConstantVar* constant_var) = 0;
  virtual void Visit(CompoundStatement* compound_statement) = 0;
  virtual void Visit(ArgumentList* argument_list) = 0;
  virtual void Visit(Arguments* arguments) = 0;
  virtual void Visit(Argument* argument) = 0;
  virtual void Visit(SingleStatement* single_statement) = 0;
  virtual void Visit(ArrayAccess* array_access) = 0;
};

#endif
