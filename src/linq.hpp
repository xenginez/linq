/*!
 * \file	linq.hpp
 *
 * \author	ZhengYuanQing
 * \date	2021/02/21
 * \email	zhengyuanqing.95@gmail.com
 *
 */
#ifndef LINQ_HPP__900F519C_610C_471B_81EB_933C83B3A14A
#define LINQ_HPP__900F519C_610C_471B_81EB_933C83B3A14A

namespace linq
{
	template< typename Container >
	auto from( const Container & container )
	{
		return enumerable< typename Container::const_iterator >( std::begin( container ), std::end( container ) );
	}

	template< typename Iterator >
	auto from( Iterator begin, Iterator end )
	{
		return enumerable< Iterator >( begin, end );
	}

	template< typename Container >
	auto from( const std::shared_ptr< Container > & container )
	{
		using storage_iterator_type = storage_iterator< typename Container::const_iterator, std::shared_ptr< Container > >;

		return enumerable< storage_iterator_type >(
			storage_iterator_type( container, std::begin( *container ), std::end( *container ) ),
			storage_iterator_type( container, std::end( *container ), std::end( *container ) )
			);
	}



	template< typename Iterator >
	class enumerable
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		enumerable( Iterator beg, Iterator end )
			:_begin( beg ), _end( end )
		{
		}

	public:
		auto empty() const
		{
			return _begin == _end;
		}

		auto count() const
		{
			std::size_t sz = 0;

			for( auto it = _begin; it != _end; ++it, ++sz );

			return sz;
		}

	public:
		auto begin() const
		{
			return _begin;
		}

		auto end() const
		{
			return _end;
		}

	public:
		auto first() const
		{
			if( empty() )
			{
				throw std::out_of_range( "failed to get a value from an empty collection!" );
			}

			return *_begin;
		}

		auto first_or_default( const value_type & value ) const
		{
			if( empty() )
			{
				return value;
			}

			return *_begin;
		}

		auto last() const
		{
			if( empty() )
			{
				throw std::out_of_range( "failed to get a value from an empty collection!" );
			}

			auto tmp = _begin;
			for( auto it = _begin; it != _end; ++it )
			{
				tmp = it;
			}

			return *tmp;
		}

		auto last_or_default( const value_type & value ) const
		{
			if( empty() )
			{
				return value;
			}

			auto tmp = _begin;
			for( auto it = _begin; it != _end; ++it )
			{
				tmp = it;
			}

			return *tmp;
		}

		auto at( std::size_t index ) const
		{
			auto it = _begin;

			for( std::size_t i = 0; i < index && it != _end; ++i, ++it )
			{
				if( i == index )
				{
					return *it;
				}
			}

			throw std::out_of_range( "argument out of range: index" );
		}

	public:
		template< typename Container >
		void emplace_back( Container & container )
		{
			for( auto i : *this )
			{
				container.emplace_back( i );
			}
		}

		template< typename Container >
		void push_back( Container & container )
		{
			for( auto i : *this )
			{
				container.push_back( i );
			}
		}

		template< typename Container >
		void insert( Container & container )
		{
			for( auto i : *this )
			{
				container.insert( i );
			}
		}

		template< typename Container, typename Function >
		void insert( Container & container, const Function & function )
		{
			for( auto i : *this )
			{
				container.insert( { function( i ),  i } );
			}
		}

	public:
		template< typename Function >
		auto select( const Function & function ) const
		{
			using select_iterator_type = select_iterator< Iterator, Function >;

			return from(
				select_iterator_type( _begin, function ),
				select_iterator_type( _end, function )
			);
		}

		template< typename Function >
		auto where( const Function & function ) const
		{
			using where_iterator_type = where_iterator< Iterator, Function >;

			return from(
				where_iterator_type( _begin, _end, function ),
				where_iterator_type( _end, _end, function )
			);
		}

		template< typename Count >
		auto take( Count count ) const
		{
			using take_iterator_type = take_iterator< Iterator, Count >;

			return from(
				take_iterator_type( _begin, _end, count ),
				take_iterator_type( _end, _end, count )
			);
		}

		template< typename Count >
		auto skip( Count count ) const
		{
			using skip_iterator_type = skip_iterator< Iterator >;

			return from(
				skip_iterator_type( _begin, _end, count ),
				skip_iterator_type( _end, _end, 0 )
			);
		}

		template< typename Function >
		auto take_while( const Function & function ) const
		{
			using take_while_iterator_type = take_while_iterator< Iterator, Function >;

			return from(
				take_while_iterator_type( _begin, _end, function ),
				take_while_iterator_type( _end, _end, function )
			);
		}

