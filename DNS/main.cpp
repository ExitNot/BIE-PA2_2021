#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#include "ipaddress.h"
using namespace std;
#endif /* __PROGTEST__ */

class CRec
{
	string  name;
public:
//	constructor and destructor
	explicit CRec( const string & in_name );
	virtual ~CRec() = default;
// getters
	string Name() const;
	virtual string Type() const = 0;
//	operators
	virtual bool operator == ( const CRec & rhs ) const = 0;
	friend ostream & operator << ( ostream & os, const CRec & obj );
//	additional methods
	virtual void print( ostream & os ) const = 0;
	virtual CRec * clone () const = 0;
};

string CRec::Name() const {
	return name;
}

CRec::CRec(const string &in_name) {
	name = in_name;
}

ostream & operator << ( ostream & os, const CRec & obj ) {
	obj.print( os );
	return os;
}


class CRecA : public CRec
{
	CIPv4 ipAddress;
public:
	// constructor
	CRecA( const string & name, const CIPv4 & ipAddress );
	~CRecA() override = default;
	// getters
	string Type() const;
	CIPv4 getIP() const;
	// overrides
	bool operator == ( const CRec & rhs ) const override;
	void print( ostream &os ) const override;
	CRec * clone () const override;
};

string CRecA::Type() const {
	return "A";
}

CIPv4 CRecA::getIP() const{
	return ipAddress;
}


bool CRecA::operator == ( const CRec & rhs ) const {
	if( auto * child = dynamic_cast<const CRecA *>(&rhs) ){
		return ((*this).Name() == child->Name() &&
				(*this).Type() == child->Type() &&
				(*this).getIP() == child->getIP());
	} else
		return false;
}

CRecA::CRecA(const string & in_name, const CIPv4 & in_ipAddress) : CRec{in_name}, ipAddress{in_ipAddress} {}

CRec * CRecA::clone() const {
	return new CRecA(*this);
}

void CRecA::print( ostream &os ) const {
	auto * child = dynamic_cast<const CRecA *>(this);
	os << child->Name() << " " << child->Type() << " " << child->getIP();
}

class CRecAAAA : public CRec
{
	CIPv6 ipAddress;
public:
	// constructor
	CRecAAAA( const string & in_name, const CIPv6 & in_ipAddress );
	~CRecAAAA() override = default;
	// getters
	string Type() const;
	CIPv6 getIP() const;
	// overrides
	bool operator == ( const CRec & rhs ) const override;
	void print( ostream & os ) const override;
	CRec * clone () const override;
};

CRecAAAA::CRecAAAA( const string & in_name, const CIPv6 & in_ipAddress ) : CRec{in_name}, ipAddress{in_ipAddress} {}

string CRecAAAA::Type() const {
	return "AAAA";
}

CIPv6 CRecAAAA::getIP() const{
	return ipAddress;
}

bool CRecAAAA::operator==(const CRec &rhs) const {
	if( auto * child = dynamic_cast<const CRecAAAA *>(&rhs) ){
		return ((*this).Name() == child->Name() &&
				(*this).Type() == child->Type() &&
				(*this).getIP() == child->getIP());
	} else
		return false;
}

void CRecAAAA::print( ostream & os ) const {
	auto * child = dynamic_cast<const CRecAAAA *>(this);
	os << child->Name() << " " << child->Type() << " " << child->getIP();
}

CRec *CRecAAAA::clone() const {
	return new CRecAAAA(*this);
}

class CRecMX : public CRec
{
	string ipAddress;
	unsigned priority;
public:
	// constructor
	CRecMX( const string & in_name, string  in_ipAddress, const unsigned & priority);
	~CRecMX() override = default;
	// getters
	string Type() const;
	string getIP() const;
	// overrides
	bool operator == ( const CRec & rhs ) const override;
	void print( ostream & os ) const override;
	CRec * clone () const override;
};

CRecMX::CRecMX( const string & in_name, string in_ipAddress, const unsigned & in_priority ) :
CRec{in_name}, ipAddress{std::move(in_ipAddress)}, priority{in_priority} {}

string CRecMX::Type() const {
	return "MX";
}

string CRecMX::getIP() const {
	return ipAddress;
}

bool CRecMX::operator == ( const CRec & rhs ) const {
	if( auto * child = dynamic_cast<const CRecMX *>(&rhs) ){
		return ((*this).Name() == child->Name() &&
				(*this).Type() == child->Type() &&
				(*this).priority == child->priority &&
				(*this).getIP() == child->getIP());
	} else
		return false;
}

