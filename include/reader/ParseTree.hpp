#ifndef __PARSE_TREE_HPP__
#define __PARSE_TREE_HPP__ 1

#include "reader/SourceLocation.hpp"

#include <iosfwd>

namespace lllm {
	class ParseTreeVisitor;

	typedef const char* CStr;

	class ParseTree;
	class IntTree;
	class RealTree;
	class CharTree;
	class StringTree;
	class SymbolTree;
	class ListTree;

	std::ostream& operator<<( std::ostream&, const ParseTree& );
	std::ostream& operator<<( std::ostream&, const ParseTree* );

	class ParseTree {
		public:
			ParseTree( const SourceLocation& );

			virtual void getVisitedBy( ParseTreeVisitor& ) = 0;

			const SourceLocation location;

			// these are here to help testing only
			static bool equal( const ParseTree*, const ParseTree* );

			static ParseTree* number( long );
			static ParseTree* number( double );
			static ParseTree* character( char );
			static ParseTree* string( CStr );
			static ParseTree* symbol( CStr );
			static ParseTree* list( ParseTree*, ListTree* = nullptr );
	
			template<typename T, typename... Ts>
			static ParseTree* list( T t, Ts... ts ) {
				return list( t, list( ts... ) );
			}
		protected:
			virtual void printTo( std::ostream& ) const = 0;

		friend std::ostream& operator<<( std::ostream&, const ParseTree& );
		friend std::ostream& operator<<( std::ostream&, const ParseTree* );			
	};
	class NumberTree : public ParseTree {
		public:
			NumberTree( const SourceLocation& );

			virtual ~NumberTree() = 0;
	};
	class IntTree final : public NumberTree {
		public:
			IntTree( const SourceLocation&, long );

			void getVisitedBy( ParseTreeVisitor& ) override final;

			const long value;
		private:
			void printTo( std::ostream& ) const override final;
	};
	class RealTree final : public NumberTree {
		public:
			RealTree( const SourceLocation&, double );

			void getVisitedBy( ParseTreeVisitor& ) override final;

			const double value;
		private:
			void printTo( std::ostream& ) const override final;
	};
	class CharTree final : public ParseTree {
		public:
			CharTree( const SourceLocation&, char );

			void getVisitedBy( ParseTreeVisitor& ) override final;

			const char value;
		private:
			void printTo( std::ostream& ) const override final;
	};
	class StringTree final : public ParseTree {
		public:
			StringTree( const SourceLocation&, CStr );

			void getVisitedBy( ParseTreeVisitor& ) override final;

			const CStr value;
		private:
			void printTo( std::ostream& ) const override final;
	};
	class SymbolTree final : public ParseTree {
		public:
			SymbolTree( const SourceLocation&, CStr );

			void getVisitedBy( ParseTreeVisitor& ) override final;

			const CStr value;
		private:
			void printTo( std::ostream& ) const override final;
	};
	class ListTree final : public ParseTree {
		public:
			ListTree( const SourceLocation&, ParseTree*, ListTree* );

			void getVisitedBy( ParseTreeVisitor& ) override final;

			const ParseTree* car;
			const ListTree*  cdr;
		private:
			void printTo( std::ostream& ) const override final;
	};

	struct ParseTreeVisitor {
		virtual void visit( IntTree*    ) = 0;
		virtual void visit( RealTree*   ) = 0;
		virtual void visit( CharTree*   ) = 0;
		virtual void visit( StringTree* ) = 0;
		virtual void visit( SymbolTree* ) = 0;
		virtual void visit( ListTree*   ) = 0;
	};
}

#endif /* __PARSE_TREE_HPP__ */

