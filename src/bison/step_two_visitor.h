#ifndef STEP_TWO_VISITOR_H_
#define STEP_TWO_VISITOR_H_

#include "visitor.h"
#include "symbol_table.h"

class StepTwoVisitor : public Visitor {
 public:
  StepTwoVisitor(StatementList* statements) : statements_(statements) {}
  void Visit(Int* int_value) {}
  void Visit(Value* value) {}
  void Visit(Float* float_value) {}
  void Visit(String* string_value) {}
  void Visit(NodeList* node_list) {}
  void Visit(IntList* int_list) {}
  void Visit(PrimitiveType* primitive_type) {}
  void Visit(NdArrayType* nd_array_type) {}
  void Visit(NdArrayDomainType* nd_array_domain_type) {}
  void Visit(ScalarArrayType* scalar_array_Type) {}
  void Visit(PrimarySymbolTableReference* primary_symbol_table_reference) {}
  void Visit(SymbolTableReference* symbol_table_reference) {}
  void Visit(ValueList* value_list) {}
  void Visit(ArrayValue* array_value) {}
  void Visit(KeyValue* key_value) {}
  void Visit(KeyValueList* key_value_list) {}
  void Visit(Attributes* attributes) {}
  void Visit(StatementHeader* statement_header) {}
  void Visit(Symbol* symbol);
  void Visit(Program* program);
  void Visit(BlockScope* block_scope);
  void Visit(Function* function);
  void Visit(CompoundStatement* compound_statement);
  void Visit(ConstantVar* constant_var) {}
  void Visit(ArgumentList* argument_list) {}
  void Visit(Arguments* arguments) {}
  void Visit(Argument* argument) {}
  void Visit(SingleStatement* single_statement) {}
  void Visit(ArrayAccess* array_access) {}

 private:
  StatementList* statements_;
  SymbolTable symbol_table_;
};

#endif
