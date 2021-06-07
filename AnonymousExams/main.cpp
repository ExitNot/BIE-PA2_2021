#ifndef __PROGTEST__
#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

class CResult
{
  public:
                   CResult     ( const string & name,
                                 unsigned int   studentID,
                                 const string & test,
                                 int            result )
      : m_Name ( name ),
        m_StudentID ( studentID ),
        m_Test ( test ),
        m_Result ( result )
    {
    }
    bool           operator == ( const CResult& other ) const
    {
      return m_Name == other . m_Name
             && m_StudentID == other . m_StudentID
             && m_Test == other . m_Test
             && m_Result == other . m_Result;
    }
    string         m_Name;
    unsigned int   m_StudentID;
    string         m_Test;
    int            m_Result;
 };
#endif /* __PROGTEST__ */

struct Student{
	string name;  // name and surname
	unsigned studentId{0};
	set<string> cardIds;
};

/* Comparator */
inline bool operator < ( const Student & a, const Student & b ){
	return a.studentId < b.studentId;
}

class CExam
{
  public:
    static const int SORT_NONE   = 0;
    static const int SORT_ID     = 1;
    static const int SORT_NAME   = 2;
    static const int SORT_RESULT = 3;

    bool           Load        ( istream      & cardMap );
    bool           Register    ( const string & cardID,
                                 const string & test );
    bool           Assess      ( unsigned int   studentID,
                                 const string & test,
                                 int            result );
    list<CResult>  ListTest    ( const string & testName,
                                 int            sortBy ) const;
    set<unsigned int> ListMissing ( const string & testName ) const;

    CExam() = default;
private:

	map<string, vector< pair<Student, int> >> exams;
	set<Student> studentList;
	map<string, unsigned> cardsList;
};


bool CExam::Load( istream & cardMap ){
	string line, token;
	size_t pos = 0;
	set<Student> in_students;
	map<string, unsigned> in_cards;  // mapping cardID -> studentID

	/* Getting each line(student) one by one */
	while( getline( cardMap, line ) ){
		Student student;

		/* read studentID */
		pos = line.find(':');
		token = line.substr(0, pos);  // token = studentID
		auto rm = remove(token.begin(), token.end(), ' ');
		token.erase( rm, token.end());
		student.studentId = stoi(token);
		line.erase(0, pos + 1 );

		/* read name_and_surname */
		pos = line.find(':');
		student.name = line.substr(0, pos);
		line.erase(0, pos + 1 );

		/* read all cardID's */
		while( (pos = line.find(',')) != string::npos ){
			token = line.substr(0, pos);  // token = cardID
			auto it = remove(token.begin(), token.end(), ' ');
			token.erase( it, token.end());
			student.cardIds.insert(token);
			line.erase(0, pos + 2);

			/* check for duplicates of cardID's */
			if( cardsList.find(token) != end(cardsList) )
				return false;
			in_cards.insert(make_pair(token, student.studentId));
		}
		token = line;  // token = cardID
		rm = remove(token.begin(), token.end(), ' ');
		token.erase( rm, token.end());
		student.cardIds.insert(token);

		/* check for duplicates of cardID's */
		if( cardsList.find(token) != end(cardsList) || in_cards.find(token) != end(in_cards) )
			return false;
		in_cards.insert(make_pair(token, student.studentId));

		/* check for duplicates of studentID */
		if( studentList.find( student ) != end(studentList) || in_students.find(student) != end(in_students) )
			return false;

		in_students.insert( student );  // pair(iterator, isInserted)
	}

	/* Concatenating students from stream to main list of students */
	studentList.insert( in_students.begin(), in_students.end() );

	/* Concatenating cards from stream to main list of cards */
	cardsList.insert( begin(in_cards), end(in_cards) );

	return true;
}

bool CExam::Register( const string & cardID, const string & test ){
	Student stud{"", 0, {""}};
	bool exam_exist = false;

	/* Searching cardID, if it exists, and student who own it */
	auto card_it = cardsList.find( cardID );
	if( card_it != end(cardsList) ){
		stud.studentId = card_it->second;
		stud = *studentList.find( stud );
	} else
		return false;

	/* Searching needed test. Create if not found */
	auto exam_it = exams.find( test );
	if( exam_it == end(exams) ){
		vector<pair<Student, int>> pushVec;
		pushVec.emplace_back(stud, -1 );  // mark '-1' - means that exam was registered
		exams.insert( make_pair(test, pushVec) );
	} else{
		/* Checking whether student is already registered for this exam */
		auto it = find_if( begin(exam_it->second), end(exam_it->second),
						   [ stud ]( const pair<Student, int> & elem ){ return (elem.first.studentId == stud.studentId); } );
		if( it != end(exam_it->second) )
			return false;

		/* Register student */
		exam_it->second.emplace_back( stud, -1 );  // mark '-1' - means that exam was registered
	}

	return true;
}

