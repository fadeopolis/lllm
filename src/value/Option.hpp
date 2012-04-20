#ifndef __OPTION_HPP__
#define __OPTION_HPP__ 1

namespace lllm {
	template<typename T> 
	class Option {
		private:
			inline constexpr Option( bool flag ) : flag( flag ) {}
			bool flag;
		friend class Some;
		friend class None;
	};

	template<typename T>
	class Some : public Option<T> {
		public:
			inline constexpr Some( const T&  t ) : val( t ) {}
			inline constexpr Some( const T&& t ) : val( t ) {}

			

			inline constexpr T&       get()       { return val; }
			inline constexpr const T& get() const { return val; }
		private:
			T val;
	};
	template<typename T>
	class None : public Option<T> {

	};
}

#endif /* __OPTION_HPP__ */

