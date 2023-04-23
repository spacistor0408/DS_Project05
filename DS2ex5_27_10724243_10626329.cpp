#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <queue>

using namespace std;

#define MAX_LEN 10
#define MAX_CAPACITY 200

typedef struct sP
{

    char putID[MAX_LEN] ;
    char getID[MAX_LEN] ;
    float weight ;

    bool operator<( const sP& sp2 ) const
    {
        if ( this->weight != sp2.weight )
            return this->weight < sp2.weight ;

        if ( atoi(this->putID) != atoi(sp2.putID) )
            return atoi(this->putID) > atoi(sp2.putID) ;

        return atoi(this->getID) > atoi(sp2.getID) ;

    }

    /*
        bool operator<( const sP& sp2 ) const {
            return this->weight <= sp2.weight ;
        }
    */
} studentPair ;

typedef struct IndexT
{
    int offset = -1 ;
    float weight ;
} IndexTable;

class ExternalMergeSort   // Data manage and In/Output
{

    priority_queue< studentPair > buffer ;

    queue< studentPair > run1, run2, buff ;
    string fname ;
    int maxRun, run ;
    int indexNum ;
    float cur_weight ;
    vector<IndexTable> index ;

    clock_t begin, end ;

    void ReadFileName()
    {
        fname.clear() ;
        cin >> fname ;
    } // ReadFileName()

    // when divide
    bool Write( )
    {

        string num = "_" + to_string( maxRun ) + "_" + to_string(run) ;
        fstream fOut( ("sorted" + fname + num + ".bin" ).c_str(), fstream::out | fstream::binary ) ;

        studentPair onePr ;

        while( !buffer.empty() )
        {

            // fOut << "[" << curData << "] " ;

            onePr = buffer.top() ;
            fOut.write( (char*)&onePr, sizeof(onePr) ) ;
            // fOut << setw(10) << onePr.putID << ", " << onePr.getID << ", " << onePr.weight << endl ;


            buffer.pop() ;

        } // while
        fOut.close() ;

    } // Write()

    // when merge
    bool Write( int nextMaxRun )
    {

        string num = "_" + to_string( nextMaxRun ) + "_" + to_string(run/2) ;
        fstream fOut( ("sorted" + fname + num + ".bin" ).c_str(), fstream::app | fstream::binary ) ;

        studentPair onePr ;

        int count = 0;

        while( !buff.empty() )
        {

            // fOut << "[" << curData << "] " ;

            onePr = buff.front() ;
            //fOut << onePr.putID << onePr.getID << onePr.weight << endl ;
            fOut.write( (char*)&onePr, sizeof(onePr) ) ;

            buff.pop() ;
            count ++ ;
        } // while

        // cout << "write the piece of " << count << " datas successful!" << endl ; // debug line
        fOut.close() ;

    } // Write()

    // when export
    bool Export( string fileName, string outname )
    {

        fstream binFile;
        studentPair oneSt;
        int stNo = 0;

        binFile.open( fileName.c_str(), fstream::in | fstream::binary );
        ofstream fout( outname.c_str(), fstream::out | fstream::binary ) ;

        if( binFile.is_open() )
        {
            binFile.seekg( 0, binFile.end );
            stNo = binFile.tellg() / sizeof( oneSt );
            binFile.seekg( 0, binFile.beg );
            cout << endl;
            for( int i = 0 ; i < stNo ; i++ )
            {
                binFile.read( ( char * )&oneSt, sizeof( oneSt ) );

                fout.write((char*)&oneSt, sizeof(oneSt)) ;
//	           		fout.write((char*)&oneSt.putID, sizeof(oneSt.putID)) ;
//					fout.write((char*)&oneSt.getID, sizeof(oneSt.getID)) ;
//					fout.write((char*)&oneSt.weight, sizeof(oneSt.weight)) ;

            } // for
        }
        else
        {
            return false ;
        }

        binFile.close();
        fout.close() ;
        remove( fileName.c_str() ) ;

        return true ;
    } // Export()

public:

    ExternalMergeSort()
    {
        run = 0 ;    // constructor
        indexNum = 1 ;
        cur_weight = 1.00 ;
    }

