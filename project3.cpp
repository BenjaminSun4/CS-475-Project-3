#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


int    NowYear = 2022;      // 2022-2027
int    NowMonth = 0;        // 0-11

float    NowPrecip;       // inches of rain per month
float    NowTemp;         // temperature this month
unsigned int seed = 0; 
float 	NowHeight = 1.0;      // grain height in inches
int 	NowNumDeer = 1.0;        // number of deer in the current population
int 	NowNumBobcat = 1;         


const float GRAIN_GROWS_PER_MONTH =          9.0;
const float ONE_BOBCAT_EATS_PER_MONTH =      1.0;
const float ONE_DEER_EATS_PER_MONTH =        1.0;

const float AVG_PRECIP_PER_MONTH =     7.0;    //average
const float AMP_PRECIP_PER_MONTH =     6.0;    // plus or minus
const float RANDOM_PRECIP =            2.0;    // plus or minus noise

const float AVG_TEMP =               60.0;    // average
const float AMP_TEMP =               20.0;    // plus or minus
const float RANDOM_TEMP =            10.0;    // plus or minus noise

const float MIDTEMP =                40.0;
const float MIDPRECIP =              10.0;

float SQR( float x ) 
{
    return x*x;
}

float Ranf( unsigned int *seedp,  float low, float high ) 
{
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX
    
    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int Ranf( unsigned int *seedp, int ilow, int ihigh ) 
{
    float low = (float)ilow;
    float high = (float)ihigh + 0.9999f;
    
    return (int)(  Ranf(seedp, low,high) );
}
float x = Ranf( &seed, -1.f, 1.f );

void Deer() 
{
    while( NowYear < 2028 )
    {
      
        int NextNumDeer = NowNumDeer;
        if (NextNumDeer != NowHeight) 
        {
            if (NextNumDeer > NowHeight) 
            {
                NextNumDeer--;
            } 
            else 
            {
                NextNumDeer++;
            }
        }
        
        // DoneComputing barrier:
        #pragma omp barrier
        NowNumDeer = NextNumDeer;
       
        // DoneAssigning barrier:
        #pragma omp barrier
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void Grain()
{
    while( NowYear < 2028 )
    {
        
        float nextHeight = NowHeight;
        float tempFactor = exp(-SQR((NowTemp - MIDTEMP)/ 10.0));
        float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP)/ 10.0));
      
        nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
        if (nextHeight < 0) 
        {
            nextHeight = 0;
        }
        // DoneComputing barrier:
        #pragma omp barrier
        NowHeight = nextHeight;
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
    
}

void Watcher() 
{
    while( NowYear < 2028 )
    {
      
        
        // DoneComputing barrier:
        #pragma omp barrier
        
        // DoneAssigning barrier:
        #pragma omp barrier
   	fprintf(stderr, "NowMonth: %2d; NowYear: %4d; NowNumDeer: %2d; NowPrecip: %6.2f; NowTemp: %6.2f; NowHeight: %6.2f; NowNumBobcat: %2d;\n" , NowMonth, NowYear, NowNumDeer, NowPrecip,NowTemp, NowHeight, NowNumBobcat);
     
        if(NowMonth == 11) 
        {
            NowMonth = 0;
            NowYear++;
        } 
        else 
        {
            NowMonth++;
        }
        float ang = (30.0 * (float) NowMonth + 15.0) * (M_PI / 180.0);
        float temp = AVG_TEMP - AMP_TEMP * cos( ang );
        unsigned int seed = 0;
        NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP);
        
        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
        NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
        if( NowPrecip < 0. )
            NowPrecip = 0.;
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void Bobcat() 
{
    while( NowYear < 2028 )
    {
     
        int hunt = Ranf(&seed,0,5);
        int deer = NowNumDeer;
        
        if (hunt == 1) 
        {
            deer--;
        }
        
        // DoneComputing barrier:
        #pragma omp barrier
        NowNumDeer = deer;
        
        // DoneAssigning barrier:
        #pragma omp barrier

        
        // DonePrinting barrier:
        #pragma omp barrier

    }
}

int main( int argc, char *argv[ ] )
{
   
    omp_set_num_threads( 4 );    // same as # of sections
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Deer( );
        }
        
        #pragma omp section
        {
            Grain( );
        }
        
        #pragma omp section
        {
            Watcher( );
        }
        
        #pragma omp section
        {
            Bobcat( );   
        }
    }       
    return 0;
}