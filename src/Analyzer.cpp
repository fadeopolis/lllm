
#include "lllm/Analyzer.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/sexpr/SexprIO.hpp"
#include "lllm/util/fail.hpp"
#include "lllm/util/util_io.hpp"

#include <cassert>
#include <cstring>
#include <map>
#include <iostream>

using namespace lllm;
using namespace lllm::ast;
using namespace lllm::util;

typedef util::ScopePtr<ast::VariablePtr> AnalyzerScopePtr;

class LocalScope : public util::Scope<ast::VariablePtr> {
	public:
		LocalScope( util::ScopePtr<ast::VariablePtr> parent );

		bool lookup( const util::InternedString& name, ast::VariablePtr* dst ) override final;
		bool contains( const util::InternedString& name ) override final;

		void addLocal( sexpr::SymbolPtr sym, ast::AstPtr ast );

		bool containsLocal( const util::InternedString& name );

		ast::LetStar::Bindings bindings() const;
	private:
		const util::ScopePtr<ast::VariablePtr>          parent;
		ast::LetStar::Bindings                          asts;
		std::map<util::InternedString,ast::VariablePtr> vars;
};
struct LambdaScope : public util::Scope<ast::VariablePtr> {
	LambdaScope( util::ScopePtr<ast::VariablePtr> parent );

	bool lookup( const util::InternedString& name, ast::VariablePtr* dst ) override final;
	bool contains( const util::InternedString& name ) override final;
	
	void setName( sexpr::SymbolPtr );
	
	void addParam( sexpr::SymbolPtr sym );

	bool containsParam( const util::InternedString& name );

	util::InternedString    name() const;
	const Lambda::Bindings& parameters() const;
	const Lambda::Bindings& captured()   const;
private:
	const util::ScopePtr<ast::VariablePtr> parent;
	VariablePtr                            self;
	ast::Lambda::Bindings                  params;
	ast::Lambda::Bindings                  capture;
};

