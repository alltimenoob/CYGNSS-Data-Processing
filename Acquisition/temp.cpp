#include "acquisition.h"

void makeCATable(unordered_map<int,vector<complex<double>>> &caTable);
vector<double> generateCACode(int prn);

int main()
{
    int N = 16036;
    unordered_map< int , vector<complex<double>> > caCodesTable;
    makeCATable(caCodesTable);
    complex<double> *in;
    complex<double> *out;
    out = (complex<double>*) malloc(sizeof(complex<double>) * N);

    fftw_plan p;
    
    int i;
	
    in = &caCodesTable[1][0];
    
    //p = fftw_plan_dft_r2c_1d(N, in ,reinterpret_cast<fftw_complex*>(out), FFTW_ESTIMATE);
    p = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(in) ,reinterpret_cast<fftw_complex*>(out), FFTW_FORWARD,FFTW_ESTIMATE);
    
    fftw_execute(p);
    p = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(out) ,reinterpret_cast<fftw_complex*>(in), FFTW_BACKWARD,FFTW_ESTIMATE);
    
    fftw_execute(p);

    for (i = 0; i < 120; i++){

   	    cout<<i<<"  "<<out[i]<<" - " <<in[i]<<endl;
    }

     
    fftw_destroy_plan(p);
    fftw_cleanup();
}
