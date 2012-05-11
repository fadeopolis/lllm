
#include "lllm/ast/Ast.hpp"

#include "lllm/util/fail.hpp"

using namespace lllm;
using namespace lllm::ast;
using namespace lllm::util;

Ast::Ast( Type t, const SourceLocation& loc ) : location( loc ), type( t ) {}

//***** ATOMS                ****************************************************************//
Nil::Nil( const SourceLocation& loc ) : Ast( Type::Nil, loc ) {}
Int::Int( const SourceLocation& loc, long value )    : Ast( Type::Int,    loc ), value( value ) {}
Real::Real( const SourceLocation& loc, double value ) : Ast( Type::Real,   loc ), value( value ) {}
Char::Char( const SourceLocation& loc, char value )   : Ast( Type::Char,   loc ), value( value ) {}
String::String( const SourceLocation& loc, CStr value ) : Ast( Type::String, loc ), value( value ) {}

TypeSet Nil   ::possibleTypes() const { return TypeSet::Nil();    }
TypeSet Int   ::possibleTypes() const { return TypeSet::Int();    }
TypeSet Real  ::possibleTypes() const { return TypeSet::Real();   }
TypeSet Char  ::possibleTypes() const { return TypeSet::Char();   }
TypeSet String::possibleTypes() const { return TypeSet::String(); }

//***** VARIABLES            ****************************************************************//
Variable::Variable( const util::SourceLocation& loc, const util::InternedString& name, const util::TypeSet& types, bool global ) :
  Ast( Type::Variable, loc ),
  name( name ),
  hasGlobalStorage( global ),
  getsCaptured( false ),
  types( types ) {}

TypeSet Variable::possibleTypes() const { return types; }

/*
Variable::Variable( Type type, const SourceLocation& loc, const util::InternedString& name ) :
	Ast( type, loc ), name( name ) {}
Builtin::Builtin( const util::InternedString& name, TypeSet types ) :
	Variable( Type::Builtin, SourceLocation("*builtin*"), name ), _types( types ) {}
Global::Global( const SourceLocation& loc, const util::InternedString& name, AstPtr value ) :
	Variable( Type::Global, loc, name ), value( value ) {}
Captured::Captured( const SourceLocation& loc, const util::InternedString& name, TypeSet types ) :
	Variable( Type::Captured, loc, name ), _types( types ) {}
Parameter::Parameter( const SourceLocation& loc, const util::InternedString& name ) :
	Variable( Type::Parameter, loc, name ) {}
Local::Local( const SourceLocation& loc, const util::InternedString& name, AstPtr value ) :
	Variable( Type::Local, loc, name ), value( value ) {}

TypeSet Builtin  ::possibleTypes() const { return _types;                 }
TypeSet Global   ::possibleTypes() const { return value->possibleTypes(); }
TypeSet Captured ::possibleTypes() const { return _types;                 }
TypeSet Parameter::possibleTypes() const { return TypeSet::all();         }
TypeSet Local    ::possibleTypes() const { return value->possibleTypes(); }

bool Builtin  ::hasGlobalStorage() const { return true;  }
bool Global   ::hasGlobalStorage() const { return true;  }
bool Captured ::hasGlobalStorage() const { return false; }
bool Parameter::hasGlobalStorage() const { return false; }
bool Local    ::hasGlobalStorage() const { return false; }
*/

//***** SPECIAL FORMS        ****************************************************************//
Quote::Quote( const SourceLocation& loc, value::ValuePtr value ) : Ast( Type::Quote, loc ), value( value ) {}	
If::If( const SourceLocation& loc, AstPtr test, AstPtr thenBranch, AstPtr elseBranch ) :
	  Ast( Type::If, loc ),
	  test( test ),
	  thenBranch( thenBranch ), 
	  elseBranch( elseBranch ) {}
Do::Do( const SourceLocation& loc, const std::vector<AstPtr>& exprs ) : Ast( Type::Do, loc ), exprs( exprs ) {}
Let::Let( const SourceLocation& loc, const Let::Bindings& bindings, AstPtr expr ) :
	Ast( Type::Let, loc ),
	bindings( bindings ),
	body( expr ) {}
Lambda::Lambda( const SourceLocation&       loc, 
                LambdaPtr                   parent, 
                const util::InternedString& name,
                const Lambda::Bindings&     parameters, 
                const Lambda::Bindings&     captured, 
                AstPtr                      expr ) :
	Ast( Type::Lambda, loc ),
	parent( parent ),
	name( name ),
    data( new value::Lambda::Data( this ) ),
	parameters( parameters ),
	capturedVariables( captured ),
	body( expr ) {
	}
Define::Define( const SourceLocation& loc, const util::InternedString& name, AstPtr ast ) :
	Ast( Type::Define, loc ),
	name( name ),
	expr( ast ) {}

TypeSet Quote ::possibleTypes() const { return value::typeOf( value ); }
TypeSet If    ::possibleTypes() const { return thenBranch->possibleTypes() | elseBranch->possibleTypes(); }
TypeSet Do    ::possibleTypes() const { return exprs.back()->possibleTypes(); }
TypeSet Let   ::possibleTypes() const { return body->possibleTypes(); }
TypeSet Lambda::possibleTypes() const { return TypeSet::Lambda(); }
TypeSet Define::possibleTypes() const { return expr->possibleTypes();  }

std::vector<AstPtr>::const_iterator Do::begin() const { return exprs.begin(); }
std::vector<AstPtr>::const_iterator Do::end()   const { return exprs.end();   }

Let::Bindings::const_iterator Let::begin() const { return bindings.begin(); }
Let::Bindings::const_iterator Let::end()   const { return bindings.end();   }

AstPtr Do::back() const { return exprs.back(); }

size_t Lambda::arity()   const { return parameters.size();        }
size_t Lambda::envSize() const { return capturedVariables.size(); }

//***** FUNCTION APPLICATION ****************************************************************//
Application::Application( const SourceLocation& loc, AstPtr fun, const std::vector<AstPtr>& args ) : 
	Ast( Type::Application, loc ),
	fun( fun ),
	args( args ) {}

TypeSet Application::possibleTypes() const { return fun->possibleTypes(); }

Application::iterator Application::begin() const { return args.begin(); }
Application::iterator Application::end()   const { return args.end();   }
size_t                Application::arity() const { return args.size();  }














