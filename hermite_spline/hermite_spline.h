// hermite_spline.h

// Source: http://people.sc.fsu.edu/~jburkardt/c_src/hermite_cubic/hermite_cubic.html

#ifndef _HERMITE_SPLINE_h
#define _HERMITE_SPLINE_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

class HermiteSpline
{
 public:
     static void cubic_spline_value(int nn, float xn[], float fn[],
         float dn[], int n, float x[], float f[], float d[], float s[]);
     static void cubic_value(float x1, float f1, float d1, float x2,
         float f2, float d2, int n, float x[], float f[], float d[],
         float s[]);
     static void r8vec_bracket3(int n, float t[], float tval, int *left);
 private:
     
    
}; 
#endif