void CRecMX::print( ostream & os ) const {
	auto * child = dynamic_cast<const CRecMX *>(this);
	os << child->Name() << " " << child->Type() << " " << this->priority << " " << child->getIP();
}

CRec *CRecMX::clone() const {
	return new CRecMX(*this);
}

class CRecList
{
	vector<CRec*> list;
public:
//	constructor
	explicit CRecList( vector<CRec*> in_list );
//	operators
	friend ostream & operator << ( ostream & os, const CRecList & rhs );
	CRec & operator [] ( unsigned index );
//	additional
	unsigned Count();
};

CRecList::CRecList( vector<CRec*> in_list ) {
	list = std::move(in_list);
}

ostream & operator << ( ostream & os, const CRecList & rhs ) {
	for( auto rec : rhs.list ){
		os << *rec << "\n";
	}
	return os;
}

CRec & CRecList::operator [] ( unsigned int index ) {
	if( index >= list.size() ){
		throw out_of_range("");
	}
	return *list[index];
}

unsigned CRecList::Count() {
	return list.size();
}

class CZone
{
	string zoneName;
	vector<CRec*> records;
public:
	// constructor(s)
	explicit CZone( const string & in_zoneName );
	CZone( const CZone & zone );
	// destructor (if needed)
	~CZone();
	// operator = (if needed)
	CZone & operator = ( const CZone & zone );
	// Add ()
	bool Add( const CRec & elem );
	// Del ()
	bool Del( const CRec & elem );
	// Search ()
	CRecList Search( const string & target ) const;
	// operator <<
	friend ostream & operator << ( ostream & os, const CZone & zone );

	// todo CZone
};

CZone::CZone( const string & in_zoneName ) {
	zoneName = in_zoneName;
}

CZone::CZone( const CZone & zone ) {
	zoneName = zone.zoneName;

	for( auto rec : zone.records ){
		records.push_back( rec->clone() );
	}
}

CZone::~CZone() {
	for( auto i : records ){
		delete i;
	}
}

bool CZone::Add( const CRec & elem ) {
	auto it = find_if( records.begin(), records.end(), [ & elem ]( const CRec * record ){
		return (*record == elem);
	} );
	if( it != records.end() )
		return false;

	records.push_back( elem.clone() );
	return true;
}

ostream & operator << ( ostream & os, const CZone & zone ) {
	os << zone.zoneName << "\n";

	for( CRec * record : zone.records ){
		if( record == zone.records.back() )
			os << " \\- ";
		else
			os << " +- ";
		os << *record << "\n";
	}

	return os;
}

CRecList CZone::Search( const string & target ) const {
	vector<CRec*> out;
	auto it = records.begin();

	while( (it = find_if( it, records.end(),
				 [target]( const CRec * rec){ return rec->Name() == target; } )) != records.end() ){
		out.push_back(*it);
		it++;
	}

	return CRecList(out);
}

bool CZone::Del(const CRec &elem) {
	auto it = find_if( records.begin(), records.end(), [ & elem ]( const CRec * record ){
		return (*record == elem);
	} );
	if( it == records.end() )
		return false;

	CRec * ptr = *it;
	records.erase( it );
	delete ptr;
	return true;
}

CZone & CZone::operator = ( const CZone & zone ) {
	zoneName = zone.zoneName;
	for( auto i : records )
		delete i;
	records.clear();

	for( auto rec : zone.records ){
		records.push_back( rec->clone() );
	}

	return *this;
}


