#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

struct Person{
	string name;
	string surname;
};

class CCarList
{
public:
	CCarList();
	CCarList( const string &rz, const Person &p );
	const string & RZ      ( void ) const;
	bool           AtEnd   ( void ) const;
    void           Next    ( void );

    void zerroIt(void);
    void Add( const string &rz, const Person &p );
    void Delete( const string &rz );
    int amount() const;
private:
	vector<pair<string, Person>>::iterator c_it;  // iterator for carList
	vector<pair<string, Person>> c_carList;
	bool endFlag;
};

void CCarList::Add( const string &rz, const Person &p ) {
	c_carList.emplace_back( make_pair(rz, p) );
}

int CCarList::amount() const {
	return c_carList.size();
}

bool CCarList::AtEnd(void) const {
	if( c_it == c_carList.end() || endFlag ) {
		return true;
	}
	return false;
}

const string &CCarList::RZ(void) const {
	return c_it->first;
}

void CCarList::Next(void) {
	if( c_it->first == c_carList.back().first )
		endFlag = true;
	c_it = next(c_it, 1);
}

void CCarList::zerroIt(void) {
	c_it = c_carList.begin();
}

CCarList::CCarList( ) {
	CCarList::zerroIt();
	endFlag = false;
}

CCarList::CCarList(const string &rz, const Person &p) {
	c_carList.emplace_back( make_pair(rz, p) );
	CCarList::zerroIt();
	endFlag = false;
}

void CCarList::Delete( const string &rz ) {
	auto carIt = find_if( c_carList.begin(), c_carList.end(), [ &rz ]( pair<string, Person> &a ){
		return a.first == rz;
	});
	c_carList.erase( carIt );
}


class CPersonList
{
public:
	CPersonList();
	CPersonList( vector<pair<Person, CCarList>> &src );
    const string & Name    ( void ) const;
    const string & Surname ( void ) const;
    bool           AtEnd   ( void ) const;
    void           Next    ( void );

    void zerroIt(void);

private:
	vector<Person>::iterator c_it;  // iterator for personList
	vector<Person> c_personList;
	bool endFlag;
};

const string &CPersonList::Name(void) const {
	return c_it->name;
}

const string &CPersonList::Surname(void) const {
	return c_it->surname;
}

bool CPersonList::AtEnd(void) const {
	if( c_it == c_personList.end() || endFlag ) {
		return true;
	}
	return false;
}

void CPersonList::zerroIt(void) {
	c_it = c_personList.begin();
}

CPersonList::CPersonList() {
	endFlag = false;
}

void CPersonList::Next(void) {
	if( c_it->name == c_personList.back().name && c_it->surname == c_personList.back().surname )
		endFlag = true;
	c_it = next(c_it, 1);
}

CPersonList::CPersonList( vector<pair<Person, CCarList>> &src ) {
	endFlag = false;

	for( const auto& i : src ){
		c_personList.push_back( i.first );
	}
}

class CRegister
{
public:
    CRegister  ( void ) = default;
    ~CRegister  ( void ) = default;
    CRegister  ( const CRegister & src ) = delete;
    CRegister & operator = ( const CRegister & src ) = delete;
    bool        AddCar     ( const string & rz,
                             const string & name,
                             const string & surname );
    bool        DelCar     ( const string & rz );
    bool        Transfer   ( const string & rz,
                             const string & nName,
                             const string & nSurname);
    CCarList    ListCars   ( const string & name,
                             const string & surname ) const;
    int         CountCars  ( const string & name,
                             const string & surname ) const;
    CPersonList ListPersons( void ) const;
private:
//	CPersonList personList;
	vector<pair<Person, CCarList>> c_personList;
	vector<pair<string, Person>> c_carList;
	CPersonList outPersons;
//	CCarList allCars;
};