    bool DivideBin()
    {

        fstream binFile ;

        binFile.open( ("pairs" + fname + ".bin" ).c_str(), fstream::in | fstream::binary ) ;

        if ( binFile.is_open() )
        {

            studentPair onePr ;
            int stNo = 0 ;

            binFile.seekg(0,binFile.end) ; // Goto the end of file
            stNo = ceil( binFile.tellg() / sizeof(onePr) ) ;
            binFile.seekg(0,binFile.beg) ; // Goto the begin

            maxRun = ceil( stNo/MAX_CAPACITY ) ;

            for ( int i = 0 ; i<stNo ; i++ )
            {

                binFile.read( (char*)&onePr, sizeof(onePr) ) ;
                buffer.push(onePr) ;

                if ( (i+1)%MAX_CAPACITY == 0 || (i+1) == stNo )
                {

                    Write( ) ;
                    run++ ;

                } // if

            } // for

        } // if
        else
        {
            return false ;
        }

        binFile.close() ;
        return true ;

    } // DivideBin()

    // when marge
    bool ReadBin1( int off )
    {

        fstream binFile ;

        string num = "_" + to_string( maxRun ) + "_" + to_string(run) ;
        binFile.open( ("sorted" + fname + num + ".bin" ).c_str(), fstream::in | fstream::binary ) ;

        if ( binFile.is_open() )
        {

            studentPair onePr ;
            int stNo = 0 ;

            binFile.seekg(0,binFile.end) ; // Goto the end of file
            stNo = ceil( binFile.tellg() / sizeof(onePr) ) ;

            // cout << "[" << stNo << "]" << endl ; // debug line

            if ( off >= stNo ) return false ;
            else binFile.seekg( off*sizeof(onePr),binFile.beg ) ;  // Goto the off data

            for ( int i = off ; i<off+MAX_CAPACITY/2 && i<stNo ; i++ )
            {

                binFile.read( (char*)&onePr, sizeof(onePr) ) ;
                run1.push(onePr) ;
                // cout << i << "\t" << onePr.weight << endl ; // debug line

            } // for

        } // if
        else
        {
            return false ;
        }

        binFile.close() ;
        return true ;

    } // ReadBin1()

    // when marge
    bool ReadBin2( int off )
    {

        fstream binFile ;

        string num = "_" + to_string( maxRun ) + "_" + to_string(run+1) ;
        binFile.open( ("sorted" + fname + num + ".bin" ).c_str(), fstream::in | fstream::binary ) ;

        if ( binFile.is_open() )
        {

            studentPair onePr ;
            int stNo = 0 ;

            binFile.seekg(0,binFile.end) ; // Goto the end of file
            stNo = ceil( binFile.tellg() / sizeof(onePr) ) ;

            // cout << off << '\t' << stNo << endl ; // debug line

            if ( off >= stNo ) return false ;
            else binFile.seekg( off*sizeof(onePr),binFile.beg) ;  // Goto the offset data

            // cout << "true" << endl ; // debug line

            for ( int i = off ; i<off+MAX_CAPACITY/2 && i<stNo ; i++ )
            {

                binFile.read( (char*)&onePr, sizeof(onePr) ) ;
                // cout << "run2 left : " << onePr.putID << '\t' << onePr.getID << '\t' << onePr.weight << endl ; // debug line
                run2.push(onePr) ;

            } // for

        } // if
        else
        {
            return false ;
        }

        binFile.close() ;
        return true ;

    } // ReadBin2()

