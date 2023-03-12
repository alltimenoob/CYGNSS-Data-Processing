#include "acquisition.h"

vector<complex<double>> generateCACode(int prn)
{
	int codelength = 1023;

	vector<int> g2s = {  5,   6,   7,   8,  17,  18, 139, 140, 141, 251,
       	252, 254, 255, 256, 257, 258, 469, 470, 471, 472,
       	473, 474, 509, 512, 513, 514, 515, 516, 859, 860,
       	861, 862, 145, 175,  52,  21, 237, 235, 886, 657,
       	634, 762, 355, 1012, 176, 603, 130, 359, 595, 68,
       	386};

	int g2shift = g2s[prn-1];
	
	vector<double> g1(codelength);
	vector<double> reg(10,-1);

	for(int i = 0 ; i < codelength ; i++){
		g1[i] = reg[9];
		int save = reg[2] * reg[9];
		rotate(reg.rbegin(), reg.rbegin() + 1, reg.rend());
		reg[0] = save;
	}

	vector<double> g2(1023);
	
	fill_n(reg.begin(), 10, -1);
	
	for(int i = 0 ; i < codelength; i++){
		g2[i] = reg[9];
		int save = reg[1] * reg[2] * reg[5] * reg[7] * reg[8] * reg[9];
		rotate(reg.rbegin() , reg.rbegin() + 1 , reg.rend());
		reg[0] = save;
	}


	vector<complex<double>> ans(codelength);

	int index;
	for(int i = 0 ; i < codeLength; i++){
		if(codeLength - g2shift + i < codeLength ){
			index = codeLength  - g2shift + i;
			ans[i] = -(g1[i] * g2[index]);
			index = -1;
		}
		else{
			index +=1;
			ans[i] = -(g1[i] * g2[index]) + 0*I;
		}
	}	
	return ans;
}