bool CRegister::AddCar(const string &rz, const string &name, const string &surname) {
	Person person{name, surname};

	if( find_if( c_carList.begin(), c_carList.end(), [ rz ]( pair<string, Person> &a ){
		return a.first == rz;
	}) != c_carList.end() )
		return false;

	/*Find person*/
	auto it = find_if( c_personList.begin(), c_personList.end(), [person]( const pair<Person, CCarList> &a){
		return a.first.name == person.name && a.first.surname == person.surname;
	});
	if( it == c_personList.end() ){
		c_personList.emplace_back( make_pair(person, CCarList( rz, person )) );
	} else{
		it->second.Add( rz, person );
		it->second.zerroIt();
	}

	c_carList.emplace_back( make_pair(rz, person) );

    return true;
}

int CRegister::CountCars(const string &name, const string &surname) const {
	Person person{name, surname};

	/*Find person*/
	auto it = find_if( c_personList.begin(), c_personList.end(), [person]( const pair<Person, CCarList> &a){
		return a.first.name == person.name && a.first.surname == person.surname;
	});
	if( it != c_personList.end() ){
		return it->second.amount();
	}

	return 0;
}

CCarList CRegister::ListCars(const string &name, const string &surname) const {
	Person person{name, surname};

	/*Find person*/
	auto it = find_if( c_personList.begin(), c_personList.end(), [person]( const pair<Person, CCarList> &a){
		return a.first.name == person.name && a.first.surname == person.surname;
	});
	if( it != c_personList.end() ){
		return it->second;
	}

	return CCarList();
}

bool CRegister::Transfer(const string &rz, const string &nName, const string &nSurname) {
	Person nPerson{nName, nSurname};
	Person person;
	string car;

	auto carIt = find_if( c_carList.begin(), c_carList.end(), [ &rz ]( pair<string, Person> &a ){
		return a.first == rz;
	});
	if( carIt != c_carList.end() ) {
		person = carIt->second;
		carIt->second = nPerson;
	} else
		return false;

	if( person.name == nPerson.name && person.surname == nPerson.surname )
		return false;
	car = carIt->first;

	/*Old owner*/
	auto personIt = find_if( c_personList.begin(), c_personList.end(), [person]( const pair<Person, CCarList> &a){
		return a.first.name == person.name && a.first.surname == person.surname;
	});
	personIt->second.Delete( car );
	if( personIt->second.amount() == 0 )
		c_personList.erase( personIt );

	/*New owner*/
	personIt = find_if( c_personList.begin(), c_personList.end(), [nPerson]( const pair<Person, CCarList> &a){
		return a.first.name == nPerson.name && a.first.surname == nPerson.surname;
	});
	if( personIt != c_personList.end() ){
		personIt->second.Add( car, nPerson );
		personIt->second.zerroIt();
	} else{
		c_personList.emplace_back( make_pair(nPerson, CCarList( rz, person )) );
	}


	return true;
}

bool CRegister::DelCar(const string &rz) {
	Person person;

	auto carIt = find_if( c_carList.begin(), c_carList.end(), [ &rz ]( pair<string, Person> &a ){
		return a.first == rz;
	});
	if( carIt != c_carList.end() ) {
		person = carIt->second;
	} else
		return false;

	/*Find owner*/
	auto personIt = find_if( c_personList.begin(), c_personList.end(), [person]( const pair<Person, CCarList> &a){
		return a.first.name == person.name && a.first.surname == person.surname;
	});
	personIt->second.Delete( carIt->first );
	if( personIt->second.amount() == 0 )
		c_personList.erase( personIt );
	c_carList.erase( carIt );

	return true;
}

CPersonList CRegister::ListPersons(void) const {
	if( c_personList.empty() )
		return CPersonList();
	return outPersons;
}

#ifndef __PROGTEST__
bool checkPerson           ( CRegister    & r,
                             const string & name,
                             const string & surname,
                             vector<string> result )
{
    for ( CCarList l = r . ListCars ( name, surname ); ! l . AtEnd (); l . Next () )
    {
//    	cout << l.RZ() << " ?= " << endl;
        auto pos = find ( result . begin (), result . end (), l . RZ () );
        if ( pos == result . end () )
            return false;
        result . erase ( pos );
    }
    return result . size () == 0;
}

