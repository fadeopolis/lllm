
#include "lllm/analyzer.hpp"
#include "lllm/analyzer/AstIO.hpp"
#include "lllm/reader/SexprIO.hpp"
#include "lllm/util/fail.hpp"

#include <cstring>
#include <cassert>

#include <iostream>

using namespace lllm;
using namespace lllm::reader;
using namespace lllm::analyzer;

static AstPtr analyze_( reader::SexprPtr expr, ScopePtr ctx );

static AstPtr analyzeQuote( reader::ListPtr expr ) {
	using namespace value;

	if ( expr->length() != 2 ) LLLM_FAIL( expr->location << "A quote must be of the form (quote <value>) not " << expr );

	struct Visitor final {
		ValuePtr       visit( reader::IntPtr    expr ) const { return value::number( expr->value );    }
		ValuePtr       visit( reader::RealPtr   expr ) const { return value::number( expr->value );    }
		ValuePtr       visit( reader::CharPtr   expr ) const { return value::character( expr->value ); }
		ValuePtr       visit( reader::StringPtr expr ) const { return value::string( expr->value );    }
		ValuePtr       visit( reader::SymbolPtr expr ) const { return value::symbol( expr->value );    }
		value::ListPtr visit( reader::ListPtr   expr ) const { return visit( expr, 0 );                }
		value::ListPtr visit( reader::ListPtr expr, int i ) const {
			if ( i < expr->length() ) {
				return value::cons(
					(*expr)[i]->visit<ValuePtr>( *this ), 
					visit( expr, i + 1 ) 
				);
			} else {
				return value::nil();
			}
		}
	};
	return new Quote( expr->location, (*expr)[1]->visit<ValuePtr>( Visitor() ) );
}
static AstPtr analyzeIf( reader::ListPtr expr, ScopePtr ctx ) {
	if ( expr->length() != 4 ) 
		LLLM_FAIL( expr->location << ": A if must be of the form (if <test> <then> <else>) not " << expr );

	return new If(
		expr->location,
		analyze_( (*expr)[1], ctx ),	
		analyze_( (*expr)[2], ctx ),
		analyze_( (*expr)[3], ctx ) 
	);
}
static AstPtr analyzeLet( reader::ListPtr expr, ScopePtr ctx ) {
	using namespace value;

	LocalScopePtr localCtx = new LocalScope( ctx );

	if ( expr->length() != 3 ) 
		LLLM_FAIL( expr->location << ": A let must be of the form (let ((<name> <value>)...) <sexpr>) not " << expr );

	if ( reader::ListPtr list = (*expr)[1]->asList() ) {
		if ( list->length() < 1 ) 
			LLLM_FAIL( list->location << ": A let form must contain at least one binding" );

		for ( auto it = list->begin(), end = list->end(); it != end; ++it ) {
			if ( reader::ListPtr binding = (*it)->asList() ) {
				if ( binding->length() != 2 ) 
					LLLM_FAIL( binding->location << ": A binding of a let must be of the form (<name> <value>) not " << binding );

				if ( reader::SymbolPtr sym = binding->at(0)->asSymbol() ) {
					CStr   key = sym->value;
					AstPtr val = analyze_( binding->at(1), ctx );

					localCtx->addLocal( sym->location, key, val );
				} else {
					LLLM_FAIL( (*it)->location << ": The of a let must be of the form (<name> <value>) not " << (*it) );
				}
			} else {
				LLLM_FAIL( (*it)->location << ": A binding of a let must be of the form (<name> <value>) not " << (*it) );
			}
		}
	} else {
		LLLM_FAIL( (*expr)[1]->location << ": The second argument of a let must be of the form ((<name> <value>)...) not " << expr[1] );
	}

	AstPtr body   = analyze_( (*expr)[2], localCtx );
	auto bindings = localCtx->bindings();

	return new Let( expr->location, bindings, body );
}
static AstPtr analyzeDo( reader::ListPtr expr, ScopePtr ctx ) {
	if ( expr->length() < 2 ) 
		LLLM_FAIL( expr->location << ": A do form must be of the form (do <value>...), not " << expr );

	std::vector<AstPtr> exprs;

	for ( auto it = ++(expr->begin()), end = expr->end(); it != end; ++it ) {
		reader::SexprPtr sexpr = *it;

		exprs.push_back( analyze_( sexpr, ctx ) );
	}

	return new Do( expr->location, exprs );
}

static inline bool isLambda( reader::SexprPtr form );

