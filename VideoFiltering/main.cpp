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


using namespace std;

const uint32_t BIG_ENDIAN_ID    = 0x4d6d6d4d;
const uint32_t LITTLE_ENDIAN_ID = 0x49696949;
const uint32_t FRAME_ID         = 0x46727246;
const uint32_t VIDEO_DATA_ID    = 0x56696956;
const uint32_t AUDIO_LIST_ID    = 0x416c6c41;
const uint32_t AUDIO_DATA_ID    = 0x41757541;
#endif /* __PROGTEST__ */

class CData{
    uint32_t payloadLen;

public:

//    bool redirectPayload( ifstream & in, ofstream & out ){
//
//        copy_n( in, out, payloadLen );
//        return true;
//    }

    void skipPayload( ifstream & in, ofstream & out ){
        uint32_t buff;

        in.ignore( payloadLen );
        in.read((char *) &buff, sizeof(uint32_t));  // crc32
    }

    static void copy_n( ifstream & in, ofstream & out, int n ){  // n - number of bytes to copy
        uint32_t buff;

        while( n > (int) sizeof(uint32_t) ){
            in.read((char *) &buff, sizeof(uint32_t));
            out.write( (char *) (&buff), sizeof(uint32_t) );

            n -= 4;
            buff = 0;
        }
        in.read((char *) &buff, n);
        out.write( (char *) (&buff), n );
    }

    static void readCRC32( ifstream & in, ofstream & out ){
        uint32_t crc;  // temporary solution for mandatory tests | TODO recalculate CRC32

        in.read((char *) &crc, sizeof(uint32_t));
        out.write((char *) &crc, sizeof(uint32_t));
    }

    uint32_t getPayloadLen() const {
        return payloadLen;
    }

    void setPayloadLen(uint32_t in_payloadLen) {
        CData::payloadLen = in_payloadLen;
    }
};

class CAudioData : public CData{
    char lang[2];

public:

    bool readHead( ifstream & in ){
        uint32_t buff;

        in.read((char *) &buff, sizeof(uint32_t));
        if( buff != AUDIO_DATA_ID )
            return false;

        in.read((char *) &lang, 2*sizeof(char) );

        in.read( (char *) &buff, sizeof(uint32_t) );
        this->setPayloadLen(buff);

        return true;
    }

    void writeHead( ofstream & out ){
        out.write( (char *) (&AUDIO_DATA_ID), sizeof(uint32_t) );
        out.write( (char *) &lang, sizeof(lang) );
        uint32_t tmp = this->getPayloadLen();
        out.write( (char *) &tmp, sizeof(uint32_t) );
    }

    const char * getLang() const {
        return lang;
    }
};

class CVideoData : public CData{

public:
    bool verifyHead( ifstream & in, ofstream & out ){
        uint32_t buff;

        in.read((char *) &buff, sizeof(uint32_t));
        if( buff != VIDEO_DATA_ID )
            return false;
        out.write( (char *) (&VIDEO_DATA_ID), sizeof(uint32_t) );

        in.read( (char *) &buff, sizeof(uint32_t) );
        this->setPayloadLen(buff);
        out.write( (char *) &buff, sizeof(uint32_t) );

        return true;
    }
};

class CFrame{
    uint32_t payloadLen;
    CVideoData videoData{};

    uint32_t audioStreamsNum;
    uint32_t listPayloadLen;
    bool foundCAD = false;
public:

    CFrame() {
        payloadLen = 0;
        audioStreamsNum = 0;
        listPayloadLen = 0;
    }

    /*  Read video data  */
    bool readCVD( ifstream & in, ofstream & out ){
        if( !videoData.verifyHead( in, out ) )
            return false;
        CData::copy_n( in, out, videoData.getPayloadLen() );
        CData::readCRC32( in, out );

        return true;
    }

    /*  Read audio data and write only if lang is the same  */
    bool readCADbyLang( ifstream & in, ofstream & out, const char * lang ){
        CAudioData audioData{};

        if( !audioData.readHead( in ) )
            return false;

        if( audioData.getLang()[0] == lang[0] && audioData.getLang()[1] == lang[1] ){
            listPayloadLen = 14 + audioData.getPayloadLen();
            foundCAD = true;
            writeCALheader(out);
            audioData.writeHead(out);
            CData::copy_n( in, out, audioData.getPayloadLen() );
            CData::readCRC32( in, out );
        }else{
            audioData.skipPayload( in, out );

        }

        return true;
    }

    /*  Read audio list  */
    bool readCAL( ifstream & in, ofstream & out, const char * lang ){
        uint32_t buff;

        /*Reading header of the list*/
        in.read((char *) &buff, sizeof(uint32_t));
        if( buff != AUDIO_LIST_ID )
            return false;

        in.read((char *) &audioStreamsNum, sizeof(uint32_t));

        in.read((char *) &listPayloadLen, sizeof(uint32_t));

        /*Read CAD() num of audio streams times */
        for( uint32_t i = 0; i < audioStreamsNum; i++ ){
            if( !readCADbyLang( in, out, lang ) )
                return false;
        }
        if(!foundCAD){
            listPayloadLen = 0;
            writeCALheader(out);
        }
        CData::readCRC32( in, out );


        return true;
    }