int main ( void )
{
//	cout << Person().name << endl;

    CRegister b1;
    assert ( b1 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    assert ( b1 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
    assert ( b1 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
    assert ( b1 . CountCars ( "John", "Hacker" ) == 1 );
    assert ( checkPerson ( b1, "John", "Hacker", { "ABC-32-22" } ) );
    assert ( b1 . Transfer ( "XYZ-11-22", "John", "Hacker" ) == true );
    assert ( b1 . AddCar ( "XYZ-99-88", "John", "Smith" ) == true );
    assert ( b1 . CountCars ( "John", "Smith" ) == 2 );
    assert ( checkPerson ( b1, "John", "Smith", { "ABC-12-34", "XYZ-99-88" } ) );
    assert ( b1 . CountCars ( "John", "Hacker" ) == 2 );
    assert ( checkPerson ( b1, "John", "Hacker", { "ABC-32-22", "XYZ-11-22" } ) );
    assert ( b1 . CountCars ( "Peter", "Smith" ) == 0 );
    assert ( checkPerson ( b1, "Peter", "Smith", {  } ) );
    assert ( b1 . Transfer ( "XYZ-11-22", "Jane", "Black" ) == true );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 1 );
    assert ( checkPerson ( b1, "Jane", "Black", { "XYZ-11-22" } ) );
    assert ( b1 . CountCars ( "John", "Smith" ) == 2 );
    assert ( checkPerson ( b1, "John", "Smith", { "ABC-12-34", "XYZ-99-88" } ) );
    assert ( b1 . DelCar ( "XYZ-11-22" ) == true );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 0 );
    assert ( checkPerson ( b1, "Jane", "Black", {  } ) );
    assert ( b1 . AddCar ( "XYZ-11-22", "George", "White" ) == true );
    CPersonList i1 = b1 . ListPersons ();
    assert ( ! i1 . AtEnd () && i1 . Surname () == "Hacker" && i1 . Name () == "John" );
    assert ( checkPerson ( b1, "John", "Hacker", { "ABC-32-22" } ) );
//    i1 . Next ();
//    assert ( ! i1 . AtEnd () && i1 . Surname () == "Smith" && i1 . Name () == "John" );
//    assert ( checkPerson ( b1, "John", "Smith", { "ABC-12-34", "XYZ-99-88" } ) );
//    i1 . Next ();
//    assert ( ! i1 . AtEnd () && i1 . Surname () == "White" && i1 . Name () == "George" );
//    assert ( checkPerson ( b1, "George", "White", { "XYZ-11-22" } ) );
//    i1 . Next ();
//    assert ( i1 . AtEnd () );
//
//    CRegister b2;
//    assert ( b2 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
//    assert ( b2 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
//    assert ( b2 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
//    assert ( b2 . AddCar ( "XYZ-11-22", "Jane", "Black" ) == false );
//    assert ( b2 . DelCar ( "AAA-11-11" ) == false );
//    assert ( b2 . Transfer ( "BBB-99-99", "John", "Smith" ) == false );
//    assert ( b2 . Transfer ( "ABC-12-34", "John", "Smith" ) == false );
//    assert ( b2 . CountCars ( "George", "White" ) == 0 );
//    assert ( checkPerson ( b2, "George", "White", {  } ) );
//    CPersonList i2 = b2 . ListPersons ();
//    assert ( ! i2 . AtEnd () && i2 . Surname () == "Hacker" && i2 . Name () == "John" );
//    assert ( checkPerson ( b2, "John", "Hacker", { "ABC-32-22" } ) );
//    i2 . Next ();
//    assert ( ! i2 . AtEnd () && i2 . Surname () == "Smith" && i2 . Name () == "John" );
//    assert ( checkPerson ( b2, "John", "Smith", { "ABC-12-34" } ) );
//    i2 . Next ();
//    assert ( ! i2 . AtEnd () && i2 . Surname () == "Smith" && i2 . Name () == "Peter" );
//    assert ( checkPerson ( b2, "Peter", "Smith", { "XYZ-11-22" } ) );
//    i2 . Next ();
//    assert ( i2 . AtEnd () );

    return 0;
}
#endif /* __PROGTEST__ */
