
#include "lllm/ast/Ast.hpp"

#include "lllm/util/fail.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::ast;
using namespace lllm::util;

Ast::Ast( Type t, const SourceLocation& loc ) : location( loc ), type( t ) {}

//***** ATOMS                ****************************************************************//
Atom::Atom( Type type, const SourceLocation& loc )      : Ast( type, loc ) {}
Nil::Nil( const SourceLocation& loc )                   : Atom( Type::Nil, loc ) {}
Int::Int( const SourceLocation& loc, long value )       : Atom( Type::Int,    loc ), value( value ) {}
Real::Real( const SourceLocation& loc, double value )   : Atom( Type::Real,   loc ), value( value ) {}
Char::Char( const SourceLocation& loc, char value )     : Atom( Type::Char,   loc ), value( value ) {}
String::String( const SourceLocation& loc, CStr value ) : Atom( Type::String, loc ), value( value ) {}

TypeSet Nil   ::possibleTypes() const { return TypeSet::Nil();    }
TypeSet Int   ::possibleTypes() const { return TypeSet::Int();    }
TypeSet Real  ::possibleTypes() const { return TypeSet::Real();   }
TypeSet Char  ::possibleTypes() const { return TypeSet::Char();   }
TypeSet String::possibleTypes() const { return TypeSet::String(); }

size_t Atom::depth() const { return 1; }

//***** VARIABLES            ****************************************************************//
Variable::Variable( const util::SourceLocation& loc, const util::InternedString& name, const AstPtr ast, bool global ) :
  Ast( Type::Variable, loc ),
  name( name ),
  ast( ast ),
  hasGlobalStorage( global ),
  getsCaptured( false ) {/*std::cout << "CREATED VAR " << name << std::endl;*/}

VariablePtr Variable::makeGlobal( const util::SourceLocation& loc, const util::InternedString& name, const AstPtr ast ) {
	return new Variable( loc, name, ast, true );
}
VariablePtr Variable::makeLocal( const util::SourceLocation& loc, const util::InternedString& name, const AstPtr ast ) {
	return new Variable( loc, name, ast, false );
}
VariablePtr Variable::makeParameter( const util::SourceLocation& loc, const util::InternedString& name ) {
	return new Variable( loc, name, nullptr, false );
}
VariablePtr Variable::makeCaptured( const util::SourceLocation& loc, const util::InternedString& name, const AstPtr ast ) {
	return new Variable( loc, name, ast, false );
}

TypeSet Variable::possibleTypes() const { return ast ? ast->possibleTypes() : TypeSet::all(); }

size_t Variable::depth() const { return ast ? ast->depth() : 1; }

//***** SPECIAL FORMS        ****************************************************************//
Quote::Quote( const SourceLocation& loc, value::ValuePtr value ) : Ast( Type::Quote, loc ), value( value ) {}	
If::If( const SourceLocation& loc, AstPtr test, AstPtr thenBranch, AstPtr elseBranch ) :
	  Ast( Type::If, loc ),
	  test( test ),
	  thenBranch( thenBranch ), 
	  elseBranch( elseBranch ),
	  _depth( std::max( std::max( test->depth(), thenBranch->depth() ), elseBranch->depth() ) ) {}
