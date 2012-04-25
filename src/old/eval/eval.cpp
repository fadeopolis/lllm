
#include "eval.hpp"
#include "Env.hpp"
#include "values.hpp"
#include "fail.hpp"

#include <cassert>
#include <cstring>
#include <string>
#include <sstream>

#include <iostream>

using namespace std;
using namespace lllm;
using namespace lllm::val;

static SymbolPtr symbol_quote  = symbol("quote");
static SymbolPtr symbol_if     = symbol("if");
static SymbolPtr symbol_do     = symbol("do");
static SymbolPtr symbol_define = symbol("define");
static SymbolPtr symbol_let    = symbol("let");
static SymbolPtr symbol_lambda = symbol("lambda");
static SymbolPtr symbol_thunk  = symbol("thunk");

static EnvPtr SPECIAL_FORM_ENV = Env::make()
	->store( "quote",  symbol_quote  )
	->store( "if",     symbol_if     )
	->store( "do",     symbol_do     )
	->store( "define", symbol_define )
	->store( "let",    symbol_let    )
	->store( "lambda", symbol_lambda )
	->store( "thunk",  symbol_thunk  )
	;

static EnvPtr BUILTIN_ENV = Env::make()
	->store( "nil",   val::nil   )
	->store( "true" , val::True  )
	->store( "false", val::False )
	;
EnvPtr lllm::builtins() { return BUILTIN_ENV; }

// special forms
static ValuePtr  evalQuote( ConsPtr body, EnvPtr env );
static ValuePtr  evalIf( ConsPtr body, EnvPtr env );
static ValuePtr  evalDo( ConsPtr body, EnvPtr env );
static EnvPtr    evalDefine( ConsPtr body, EnvPtr env );
static ValuePtr  evalLet( ConsPtr body, EnvPtr env );
static LambdaPtr evalLambda( ConsPtr body, EnvPtr env );
static ThunkPtr  evalThunk( ConsPtr body, EnvPtr env );
static ValuePtr  evalBlock( ListPtr exprs, EnvPtr env );

// helpers
static ValuePtr apply( LambdaPtr l, ListPtr args );
static ValuePtr force( ThunkPtr );

static ConsPtr mapEval( ConsPtr cons, EnvPtr env );
static inline unsigned length( ListPtr l );

static void resolveVariables( ValuePtr root, ValuePtr expr, EnvPtr env, ListPtr parameters );
static void resolveVariables( ValuePtr root, ValuePtr expr, EnvPtr env );

#define ASSERT_NUM_ARGS( FORM, NUM, SEXPR ) ({                                             \
	ListPtr sexpr = (SEXPR);                                                               \
	unsigned len      = length( sexpr );                                                   \
	unsigned expected = (NUM);                                                             \
	if ( len != expected ) {                                                               \
		LLLM_FAIL( "The special form '" #FORM "' takes exactly one argument, not " #NUM ); \
	}                                                                                      \
})

ValuePtr lllm::eval( ValuePtr v, EnvPtr e ) {
	struct Visitor final {
		static ValuePtr visit( ValuePtr v, EnvPtr e ) { return nil; }
		// stuff that evaluates to itself
		static ValuePtr visit( NilPtr    v, EnvPtr e ) { return v; }
		static ValuePtr visit( NumberPtr v, EnvPtr e ) { return v; }
		static ValuePtr visit( CharPtr   v, EnvPtr e ) { return v; }
		static ValuePtr visit( StringPtr v, EnvPtr e ) { return v; }
		static ValuePtr visit( LambdaPtr v, EnvPtr e ) { return v; }
		static ValuePtr visit( RefPtr    v, EnvPtr e ) { return v; }
		// more complex stuff
		static ValuePtr visit( SymbolPtr v, EnvPtr e ) {
			ValuePtr ptr;
			if ( !e->lookup( v->value, &ptr ) ) {
				LLLM_FAIL( "Unknown symbol: '" << v << "'" );
			} else {
				return ptr;
			}
		}
		static ValuePtr visit( ConsPtr v, EnvPtr e ) {
			ValuePtr op = v->car;

			// ** special forms
			if ( symbol_quote  == op ) return evalQuote( v, e );
			if ( symbol_if     == op ) return evalIf( v, e );
			if ( symbol_do     == op ) return evalDo( v, e );
			if ( symbol_let    == op ) return evalLet( v, e );
			if ( symbol_lambda == op ) return evalLambda( v, e );
			if ( symbol_define == op ) {
				LLLM_FAIL( "Special form define can only be used at the top level, in a lambda or in a do form" );
			}
			// ** function application
			// eval elements of the list
			ConsPtr cons = mapEval( v, e );

			if ( cons->cdr ) {
				if ( LambdaPtr lambda = castOrNil<Lambda>( cons->car ) ) {
					return apply( lambda, cons->cdr );
				} else {
					LLLM_FAIL( "Cannot apply " << cons->car << " to arguments " << cons->cdr << ", it is not a function" );
				}
			} else {
				if ( LambdaPtr lambda = castOrNil<Lambda>( cons->car ) ) {
					return apply( lambda, nil );
				} else if ( ThunkPtr thunk = castOrNil<Thunk>( cons->car ) ) {
					return force( thunk );
				} else {
					LLLM_FAIL( "Cannot apply " << cons->car << " to arguments " << cons->cdr << ", it is not a function" );
				}				
			}
		}
	};

	return visit<Visitor,ValuePtr,EnvPtr>( v, e );
};


