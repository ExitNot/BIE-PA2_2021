#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <forward_list>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */


template<typename T_, typename Cmp_ = less<T_> >
class CIntervalMin
{
	vector<T_> body;
	Cmp_ cmp_;
	T_ * ST;
//	vector<T_> ST;  // Segment tree
	size_t STsize;
public:
	// default constructor
	CIntervalMin();
	~CIntervalMin();
	// constructor with comparator
	explicit CIntervalMin( Cmp_ cmp );
	// constructor - 2 iterators
	CIntervalMin( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end );
	// constructor - 2 iterators + comparator
	CIntervalMin( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end, Cmp_ cmp );

	// const_iterator
	// push_back
	void push_back( T_ val );
	// pop_back
	void pop_back();
	// min
	const T_ & min( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end );
	// begin
	typename std::vector<T_>::const_iterator begin() const;
	// end
	typename std::vector<T_>::const_iterator end() const;
	// size
	size_t size() const;

	T_ buildST( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end, int STindex );
	// utility methods
	T_ minCmp( const T_ & lhs, const T_ & rhs ) const;
	const T_ & minUtil( typename vector<T_>::const_iterator begin_,
					 	typename vector<T_>::const_iterator end_,
						typename vector<T_>::const_iterator begin,
						typename vector<T_>::const_iterator end,
					 	int index );
};

template<typename T_, typename Cmp_>
CIntervalMin<T_, Cmp_>::CIntervalMin() {
	body.clear();
	ST = new T_(1);
	STsize = 1;
}

template<typename T_, typename Cmp_>
CIntervalMin<T_, Cmp_>::CIntervalMin( Cmp_ cmp ) : cmp_(cmp) {
	body.clear();
	ST = new T_[1];
	STsize = 1;
}

template<typename T_, typename Cmp_>
CIntervalMin<T_, Cmp_>::CIntervalMin( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end ){
	body.clear();

	/* Allocate memory and building for segment tree */
	int height = (int)(ceil( log2(distance(begin, end)) ));
	STsize = 2 * (int)(pow(2, height)) - 1;

	ST = new T_[STsize];
	memset( ST, 0, STsize );

	CIntervalMin::buildST( begin, end, 0 );
	body.insert( body.begin(), begin, end );
}

template<typename T_, typename Cmp_>
CIntervalMin<T_, Cmp_>::CIntervalMin( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end, Cmp_ cmp ) : cmp_(cmp) {
	body.clear();
	vector<T_> buff( begin, end );
	sort( buff.begin(), buff.end(), cmp_ );

	/* Allocate memory and building for segment tree */
	int height = (int)(ceil( log2(distance(begin, end)) ));
	STsize = 2 * (int)(pow(2, height)) - 1;

	ST = new T_[STsize];
	memset( ST, 0, STsize );

	CIntervalMin::buildST( begin, end, 0 );
	body.insert( body.begin(), buff.begin(), buff.end() );
}

template<typename T_, typename Cmp_>
void CIntervalMin<T_, Cmp_>::push_back(T_ val) {
	body.push_back(val);
//	TODO create algorithm for adding not recalculating
	int height = (int)(ceil( log2(distance(body.begin(), body.end())) ));
	if( (2 * (int)(pow(2, height)) - 1) > STsize ){
		size_t newSTsize = 2 * (int)(pow(2, height)) - 1;
		T_ * newST = new T_[newSTsize];
		memset( newST, 0, newSTsize );
		memcpy(ST, newST, STsize);
		delete [] ST;
		ST = newST;
		STsize = newSTsize;
	}
	CIntervalMin::buildST( body.begin(), body.end(), 0 );
}

template<typename T_, typename Cmp_>
size_t CIntervalMin<T_, Cmp_>::size() const {
	return body.size();
}

template<typename T_, typename Cmp_>
typename std::vector<T_>::const_iterator CIntervalMin<T_, Cmp_>::begin() const {
	return body.begin();
}

template<typename T_, typename Cmp_>
typename std::vector<T_>::const_iterator CIntervalMin<T_, Cmp_>::end() const {
	return body.end();
}

template<typename T_, typename Cmp_>
const T_ & CIntervalMin<T_, Cmp_>::min( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end ) {
	if( begin == end ) {
		throw std::invalid_argument("");
		return *end;
	}
//	for( unsigned i = 0; i < STsize; i++ ){   // RM
//		cout << ST[i] << " ";
//	}
	for( auto i = begin; i != end; i++){
		cout << *i << ", ";
	}
	cout << endl;
//	return * std::min_element( begin, end, cmp_ );
	return CIntervalMin::minUtil( body.begin(), body.end() - 1, begin, end - 1, 0);
}

template<typename T_, typename Cmp_>
T_ CIntervalMin<T_, Cmp_>::buildST( typename vector<T_>::const_iterator begin, typename vector<T_>::const_iterator end, int STindex ) {
	if( begin == end )
		return 0;

	if( (begin + 1) == end ){
		ST[STindex] = *begin;
		return ST[STindex];
	}

	int mid = distance( begin, end ) / 2;
	ST[STindex] = CIntervalMin::minCmp( buildST( begin, begin + mid, (STindex * 2) + 1),
										buildST( begin + mid, end , (STindex * 2) + 2));
	return ST[STindex];
}

template<typename T_, typename Cmp_>
void CIntervalMin<T_, Cmp_>::pop_back() {
	body.pop_back();
}

