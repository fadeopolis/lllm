
#include "SpecialForms.hpp"

using namespace lllm;

static SymbolPtr symbol_quote  = symbol("quote");
static SymbolPtr symbol_if     = symbol("if");
static SymbolPtr symbol_do     = symbol("do");
static SymbolPtr symbol_define = symbol("define");
static SymbolPtr symbol_let    = symbol("let");
static SymbolPtr symbol_lambda = symbol("lambda");
static SymbolPtr symbol_thunk  = symbol("thunk");

bool lllm::isSpecialForm( ConsPtr form ) {
	ValuePtr val = form->car;

	return val == symbol_quote  &&
		   val == symbol_if     &&
		   val == symbol_do     &&
		   val == symbol_define &&
		   val == symbol_let    &&
		   val == symbol_lambda &&
		   val == symbol_thunk;
}

bool lllm::isQuoteForm( ConsPtr form )  { return form->car == symbol_quote;  }
bool lllm::isIfForm( ConsPtr form )     { return form->car == symbol_if;     }
bool lllm::isDoForm( ConsPtr form )     { return form->car == symbol_do;     }
bool lllm::isDefineForm( ConsPtr form ) { return form->car == symbol_define; }
bool lllm::isLetForm( ConsPtr form )    { return form->car == symbol_let;    }
bool lllm::isLambdaForm( ConsPtr form ) { return form->car == symbol_lambda; }
bool lllm::isThunkForm( ConsPtr form )  { return form->car == symbol_thunk;  }

