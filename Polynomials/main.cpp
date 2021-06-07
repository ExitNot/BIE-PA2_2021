#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <memory>
#endif /* C++ 11 */
using namespace std;
#endif /* __PROGTEST__ */

#define EPSILON 0.1
#define MIN(a, b) a<b ? a : b
#define MAX(a, b) a>b ? a : b

class CPolynomial
{
public:
	// default constructor
	CPolynomial();
	~CPolynomial() = default;
	// operator +
	CPolynomial operator + ( const CPolynomial & rhs ) const;
	// operatnr -
	CPolynomial operator - ( const CPolynomial & rhs ) const;
	// operator * (polynomial, double)
	CPolynomial operator * ( const CPolynomial & rhs ) const;
	CPolynomial operator * ( const double & rhs ) const;
	// operator ==
	bool operator == ( const CPolynomial & rhs ) const;
	// operator !=
	bool operator != ( const CPolynomial & rhs ) const;
	// operator []
	double & operator [] ( const unsigned i );
	double operator [] ( const unsigned i ) const;
	// operator ()
	double operator () ( const double x ) const;
	// Degree (), returns unsigned value
	unsigned int Degree() const;

	// operator <<
	friend ostream & operator << ( ostream & os, const CPolynomial & data );
	static bool double_eq(const double & a, const double & b );
	void multX( int pow );  // move x powers( multiply by x^pow )
	void simplify();
private:

	vector<double> coeff;  // coefficients
};

//	TODO swap implementations in correct order
CPolynomial::CPolynomial() {
	coeff.push_back(0.0);
}

/* operator + */
CPolynomial CPolynomial::operator + ( const CPolynomial & rhs ) const {
	CPolynomial res;

	if( this->Degree() >= rhs.Degree() )
		res = *this;
	else
		res = rhs;

	for( unsigned i = 0; i <= res.Degree(); i++ ){
		if( this->Degree() >= rhs.Degree() ) {
			if( i > rhs.Degree() ) {
				res[i] = (*this)[i];
				continue;
			}
			res[i] = rhs[i] + (*this)[i];
		} else {
			if( i > (*this).Degree() ) {
				res[i] = rhs[i];
				continue;
			}
			res[i] = rhs[i] + (*this)[i];
		}
	}

	return res;
}

/* operator - */
CPolynomial CPolynomial::operator - ( const CPolynomial & rhs ) const {
	CPolynomial res;

	if( this->Degree() >= rhs.Degree() )
		res = *this;
	else
		res = rhs;

	for( unsigned i = 0; i <= res.Degree(); i++ ){
		if( this->Degree() >= rhs.Degree() ) {
			if( i > rhs.Degree() ) {
				res[i] = (*this)[i];
				continue;
			}
			res[i] = (*this)[i] - rhs[i];
		} else {
			if( i > (*this).Degree() ) {
				res[i] = -rhs[i];
				continue;
			}
			res[i] = (*this)[i] - rhs[i];
		}
	}

	return res;
}

/* operator * */
CPolynomial CPolynomial::operator * ( const CPolynomial & rhs ) const{
	vector<CPolynomial> results;
	CPolynomial res;

	res.coeff.resize( rhs.Degree() + 1 );

	for( unsigned i = 0; i < rhs.Degree() + 1; i++ ){
		if( CPolynomial::double_eq(rhs[i], 0.0) )
			continue;
		results.push_back(((*this) * rhs[i]));  // add mooving powers
		results.back().multX( i );
	}
	for( const auto& i : results ){
		res = res + i;
	}
	return res;
}

CPolynomial CPolynomial::operator * ( const double & rhs ) const {
	CPolynomial res;
	unsigned pow = 0;

	res.coeff.resize( this->coeff.size() );

	for( auto i : this->coeff ){
		res[pow] = i * rhs;
		pow++;
	}

	return res;
}

/* boolean operators */
bool CPolynomial::operator == ( const CPolynomial & rhs ) const {
	CPolynomial Lhs = (*this);
	CPolynomial Rhs = rhs;
	Lhs.simplify();
	Rhs.simplify();

	return Rhs.coeff == Lhs.coeff;
}

bool CPolynomial::operator != ( const CPolynomial & rhs ) const {
	//	simplify()
	return this->coeff != rhs.coeff;
}

/* operator [] */
double & CPolynomial::operator [] ( unsigned index ) {
	if ( coeff.size() - 1 < index )
		coeff.resize(index + 1, 0.0);
	return coeff[index];
}

double CPolynomial::operator [] ( unsigned index ) const {
	return coeff[index];
}

/* operator () */
double CPolynomial::operator()( double x ) const {
	double out = 0.0;
	unsigned power = 0;  // power of x

	for( auto i : coeff ){
		if( i < EPSILON && i > -EPSILON ) {
			power++;
			continue;
		}
		out += i * pow( x,power );
		power++;
	}

	return out;
}