template<typename T_, typename Cmp_>
T_ CIntervalMin<T_, Cmp_>::minCmp( const T_ & lhs, const T_ & rhs ) const {
	return lhs < rhs ? lhs : rhs;
}

template<typename T_, typename Cmp_>
const T_ & CIntervalMin<T_, Cmp_>::minUtil( typename vector<T_>::const_iterator begin_,  // iterator to start of body
											typename vector<T_>::const_iterator end_,  // iterator to end of body
											typename vector<T_>::const_iterator begin,  // iterator to start of interval
											typename vector<T_>::const_iterator end ,  // iterator to end of interval
											int index ){
	T_ * maxVal = new T_;
	*maxVal = numeric_limits<T_>::max();

	if( begin <= begin_ && end >= end_ ) {
//		cout << "ST[" << index << "]:  " << ST[index] << endl;  RM
		return ST[index];
	}
//	if( begin == end )
//		return *begin;
	if( begin == end ){
		cout << "min [ " << *begin_ << " : " << *end_ << " ] = " << *begin_ << endl;
		return *begin;
	}

	if( end_ < begin || begin_ > end )
		return *maxVal;

	int mid = distance( begin_, end_ ) / 2;

	ST[index] = CIntervalMin::minCmp( minUtil( begin_, begin_ + mid, begin, end, (index * 2) + 1),
									  minUtil( begin_ + mid + 1, end_, begin, end, (index * 2) + 2));
	cout << "3)min [ " << *begin_ << " : " << *end_ << " ] = " << ST[index] << endl;
	return ST[index];
}

template<typename T_, typename Cmp_>
CIntervalMin<T_, Cmp_>::~CIntervalMin() {
	delete [] ST;
}


#ifndef __PROGTEST__
//-------------------------------------------------------------------------------------------------
class CStrComparator
{
public:
	CStrComparator ( bool byLength = true )
			: m_ByLength ( byLength )
	{
	}
	bool       operator () ( const string & a, const string & b ) const
	{
		return m_ByLength ? a . length () < b . length () : a < b;
	}
private:
	bool       m_ByLength;
};
//-------------------------------------------------------------------------------------------------
bool strCaseCmpFn ( const string & a, const string & b )
{
	return strcasecmp ( a . c_str (), b . c_str () ) < 0;
}
//-------------------------------------------------------------------------------------------------
int main ( void )
{
//	vector<int> vec { 1, 8, 3, -5 };
//	CIntervalMin <int> inter(vec.begin(), vec.end());
//
//	assert ( inter . min ( inter . begin (), inter . end() ) == -5 );
//	inter.push_back( 9 );
//	inter.push_back( 12 );
//	inter.push_back( 15 );
//	assert ( inter . min ( inter . begin (), inter . end() ) == -5 );

	/* testing from progtest */
	CIntervalMin <int> a1;
	for ( auto x : initializer_list<int> { 5, 15, 79, 62, -3, 0, 92, 16, 2, -4 } )
		a1 . push_back ( x );

	assert ( a1 . size () == 10 );
	ostringstream oss;
	for ( auto x : a1 )
		oss << x << ' ';

	assert ( oss . str () == "5 15 79 62 -3 0 92 16 2 -4 " );
//	cout << a1 . min ( a1 . begin (), a1 . end () );
	assert ( a1 . min ( a1 . begin (), a1 . end () ) == -4 );
	assert ( a1 . min ( a1 . begin () + 2, a1 . begin () + 3 ) == 79 );
//	cout << a1 . min ( a1 . begin () + 2, a1 . begin () + 9 );
	assert ( a1 . min ( a1 . begin () + 2, a1 . begin () + 9 ) == -3 );

	try
	{
		a1 . min ( a1 . begin (), a1 . begin () );
		assert ( "Missing an exception" == nullptr );
	}
	catch ( const invalid_argument & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception" == nullptr );
	}

	a1 . pop_back ();
	assert ( a1 . size () == 9 );
	a1 . push_back ( 42 );

	assert ( a1 . min ( a1 . begin (), a1 . end () ) == -3 );

	vector<string> words{ "auto", "if", "void", "NULL" };
	CIntervalMin <string> a2 ( words . begin (), words . end () );
	assert ( a2 . min ( a2 . begin (), a2 . end () ) ==  "NULL" );

//	CIntervalMin <string, bool(*)(const string &, const string &)> a3 ( words . begin (), words . end (), strCaseCmpFn );
//	assert ( a3 . min ( a3 . begin (), a3 . end () ) == "auto" );
//
//	CIntervalMin <string, CStrComparator> a4 ( words . begin (), words . end () );
//	assert ( a4 . min ( a4 . begin (), a4 . end () ) == "if" );
//
//	CIntervalMin <string, CStrComparator> a5 ( words . begin (), words . end (), CStrComparator ( false ) );
//	assert ( a5 . min ( a5 . begin (), a5 . end () ) == "NULL" );
//
//	CIntervalMin <string, function<bool(const string &, const string &)> > a6 ( [] ( const string & a, const string & b )
//																				{
//																					return a > b;
//																				} );
//	for ( const auto & w : words )
//		a6 . push_back ( w );
//	assert ( a6 . min ( a6 . begin (), a6 . end () ) == "void" );

	return 0;
}
#endif /* __PROGTEST__ */
