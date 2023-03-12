#include "acquisition.h"

void makeCATable(unordered_map<int,vector<complex<double>>> &caTable);
vector<complex<double>> generateCACode(int prn);
vector<double> max(vector<vector<double>> vec);

void red () {
  printf("\033[1;31m");
}

void yellow() {
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}

void acquisition(vector<char> longSignal)
{
	int samplesPerCode = samplingFreq / ( codeFreqBasis / codeLength );

	vector<char> signal1(longSignal.begin(),longSignal.begin()+samplesPerCode);
	vector<char> signal2(longSignal.begin()+samplesPerCode,(longSignal.begin()+2*samplesPerCode) );

	double ts = 1 / samplingFreq;

	vector<double> phasePoints(samplesPerCode);

	for(int i = 0 ; i < samplesPerCode ; i++){
		phasePoints[i] = i * 2 * M_PI * ts;
	}

	int numberOfFrqBins = round(acqSearchBand * 2) + 1 ;

	cout<<numberOfFrqBins<<endl;

	unordered_map< int , vector<complex<double>> > caCodesTable;

	makeCATable(caCodesTable);

	vector<vector<double>> results( numberOfFrqBins , vector<double>(samplesPerCode) );

	vector<int> frqBins(numberOfFrqBins);

	vector<int> carrFreq(32);
	vector<int> codePhase(32);
	vector<int> peakMetric(32);

	for(int prn = 1 ; prn <= acqSatelliteList; prn++)
	{
		cout<<"Computing PRN "<<prn<<endl;
		vector<complex<double>> caCodeFreqDom(samplesPerCode);
		
		fftw_plan p ;
	
		//p = fftw_plan_dft_r2c_1d(samplesPerCode, reinterpret_cast<double*>(&caCodesTable[prn]) , reinterpret_cast<fftw_complex*>(&caCodeFreqDom[0]),FFTW_ESTIMATE);
	
    		p = fftw_plan_dft_1d(samplesPerCode, reinterpret_cast<fftw_complex*>(&caCodesTable[prn][0]) ,reinterpret_cast<fftw_complex*>(&caCodeFreqDom[0]), FFTW_FORWARD,FFTW_ESTIMATE);
		fftw_execute(p);

		for (int i = 0; i < samplesPerCode; i++)
		{
			caCodeFreqDom[i] = conj(caCodeFreqDom[i]);
		}
		
		//cout<<caCodeFreqDom[16035]<<endl; //Pass
		
		for(int frqIndex = 0 ; frqIndex < numberOfFrqBins ; frqIndex++)
		{
			frqBins[frqIndex] = IF - (acqSearchBand / 2) * 1000 + 500 * (frqIndex);
		
			vector<complex<double>> IQ1(samplesPerCode),IQ2(samplesPerCode),IQFreq1(samplesPerCode),IQFreq2(samplesPerCode);

			for(int i = 0 ; i < samplesPerCode; i++){
				IQ1[i] = ( exp(complex<double>(0,phasePoints[i]) * complex<double>(frqBins[frqIndex],0) ) * complex<double>(signal1[i]));
				IQ2[i] = ( exp(complex<double>(0,phasePoints[i]) * complex<double>(frqBins[frqIndex],0) ) * complex<double>(signal2[i]));					     
			}
	
			p = fftw_plan_dft_1d(samplesPerCode, reinterpret_cast<fftw_complex*>(&IQ1[0]), reinterpret_cast<fftw_complex*>(&IQFreq1[0]),FFTW_FORWARD,FFTW_ESTIMATE);
			fftw_execute(p);	
				
			p = fftw_plan_dft_1d(samplesPerCode, reinterpret_cast<fftw_complex*>(&IQ2[0]), reinterpret_cast<fftw_complex*>(&IQFreq2[0]),FFTW_FORWARD,FFTW_ESTIMATE);
			fftw_execute(p);	
			
			vector<complex<double>> convIQ1(samplesPerCode);
			vector<complex<double>> convIQ2(samplesPerCode);

			for(int i = 0 ; i < samplesPerCode ; i++){
				convIQ1[i] = IQFreq1[i] * caCodeFreqDom[i];
				convIQ2[i] = IQFreq2[i] * caCodeFreqDom[i];			
#if 0
				if(i<16 && prn==1 && frqIndex==0)
				cout<<convIQ1[i]<< "   " << convIQ2[i]<<"  "<<i <<endl;	
#endif				
			}

			vector<complex<double>> InvDFT1(samplesPerCode);
			vector<complex<double>> InvDFT2(samplesPerCode);
			
			p = fftw_plan_dft_1d(samplesPerCode, reinterpret_cast<fftw_complex*>(&convIQ1[0]), reinterpret_cast<fftw_complex*>(&InvDFT1[0]),FFTW_BACKWARD,FFTW_ESTIMATE);
			fftw_execute(p);	
				
			p = fftw_plan_dft_1d(samplesPerCode, reinterpret_cast<fftw_complex*>(&convIQ2[0]), reinterpret_cast<fftw_complex*>(&InvDFT2[0]),FFTW_BACKWARD,FFTW_ESTIMATE);
			fftw_execute(p);	
				
			vector<double> acqRes1(samplesPerCode);
			vector<double> acqRes2(samplesPerCode);
			
			double max1 , max2;
			for(int i = 0 ; i < samplesPerCode ; i++){
				acqRes1[i] = pow(abs(InvDFT1[i])/samplesPerCode,2);	
				acqRes2[i] = pow(abs(InvDFT2[i])/samplesPerCode,2);	
#if 0
				if(i<1 && prn==1 )
				cout<<acqRes1[i]<<"   " <<frqIndex<<endl;
#endif
				if(acqRes1[i] > max1)
					max1 = acqRes1[i];
				if(acqRes2[i] > max2)
					max2 = acqRes2[i];
			}

			if(max1 > max2)
				results[frqIndex] = acqRes1;
			else
				results[frqIndex] = acqRes2;
		
		}
		
	    	
		vector<double> peak = max(results);
		cout<<peak[0]<<"  "<<peak[1]<<"  "<<peak[2]<<"  "<<peak[3]<<endl;
		
		cout<<(peak[0] / peak[3]);
	
		cout<<endl<<endl;		
		fftw_destroy_plan(p);
    		fftw_cleanup();


		double samplesPerCodeChip = round(samplingFreq / codeFreqBasis);
		double rmRangeIndex1 = peak[1] - samplesPerCodeChip;
		double rmRangeIndex2 = peak[1] + samplesPerCodeChip;

		vector<double> codePhaseRange;
		if(rmRangeIndex1 < 2)
		{
		//	for(int i = rmRangeIndex ; 
			//codePhaseRange.push_back(rmRangeIndex++);
		}
	}


}



vector<double> max(vector<vector<double>> vec){
	vector<double> max(4) ;

	for(int i = 0; i < vec.size() ; i++)
	{
		for(int k = 0; k < vec[i].size() ; k++)
		{
			if(vec[i][k]>max[0]){
				max[0] = vec[i][k];
       				max[1] = k;
				max[2] = i;
			}
		}
	}

	for(int k = 0 ; k < vec[max[2]].size() ; k++)
	{	
		if( vec[max[2]][k] > max[3] && ( k < (max[1] - 16) || k > (max[1] + 16) )  )
		{
			max[3] = vec[max[2]][k];
		}
	}

	return max;
}
