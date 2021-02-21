# linq

为C++提供了linq的实现。目前它只支持C++ Object数据源。

获取linq的实现代码：**[linq.hpp](/src/linq.hpp)**

## 演示示例
```C++
std::vector<int> v = { 4, 5, 6, 1, 2, 3, 7, 8, 9 };
std::vector<int> v2 = { 14, 15, 16, 11, 12, 13, 17, 18, 19 };
std::vector<int> v3 = { 5, 1, 3, 7, 9 };
std::vector<int> v4;
std::multimap<int, int> cm;

auto q = from( v ).select( []( int x ) { return x + 10; } );
assert( std::equal( v2.begin(), v2.end(), q.begin() ) );

auto w = from( v )
	.where( []( int x ) { return ( x % 2 ) == 1; } )
	.select( []( int x ) { return x; } );
assert( std::equal( v3.begin(), v3.end(), w.begin() ) );

std::cout << "count: " << w.count() << std::endl;
std::cout << "empty: " << w.empty() << std::endl;
std::cout << "first: " << w.first() << std::endl;
std::cout << "sum: " << w.sum() << std::endl;

auto n = from( v ).group_by( cm, []( auto & c, int x ) { c.insert( { x % 2, x } ); } );
for( auto i : n )
{
	std::cout << i.first << ":" << i.second << std::endl;
}

auto b = from( v ).order_by( v4, []( auto & c, int x ) { c.push_back( x ); }, []( auto & c ) { std::sort( c.begin(), c.end() ); } );
for( auto i : b )
{
	std::cout << i << std::endl;
}

v4.clear();
auto m = from( v ).join( v4, from( v2 ), []( int x ) { return true; }, []( int x ) { return true; }, []( auto & c, int x, int y )
							{
								c.push_back( x + y );
							} );

for( auto i : m )
{
	std::cout << i << std::endl;
}
```

## 目前支持的功能如下：
#### 过滤:
- `where`
- `take`, `take_while`
- `skip`, `skip_while`
- `order_by`
- `concat`

#### 变换:
- `select`
- `group_by`

#### 聚合:
- `sum`
- `average`
- `aggregate`
- `min`
- `max`
- `count`
- `at`
- `first`, `first_or_default`
- `last`, `last_or_default`
- `emplace_back`, `push_back`, `insert`
