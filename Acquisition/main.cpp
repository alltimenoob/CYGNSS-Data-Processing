#include "acquisition.h"

void acquisition(vector<char> longSignal);

int main()
{
	fstream file;
	file.open(fileName,ios::in);

	int samplesPerCode = samplingFreq / (codeFreqBasis / codeLength) ;
		
	vector<char> longSignal(4*samplesPerCode);

	if(file.read(longSignal.data() , 4*samplesPerCode))
	{
		cout<<"File Readed."<<endl;       
		acquisition(longSignal);
	}
	
}