// special forms
// (quote <val>)
ValuePtr evalQuote( ConsPtr body, EnvPtr env ) {
	ASSERT_NUM_ARGS( quote, 1, body->cdr );

	return static_cast<ConsPtr>( body->cdr )->car;
}
// (if <test> <then> <else>) 
ValuePtr evalIf( ConsPtr body, EnvPtr env ) {
	ASSERT_NUM_ARGS( if, 3, body->cdr );

	ValuePtr test     = car( body->cdr );
	ValuePtr thenPart = cadr( body->cdr );
	ValuePtr elsePart = caddr( body->cdr );

	if ( eval( test, env ) ) {
		return eval( thenPart, env );
	} else {
		return eval( elsePart, env );
	}
}
// (do exprs...)
ValuePtr evalDo( ConsPtr body, EnvPtr env ) {
	if ( !body->cdr ) {
		LLLM_FAIL( "The special form 'do' requires at least one argument." );
	}

	ConsPtr cons = static_cast<ConsPtr>( body->cdr );

	ValuePtr expr;
	while ( cons ) {
		expr = cons->car;

		if ( ConsPtr c = castOrNil<Cons>( expr ) ) {
			if ( c->car == symbol_define ) {
				env = evalDefine( c, env );
			}
		} else {
			expr = eval( expr, env );
		}

		cons = castOrNil<Cons>( cons->cdr );
	}

	return expr;
}
// (define <name> <val>)
EnvPtr evalDefine( ConsPtr body, EnvPtr env ) {
	ASSERT_NUM_ARGS( define, 2, body->cdr );

	if ( SymbolPtr symbol = castOrNil<Symbol>( cadr( body ) ) ) {
		return env->store( symbol->value, cddr( body ) );
	} else {
		LLLM_FAIL( "The first argument to special form 'define' must be a symbol" );
	}
}
// (let ((<name> <val>)...) exprs...)
ValuePtr evalLet( ConsPtr body, EnvPtr env ) {
	if ( length( body->cdr ) < 2 ) {
		LLLM_FAIL( "The special form 'let' takes at least two arguments, not " << length( body->cdr ) );
	}

	// process bindings
	if ( ConsPtr bindings = castOrNil<Cons>( cadr( body ) ) ) {
		while ( bindings ) {
			ConsPtr binding = castOrNil<Cons>( bindings->car );

			if ( !binding || length( binding ) != 2 || typeOf( binding->car ) != Type::Symbol ) {
				LLLM_FAIL( "A binding in a let form must be of the form (<symbol> <value>), not " << binding );
			}			

			SymbolPtr symbol = static_cast<SymbolPtr>( binding->car );

			env = env->store( symbol->value, cadr( binding ) );

			bindings = castOrNil<Cons>( bindings->cdr );
		}
	} else {
		LLLM_FAIL( "The first argument to special form 'let' must be a list of bindings" );
	}

	ValuePtr result;
	for ( ListPtr exprs = cddr( body ); exprs; exprs = cdr( exprs ) ) {
		result = eval( car( exprs ), env );
	} 

	return result;
}
// (lambda (<name>...) exprs...)
LambdaPtr evalLambda( ConsPtr body, EnvPtr env ) {
	if ( length( body->cdr ) < 2 ) {
		LLLM_FAIL( "The special form 'lambda' takes at least two arguments, not " << length( body->cdr ) );
	}

	// check that parameter list contains only symbols
	ValuePtr tmp = cadr( body );
	if ( tmp ) {
		if ( typeOf( tmp ) != Type::Cons ) {
			LLLM_FAIL( "The first argument of the special form 'lambda' must be a list of symbols, not " << tmp );
		}

		for ( ListPtr l = static_cast<ConsPtr>( tmp ); l; l = cdr( l ) ) {
			if ( typeOf( car( l ) ) != Type::Symbol ) {
				LLLM_FAIL( "The parameter list of a lambda form may contain only symbols, not " << car( l ) );
			}
		}
	}

	resolveVariables( body, caddr( body ), env, nil );

	return lambda( static_cast<ConsPtr>( tmp ), cddr( body ), env );
}
ThunkPtr evalThunk( ConsPtr body, EnvPtr env ) {
	if ( !body->cdr ) {
		LLLM_FAIL( "The special form 'thunk' takes at least one argument" );
	}
	
	return thunk( body->cdr, env );
}
ValuePtr evalBlock( ListPtr exprs, EnvPtr env ) {
	ValuePtr expr;

	for ( ; exprs; exprs = cdr( exprs ) ) {
		expr = car( exprs );

		if ( ConsPtr c = castOrNil<Cons>( expr ) ) {
			if ( c->car == symbol_define ) {
				env = evalDefine( c, env );
			}
		} else {
			expr = eval( expr, env );
		}
	}

	return expr;
}

