//////////////////////////////////////////////////////////////////////////
// Data Structures and Global Variables  // 
//////////////////////////////////////////////////////////////////////////

//Family Data Structure // 
typedef struct
{
	char father[15]; 
	char mother[15];

	char children[15][15];
	int childCount;
}family;

//Global Variables (counters)//  
int generation = 0;
int familiesNum;

//Pointer for family array// 
family *famArr = NULL;
