#include "acquisition.h"

vector<complex<double>> generateCACode(int prn);

void makeCATable(unordered_map<int,vector<complex<double>>> &caCodeTable){

	int samplesPerCode =  samplingFreq / ( codeFreqBasis / codeLength ) ;

	float ts = 1.0 / samplingFreq;
	float tc = 1.0 / codeFreqBasis;

	
	for(int i = 1 ; i <= acqSatelliteList ; i++)
	{
		vector<complex<double>> caCode = generateCACode(i);

		vector<complex<double>> codeValueIndex(samplesPerCode);

		for(int j = 0 ; j < samplesPerCode ; j++)
		{
			codeValueIndex[j] = caCode[floor(ts * j / tc)];
		}

		caCodeTable[i] = codeValueIndex;
	}	
}