    // when marge
    void MergeSort()
    {

        int off1 = 0, off2 = 0 ;
        bool isOnlyOneRun = false ;

        ReadBin1( off1 ) ;
        if ( !ReadBin2( off2 ) ) isOnlyOneRun = true ;

        studentPair onePr ;
        int count = 0 ;

        if ( run1.empty() ) isOnlyOneRun = true ;
        if ( run2.empty() ) isOnlyOneRun = true ;

        while ( !isOnlyOneRun )
        {
            /*
                        // debug block
                        cout << count << '\t' ;

                        if ( !run1.empty() ) cout << run1.front().weight ;
                        else cout << "empty!" ;

                        cout << '\t' ;

                        if ( !run2.empty() ) cout << run2.front().weight ;
                        else cout << "empty!" ;

                        cout << '\t' ;

                        // debug block
            */

            if ( run1.front().weight >= run2.front().weight )
            {
                onePr = run1.front() ;
                buff.push( onePr ) ;
                run1.pop() ;
            }
            else
            {
                onePr = run2.front() ;
                buff.push( onePr ) ;
                run2.pop() ;
            } // else

            if ( run1.empty() && !isOnlyOneRun )
            {
                off1 += MAX_CAPACITY/2 ;
                if ( !ReadBin1( off1 ) )
                {
                    isOnlyOneRun = true ;
                    remove(("sorted" + fname + "_" + to_string( maxRun ) + "_" + to_string(run) + ".bin" ).c_str()) ; // remove file
                }
            }
            if ( run2.empty() && !isOnlyOneRun )
            {
                off2 += MAX_CAPACITY/2 ;
                if ( !ReadBin2( off2 ) )
                {
                    isOnlyOneRun = true ;
                    remove(("sorted" + fname + "_" + to_string( maxRun ) + "_" + to_string(run+1) + ".bin" ).c_str()) ; // remove file
                }
            }
            // cout << isOnlyOneRun << endl ;

            // cout << "[" << run1.empty() << "]" << endl ;



            if ( buff.size() == MAX_CAPACITY )
            {
                Write( maxRun/2 ) ;
                if ( isOnlyOneRun ) break ;
            } // if

            count++ ;

        } // while

        // put the left datas
        if ( isOnlyOneRun )
        {

            while ( !run1.empty() )
            {
                // cout << "run1 left : " << run1.front().putID << '\t' << run1.front().getID << '\t' << run1.front().weight << endl ; // debug line
                onePr = run1.front() ;
                buff.push( onePr ) ;
                run1.pop() ;

                if ( run1.empty() )
                {
                    off1 += MAX_CAPACITY/2 ;
                    if ( !ReadBin1( off1 ) )
                    {
                        remove(("sorted" + fname + "_" + to_string( maxRun ) + "_" + to_string(run) + ".bin" ).c_str()) ; // remove file
                        break ;
                    }
                }
            } // while

            while ( !run2.empty() )
            {
                // cout << "run2 left : " << run2.front().putID << '\t' << run2.front().getID << '\t' << run2.front().weight << endl ; // debug line
                onePr = run2.front() ;
                buff.push( onePr ) ;
                run2.pop() ;

                if ( run2.empty() )
                {
                    off2 += MAX_CAPACITY/2 ;
                    if ( !ReadBin2( off2 ) )
                    {
                        remove(("sorted" + fname + "_" + to_string( maxRun ) + "_" + to_string(run+1) + ".bin" ).c_str()) ; // remove file
                        break ;
                    }
                }
            } // while

        } // if

        Write( maxRun/2 ) ;

    } // MergeSort()

    // when marge
    void Merge()
    {

        run = 0 ;

        while( run <= maxRun )
        {

            MergeSort() ;
            // cout << "-------------------- success ---------------------" << endl << endl ;
            run += 2 ;
            // cout << endl << endl ;

        } // while
        if ( maxRun == 1 ) maxRun = 0 ;
        else maxRun = maxRun/2 ;

    } // Merge()

    bool ReadFile()
    {

        /*---------- deal with File ----------*/
        do
        {
            cout << "Input a file number (501, 502, ...[0]Quit): " ;
            ReadFileName() ;

            if ( fname == "0" )
            {
                return false ;
            } // Quit to open file

            cout << endl ;

            if ( DivideBin() )
            {

                begin = clock() ;

                // external marge sort
                while ( maxRun != 0 )
                {
                    cout << "Now there are " << maxRun << " runs." << endl << endl ;
                    Merge() ;
                } // while
                Export( "sorted" + fname + "_0_0.bin", "sorted" + fname + ".bin" ) ;
                Clear() ;

                end = clock() ;
                cout << "Cost time: " << (float)(end-begin)/CLOCKS_PER_SEC << "s" << endl ;

                system("pause") ;
                cout << endl ;


                // primary index
                cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl ;
                cout << "Mission 2: Build the primary index" << endl ;
                cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << endl ;
                cout << "<Primary index>: (key, offset)" << endl ;
                IndexBuild() ;

                system("pause") ;
                cout << endl ;
                cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl ;
                cout << "Mission 3: Threshold search on primary index" << endl ;
                cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << endl ;
                RangeSearch();

                break ;
            } // if
            else
            {
                cout << "Error:Open file fail" << endl << endl ;
            } // if Error

        }
        while ( true ) ;

        return true ;

    } // ReadFile()

    void Clear()
    {

        while( !buffer.empty() )
        {
            buffer.pop() ;
        }
        while( !run1.empty() )
        {
            buffer.pop() ;
        }
        while( !run2.empty() )
        {
            buffer.pop() ;
        }
        while( !buff.empty() )
        {
            buffer.pop() ;
        }
        maxRun = 0 ;
        run = 0 ;

    } // Clear()

