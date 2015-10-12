// header files
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

// global constants

   const int STD_STR_LEN = 45;
   const int MAX_LINE_LENGTH = 70;
   const int OPTIME_MAX = 15;
   const int OPTIME_MIN = 5;
   const int BASE_TEN = 10;

// function prototypes

void getNewOp( int percent, char opStr[] );
bool getOdds( int oddPercent );
int getRandBetween( int low, int high );
int checkLineLength( char str[], int startLen, ofstream &outF );
  char* itoa(int value, char* result, int base);

// main program
int main()
   {
    // initialize program

       // initialize variables
       int procPercentage, numOps, numPrograms;
       int progCtr, opCtr, lineLength = 0;
       char fileName[ STD_STR_LEN ];
       ofstream outFileObject;
       char opString[ STD_STR_LEN ];
       char tempString[ STD_STR_LEN ];

       // initialize random generator
       srand( time( NULL ) );

       // show title
       cout << "Program Meta-Data Creation Program" 
            << endl << endl;

    // get file name for meta-data file
    cout << "Enter file name to use: ";
    cin >> fileName;

    // get weight of processing
    cout << "Enter percentage of processing: ";
    cin >> procPercentage;

    // get number of actions
    cout << "Enter number of operations per program: ";
    cin >> numOps;

    // get number of programs to generate
    cout << "Enter number of programs: ";
    cin >> numPrograms;

    // open file
    outFileObject.open( fileName );

    // output file description header
    outFileObject << "Start Program Meta-Data Code:" << endl;

    // set temporary string with string literal
    strcpy( tempString, "S(start)0; " );

    // output Operating System start
    outFileObject << tempString;
   
    // update length, output end of line as needed
    lineLength = checkLineLength( tempString, lineLength, outFileObject );

    // loop across number of programs
    for( progCtr = 0; progCtr < numPrograms; progCtr++ )
       {
        // set temporary string with string literal
        strcpy( tempString, "A(start)0; " );
    
        // update length, output end of line as needed
        lineLength = checkLineLength( tempString, lineLength, outFileObject );

        // show begin of program
        outFileObject << tempString;

        // loop across number of operations
        for( opCtr = 0; opCtr < numOps; opCtr++ )
           {
            // get new op
            getNewOp( procPercentage, opString );

            // update length, output end of line as needed
            lineLength = checkLineLength( opString, lineLength, outFileObject );

            // output new op
            outFileObject << opString;
           }
        // end loop across number of operations

        // set temporary string with string literal
        strcpy( tempString, "A(end)0; " );

        // update length, output end of line as needed
        lineLength = checkLineLength( tempString, lineLength, outFileObject );

        // add end of program
        outFileObject << tempString;
       }
    // end number of programs loop

    // set temporary string with string literal
    strcpy( tempString, "S(end)0." );

    // update length, output end of line as needed
    lineLength = checkLineLength( tempString, lineLength, outFileObject );

    // output end of operating system
    outFileObject << tempString;

    // output extra endline        
    outFileObject << endl;

    // output file description header
    outFileObject << "End Program Meta-Data Code." << endl;

    // close file
    outFileObject.close();

    // shut down program

       // hold program
       system( "pause" );

       // return success
       return 0; 
   }

// supporting function implementations

int getRandBetween( int low, int high )
   {
    int range = high - low + 1;

    return rand() % range + low;    
   }

bool getOdds( int oddPercent )
   {
    int randVal = rand() % 100 + 1;

    if( randVal <= oddPercent )
       {
        return true;
       }

    return false;
   }

void getNewOp( int percent, char opStr[] )
   {
    int cycleNum;
    char numStr[ STD_STR_LEN ];
    static int ioOpVal = 0;

    // check for chance of processing
    if( getOdds( percent ) )
       {
        strcpy( opStr, "P(run)" );
       }

    else
       {
        if( ioOpVal == 0 ) // input process
           {
            if( getOdds( 50 ) ) // toss coin
               {
                strcpy( opStr, "I(hard drive)" );
               }
            else
               {
                strcpy( opStr, "I(keyboard)" );
               }

            ioOpVal = 1;
           }
        else
           {
            if( getOdds( 50 ) ) // toss coin
               {
                strcpy( opStr, "O(hard drive)" );
               }
            else
               {
                strcpy( opStr, "O(monitor)" );
               }

            ioOpVal = 0;
           }
       }

    cycleNum = getRandBetween( OPTIME_MIN, OPTIME_MAX );

    itoa( cycleNum, numStr, BASE_TEN );

    strcat( opStr, numStr );

    strcat( opStr, "; " );
   }

int checkLineLength( char str[], int startLen, ofstream &outF )
   {
    int newLength = startLen + strlen( str );

    if( newLength > MAX_LINE_LENGTH )
       {
        outF << endl;

        return 0;
       }

    return newLength;
   }


 //* C++ version char* style "itoa":
  
   
  /**
   * C++ version 0.4 char* style "itoa":
   * Written by Lukás Chmela
   * Released under GPLv3.
   */
  char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }
  
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
  
    do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
  
    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
    }
    return result;
  }
  
  