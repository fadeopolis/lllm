
#include "lllm/analyzer/Ast.hpp"

using namespace lllm;
using namespace lllm::analyzer;

Ast::Ast( const Ast::Type& t, const reader::SourceLocation& loc ) : type( t ), location( loc ) {}

//***** ATOMS                ****************************************************************//
Nil::Nil( const reader::SourceLocation& loc ) : Ast( Type::Nil, loc ) {}
Int::Int( const reader::SourceLocation& loc, long value )    : Ast( Type::Int,    loc ), value( value ) {}
Real::Real( const reader::SourceLocation& loc, double value ) : Ast( Type::Real,   loc ), value( value ) {}
Char::Char( const reader::SourceLocation& loc, char value )   : Ast( Type::Char,   loc ), value( value ) {}
String::String( const reader::SourceLocation& loc, CStr value ) : Ast( Type::String, loc ), value( value ) {}
Variable::Variable( const reader::SourceLocation& loc, const util::InternedString& name, AstPtr val, StorageType type ) :
	Ast( Type::Variable, loc ), name( name ), value( val ), storage( type ) {}
Builtin::Builtin( const util::InternedString& name, TypeSet possibleTypes ) : 
	Ast( Type::Builtin, reader::SourceLocation("*builtin*") ), 
	name( name ),
	_possibleTypes( possibleTypes ) {}
/*
GlobalVariable::GlobalVariable( const reader::SourceLocation& loc, const util::InternedString& str, AstPtr value ) :
	Variable( Type::GlobalVariable, loc, str ),
	value( value ) {}
Parameter::Parameter( const reader::SourceLocation& loc, const util::InternedString& str )               : Variable( Type::Parameter       , loc, str ) {}
LocalVariable::LocalVariable( const reader::SourceLocation& loc, const util::InternedString& str, AstPtr value ) :
	Variable( Type::LocalVariable, loc, str ),
	value( value ) {}
CapturedVariable::CapturedVariable( const reader::SourceLocation& loc, const util::InternedString& str ) : Variable( Type::CapturedVariable, loc, str ) {}
*/

TypeSet Nil::possibleTypes() const { return TypeSet::Nil(); }
TypeSet Int::possibleTypes() const { return TypeSet::Int(); }
TypeSet Real::possibleTypes() const { return TypeSet::Real(); }
TypeSet Char::possibleTypes() const { return TypeSet::Char(); }
TypeSet String::possibleTypes() const { return TypeSet::String(); }
TypeSet Variable::possibleTypes() const { return TypeSet::all(); }
TypeSet Builtin::possibleTypes() const { return _possibleTypes; }

//Variable::~Variable() {}

//***** SPECIAL FORMS        ****************************************************************//
Quote::Quote( const reader::SourceLocation& loc, value::ValuePtr value ) : Ast( Type::Quote, loc ), value( value ) {}	
If::If( const reader::SourceLocation& loc, AstPtr test, AstPtr thenBranch, AstPtr elseBranch ) :
	  Ast( Type::If, loc ),
	  test( test ),
	  thenBranch( thenBranch ), 
	  elseBranch( elseBranch ) {}
Do::Do( const reader::SourceLocation& loc, const std::vector<AstPtr>& exprs ) : Ast( Type::Do, loc ), exprs( exprs ) {}
Define::Define( const reader::SourceLocation& loc, const util::InternedString& name, AstPtr var ) :
	Ast( Type::Define, loc ),
	name( name ),
	var( var ) {}
Let::Let( const reader::SourceLocation& loc, const Bindings& bindings, AstPtr expr ) :
	Ast( Type::Let, loc ),
	bindings( bindings ),
	expr( expr ) {}
Lambda::Lambda( const reader::SourceLocation& loc, LambdaPtr parent, const VarList& parameters, const VarList& captured, AstPtr expr ) :
	Ast( Type::Lambda, loc ),
	parent( parent ),
	parameters( parameters ),
	capturedVariables( captured ),
	expr( expr ) {}

TypeSet Quote::possibleTypes() const { return value::typeOf( value ); }
TypeSet If::possibleTypes() const { return thenBranch->possibleTypes() | elseBranch->possibleTypes(); }
TypeSet Do::possibleTypes() const { return exprs.back()->possibleTypes(); }
TypeSet Define::possibleTypes() const { return var->possibleTypes(); }
TypeSet Let::possibleTypes() const { return expr->possibleTypes(); }
TypeSet Lambda::possibleTypes() const { return expr->possibleTypes(); }
TypeSet Application::possibleTypes() const { return fun->possibleTypes(); }

size_t Lambda::arity() const { return parameters.size(); }


//***** FUNCTION APPLICATION ****************************************************************//
Application::Application( const reader::SourceLocation& loc, AstPtr fun, const std::vector<AstPtr>& args ) : 
	Ast( Type::Application, loc ),
	fun( fun ),
	args( args ) {}