bool CExam::Assess( unsigned int studentID, const string & test, int result ){
	Student stud{"", studentID, {""}};

	/* Search by studentID */
	if( studentList.find( stud ) == end(studentList) )
		return false;

	/* Searching an exam */
	auto exam_it = exams.find( test );
	if( exam_it != end(exams) ){
		/* Checking whether student is registered for this exam */
		auto it = find_if( begin(exam_it->second), end(exam_it->second),
							[ stud ]( const pair<Student, int> & elem ){ return (elem.first.studentId == stud.studentId); } );
		if( it == end(exam_it->second) || (*it).second != -1 )  // The student was not registered for the exam or he has already been assessed
			return false;

		(*it).second = result;
		rotate( it, it + 1, exam_it->second.end() );
	} else
		return false;

	return true;
}

list<CResult> CExam::ListTest( const string & testName, int sortBy ) const{
	list<CResult> out;

	/* Searching an exam and taking all values except '-1' to out */
	auto exam_it = exams.find( testName );
	if( exam_it != end(exams) ){
		for( auto & student : exam_it->second ){
			if( student.second == -1 )
				continue;
			out.emplace_back( student.first.name, student.first.studentId, testName, student.second );
		}
	}

	/* Sorting */
	if( sortBy == CExam::SORT_ID ) {  // SORT_ID (by student ID, ascending)
		out.sort( []( const CResult & lhs, const CResult & rhs ){
			return (lhs.m_StudentID < rhs.m_StudentID);
		} );
	}else if( sortBy == CExam::SORT_NAME ) {  // SORT_NAME (by name, ascending)
		out.sort( []( const CResult & lhs, const CResult & rhs ){
			return (lhs.m_Name.compare(rhs.m_Name) < 0);
		} );
	}else if( sortBy == CExam::SORT_RESULT ) {  // SORT_RESULT (by the result, descending)
		out.sort( []( const CResult & lhs, const CResult & rhs ){
			return lhs.m_Result > rhs.m_Result;
		} );
	}

	return out;
}

set<unsigned int> CExam::ListMissing( const string & testName ) const{
	set<unsigned int> out;

	/* Searching an exam and taking all '-1' values */
	auto exam_it = exams.find( testName );
	if( exam_it != end(exams) ){
		for( auto & student : exam_it->second ){
			if( student.second != -1 )
				continue;
			out.insert( student.first.studentId );
		}
	}

	return out;
}

