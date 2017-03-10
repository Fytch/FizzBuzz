#include <cstddef>
#include <type_traits>
#include <utility>
#include <ostream>

namespace fizzbuzz {
	template< typename... T >
	struct typelist {
		static constexpr std::size_t size = sizeof...( T );
	};

	namespace detail {
		template< typename... T >
		struct concat_impl {
		};
		template<>
		struct concat_impl<> {
			using t = typelist<>;
		};
		template< typename... head, typename... tail >
		struct concat_impl< typelist< head... >, typelist< tail... > > {
			using t = typelist< head..., tail... >;
		};
		template< typename... head, typename... tail >
		struct concat_impl< typelist< head... >, tail... > {
			using t = typename concat_impl< typelist< head... >, typename concat_impl< tail... >::t >::t;
		};
		template< typename head, typename... tail >
		struct concat_impl< head, tail... > {
			using t = typename concat_impl< typelist< head >, tail... >::t;
		};
	}
	template< typename... T >
	using concat = typename detail::concat_impl< T... >::t;

	template< char c >
	struct char_ {
		static constexpr auto v = c;
	};
	template< int i >
	struct int_ {
		static constexpr auto v = i;
	};

	namespace detail {
		template< typename list >
		struct discard_trailing_zeroes {
		};
		template< typename head, typename... tail >
		struct discard_trailing_zeroes< typelist< head, tail... > > {
			using t = concat< head, typename discard_trailing_zeroes< typelist< tail... > >::t >;
		};
		template< typename... tail >
		struct discard_trailing_zeroes< typelist< char_< '\0' >, tail... > > {
			using t = typelist<>;
		};
		template<>
		struct discard_trailing_zeroes< typelist<> > {
			using t = typelist<>;
		};
	}
#define LITERAL2LIST_1( str, i )	\
	::fizzbuzz::char_< ( sizeof( str ) / sizeof( *str ) > ( i ) ? str[ ( i ) ] : '\0' ) >
#define LITERAL2LIST_2( str, i )	\
	LITERAL2LIST_1( str, i + 0 ),	\
	LITERAL2LIST_1( str, i + 1 )
#define LITERAL2LIST_4( str, i )	\
	LITERAL2LIST_2( str, i + 0 ),	\
	LITERAL2LIST_2( str, i + 2 )
#define LITERAL2LIST_8( str, i )	\
	LITERAL2LIST_4( str, i + 0 ),	\
	LITERAL2LIST_4( str, i + 4 )
#define LITERAL2LIST_16( str, i )	\
	LITERAL2LIST_8( str, i + 0 ),	\
	LITERAL2LIST_8( str, i + 8 )
#define LITERAL2LIST( str ) std::enable_if_t< ( sizeof( str ) - 1 < 16 ), ::fizzbuzz::detail::discard_trailing_zeroes< ::fizzbuzz::typelist< LITERAL2LIST_16( str, 0 ) > >::t >

	inline std::ostream& operator<<( std::ostream& stream, typelist<> ) {
		return stream;
	}
	template< char... c >
	std::ostream& operator<<( std::ostream& stream, typelist< char_< c >... > ) {
		constexpr char s[] = { c... };
		return stream.write( s, sizeof( s ) / sizeof( *s ) );
	}
	template< int head, int... tail >
	std::ostream& operator<<( std::ostream& stream, typelist< int_< head >, int_< tail >... > ) {
		return stream << head << ' ' << typelist< int_< tail >... >{};
	}
	template< int head >
	std::ostream& operator<<( std::ostream& stream, typelist< int_< head > > ) {
		return stream << head;
	}

	namespace detail {
		template< std::size_t n, int a0, int d >
		struct arithmetic_progression_impl {
			using t = concat< typename arithmetic_progression_impl< n / 2, a0, d >::t, typename arithmetic_progression_impl< n - n / 2, a0 + n / 2 * d, d >::t >;
		};
		template< int d, int a0 >
		struct arithmetic_progression_impl< 0, a0, d > {
			using t = typelist<>;
		};
		template< int d, int a0 >
		struct arithmetic_progression_impl< 1, a0, d > {
			using t = typelist< int_< a0 > >;
		};
	}
	template< std::size_t n, int a0 = 0, int d = 1 >
	using arithmetic_progression = typename detail::arithmetic_progression_impl< n, a0, d >::t;