#ifndef __PROGTEST__
int main ( void )
{
	ostringstream oss;

	CZone z0 ( "fit.cvut.cz" );
	assert ( z0 . Add ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == true );
	assert ( z0 . Add ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == false );  // RM
	assert ( z0 . Add ( CRecAAAA ( "progtest", CIPv6 ( "2001:718:2:2902:0:1:2:3" ) ) ) == true );
	assert ( z0 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.158" ) ) ) == true );
	assert ( z0 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.160" ) ) ) == true );
	assert ( z0 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.159" ) ) ) == true );
	assert ( z0 . Add ( CRecAAAA ( "progtest", CIPv6 ( "2001:718:2:2902:1:2:3:4" ) ) ) == true );
	assert ( z0 . Add ( CRecMX ( "courses", "relay.fit.cvut.cz.", 0 ) ) == true );
	assert ( z0 . Add ( CRecMX ( "courses", "relay2.fit.cvut.cz.", 10 ) ) == true );
	oss . str ( "" );
	oss << z0;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- progtest A 147.32.232.142\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " +- courses A 147.32.232.158\n"
			 " +- courses A 147.32.232.160\n"
			 " +- courses A 147.32.232.159\n"
			 " +- progtest AAAA 2001:718:2:2902:1:2:3:4\n"
			 " +- courses MX 0 relay.fit.cvut.cz.\n"
			 " \\- courses MX 10 relay2.fit.cvut.cz.\n" );
	assert ( z0 . Search ( "progtest" ) . Count () == 3 );
	oss . str ( "" );
	oss << z0 . Search ( "progtest" );
	assert ( oss . str () ==
			 "progtest A 147.32.232.142\n"
			 "progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 "progtest AAAA 2001:718:2:2902:1:2:3:4\n" );
	assert ( z0 . Del ( CRecA ( "courses", CIPv4 ( "147.32.232.160" ) ) ) == true );
	assert ( z0 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.122" ) ) ) == true );
	oss . str ( "" );
	oss << z0;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- progtest A 147.32.232.142\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " +- courses A 147.32.232.158\n"
			 " +- courses A 147.32.232.159\n"
			 " +- progtest AAAA 2001:718:2:2902:1:2:3:4\n"
			 " +- courses MX 0 relay.fit.cvut.cz.\n"
			 " +- courses MX 10 relay2.fit.cvut.cz.\n"
			 " \\- courses A 147.32.232.122\n" );
	assert ( z0 . Search ( "courses" ) . Count () == 5 );
	oss . str ( "" );
	oss << z0 . Search ( "courses" );
	assert ( oss . str () ==
			 "courses A 147.32.232.158\n"
			 "courses A 147.32.232.159\n"
			 "courses MX 0 relay.fit.cvut.cz.\n"
			 "courses MX 10 relay2.fit.cvut.cz.\n"
			 "courses A 147.32.232.122\n" );
	oss . str ( "" );
	oss << z0 . Search ( "courses" ) [ 0 ];
	assert ( oss . str () == "courses A 147.32.232.158" );
	assert ( z0 . Search ( "courses" ) [ 0 ] . Name () == "courses" );
	assert ( z0 . Search ( "courses" ) [ 0 ] . Type () == "A" );
	oss . str ( "" );
	oss << z0 . Search ( "courses" ) [ 1 ];
	assert ( oss . str () == "courses A 147.32.232.159" );
	assert ( z0 . Search ( "courses" ) [ 1 ] . Name () == "courses" );
	assert ( z0 . Search ( "courses" ) [ 1 ] . Type () == "A" );
	oss . str ( "" );
	oss << z0 . Search ( "courses" ) [ 2 ];
	assert ( oss . str () == "courses MX 0 relay.fit.cvut.cz." );
	assert ( z0 . Search ( "courses" ) [ 2 ] . Name () == "courses" );
	assert ( z0 . Search ( "courses" ) [ 2 ] . Type () == "MX" );
	try
	{
		oss . str ( "" );
		oss << z0 . Search ( "courses" ) [ 10 ];
		assert ( "No exception thrown!" == nullptr );
	}
	catch ( const out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown!" == nullptr );
	}
	dynamic_cast<const CRecAAAA &> ( z0 . Search ( "progtest" ) [ 1 ] );
	CZone z1 ( "fit2.cvut.cz" );
	z1 . Add ( z0 . Search ( "progtest" ) [ 2 ] );
	z1 . Add ( z0 . Search ( "progtest" ) [ 0 ] );
	z1 . Add ( z0 . Search ( "progtest" ) [ 1 ] );
	z1 . Add ( z0 . Search ( "courses" ) [ 2 ] );
	oss . str ( "" );
	oss << z1;
	assert ( oss . str () ==
			 "fit2.cvut.cz\n"
			 " +- progtest AAAA 2001:718:2:2902:1:2:3:4\n"
			 " +- progtest A 147.32.232.142\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " \\- courses MX 0 relay.fit.cvut.cz.\n" );
	dynamic_cast<const CRecA &> ( z1 . Search ( "progtest" ) [ 1 ] );

	CZone z2 ( "fit.cvut.cz" );
	assert ( z2 . Add ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == true );
	assert ( z2 . Add ( CRecAAAA ( "progtest", CIPv6 ( "2001:718:2:2902:0:1:2:3" ) ) ) == true );
	assert ( z2 . Add ( CRecA ( "progtest", CIPv4 ( "147.32.232.144" ) ) ) == true );
	assert ( z2 . Add ( CRecMX ( "progtest", "relay.fit.cvut.cz.", 10 ) ) == true );
	assert ( z2 . Add ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == false );
	assert ( z2 . Del ( CRecA ( "progtest", CIPv4 ( "147.32.232.140" ) ) ) == false );
	assert ( z2 . Del ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == true );
	assert ( z2 . Del ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == false );
	assert ( z2 . Add ( CRecMX ( "progtest", "relay.fit.cvut.cz.", 20 ) ) == true );
	assert ( z2 . Add ( CRecMX ( "progtest", "relay.fit.cvut.cz.", 10 ) ) == false );
	oss . str ( "" );
	oss << z2;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " +- progtest A 147.32.232.144\n"
			 " +- progtest MX 10 relay.fit.cvut.cz.\n"
			 " \\- progtest MX 20 relay.fit.cvut.cz.\n" );
	assert ( z2 . Search ( "progtest" ) . Count () == 4 );
	oss . str ( "" );
	oss << z2 . Search ( "progtest" );
	assert ( oss . str () ==
			 "progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 "progtest A 147.32.232.144\n"
			 "progtest MX 10 relay.fit.cvut.cz.\n"
			 "progtest MX 20 relay.fit.cvut.cz.\n" );
	assert ( z2 . Search ( "courses" ) . Count () == 0 );
	oss . str ( "" );
	oss << z2 . Search ( "courses" );
	assert ( oss . str () == "" );
	try
	{
		dynamic_cast<const CRecMX &> ( z2 . Search ( "progtest" ) [ 0 ] );
		assert ( "Invalid type" == nullptr );
	}
	catch ( const bad_cast & e )
	{
	}

	CZone z4 ( "fit.cvut.cz" );
	assert ( z4 . Add ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == true );
	assert ( z4 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.158" ) ) ) == true );
	assert ( z4 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.160" ) ) ) == true );
	assert ( z4 . Add ( CRecA ( "courses", CIPv4 ( "147.32.232.159" ) ) ) == true );
	CZone z5 ( z4 );
	assert ( z4 . Add ( CRecAAAA ( "progtest", CIPv6 ( "2001:718:2:2902:0:1:2:3" ) ) ) == true );
	assert ( z4 . Add ( CRecAAAA ( "progtest", CIPv6 ( "2001:718:2:2902:1:2:3:4" ) ) ) == true );
	assert ( z5 . Del ( CRecA ( "progtest", CIPv4 ( "147.32.232.142" ) ) ) == true );
	oss . str ( "" );
	oss << z4;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- progtest A 147.32.232.142\n"
			 " +- courses A 147.32.232.158\n"
			 " +- courses A 147.32.232.160\n"
			 " +- courses A 147.32.232.159\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " \\- progtest AAAA 2001:718:2:2902:1:2:3:4\n" );
	oss . str ( "" );
	oss << z5;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- courses A 147.32.232.158\n"
			 " +- courses A 147.32.232.160\n"
			 " \\- courses A 147.32.232.159\n" );
	z5 = z4;
	assert ( z4 . Add ( CRecMX ( "courses", "relay.fit.cvut.cz.", 0 ) ) == true );
	assert ( z4 . Add ( CRecMX ( "courses", "relay2.fit.cvut.cz.", 10 ) ) == true );
	oss . str ( "" );
	oss << z4;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- progtest A 147.32.232.142\n"
			 " +- courses A 147.32.232.158\n"
			 " +- courses A 147.32.232.160\n"
			 " +- courses A 147.32.232.159\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " +- progtest AAAA 2001:718:2:2902:1:2:3:4\n"
			 " +- courses MX 0 relay.fit.cvut.cz.\n"
			 " \\- courses MX 10 relay2.fit.cvut.cz.\n" );
	oss . str ( "" );
	oss << z5;
	assert ( oss . str () ==
			 "fit.cvut.cz\n"
			 " +- progtest A 147.32.232.142\n"
			 " +- courses A 147.32.232.158\n"
			 " +- courses A 147.32.232.160\n"
			 " +- courses A 147.32.232.159\n"
			 " +- progtest AAAA 2001:718:2:2902:0:1:2:3\n"
			 " \\- progtest AAAA 2001:718:2:2902:1:2:3:4\n" );

	return 0;
}
#endif /* __PROGTEST__ */