		template< typename Function >
		auto skip_while( const Function & function ) const
		{
			using skip_while_iterator_type = skip_while_iterator< Iterator, Function >;

			return from(
				skip_while_iterator_type( _begin, _end, function ),
				skip_while_iterator_type( _end, _end, function )
			);
		}

		template< typename Iterator2 >
		auto concat( const enumerable<Iterator2> & enumer ) const
		{
			using concat_iterator_type = concat_iterator< Iterator, Iterator2 >;

			return from(
				concat_iterator_type( _begin, _end, enumer.begin() ),
				concat_iterator_type( _end, _end, enumer.end() )
			);
		}

	public:
		template< typename Function >
		auto aggregate( const Function & function ) const
		{
			if( empty() )
			{
				throw std::out_of_range( "failed to get a value from an empty collection!" );
			}

			auto it = _begin;

			auto result = *it;

			while( ++it != _end )
			{
				result = function( result, *it );
			}

			return result;
		}

		auto sum() const
		{
			return aggregate( []( const auto & left, const auto & right ) { return left + right; } );
		}

		auto max() const
		{
			return aggregate( []( const auto & left, const auto & right ) { return std::max( left, right ); } );
		}

		auto min() const
		{
			return aggregate( []( const auto & left, const auto & right ) { return std::min( left, right ); } );
		}

		template< typename Result >
		Result average() const
		{
			if( empty() )
			{
				throw std::out_of_range( "failed to get a value from an empty collection!" );
			}

			Result sum = 0;

			std::size_t count = 0;
			for( auto it = _begin; it != _end; ++it, ++count )
			{
				sum += static_cast< Result >( *it );
			}

			return sum / count;
		}

	public:
		template< typename Container, typename Function >
		auto group_by( Container & container, const Function & function ) const
		{
			for( auto it = _begin; it != _end; ++it )
			{
				function( container, *it );
			}

			return from( container );
		}

		template< typename Container, typename Function1, typename Function2 >
		auto order_by( Container & container, const Function1 & function1, const Function2 & function2 ) const
		{
			for( auto it = _begin; it != _end; ++it )
			{
				function1( container, *it );
			}

			function2( container );

			return from( container );
		}

		template< typename Container, typename Iterator2, typename Function1, typename Function2, typename Function3 >
		auto join( Container & container, const enumerable< Iterator2 > & enumer, const Function1 & function1, const Function2 & function2, const Function3 & function3 ) const
		{
			for( auto it1 = _begin; it1 != _end; ++it1 )
			{
				for( auto it2 = enumer.begin(); it2 != enumer.end(); ++it2 )
				{
					if( function1( *it1 ) == function2( *it2 ) )
					{
						function3( container, *it1, *it2 );
					}
				}
			}

			return from( container );
		}