static AstPtr analyzeExpr( sexpr::SexprPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeQuote( sexpr::ListPtr expr );
static AstPtr analyzeIf( sexpr::ListPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeLet( sexpr::ListPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeLetStar( sexpr::ListPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeDo( sexpr::ListPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeLambda( sexpr::ListPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeDefine( sexpr::ListPtr expr, AnalyzerScopePtr ctx );
static AstPtr analyzeApplication( sexpr::ListPtr expr, AnalyzerScopePtr ctx );

static inline bool isLambda( sexpr::SexprPtr form );

AstPtr Analyzer::analyze( sexpr::SexprPtr expr, GlobalScopePtr scope ) {
	// handle define form specially
	if ( sexpr::ListPtr form = expr->asList() ) {
		if ( sexpr::length( form ) > 0 ) {
			if ( sexpr::SymbolPtr sym = sexpr::at( form, 0 )->asSymbol() ) {
				if ( "define" == sym->value ) {
					return analyzeDefine( form, scope );
				}
			}
		}
	}	

	return analyzeExpr( expr, scope );
}

AstPtr analyzeExpr( sexpr::SexprPtr expr, AnalyzerScopePtr ctx ) {
	struct Visitor final {
		AstPtr visit( sexpr::IntPtr    expr, AnalyzerScopePtr ctx ) const {
			return new Int( expr->location, expr->value );
		}
		AstPtr visit( sexpr::RealPtr   expr, AnalyzerScopePtr ctx ) const {
			return new Real( expr->location, expr->value );
		}
		AstPtr visit( sexpr::CharPtr   expr, AnalyzerScopePtr ctx ) const {
			return new Char( expr->location, expr->value );
		}
		AstPtr visit( sexpr::StringPtr expr, AnalyzerScopePtr ctx ) const {
			return new String( expr->location, expr->value );
		}
		AstPtr visit( sexpr::SymbolPtr expr, AnalyzerScopePtr ctx ) const {
			VariablePtr var;
			if ( ctx->lookup( expr->value, &var ) ) {
				return var;
			} else {
				LLLM_FAIL( expr->location << ": Undefined symbol '" << expr << "'" );			
			}
		}
		AstPtr visit( sexpr::ListPtr   expr, AnalyzerScopePtr ctx ) const {
			if ( sexpr::length( expr ) == 0 ) return new Nil( expr->location );
	
			if ( sexpr::SymbolPtr sym = sexpr::at( expr, 0 )->asSymbol() ) {
				// check for special forms
				if ( "quote"  == sym->value ) return analyzeQuote( expr );
				if ( "if"     == sym->value ) return analyzeIf( expr, ctx );
				if ( "let"    == sym->value ) return analyzeLet( expr, ctx );
				if ( "let*"   == sym->value ) return analyzeLetStar( expr, ctx );
				if ( "do"     == sym->value ) return analyzeDo( expr, ctx );
				if ( "lambda" == sym->value ) return analyzeLambda( expr, ctx );
				if ( "define" == sym->value ) {
					LLLM_FAIL( expr->location << " : define forms may only appear at the top level" );
				}
			}

			return analyzeApplication( expr, ctx );
		}
	};

//	std::cout << "ANALYZING   '" << expr << "'" << std::endl;
	auto ast = expr->visit<AstPtr>( Visitor(), ctx );
//	std::cout << "ANALYZED TO '" << ast  << "'" << std::endl;

	return ast;
}

AstPtr analyzeQuote( sexpr::ListPtr expr ) {
	using namespace value;

	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( sexpr::at( expr, 0 )->asSymbol()->value == "quote" );

	if ( sexpr::length( expr ) != 2 ) LLLM_FAIL( expr->location << "A quote must be of the form (quote <value>) not " << expr );

	struct Visitor final {
		ValuePtr       visit( sexpr::IntPtr    expr ) const { return value::number( expr->value );    }
		ValuePtr       visit( sexpr::RealPtr   expr ) const { return value::number( expr->value );    }
		ValuePtr       visit( sexpr::CharPtr   expr ) const { return value::character( expr->value ); }
		ValuePtr       visit( sexpr::StringPtr expr ) const { return value::string( expr->value );    }
		ValuePtr       visit( sexpr::SymbolPtr expr ) const { return value::symbol( expr->value );    }
		value::ListPtr visit( sexpr::ListPtr   expr ) const { return visit( expr, 0 );                }
		value::ListPtr visit( sexpr::ListPtr expr, int i ) const {
			if ( i < sexpr::length( expr ) ) {
				return value::cons(
					sexpr::at( expr, i )->visit<ValuePtr>( *this ), 
					visit( expr, i + 1 ) 
				);
			} else {
				return value::nil;
			}
		}
	};

	ValuePtr val = sexpr::at( expr, 1 )->visit<ValuePtr>( Visitor() );

	return new Quote( expr->location, val );
}

AstPtr analyzeIf( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	if ( sexpr::length( expr ) != 4 ) 
		LLLM_FAIL( expr->location << ": A if must be of the form (if <test> <then> <else>) not " << expr );

	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( sexpr::at( expr, 0 )->asSymbol()->value == "if" );

	return new If(
		expr->location,
		analyzeExpr( sexpr::at( expr, 1 ), ctx ),
		analyzeExpr( sexpr::at( expr, 2 ), ctx ),
		analyzeExpr( sexpr::at( expr, 3 ), ctx ) 
	);
}
static AstPtr analyzeLet( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	std::cout << "LET " << expr << std::endl;

	using namespace value;

	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( sexpr::at( expr, 0 )->asSymbol()->value == "let" );

	LocalScope locals( ctx );

	if ( sexpr::length( expr ) < 3 ) 
		LLLM_FAIL( expr->location << ": A let must be of the form (let (<name> <value>)... <sexpr>) not " << expr );

	const size_t numBindings = sexpr::length( expr ) - 2;	

	for ( size_t i = 0; i < numBindings; i++ ) {
		if ( sexpr::ListPtr binding = sexpr::at( expr, i + 1 )->asList() ) {
			if ( sexpr::length( binding ) != 2 ) 
				LLLM_FAIL( binding->location << ": A binding of a let must be of the form (<name> <value>) not " << binding );
			
			if ( sexpr::SymbolPtr sym = sexpr::at( binding, 0 )->asSymbol() ) {
				if ( locals.containsLocal( sym->value ) ) {
					LLLM_FAIL( sym->location << " : local variable " << sym << " declared twice" );
				}

				AstPtr ast = analyzeExpr( sexpr::at( binding, 1 ), ctx );

				locals.addLocal( sym, ast );
			} else {
				LLLM_FAIL( binding->location << ": A binding a let must be of the form (<name> <value>) not " << binding );
			}
		} else {
			LLLM_FAIL( binding->location << ": A binding a let must be of the form (<name> <value>) not " << binding );
		}
	}

	AstPtr body = analyzeExpr( sexpr::last( expr ), &locals );

	return new Let( expr->location, locals.bindings(), body );
}
static AstPtr analyzeLetStar( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	using namespace value;

	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( sexpr::at( expr, 0 )->asSymbol()->value == "let*" );

	LocalScope locals( ctx );

	if ( sexpr::length( expr ) < 3 ) 
		LLLM_FAIL( expr->location << ": A let* must be of the form (let* (<name> <value>)... <sexpr>) not " << expr );

	const size_t numBindings = sexpr::length( expr ) - 2;	

	for ( size_t i = 0; i < numBindings; i++ ) {
		if ( sexpr::ListPtr binding = sexpr::at( expr, i + 1 )->asList() ) {
			if ( sexpr::length( binding ) != 2 ) 
				LLLM_FAIL( binding->location << ": A binding of a let* must be of the form (<name> <value>) not!! " << sexpr::length( binding ) << " " << binding );
			
			if ( sexpr::SymbolPtr sym = sexpr::at( binding, 0 )->asSymbol() ) {
				if ( locals.containsLocal( sym->value ) ) {
					LLLM_FAIL( sym->location << " : local variable " << sym << " declared twice" );
				}

				AstPtr ast = analyzeExpr( sexpr::at( binding, 1 ), &locals );

				locals.addLocal( sym, ast );
			} else {
				LLLM_FAIL( binding->location << ": A binding a let* must be of the form (<name> <value>) not " << binding );
			}
		} else {
			LLLM_FAIL( binding->location << ": A binding a let* must be of the form (<name> <value>) not " << binding );
		}
	}

	AstPtr body = analyzeExpr( sexpr::last( expr ), &locals );

	return new LetStar( expr->location, locals.bindings(), body );
}
static AstPtr analyzeDo( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	if ( sexpr::length( expr ) < 2 ) 
		LLLM_FAIL( expr->location << ": A do form must be of the form (do <value>...), not " << expr );

	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( sexpr::at( expr, 0 )->asSymbol()->value == "do" );

	std::vector<AstPtr> exprs;

	for ( auto it = ++(sexpr::begin( expr )), end = sexpr::end( expr ); it != end; ++it ) {
		sexpr::SexprPtr sexpr = *it;

		exprs.push_back( analyzeExpr( sexpr, ctx ) );
	}

	return new Do( expr->location, exprs );
}

static AstPtr analyzeLambda( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	if ( sexpr::length( expr ) != 3 && sexpr::length( expr ) != 4 ) 
		LLLM_FAIL( expr->location << ": A lambda must be of the form (lambda <name>? (<name>...) <expr>) not " << expr << " " << sexpr::length( expr ) );

	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( sexpr::at( expr, 0 )->asSymbol()->value == "lambda" );

	int idx = 1;

	LambdaScope lambda( ctx );

	// if the second element of the form is a symbol this is a recursive lambda,
	// add the functions name to its scope
	if ( sexpr::SymbolPtr name = sexpr::at( expr, 1 )->asSymbol() ) {
		lambda.setName( name );
		idx++;
	}

	// check parameter list
	if ( sexpr::ListPtr params = sexpr::at( expr, idx++ )->asList() ) {
		for ( auto it = sexpr::begin( params ), end = sexpr::end( params ); it != end; ++it ) {
			if ( sexpr::SymbolPtr param = (*it)->asSymbol() ) {
				if ( lambda.containsParam( param->value ) ) {
					LLLM_FAIL( param->location << " : parameter " << param << " declared twice" );
				}

				lambda.addParam( param );
			} else {
				LLLM_FAIL( (*it)->location << ": Parameters of a lambda must all be symbols, not " << (*it) );
			}
		}
	} else {
		LLLM_FAIL( expr->location << ": A lambda must be of the form (lambda (<name>...) <expr>) not " << expr );
	}

	// get variables captured from outer scopes in body
	AstPtr body = analyzeExpr( sexpr::at( expr, idx ), &lambda );

	return new Lambda( expr->location, lambda.name(), lambda.parameters(), lambda.captured(), body );
}
static AstPtr analyzeApplication( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	assert( sexpr::length( expr ) );

	AstPtr fun = analyzeExpr( sexpr::at( expr, 0 ), ctx );

	if ( !fun->possibleTypes().contains( value::Type::Lambda ) ) {
		LLLM_FAIL( expr->location << " : The head of an application must be a function, " << fun << ", is one of " << fun->possibleTypes() );
	}

	std::vector<AstPtr> args;
	
	for ( auto it = ++sexpr::begin( expr ), end = sexpr::end( expr ); it != end; ++it ) {
		args.push_back( analyzeExpr( *it, ctx ) );
	}	

	return new Application( expr->location, fun, args );

}

AstPtr analyzeDefine( sexpr::ListPtr expr, AnalyzerScopePtr ctx ) {
	if ( sexpr::length( expr ) != 3 ) LLLM_FAIL( expr->location << ": A define must be of the form (define <name> <value>) not " << expr );

	assert( expr );
	assert( sexpr::at( expr, 0 )->asSymbol() );
	assert( "define" == sexpr::at( expr, 0 )->asSymbol()->value );

	if ( sexpr::SymbolPtr sym = sexpr::at( expr, 1 )->asSymbol() ) {
		InternedString  name = sym->value;
		sexpr::SexprPtr body = sexpr::at( expr, 2 );

		AstPtr val = analyzeExpr( body, ctx );

		return new Define( expr->location, name, val ); 
	} else {
		LLLM_FAIL( expr->location << ": A define must be of the form (define <name> <value>) not " << expr );
	}
}

LocalScope::LocalScope( util::ScopePtr<ast::VariablePtr> parent )   : parent( parent ) {}
LambdaScope::LambdaScope( util::ScopePtr<ast::VariablePtr> parent ) : parent( parent ), self( nullptr ) {}

bool LocalScope::lookup( const util::InternedString& name, ast::VariablePtr* dst ) {
	auto lb = vars.lower_bound( name );

	if ( lb != vars.end() && lb->first == name ) {
		*dst = lb->second;
		return true;
	} else {
		return parent->lookup( name, dst );
	}
}
bool LocalScope::contains( const util::InternedString& name ) {
	return (vars.count( name ) > 0) || parent->contains( name );
}
bool LocalScope::containsLocal( const util::InternedString& name ) {
	return (vars.count( name ) > 0);
}

void LocalScope::addLocal( sexpr::SymbolPtr sym, AstPtr ast ) {
	const util::InternedString& name = sym->value;

	asts.push_back( std::make_pair( name, ast ) );
	vars[name] = Variable::makeLocal( sym->location, name, ast );
}

Let::Bindings LocalScope::bindings() const {
	return asts;
}

bool LambdaScope::lookup( const util::InternedString& name, ast::VariablePtr* dst ) {
//	std::cout << "Lambda::lookup(" << name << ")" << std::endl;

	// check parameters
	for ( auto it = params.begin(), end = params.end(); it != end; ++it ) {
		const VariablePtr var = *it;
		if ( var->name == name ) {
			*dst = var;
			return true;
		}
	}
	// check for recursion
	if ( self && self->name == name ) {
		*dst = self;
		return true;
	}
	// check captured variables
	for ( auto it = capture.begin(), end = capture.end(); it != end; ++it ) {
		const VariablePtr var = *it;
		if ( var->name == name ) {
			*dst = var;
			return true;
		}
	}
	// get var from outer scope 
	VariablePtr var;

	if ( parent->lookup( name, &var ) ) {
		if ( var->hasGlobalStorage ) {
			*dst = var;
			return true;
		}

		// capture variable
		VariablePtr cap = Variable::makeCaptured( var->location, var->name, var->ast );
		var->getsCaptured = true;

		capture.push_back( cap );
				
		*dst = cap;
		return true;
	} else {
		return var;
	}
}
bool LambdaScope::contains( const util::InternedString& name ) {
	// check parameters
	if ( containsParam( name ) ) return true;	

	// check for recursion
	if ( self && self->name == name ) {
		return true;
	}
	// check captured variables
	for ( auto it = capture.begin(), end = capture.end(); it != end; ++it ) {
		const VariablePtr var = *it;
		if ( var->name == name ) {
			return true;
		}
	}
	// get var from outer scope 
	return parent->contains( name );
}
bool LambdaScope::containsParam( const util::InternedString& name ) {
	// check parameters
	for ( auto it = params.begin(), end = params.end(); it != end; ++it ) {
		const VariablePtr var = *it;
		if ( var->name == name ) {
			return true;
		}
	}

	return false;
}

void LambdaScope::setName( sexpr::SymbolPtr name ) {
	this->self = Variable::makeParameter( name->location, name->value );
}
void LambdaScope::addParam( sexpr::SymbolPtr sym ) {
	params.push_back( Variable::makeParameter( sym->location, sym->value ) );
}

util::InternedString    LambdaScope::name() const {
	return self ? self->name : "";
}
const Lambda::Bindings& LambdaScope::parameters() const {
	return params;
}
const Lambda::Bindings& LambdaScope::captured()   const {
	return capture;
}