#ifndef __PROGTEST__
int main ( void )
{
	istringstream iss;
	CExam         m;
	iss . clear ();
	iss . str ( "123456:Smith John:er34252456hjsd2451451, 1234151asdfe5123416, asdjklfhq3458235\n"
	  "654321:Nowak Jane: 62wtsergtsdfg34\n"
	  "456789:Nowak Jane: okjer834d34\n"
	  "987:West Peter Thomas:sdswertcvsgncse\n" );
	assert ( m . Load ( iss ) );

	assert ( m . Register ( "62wtsergtsdfg34", "PA2 - #1" ) );
	assert ( m . Register ( "62wtsergtsdfg34", "PA2 - #2" ) );
	assert ( m . Register ( "er34252456hjsd2451451", "PA2 - #1" ) );
	assert ( m . Register ( "er34252456hjsd2451451", "PA2 - #3" ) );
	assert ( m . Register ( "sdswertcvsgncse", "PA2 - #1" ) );
	assert ( !m . Register ( "1234151asdfe5123416", "PA2 - #1" ) );
	assert ( !m . Register ( "aaaaaaaaaaaa", "PA2 - #1" ) );

	assert ( m . Assess ( 123456, "PA2 - #1", 50 ) );
	assert ( m . Assess ( 654321, "PA2 - #1", 30 ) );
	assert ( m . Assess ( 654321, "PA2 - #2", 40 ) );
	assert ( m . Assess ( 987, "PA2 - #1", 100 ) );
	assert ( !m . Assess ( 654321, "PA2 - #1", 35 ) );
	assert ( !m . Assess ( 654321, "PA2 - #3", 35 ) );
	assert ( !m . Assess ( 999999, "PA2 - #1", 35 ) );

	assert ( m . ListTest ( "PA2 - #1", CExam::SORT_RESULT ) == (list<CResult>
	{
	CResult ( "West Peter Thomas", 987, "PA2 - #1", 100 ),
	CResult ( "Smith John", 123456, "PA2 - #1", 50 ),
	CResult ( "Nowak Jane", 654321, "PA2 - #1", 30 )
	} ) );
	assert ( m . ListTest ( "PA2 - #1", CExam::SORT_NAME ) == (list<CResult>
	{
	CResult ( "Nowak Jane", 654321, "PA2 - #1", 30 ),
	CResult ( "Smith John", 123456, "PA2 - #1", 50 ),
	CResult ( "West Peter Thomas", 987, "PA2 - #1", 100 )
	} ) );
	assert ( m . ListTest ( "PA2 - #1", CExam::SORT_NONE ) == (list<CResult>
	{
	CResult ( "Smith John", 123456, "PA2 - #1", 50 ),
	CResult ( "Nowak Jane", 654321, "PA2 - #1", 30 ),
	CResult ( "West Peter Thomas", 987, "PA2 - #1", 100 )
	} ) );
	assert ( m . ListTest ( "PA2 - #1", CExam::SORT_ID ) == (list<CResult>
	{
	CResult ( "West Peter Thomas", 987, "PA2 - #1", 100 ),
	CResult ( "Smith John", 123456, "PA2 - #1", 50 ),
	CResult ( "Nowak Jane", 654321, "PA2 - #1", 30 )
	} ) );
	assert ( m . ListMissing ( "PA2 - #3" ) == (set<unsigned int>{ 123456 }) );
	iss . clear ();
	iss . str ( "888:Watson Joe:25234sdfgwer52, 234523uio, asdf234235we, 234234234\n" );
	assert ( m . Load ( iss ) );

	assert ( m . Register ( "234523uio", "PA2 - #1" ) );
	assert ( m . Assess ( 888, "PA2 - #1", 75 ) );
	iss . clear ();
	iss . str ( "555:Gates Bill:ui2345234sdf\n"
	  "888:Watson Joe:2345234634\n" );
	assert ( !m . Load ( iss ) );

	assert ( !m . Register ( "ui2345234sdf", "PA2 - #1" ) );
	iss . clear ();
	iss . str ( "555:Gates Bill:ui2345234sdf\n"
	  "666:Watson Thomas:okjer834d34\n" );
	assert ( !m . Load ( iss ) );

	assert ( !m . Register ( "ui2345234sdf", "PA2 - #3" ) );
	iss . clear ();
	iss . str ( "555:Gates Bill:ui2345234sdf\n"
	  "666:Watson Thomas:jer834d3sdf4\n" );
	assert ( m . Load ( iss ) );

	assert ( m . Register ( "ui2345234sdf", "PA2 - #3" ) );
	assert ( m . ListMissing ( "PA2 - #3" ) == (set<unsigned int>{ 555, 123456 }) );

	/* MY TESTING */
	CExam         e;
	iss . str ("1:Ker Solodov: prow, 1card\n"
				"2:Inna Germanova: tret, 2card\n"
				"3:Ivan Pidorov: bret, 3card\n"
				"4:Vchem Sila: brat, 4card\n"
				"5:Sam Minterns: mint, 5card\n"
				"6:Neal Vasnecov: volt, 6card\n"
	);
	iss . clear ();
	assert ( e . Load ( iss ) );

	assert ( e . Register ( "prow", "PA2" ) );
	assert ( e . Register ( "tret", "PA2" ) );
	assert ( e . Register ( "bret", "PA2" ) );
	assert ( e . Register ( "brat", "PA2" ) );
	assert ( e . Register ( "mint", "PA2" ) );
	assert ( e . Register ( "volt", "PA2" ) );
	assert ( e . Register ( "prow", "BEZ" ) );
	assert ( e . Register ( "tret", "BEZ" ) );
	assert ( e . Register ( "bret", "BEZ" ) );
	assert ( e . Register ( "brat", "SAP" ) );
	assert ( e . Register ( "mint", "SAP" ) );
	assert ( e . Register ( "volt", "SAP" ) );

	assert ( !e . Register ( "random", "PA2" ) );
	assert ( !e . Register ( "1card", "PA2" ) );
	assert ( !e . Register ( "3card", "PA2" ) );
	assert ( !e . Register ( "5card", "PA2" ) );
	assert ( e . Assess( 1, "PA2", 10 ) );
	assert ( e . Assess( 2, "PA2", 45 ) );
	assert ( e . Assess( 3, "PA2", 130 ) );
	assert ( !e . Register ( "1card", "PA2" ) );
	assert ( !e . Register ( "1card", "BEZ" ) );
	assert ( !e . Register ( "2card", "BEZ" ) );
	assert ( !e . Register ( "3card", "BEZ" ) );
	assert ( e . Register ( "1card", "SAP" ) );
	assert ( !e . Register ( "7card", "PA2" ) );



  return 0;
}
#endif /* __PROGTEST__ */
