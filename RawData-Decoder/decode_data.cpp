/*
 * CYGNSS RAW IF DATA DECODER
 * By Mihir@Trainee/SAC
 * 12-Feb-2023
 *
 * Command Line Arguments : 
 * 	1	Input File Path
 * 	2	Output Directory Patg
 *
 */ 
#include<iostream>
#include<sys/stat.h>
#include<cstring>
#include<bitset>
#include<fstream>
#include<assert.h>
using namespace std;

static char *output_path;
void process_file(fstream &base_file);
char * process_byte(char &byte);

int main(int argc , char * argv[])
{	
	assert(argc>1 && "Please Provide File Path");

	assert(argc>2 && "Please Provide Output Path");

	output_path = argv[2];

	mkdir(output_path ,0777);

	cout<<"\n======= Reading File ====== \n"<<argv[1]<<endl;
	
	fstream file;
	
	file.open( argv[1] , ios::in | ios::binary );

	cout<<"======= File Readed ======= "<<endl;
	
	process_file(file);

	file.close();

	cout<<endl;
}

void process_file(fstream &base_file){
	/* Input : Base File (fstream)
	 * Output : Zenith File
	 * 	    Startboard File
	 * 	    Port File
	 *
	 * This Function Processes Raw Binary Data As Per Data Format 2 
	 *  { [Channel 1,2,3 I Only] 2-Bit Signed Data }
	 *
	 *  2 Bit Signed Data Are Assumed To Have Following Values,
	 *
	 * [b-1] [b-2] [Value]
	 *   0	   0     1
	 *   0	   1     3
	 *   1	   0    -1
	 *   1	   1    -3
	 */

	base_file.seekg( 0 , ios::end );	

	long long int total_file_size = base_file.tellg();

	char * buffer = (char*) malloc( total_file_size * sizeof(char) );

	base_file.seekg( 35 , ios::beg );
	
	base_file.read(buffer,total_file_size);

	fstream antenna_files[3];

	cout<<"\n======= Writing Files =======\n";
	
	int path_length = strlen(output_path);

	sprintf(output_path+path_length,"/zenith.bin");
	cout<<output_path<<endl;
	antenna_files[0].open(output_path,ios::out | ios::binary);
	sprintf(output_path+path_length,"/starboard.bin");
	cout<<output_path<<endl;
	antenna_files[1].open(output_path,ios::out | ios::binary);
	sprintf(output_path+path_length,"/port.bin");
	cout<<output_path<<endl;
	antenna_files[2].open(output_path,ios::out | ios::binary);


	for(long long int i = 0 ; i < total_file_size; i++){
		char *s ;
		s = process_byte(buffer[i]);
		antenna_files[i%3]<<s[0]<<s[1]<<s[2]<<s[3];	
		//print_sample(s);	
	}

	cout<<"======= Files Written ======\n";
}	

char * process_byte(char &byte){
	
	/* Input : 1 Byte
	 * Output : 4 Sample
	 *
	 * This Function Augments 1 Byte Data To 4 Bytes According To 
	 * The Given Format,
	 *    0  [00]
	 *   64  [01]
	 * -128  [10]
	 *  -64  [11]
	 */

	static char s[4];

	for(int i = 0 ; i < 4 ; i++){
		s[i] = byte & 192;
		switch((int)s[i]){
			case    0:s[i]=1;break;
			case   64:s[i]=3;break;
			case -128:s[i]=-1;break;
			case  -64:s[i]=-3;break;
			default:break;	
		}
		byte = (byte << 2) ;
	}

	return s;
}