	private:
		Iterator _begin;
		Iterator _end;
	};



	template< typename Iterator, typename Function >
	class select_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		select_iterator( Iterator iterator, const Function & function )
			:_iterator( iterator ), _function( function )
		{
		}

	public:
		auto & operator++()
		{
			++_iterator;
			return *this;
		}

		auto operator*() const
		{
			return _function( *_iterator );
		}

		bool operator==( const select_iterator< Iterator, Function > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const select_iterator< Iterator, Function > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Iterator _iterator;
		Function _function;
	};

	template< typename Iterator, typename Function >
	class where_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		where_iterator( Iterator iterator, Iterator end, const Function & function )
			:_iterator( iterator ), _end( end ), _function( function )
		{
			while( _iterator != _end && !_function( *_iterator ) )
			{
				++_iterator;
			}
		}

	public:
		auto & operator++()
		{
			if( _iterator == _end )
			{
				return *this;
			}

			++_iterator;

			while( _iterator != _end && !_function( *_iterator ) )
			{
				++_iterator;
			}

			return *this;
		}

		auto operator*() const
		{
			return *_iterator;
		}

		bool operator==( const where_iterator< Iterator, Function > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const where_iterator< Iterator, Function > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Iterator _iterator;
		Iterator _end;
		Function _function;
	};

	template< typename Iterator, typename Count >
	class take_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		take_iterator( Iterator iterator, Iterator end, Count count )
			:_iterator( iterator ), _end( end ), _count( count ), _current( 0 )
		{
			if( _current == _count )
			{
				_iterator = _end;
			}
		}

	public:
		auto & operator++()
		{
			if( ++_current == _count )
			{
				_iterator = _end;
			}
			else
			{
				++_iterator;
			}

			return *this;
		}

		auto operator*() const
		{
			return *_iterator;
		}

		bool operator==( const take_iterator< Iterator, Count > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const take_iterator< Iterator, Count > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Iterator _iterator;
		Iterator _end;
		Count _count;
		Count _current;
	};

	template< typename Iterator, typename Count >
	class skip_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		skip_iterator( Iterator iterator, Iterator end, Count count )
			:_iterator( iterator ), _end( end ), _count( count )
		{
			while( _iterator != _end && --_count > 0 )
			{
				++_iterator;
			}
		}

	public:
		auto & operator++()
		{
			++_iterator;

			return *this;
		}

		auto operator*() const
		{
			return *_iterator;
		}

		bool operator==( const skip_iterator< Iterator, Count > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const skip_iterator< Iterator, Count > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Iterator _iterator;
		Iterator _end;
		Count _count;
	};

	template< typename Iterator, typename Function >
	class take_while_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		take_while_iterator( Iterator iterator, Iterator end, const Function & function )
			:_iterator( iterator ), _end( end ), _function( function )
		{
			if( _iterator != _end && !_function( *_iterator ) )
			{
				_iterator = _end;
			}
		}

	public:
		auto & operator++()
		{
			++_iterator;
			if( !_function( *_iterator ) )
			{
				_iterator = _end;
			}

			return *this;
		}

		auto operator*() const
		{
			return *_iterator;
		}

		bool operator==( const take_while_iterator< Iterator, Function > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const take_while_iterator< Iterator, Function > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Iterator _iterator;
		Iterator _end;
		Function _function;
	};

	template< typename Iterator, typename Function >
	class skip_while_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		skip_while_iterator( Iterator iterator, Iterator end, const Function & function )
			:_iterator( iterator ), _end( end ), _function( function )
		{
			if( _iterator != _end && _function( *_iterator ) )
			{
				++_iterator;
			}
		}

	public:
		auto & operator++()
		{
			++_iterator;

			return *this;
		}

		auto operator*() const
		{
			return *_iterator;
		}

		bool operator==( const skip_while_iterator< Iterator, Function > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const skip_while_iterator< Iterator, Function > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Iterator _iterator;
		Iterator _end;
		Function _function;
	};

	template< typename Iterator1, typename Iterator2 >
	class concat_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator1 >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator1 >::value_type;
		using difference_type = typename std::iterator_traits< Iterator1 >::difference_type;
		using pointer = typename std::iterator_traits< Iterator1 >::pointer;
		using reference = typename std::iterator_traits< Iterator1 >::reference;

	public:
		concat_iterator( Iterator1 iterator, Iterator1 end, Iterator2 iterator2 )
			:_iterator1( iterator ), _end( end ), _iterator2( iterator2 ), _first( iterator != end )
		{
		}

	public:
		auto & operator++()
		{
			if( _first )
			{
				if( ++_iterator1 == _end )
				{
					_first = false;
				}
			}
			else
			{
				++_iterator2;
			}

			return *this;
		}

		auto operator*() const
		{
			return _first ? *_iterator1 : *_iterator2;
		}

		bool operator==( const concat_iterator< Iterator1, Iterator2 > & val ) const
		{
			return _first == val._first ? ( _first ? _iterator1 == val._iterator1 : _iterator2 == val._iterator2 ) : false;
		}

		bool operator!=( const concat_iterator< Iterator1, Iterator2 > & val ) const
		{
			return !( _first == val._first ? ( _first ? _iterator1 == val._iterator1 : _iterator2 == val._iterator2 ) : false );
		}

	private:
		Iterator1 _iterator1;
		Iterator2 _iterator2;
		Iterator1 _end;
		bool _first;
	};

	template<typename Iterator, typename Container >
	class storage_iterator
	{
	public:
		using iterator_category = typename std::iterator_traits< Iterator >::iterator_category;
		using value_type = typename std::iterator_traits< Iterator >::value_type;
		using difference_type = typename std::iterator_traits< Iterator >::difference_type;
		using pointer = typename std::iterator_traits< Iterator >::pointer;
		using reference = typename std::iterator_traits< Iterator >::reference;

	public:
		storage_iterator( const Container & container, Iterator iterator, Iterator end )
			:_container( container ), _iterator( iterator ), _end( end )
		{
		}

	public:
		auto & operator++()
		{
			++_iterator;

			return *this;
		}

		auto operator*() const
		{
			return *_iterator;
		}

		bool operator==( const storage_iterator< Iterator, Container > & val ) const
		{
			return _iterator == val._iterator;
		}

		bool operator!=( const storage_iterator< Iterator, Container > & val ) const
		{
			return _iterator != val._iterator;
		}

	private:
		Container _container;
		Iterator _iterator;
		Iterator _end;
	};
}

#endif // LINQ_HPP__900F519C_610C_471B_81EB_933C83B3A14A
