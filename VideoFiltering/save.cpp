#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include <bitset>
#include <io.h>

using namespace std;

const uint32_t BIG_ENDIAN_ID    = 0x4d6d6d4d;
const uint32_t LITTLE_ENDIAN_ID = 0x49696949;
const uint32_t FRAME_ID         = 0x46727246;
const uint32_t VIDEO_DATA_ID    = 0x56696956;
const uint32_t AUDIO_LIST_ID    = 0x416c6c41;
const uint32_t AUDIO_DATA_ID    = 0x41757541;
#endif /* __PROGTEST__ */




struct SHeader{
    uint32_t endianness;
    uint32_t frames_num;
    uint32_t payload_len;
};

SHeader read_header( ifstream & stream ){
    SHeader header{0, 0, 0};
    uint32_t tmp;

    stream.read((char *) &header.endianness, sizeof(header.endianness));
    stream.read((char *) &header.frames_num, sizeof(header.frames_num));
    stream.read((char *) &header.payload_len, sizeof(header.payload_len));
    cout << "*******Global header reading***********" << endl;
    cout << "endianness: 0x" << hex << header.endianness << endl;
    cout << "frames_num: " << header.frames_num << endl;
    cout << "payload_len: " << header.payload_len << endl;

    return header;
}

bool  filterFile          ( const char      * srcFileName,
                            const char      * dstFileName,
                            const char      * lang ){
    SHeader header;
    uint32_t tmp;
    ifstream src_file;
    src_file.open( srcFileName, ios::in | ios::binary );
    if( !src_file ){
        src_file.close();
        return false;
    }

    header = read_header( src_file );
    if( header.endianness != BIG_ENDIAN_ID && header.endianness != LITTLE_ENDIAN_ID ){
        src_file.close();
        return false;
    }

    for( int i = 0; i < header.frames_num; i++ ){
        cout << "*******Frame#" << i << " header reading***********" << endl;
        src_file.read((char *) &tmp, sizeof(tmp));
        if( tmp != FRAME_ID ){
            src_file.close();
            return false;
        }
    }
}

#ifndef __PROGTEST__

bool identicalFiles ( const char * fileName1,
                      const char * fileName2 )
{
    ifstream file1, file2;
    file1.open(fileName1, ios::in | ios::binary);
    file2.open(fileName2, ios::in | ios::binary);
    file1.seekg(0, ifstream::end);
    file2.seekg(0, ifstream::end);
    int length1 = (int) file1.tellg();
    int length2 = (int) file2.tellg();
    char tmp1, tmp2;
    if(length1 != length2){
        file1.close();
        file2.close();
        return false;
    }
    file1.seekg(0, ifstream::beg);
    file2.seekg(0, ifstream::beg);

    while(file1.get(tmp1) && file2.get(tmp2)){
        if(tmp1 != tmp2){
            file1.close();
            file2.close();
            return false;
        } else{
            continue;
        }
    }

    file1.close();
    file2.close();
    return true;
}

int main(){
    string working_dir = "C:\\Users\\kosty\\OneDrive\\dev\\c++\\PA2(2021)\\kolchkos\\homework\\hw01\\";
    string in_file = working_dir + "in_0000.in";

    filterFile( in_file.c_str(), "myout_0000_cs.out", "cs" );
//    assert( filterFile( "in_000.in", "myout_0000_cs.out", "cs" ) );
}
#endif /* __PROGTEST__ */