    void writeCALheader( ofstream & out ){
        uint32_t tmp;

        if(foundCAD)
            tmp = 1;
        else
            tmp = 0;

        out.write( (char *) (&AUDIO_LIST_ID), sizeof(uint32_t) );
        out.write( (char *) &tmp, sizeof(uint32_t) );
        out.write( (char *) &listPayloadLen, sizeof(uint32_t) );
    }

    bool verifyHead( ifstream & in, ofstream & out ){
        uint32_t buff;

        in.read((char *) &buff, sizeof(buff));
        if( buff != FRAME_ID )
            return false;
        out.write( (char *) (&FRAME_ID), sizeof(uint32_t) );

        in.read((char *) &buff, sizeof(buff));
        this->setPayloadLen(buff);
        out.write( (char *) (&buff), sizeof(uint32_t) );

        return true;
    }

    void reacalcPayload( ofstream & out ){
        long pos = out.tellp();
//        frame_len = CVDheader + CVD + crc32 + CALheader + CAL + crc32
        payloadLen = 8 + videoData.getPayloadLen() + 4 + 12 + listPayloadLen + 4;
        out.seekp(pos - (payloadLen + 4));
        out.write( (char *) (&payloadLen), sizeof(uint32_t) );
        out.seekp(pos);
    }

    uint32_t getPayloadLen() const {
        return payloadLen;
    }

    void setPayloadLen(uint32_t in_payloadLen) {
        CFrame::payloadLen = in_payloadLen;
    }
};

class CVideoFile{
    uint32_t endianness;
    uint32_t framesNum;
    uint32_t payloadLen;

    vector<CFrame> framesList;

public:

    bool scanHeader(ifstream & in, ofstream & out ){
        in.read((char *) &endianness, sizeof(uint32_t));
        in.read((char *) &framesNum, sizeof(uint32_t));
        in.read((char *) &payloadLen, sizeof(uint32_t));
//        TODO check payload length

        in.seekg(0, std::ifstream::end);
        int length = in.tellg();
        length -= 16;  // header + crc32
        if( length != payloadLen )
            return false;
        in.seekg(12);

        if( endianness != BIG_ENDIAN_ID && endianness != LITTLE_ENDIAN_ID )
            return false;

        out.write( (char *) (&endianness), sizeof(uint32_t) );
        out.write( (char *) (&framesNum), sizeof(uint32_t) );
        out.write( (char *) (&payloadLen), sizeof(uint32_t) );
        return true;
    }

    uint32_t getEndianness() const { return endianness; }

    void setEndianness(uint32_t in_endianness) {
        CVideoFile::endianness = in_endianness;
    }

    uint32_t getFramesNum() const { return framesNum; }

    void setFramesNum(uint32_t in_framesNum) {
        framesNum = in_framesNum;
    }

    uint32_t getPayloadLen() const { return payloadLen; }

    void setPayloadLen(uint32_t in_payloadLen) {
        payloadLen = in_payloadLen;
    }

    const vector<CFrame> &getFramesList() const {
        return framesList;
    }

    void addFrames(const CFrame& frame) {
        framesList.push_back( frame );
    }

};

bool  filterFile          ( const char      * srcFileName,
                            const char      * dstFileName,
                            const char      * lang ){
    auto * videoFile = new CVideoFile();
    uint32_t buff;
    vector<CFrame> frames;
    ifstream src_file;
    ofstream dst_file;

    src_file.open( srcFileName, ios::in | ios::binary );
    if( !src_file ){
        src_file.close();
        return false;
    }
    dst_file.open(dstFileName, ios::out | ios::binary);
    if( !dst_file ){
        src_file.close();
        dst_file.close();
        return false;
    }

    if( !videoFile->scanHeader( src_file, dst_file ) ){
        src_file.close();
        dst_file.close();
        return false;
    }

    for( uint32_t i = 0; i < videoFile->getFramesNum(); i++ ){
        CFrame frame;

        if( !frame.verifyHead( src_file, dst_file ) ||
            !frame.readCVD( src_file, dst_file ) ){
            src_file.close();
            dst_file.close();
            return false;
        }
        if( !frame.readCAL( src_file, dst_file, lang ) ){
            src_file.close();
            dst_file.close();
            return false;
        }
        frame.reacalcPayload( dst_file );

        CData::readCRC32( src_file, dst_file );
        frames.push_back(frame);
    }
    CData::readCRC32( src_file, dst_file );

    dst_file.seekp(8);
    buff = 0;
    for( auto i : frames ){
        buff += 8 + i.getPayloadLen() + 4;
    }
    dst_file.write( (char *) (&buff), sizeof(uint32_t) );

    src_file.close();
    dst_file.close();
    if( dst_file.fail() ){
        return false;
    }
    delete videoFile;
    return true;
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

    if( filterFile( (working_dir + "Tests\\in_4039636.bin").c_str(),
                    "out_4039636_en.out", "en" ) ){
        cout << "SUCCESSES" << endl;
    }

}
#endif /* __PROGTEST__ */
