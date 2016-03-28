CXX = g++ -std=c++11
CFLAGS = -W -Wall -Werror -Wno-sign-compare \
	-Wno-unused-parameter -O3

all: pi_compiler

pi_compiler: bin/pi_compiler

bin:
	mkdir -p bin

BISON_IR_OUTPUT := pi.tab.cc pi.tab.hh lex.pi.c
bison_ir_output: $(BISON_IR_OUTPUT)
BISON_IR_SRC := grammar/pi.ll grammar/pi.yy

run_bison_ir: $(BISON_IR_SRC)
	cd grammar; \
	bison -d pi.yy; \
	flex -olex.pi.c pi.ll

src/bison:
	mkdir -p src/bison

$(BISON_IR_OUTPUT): % : src/bison run_bison_ir
	cp grammar/$@ src/bison
	rm grammar/$@

bin/pi_compiler: bin bison_ir_output
	$(CXX) $(CFLAGS) -Wno-unused-function \
	src/bison/pi.tab.cc src/bison/lex.pi.c \
	src/ir_compiler_main.cpp -o $@

clean:
	rm -rf bin src/bison

.PHONY: clean run_bison_ir bison_ir_output