/* operator << */
ostream & operator << ( ostream & os, const CPolynomial & data ) {
	int pow = data.coeff.size() - 1;

	if( pow == 0 && CPolynomial::double_eq( *data.coeff.begin(), 0.0)){
		os << "0";
		return os;
	}

	for( auto i = data.coeff.rbegin(); i != data.coeff.rend(); i++ ){
		if( CPolynomial::double_eq( *i, 0.0) ){  // coefficient == 0
			pow--;
			continue;
		}
		if( i != data.coeff.rbegin() )
			os << " ";
		if( CPolynomial::double_eq( *i, 1.0) ){  // coefficient == 1
			if( pow != (int)data.Degree() )
				os << "+ ";
			if( pow != 0 ){
				os << "x^" << pow;
			} else{
				os << "1" << pow;
			}
		} else if( CPolynomial::double_eq( *i, -1.0) ) {  // coefficient == -1
			os << "- ";
			if( pow != 0 ){
				os << "x^" << pow;
			} else{
				os << "1" << pow;
			}
		} else{
			if( static_cast<int>(*i * 10) % 10 != 0 ){
				if( *i < 0 ) {
					os << "- ";
					os << std::fixed << std::setprecision(1) << -*i;
					if ( pow != 0 ){
						os << "*x^" << pow;
					}
				} else {
					if( pow != (int)data.Degree() )
						os << "+ ";
					os << std::fixed << std::setprecision(1) << *i;
					if ( pow != 0 ){
						os << "*x^" << pow;
					}
				}
			} else{
				if( *i < 0 ) {
					os << "- ";
					os << std::fixed << std::setprecision(0) << -*i;
					if ( pow != 0 ){
						os << "*x^" << pow;
					}
				} else {
					if( pow != (int)data.Degree() )
						os << "+ ";
					os << std::fixed << std::setprecision(0) << *i;
					if ( pow != 0 ){
						os << "*x^" << pow;
					}
				}
			}
		}
		pow--;
	}
	return os;
}

/* additional methods */
bool CPolynomial::double_eq( const double &a, const double &b ) {
	double diff = (MAX(a, b)) - (MIN(a, b));
	if( diff < EPSILON )
		return true;
	else
		return false;
}

unsigned int CPolynomial::Degree() const {
	unsigned pow = coeff.size() - 1;

	for( auto i = coeff.rbegin(); i != coeff.rend(); i++ ){
		if( !CPolynomial::double_eq(*i, 0.0) ){
			return pow;
		}
		pow--;
	}
	return 0;
}

void CPolynomial::multX( int pow ) {
	for( int i = 0; i < pow; i++ ){
		coeff.insert( coeff.begin(),  0.0 );
	}
}

void CPolynomial::simplify() {
	coeff.resize(CPolynomial::Degree() + 1 );
}

#ifndef __PROGTEST__
bool               smallDiff                               ( double            a,
															 double            b )
{

	double diff = (MAX(a, b)) - (MIN(a, b));
	if( diff < EPSILON )
		return true;
	else
		return false;
}

bool               dumpMatch                               ( const CPolynomial & x,
															 const vector<double> & ref )
{
	for( unsigned i = 0; i <= x.Degree(); i++ ){
		if( !smallDiff(ref.at(i), x[i]) ){
			return false;
		}
	}
	return true;
}

int                main                                    ( void )
{
	CPolynomial a, b, c;
	ostringstream out;

	a[0] = -10;
	a[1] = 3.5;
	a[3] = 1;
	assert ( smallDiff ( a ( 2 ), 5 ) );
	out . str ("");
	out << a;
	assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
	a = a * -2;
	assert ( a . Degree () == 3
			 && dumpMatch ( a, vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

	out . str ("");
	out << a;
	assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
	out . str ("");
	out << b;
	assert ( out . str () == "0" );
	b[5] = -1;
	out . str ("");
	out << b;
	assert ( out . str () == "- x^5" );
	c = a + b;
	assert ( c . Degree () == 5
			 && dumpMatch ( c, vector<double>{ 20.0, -7.0, 0.0, -2.0, 0.0, -1.0 } ) );

	out . str ("");
	out << c;
	assert ( out . str () == "- x^5 - 2*x^3 - 7*x^1 + 20" );
	c = a - b;
	assert ( c . Degree () == 5
			 && dumpMatch ( c, vector<double>{ 20.0, -7.0, -0.0, -2.0, -0.0, 1.0 } ) );

	out . str ("");
	out << c;
	assert ( out . str () == "x^5 - 2*x^3 - 7*x^1 + 20" );
	c = a * b;
	assert ( c . Degree () == 8
			 && dumpMatch ( c, vector<double>{ -0.0, -0.0, 0.0, -0.0, 0.0, -20.0, 7.0, -0.0, 2.0 } ) );

	out . str ("");
	out << c;
	assert ( out . str () == "2*x^8 + 7*x^6 - 20*x^5" );
	assert ( a != b );
	b[5] = 0;
	assert ( !(a == b) );
	a = a * 0;
	assert ( a . Degree () == 0
			 && dumpMatch ( a, vector<double>{ 0.0 } ) );

	assert ( a == b );
	return 0;
}
#endif /* __PROGTEST__ */
