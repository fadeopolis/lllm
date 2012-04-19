#ifndef __SOURCE_HPP__
#define __SOURCE_HPP__ 1

#include <iosfwd>
#include <fstream>

namespace lllm {
	typedef unsigned int uint;

	class Source {
		public:
			virtual int  peek() const      = 0;
			virtual void consume()         = 0; // consume next character
			virtual bool consume( char c ) = 0; // consume next character iff it matches c,
			                                    // return false otherwise

			int read();

			virtual uint line() const   = 0;
			virtual uint column() const = 0;

			virtual operator void*() const = 0;
			virtual bool operator!() const = 0;
		protected:
			Source();

			virtual void print( std::ostream& ) const = 0;
		private:
			Source( const Source& ) = delete;
			Source& operator=( const Source& ) = delete;

		friend std::ostream& operator<<( std::ostream&, const Source& );
	};

	class StringSource final : public Source {
		public:
			StringSource( const char* src );
			StringSource( const char* name, const char* src );

			int  peek() const override final;
			void consume() override final;
			bool consume( char c ) override final;

			uint line() const override final;
			uint column() const override final;

			operator void*() const override final;
			bool operator!() const override final;
		protected:
			void print( std::ostream& ) const override final;
		private:
			const char *name, *str;
			int curLine, curColumn;
	};

	class FileSource final : public Source {
		public:
			FileSource( const char* name );

			int  peek() const override final;
			void consume() override final;
			bool consume( char c ) override final;

			uint line() const override final;
			uint column() const override final;

			operator void*() const override final;
			bool operator!() const override final;
		protected:
			void print( std::ostream& ) const override final;
		private:
			const char* name;
			std::ifstream file;
			int la, curLine, curColumn;
	};

	std::ostream& operator<<( std::ostream&, const Source& );
}

#endif /* __SOURCE_HPP__ */