    void IndexBuild()
    {

        index.clear() ;

        fstream binFile ;
        binFile.open( ("sorted" + fname + ".bin" ).c_str(), fstream::in | fstream::binary ) ;


        if ( binFile.is_open() )
        {

            studentPair onePr ;
            int stNo = 0 ;

            binFile.seekg(0,binFile.end) ; // Goto the end of file
            stNo = binFile.tellg() / sizeof(onePr) ; // calculating size
            binFile.seekg(0,binFile.beg) ; // Goto the begin

            cur_weight = 1 ;
            indexNum = 1 ;
            IndexTable temp;
            int last = 0;

            // cout << stNo << endl ;
            for ( int i = 0 ; i<stNo ; i++ )
            {

                // cout << i << endl ;

                binFile.read( (char*)&onePr, sizeof(onePr) ) ;
/*
                if (  temp.offset == -1 )
                {
                    temp.weight = onePr.weight ;
                    temp.offset = last ;
                    // cout << "here" << endl;
                    // cout << temp.weight << endl;
                    // cout << "end" << endl;
                    last++ ;
                }
                else if ( temp.weight == onePr.weight )
                {
                    last ++ ;
                }
                else
                {
                    index.push_back(temp) ;
                    temp.weight = onePr.weight ;
                    temp.offset = last ;
                    last ++ ;
                }
*/
                if ( (onePr.weight - cur_weight) < 0.001 )
                {
                    //cout << (onePr.weight - cur_weight) * 10000000 << endl;

                    temp.weight = onePr.weight ;
                    temp.offset = i ;
                    index.push_back(temp) ;

                    cout << "[" << indexNum << "] (" << onePr.weight << ", " << i << ")" << endl ;
                    cur_weight -= 0.01 ;
                    indexNum++ ;
                } // if

            } // for
            index.push_back(temp) ;
        } // if

        binFile.close() ;

    } // IndexBuild()

    void RangeSearch()
    {
        fstream binFile ;
        binFile.open( ("sorted" + fname + ".bin" ).c_str(), fstream::in | fstream::binary ) ;

        float threshold = 0;
        float target = 0;
        do
        {
            cout << "Please input a threshold in the range (0,1] or 0(Quit):" << endl ;
            cin >> threshold;
            if(threshold == 0) break;

            else if ( binFile.is_open() )
            {
                binFile.seekg(0,binFile.beg) ; // Goto the begin
                studentPair onePr ;
                indexNum = 1;

                if ( threshold <= 1 && threshold > 0 ) {
                    for(int i = 0; i < index.size(); i++)
                    {
                        if( (index[i].weight - threshold) < 0.001 ) {
                            target = index[i+1].offset;
                            break ;
                        } // if

                    } // for

                    for(int i = 0; i < target; i++){
                        binFile.read( (char*)&onePr, sizeof(onePr) ) ;
                        cout << left << "[" << setw(6) << indexNum << "]  "
                            << setw(10) << onePr.putID << setw(10) << onePr.getID << setw(6) << onePr.weight << endl ;
                        indexNum++;
                    } // for
                } // if

            } // else if
        }
        while(true);

    }


}; // Class FileIO

class Mission
{

    ExternalMergeSort dataList ;
    clock_t begin, end ;

public:

    void DisplayMission()
    {

        cout << endl << "*********************************************" ;
        cout << endl << "                                             " ;
        cout << endl << "* 0. QUIT                                   *" ;
        cout << endl << "* 1. External Merge Sort on a Big File      *" ;
        cout << endl << "* 2. Construction of Primary Index          *" ;
        cout << endl << "* 3. Threshold search on primary index      *" ;
        cout << endl << "                                             " ;
        cout << endl << "*********************************************" ;
    } // End displayMission

    void One( )
    {
        dataList.ReadFile() ;

    } // One()

}; // class Mission


int main()
{

    Mission mission ;
    int command ;

    do
    {
        mission.DisplayMission() ;
        cout << endl << "Enter any key to continue...( [0]Quit ) " ;
        cin >> command ;
        cout << endl ;

        switch ( command )
        {

        case 0:
            break ;
        default :
            mission.One() ;
        } // switch()

        cout << endl ;

    }
    while ( command != 0 ) ;

    return 0;

} // main()