static AstPtr analyzeDefine( reader::ListPtr expr, ScopePtr ctx ) {
	if ( expr->length() != 3 ) LLLM_FAIL( expr->location << ": A define must be of the form (define <name> <value>) not " << expr );

	if ( reader::SymbolPtr sym = (*expr)[1]->asSymbol() ) {
		CStr name = sym->value;

		reader::SexprPtr body = (*expr)[2];

		if ( isLambda( body ) ) {
			// add name to scope before analyzing body to allow for recursive functions
			LambdaScopePtr lambda = new LambdaScope( ctx );
			lambda->addParameter( expr->location, name );

			ctx = lambda;
		}

		AstPtr val = analyze_( body, ctx );

		return new Define( expr->location, new Global( expr->location, name, val ) ); 
	} else {
		LLLM_FAIL( expr->location << ": A define must be of the form (define <name> <value>) not " << expr );
	}
}
static AstPtr analyzeLambda( reader::ListPtr expr, ScopePtr ctx ) {
	if ( expr->length() != 3 ) 
		LLLM_FAIL( expr->location << ": A lambda must be of the form (lambda (<name>...) <expr>) not " << expr << " " << expr->length() );

	LambdaScopePtr localCtx = new LambdaScope( ctx );

	// check parameter list
	if ( reader::ListPtr paramNames = (*expr)[1]->asList() ) {
		for ( auto it = paramNames->begin(), end = paramNames->end(); it != end; ++it ) {
			if ( reader::SymbolPtr param = (*it)->asSymbol() ) {
				localCtx->addParameter( param->location, param->value );
			} else {
				LLLM_FAIL( (*it)->location << ": Parameters of a lambda must all be symbols, not " << (*it) );
			}
		}
	} else {
		LLLM_FAIL( expr->location << ": A lambda must be of the form (lambda (<name>...) <expr>) not " << expr );
	}

	// get variables captured from outer scopes in body
	AstPtr body = analyze_( (*expr)[2], localCtx );

	return new Lambda( expr->location, nullptr, localCtx->parameters(), localCtx->captured(), body );
}
static AstPtr analyzeApplication( reader::ListPtr expr, ScopePtr ctx ) {
	assert( expr->length() );

	AstPtr fun = analyze_( (*expr)[0], ctx );

	if ( !fun->possibleTypes().contains( value::Value::Type::Lambda ) )
//		LLLM_FAIL( expr->location << " : The head of an application must be a function, " << fun << " isn't one	" );
		LLLM_FAIL( expr->location << " : Head of app " << fun << ", typeOf is " << fun->possibleTypes() );

	std::vector<AstPtr> args;
	
	for ( auto it = ++(expr->begin()), end = expr->end(); it != end; ++it ) {
		args.push_back( analyze_( *it, ctx ) );
	}	

	return new Application( expr->location, fun, args );
}
static AstPtr analyze_( reader::SexprPtr expr, ScopePtr ctx ) {
	struct Visitor final {
		AstPtr visit( reader::IntPtr    expr, ScopePtr ctx ) const {
			return new analyzer::Int( expr->location, expr->value );
		}
		AstPtr visit( reader::RealPtr   expr, ScopePtr ctx ) const {
			return new analyzer::Real( expr->location, expr->value );
		}
		AstPtr visit( reader::CharPtr   expr, ScopePtr ctx ) const {
			return new analyzer::Char( expr->location, expr->value );
		}
		AstPtr visit( reader::StringPtr expr, ScopePtr ctx ) const {
			return new analyzer::String( expr->location, expr->value );
		}
		AstPtr visit( reader::SymbolPtr expr, ScopePtr ctx ) const {
			if ( VariablePtr var = ctx->get( expr->value ) ) {
				return var;
			} else {
				LLLM_FAIL( expr->location << ": Undefined symbol '" << expr << "'" );			
			}
		}
		AstPtr visit( reader::ListPtr   expr, ScopePtr ctx ) const {
			if ( expr->length() == 0 ) return new Nil( expr->location );
	
			if ( reader::SymbolPtr sym = (*expr)[0]->asSymbol() ) {
				// check for special forms
				if ( std::strcmp( "quote",  sym->value ) == 0 ) return analyzeQuote( expr );
				if ( std::strcmp( "if",     sym->value ) == 0 ) return analyzeIf( expr, ctx );
				if ( std::strcmp( "let",    sym->value ) == 0 ) return analyzeLet( expr, ctx );
				if ( std::strcmp( "do",     sym->value ) == 0 ) return analyzeDo( expr, ctx );
				if ( std::strcmp( "define", sym->value ) == 0 ) {
					if ( GlobalScopePtr globals = dynamic_cast<GlobalScopePtr>( ctx ) )
						return analyzeDefine( expr, globals );
					else
						LLLM_FAIL( expr->location << " : define forms may only appear at the top level" );
				}
				if ( std::strcmp( "lambda", sym->value ) == 0 ) return analyzeLambda( expr, ctx );
			}

			return analyzeApplication( expr, ctx );
		}
	};

	return expr->visit<AstPtr,Visitor,ScopePtr>( Visitor(), ctx );
}

AstPtr lllm::analyze( reader::SexprPtr expr, GlobalScopePtr scope ) {
//	if ( reader::ListPtr form = expr->asList() ) {
//		if ( reader::SymbolPtr sym = (*form)[0]->asSymbol() ) {
//			if ( std::strcmp( "define", sym->value ) == 0 ) return analyzeDefine( expr, scope );
//		}		
//	} else {
		return analyze_( expr, scope );
//	}
}

bool isLambda( reader::SexprPtr expr ) {
	if ( reader::ListPtr form = expr->asList() ) {
		if ( reader::SymbolPtr sym = (*form)[0]->asSymbol() ) {
			if ( std::strcmp( "lambda", sym->value ) == 0 ) return true;
		}
	}
	return false;
}