ValuePtr apply( LambdaPtr l, ListPtr args ) {
	if ( length( l->parameters ) != length( args ) ) {
		LLLM_FAIL( "Cannot apply " << l << " to " << args << ": wrong number of arguments" );
	}

	EnvPtr env = l->env;

	for ( ListPtr params = l->parameters; params; params = cdr( params ), args = cdr( args ) ) {
		SymbolPtr param = static_cast<SymbolPtr>( car( params ) );

		env = env->store( param->value, car( args ) );
	}

	return evalBlock( l->body, env );
}
ValuePtr force( ThunkPtr ) {
	LLLM_FAIL( "NOT IMPLEMENTED YET" );
}

ConsPtr mapEval( ConsPtr cons, EnvPtr env ) {
	return val::cons(
		eval( cons->car, env ), 
		cons->cdr ?
			((ListPtr) mapEval( static_cast<ConsPtr>( cons->cdr ), env )) :
			nil
	);
}
unsigned length( ListPtr l ) {
	ConsPtr  cons = castOrNil<Cons>( l );
	unsigned len  = 0;
	while ( cons ) {
		len++;
		cons = castOrNil<Cons>( cons->cdr );
	}

	return len;
}

void resolveVariables( ValuePtr root, ValuePtr expr, EnvPtr env ) {
	struct Visitor {
		static void visit( ValuePtr v, ValuePtr root, EnvPtr env ) {}
		static void visit( ConsPtr  v, ValuePtr root, EnvPtr env ) {
			ValuePtr head = v->car;

			if ( symbol_quote  == head ) return;
			if ( symbol_if     == head ) return resolveVariables( root, v->cdr, env );
			if ( symbol_do     == head ) {
				for ( ListPtr exprs = v->cdr; exprs; exprs = cdr( exprs ) ) {
					ValuePtr expr = car( exprs );

					

					ConsPtr binding = cast<Cons>( car( bindings ) );
					env = env->store( binding->car, cadr( binding ) );
				}
								
			}
			if ( symbol_let    == head ) {
				for ( ListPtr bindings = static_cast<ConsPtr>( cadr( v ) ); bindings; bindings = cdr( bindings ) ) {
					ConsPtr binding = cast<Cons>( car( bindings ) );
					env = env->store( binding->car, cadr( binding ) );
				}

				resolveVariables( root, caddr( v ), env );
			}
			if ( symbol_lambda == head ) return resolveVariables( root, caddr( v ), env, castOrNil<Cons>( cadr( v ) ) );				
			if ( symbol_define == head ) return resolveVariables( root, caddr( v ), env->store( cadr( v ), nil ) );

			for ( ListPtr list = v ; list; list = cdr( list ) ) {
				resolveVariables( root, car( list ), env );
			}
		}
		static void visit( SymbolPtr v, ValuePtr root, EnvPtr env ) {
			if ( env->contains( v->value ) ) {

			} else {
				LLLM_FAIL( "Unknown symbol '" << v << "' in expression " << root );
			}
		}
	};

	visit<Visitor,void,ValuePtr,EnvPtr>( expr, root, SPECIAL_FORM_ENV->merge( env ) );
}
void resolveVariables( ValuePtr root, ValuePtr expr, EnvPtr env, ListPtr parameters ) {
	for ( ; parameters; parameters = cdr( parameters ) ) {
		env = env->store( static_cast<SymbolPtr>( car( parameters ) )->value, nil );
	}

	resolveVariables( root, expr, env );
}


