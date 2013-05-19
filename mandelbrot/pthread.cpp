/* 
 * Fachhochschule Salzburg
 * MMT-B 2011 - Erweiterte Konzepte
 * Hettegger Michael
 * Eschbacher Georg
*/

#include <iostream>
#include <fstream>
#include "omp.h"
#include "time.h"
#include "pthread.h"
#include "boost/thread.hpp"
#include "boost/bind.hpp"


using namespace std;

const unsigned y_max = 512;
const unsigned x_max = 512;
const unsigned MAX_THREADS = boost::thread::hardware_concurrency();
unsigned char data[x_max][y_max][3];
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if((end.tv_nsec - start.tv_nsec) < 0)
	{
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec-start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return temp;
}

void printTimeDiff(timespec time1, timespec time2)
{

	cout << "Time elapsed: ";
	cout << diff(time1, time2).tv_sec << ":" << diff(time1, time2).tv_nsec << endl;

}


void calcColor(unsigned x,unsigned y,double MinRe,double Re_factor,double c_im) {
    double c_re = MinRe + x*Re_factor;
    unsigned max;
    double Z_re = c_re, Z_im = c_im;
    bool isInside = true;
    unsigned MaxIterations = 100;
    for(unsigned n=0; n<MaxIterations; ++n)
    {
        double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
        if(Z_re2 + Z_im2 > 4)
        {
            isInside = false;
            break;
        }
        Z_im = 2*Z_re*Z_im + c_im;
        Z_re = Z_re2 - Z_im2 + c_re;
        max = n;
    }
    
    //Different Colors 
    if(max>15 && max<150) {
        data[y][x][0] = max*1.6;
        data[y][x][1] = 0;
        data[y][x][2] = 0;
    }
    else if(max>6 && max<16) {
        data[y][x][0] = max*15;
        data[y][x][1] = max;
        data[y][x][2] = max*15;
    }
    else if(max>149) {
        data[y][x][0] = 255;
        data[y][x][1] = 0;
        data[y][x][2] = 0;
    }
    else if(max == 0) {
        data[y][x][0] = 0;
        data[y][x][1] = 0;
        data[y][x][2] = 21;
    }
    else {
        data[y][x][0] = 0;
        data[y][x][1] = 0;
        data[y][x][2] =max*42;
    }
    if(isInside) {
        data[y][x][0] =0;
        data[y][x][1] =0;
        data[y][x][2] =0;
    }
}

void *work(void* y) {
	unsigned int lines = *(unsigned int*) y;
	double MinRe = -2.0;
	double MaxRe = 1.0;
	double MinIm = -1.2;
	double MaxIm = MinIm+(MaxRe-MinRe)*x_max/y_max;
	double Re_factor = (MaxRe-MinRe)/(x_max-1);
	double Im_factor = (MaxIm-MinIm)/(x_max-1);
    for(unsigned int i=lines; i<y_max; i+=MAX_THREADS){
	double c_im = MaxIm - i*Im_factor;
		for (unsigned int x = 0; x < x_max; ++x) {
		 calcColor(x,i,MinRe,Re_factor,c_im);
		}
    }
	delete ((unsigned int *)y);	
	return NULL;
}



int main()
{

	timespec time1, time2;
    
	clock_gettime(CLOCK_MONOTONIC, &time1);
	pthread_t threads[MAX_THREADS];
	for(unsigned int i=0; i<MAX_THREADS; ++i){
	 //unsigned* y_start = new unsigned int((y_max/MAX_THREADS * i));
	 unsigned * y_start = new unsigned int(i);
	 pthread_create(&threads[i], NULL, work, (void*)(y_start));
	}
	
	for(unsigned int i=0; i<MAX_THREADS; ++i){
	 pthread_join(threads[i], NULL);
	}	
	clock_gettime(CLOCK_MONOTONIC, &time2);
	cout << "\nTime used for PTHREAD:\n";
	printTimeDiff(time1, time2);
	
	ofstream output;
	output.open("pthread.ppm");
	output << "P3\n" << x_max << " "<<y_max<<"\n255\n";
    
	for(unsigned int i=0; i<y_max; ++i){
	 for(unsigned int j=0; j<x_max; ++j){
	  output << (int)data[i][j][0] << " " << (int)data[i][j][1] << " " << (int)data[i][j][2]<<endl; 
	 }
	}
	output.close();
	return 0;
}

