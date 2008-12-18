#include "../cvxrand.h"
#include<time.h>
#include<stdio.h>

int main()
{
    CvRNG rng = cvRNG( time(NULL) );
    double rand = cvRandGauss( &rng, 2.0 );
    printf("%lf\n", rand);
    return 0;
}