Do::Do( const SourceLocation& loc, const std::vector<AstPtr>& exprs ) : Ast( Type::Do, loc ), exprs( exprs ) {
	_depth = 0;
	for ( auto it = begin(), _end = end(); it != _end; ++it ) {
		_depth = std::max( _depth, (*it)->depth() );
	}
}
Let::Let( const SourceLocation& loc, const Let::Bindings& bindings, AstPtr expr ) :
	Ast( Type::Let, loc ),
	bindings( bindings ),
	body( expr ) {
	_depth = body->depth();
	for ( auto it = begin(), _end = end(); it != _end; ++it ) {
		_depth = std::max( _depth, (*it).second->depth() );
	}
}
LetStar::LetStar( const SourceLocation& loc, const Let::Bindings& bindings, AstPtr expr ) :
	Ast( Type::LetStar, loc ),
	bindings( bindings ),
	body( expr ) {
	_depth = body->depth();
	for ( auto it = begin(), _end = end(); it != _end; ++it ) {
		_depth = std::max( _depth, (*it).second->depth() );
	}
}
Lambda::Lambda( const SourceLocation& loc, 
                const util::InternedString& name,
                const Bindings& params,
                const Bindings& capture,
                AstPtr body )
 : Ast( Type::Lambda, loc ),
   name( name ),
   body( body ),
   params( params ),
   capture( capture ),
   data( new value::Lambda::Data( this ) ) {
	escapes.resize( arity(), EscapeStatus::NO_ESCAPE );
}
Define::Define( const SourceLocation& loc, const util::InternedString& name, AstPtr ast ) :
	Ast( Type::Define, loc ),
	name( name ),
	expr( ast ) {}

TypeSet Quote  ::possibleTypes() const { return value::typeOf( value ); }
TypeSet If     ::possibleTypes() const { return thenBranch->possibleTypes() | elseBranch->possibleTypes(); }
TypeSet Do     ::possibleTypes() const { return exprs.back()->possibleTypes(); }
TypeSet Let    ::possibleTypes() const { return body->possibleTypes(); }
TypeSet LetStar::possibleTypes() const { return body->possibleTypes(); }
TypeSet Lambda ::possibleTypes() const { return TypeSet::Lambda(); }
TypeSet Define ::possibleTypes() const { return expr->possibleTypes();  }

size_t Quote  ::depth() const { return 1; }
size_t If     ::depth() const { return _depth; }
size_t Do     ::depth() const { return _depth; }
size_t Let    ::depth() const { return _depth; }
size_t LetStar::depth() const { return _depth; }
size_t Lambda ::depth() const { return body ? body->depth() : 1; }
size_t Define ::depth() const { return expr->depth(); }

std::vector<AstPtr>::const_iterator Do::begin() const { return exprs.begin(); }
std::vector<AstPtr>::const_iterator Do::end()   const { return exprs.end();   }

Let::Bindings::const_iterator Let::begin() const { return bindings.begin(); }
Let::Bindings::const_iterator Let::end()   const { return bindings.end();   }

LetStar::Bindings::const_iterator LetStar::begin() const { return bindings.begin(); }
LetStar::Bindings::const_iterator LetStar::end()   const { return bindings.end();   }

AstPtr Do::back() const { return exprs.back(); }

size_t Lambda::arity()   const { return params.size();  }
size_t Lambda::envSize() const { return capture.size(); }

Lambda::Iterator Lambda::params_begin() const { return params.begin(); }
Lambda::Iterator Lambda::params_end()   const { return params.end();   }

Lambda::Iterator Lambda::capture_begin() const { return capture.begin(); }
Lambda::Iterator Lambda::capture_end()   const { return capture.end();   }

#include <iostream>

EscapeStatus Lambda::paramEscape( Iterator param ) { 
	std::cout << size_t( param - params_end() ) << std::endl;	
	return EscapeStatus::NO_ESCAPE;
}
void         Lambda::paramEscape( Iterator param, EscapeStatus ) {

}

//***** FUNCTION APPLICATION ****************************************************************//
Application::Application( const SourceLocation& loc, AstPtr fun, const std::vector<AstPtr>& args ) : 
  Ast( Type::Application, loc ),
  fun( fun ),
  args( args ) {
	if ( args.size() > ::lllm::MAX_ARITY ) {
		LLLM_FAIL( "Function application with more than " << MAX_ARITY << " is not supported." );
	}

	_depth = fun->depth();
	for ( auto it = begin(), _end = end(); it != _end; ++it ) {
		_depth = std::max( _depth, (*it)->depth() );
	}
}

TypeSet Application::possibleTypes() const { return fun->possibleTypes(); }

size_t Application::depth() const { return _depth; }

Application::iterator Application::begin() const { return args.begin(); }
Application::iterator Application::end()   const { return args.end();   }
size_t                Application::arity() const { return args.size();  }