	namespace detail {
		template< typename list, typename separator >
		struct separate_impl {
		};
		template< typename head, typename... tail, typename separator >
		struct separate_impl< typelist< head, tail... >, separator > {
			using t = concat< head, separator, typename separate_impl< typelist< tail... >, separator >::t >;
		};
		template< typename head, typename separator >
		struct separate_impl< typelist< head >, separator > {
			using t = typelist< head >;
		};
	}
	template< typename list, typename separator >
	using separate = typename detail::separate_impl< list, separator >::t;

	namespace detail {
		template< typename list, template< typename > class meta_functor >
		struct transform_impl {
		};
		template< typename head, typename... tail, template< typename > class meta_functor >
		struct transform_impl< typelist< head, tail... >, meta_functor > {
			using t = concat< typename meta_functor< head >::t, typename transform_impl< typelist< tail... >, meta_functor >::t >;
		};
		template< template< typename > class meta_functor >
		struct transform_impl< typelist<>, meta_functor > {
			using t = typelist<>;
		};
	}
	template< typename list, template< typename > class meta_functor >
	using transform = typename detail::transform_impl< list, meta_functor >::t;

	namespace detail {
		template< unsigned i >
		struct log10_impl {
			static constexpr int v = 1 + log10_impl< i / 10 >::v;
		};
		template<>
		struct log10_impl< 0 > {
			static constexpr int v = -1;
		};
	}
	template< unsigned i >
	static constexpr int log10 = detail::log10_impl< i >::v;

	namespace detail {
		template< unsigned i >
		struct pow10_impl {
			static constexpr unsigned v = pow10_impl< i / 2 >::v * pow10_impl< i - i / 2 >::v;
		};
		template<>
		struct pow10_impl< 1 > {
			static constexpr unsigned v = 10;
		};
		template<>
		struct pow10_impl< 0 > {
			static constexpr unsigned v = 1;
		};
	}
	template< unsigned i >
	static constexpr unsigned pow10 = detail::pow10_impl< i >::v;

	namespace detail {
		template< int i, typename = std::make_integer_sequence< int, 1 + log10< i > > >
		struct int2str_impl_impl {
		};
		template< int i, int... indices >
		struct int2str_impl_impl< i, std::integer_sequence< int, indices... > > {
			using t = typelist< char_< '0' + i / pow10< sizeof...( indices ) - 1 - indices > % 10 >... >;
		};

		template< int i, bool neg = ( i < 0 ) >
		struct int2str_impl {
			using t = concat< char_< '-' >, typename int2str_impl< -i >::t >;
		};
		template< int i >
		struct int2str_impl< i, false > {
			using t = typename int2str_impl_impl< i >::t;
		};
		template<>
		struct int2str_impl< 0, false > {
			using t = typelist< char_< '0' > >;
		};
	}
	template< typename T >
	struct int2str {
		using t = T;
	};
	template< int i >
	struct int2str< int_< i > > {
		using t = typename detail::int2str_impl< i >::t;
	};

	template< int i, typename T >
	struct divisibility_replacement_rule {
		static constexpr auto v = i;
		using t = T;
	};
	template< typename ruleset >
	struct divisibility_replacer {
		template< int x, typename = ruleset >
		struct match {
		};
		template< int x, typename head, int... i, typename... tail >
		struct match< x, typelist< head, divisibility_replacement_rule< i, tail >... > > {
			using tail_rec = match< x, typelist< divisibility_replacement_rule< i, tail >... > >;
			static constexpr bool matched_here = ( x % head::v == 0 );
			static constexpr bool matched = matched_here || tail_rec::matched;
			using t_here = std::conditional_t< matched_here, typename head::t, typelist<> >;
			using t = concat< t_here, typename tail_rec::t >;
		};
		template< int x >
		struct match< x, typelist<> > {
			static constexpr bool matched = false;
			using t = typelist<>;
		};

		template< typename U >
		struct functor {
			using t = U;
		};
		template< int x >
		struct functor< int_< x > > {
			using match_result = match< x >;
			using t = std::conditional_t< match_result::matched, typename match_result::t, int_< x > >;
		};
	};

	struct fizzbuzz_replacer :
		divisibility_replacer<
			typelist<
				divisibility_replacement_rule< 3, LITERAL2LIST( "Fizz" ) >,
				divisibility_replacement_rule< 5, LITERAL2LIST( "Buzz" ) >
			>
		>
	{
	};

	template< int min = 1, int max = 100, typename = std::enable_if_t< ( min < max ) > >
	using fizzbuzz = transform< transform< separate< arithmetic_progression< max - min + 1, min, 1 >, LITERAL2LIST( ", " ) >, fizzbuzz_replacer::functor >, int2str >;
}
