// Bezier.h

/**

@brief
CubicBezier class

The CubicBezier class allows for the computation of a cubic Bezier
spline composed of one or more span segments. Also declared in CubicBezier.h
are helper classes, Span and OrderedPair. A Bezier object contains one or more
Span objects, which in turn uses four OrderedPair control points to calculate
positions along the span.

This particular library is written specifically for the application of interpolating
motion paths of a physical motion control system through key points defined by
a user. This being the case, while locations along the Span curves are computed
as parameterized functions X(t) and Y(t), it is assumed that each Span is also
a function Y(x), where x = time in seconds or frames. This allows more intuitive 
look-up of a location along the Bezier curve by specifying a time, rather than
the value of an arbitrary paramter.

@author Michael Ploof

(c) 2015 Dynamic Perception LLC

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


/*
	Forward differencing reference:
	http://www.drdobbs.com/forward-difference-calculation-of-bezier/184403417
*/

#ifndef _BEZIER_h
#define _BEZIER_h


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class OrderedPair {

		public:
			OrderedPair();
			OrderedPair(float p_a, float p_b);
			//~OrderedPair();
			float val(int p_which);
			void val(int p_which, float p_val);		
			void x(int p_x);
			float x();
			void y(int p_y);
			float y();			

			friend OrderedPair operator *(int val, const OrderedPair &op);
			friend OrderedPair operator *(const OrderedPair &op, int val);
			friend OrderedPair operator *(float val, const OrderedPair &op);
			friend OrderedPair operator *(const OrderedPair &op, float val);
			friend OrderedPair operator *(double val, const OrderedPair &op);
			friend OrderedPair operator *(const OrderedPair &op, double val);
			friend OrderedPair operator +(int val, const OrderedPair &op);
			friend OrderedPair operator +(const OrderedPair &op, int val);
			friend OrderedPair operator +(float val, const OrderedPair &op);
			friend OrderedPair operator +(const OrderedPair &op, float val);
			friend OrderedPair operator + (const OrderedPair& p_op1, const OrderedPair& p_op2);
			friend OrderedPair operator -(int val, const OrderedPair &op);
			friend OrderedPair operator -(const OrderedPair &op);

		private:
			float m_vals[2];	// The (a, b) values
};

class Span {

	public:

		// Constructors
		Span();
		Span(OrderedPair *p_ctrl_pts, Span* p_prev_span);								
		
		// Meta functions
		int id();
		void prevSpan(Span* p_span);
		Span* prevSpan();
		void nextSpan(Span* p_span);
		Span* nextSpan();

		// Control points getter
		OrderedPair *ctrlPts();
		
		// Standard Bezier evaluation
		void incrementSize(float p_h);
		float incrementSize();
		int stepsRemaining();
		
		// Evaluation w/r/t X
		void incrementSizeX(float p_hx);
		float positionAtX(float p_x);
		float velocityAtX(float p_x);
        float accelAtX(float p_x);
		
		// Helper functions
		bool containsX(float p_x);
		bool containsY(float p_y);
        float startX();
        float stopX();
        float startY();
        float stopY();		
		float rangeX();
		float rangeY();
		
		
	private:			
		// Static vars
		static const int g_MAX_CTRL_PTS = 4;
		static int g_id_gen; 

		// Meta vars
		int m_id;
		Span *m_next_span;
		Span *m_prev_span;

		// Defining functions / vals
		void setCoeffs();
		OrderedPair m_ctrl_pts[g_MAX_CTRL_PTS];		
		OrderedPair m_coeff_A;
		OrderedPair m_coeff_B;
		OrderedPair m_coeff_C;
		OrderedPair m_coeff_D;
		
		// Evaluation w/r/t X		
		float tOfX(float x, float guess, int recursion_limit);

		// Cubic solvers
		float solveCubic(float p_T, bool p_is_x, float p_offset);
		float solveCubic(float p_T, bool p_is_x);		
		float solveCubicPrime(float p_T, bool p_is_x);
        float solveCubicDoublePrime(float p_T, bool p_is_x);
        
};	

class CubicBezier
{
	public:

		/********** Public Variables **********/

		int m_knot_count;
		Span *m_spans;
		int m_span_count;
		
		/********** Public Functions **********/

		CubicBezier();
		CubicBezier(OrderedPair *p_ctrl_pts, int p_knot_count);
		~CubicBezier();

		void init(OrderedPair *p_ctrl_pts, int p_knot_count);

		int knotCount(int p_count);
		int knotCount();

        int controlPointCount();

		void setNextX(float p_x);
		void setNextY(float p_y);

		float positionAtX(float p_x);
        float velocityAtX(float p_x);
        float accelAtX(float p_x);

        float startX();
        float stopX();
        float startY();
        float stopY();

		float incrementSize();
		int stepsRemaining();
		Span *getSpan(int p_which);
		void initSpans();
		void releaseMemory();

        /*
            These functions fetch the X and Y (F - function) values
            of the specified key frame (knot). The notation is derived
            from existing functions in the KeyFrames class.
        */
        float getXN(int p_kf);
        float getFN(int p_kf);

	private:

		/********** Private Variables **********/

        static const long g_ERROR = -987654321;
		bool m_span_mem_allocated;
		bool m_ctrl_mem_allocated;
		OrderedPair *m_ctrl_pts;		
		Span* m_cur_span;		
		int m_next_x;
		int m_next_y;
        int m_ctrl_pt_count;

        static const int g_PTS_PER_SPAN = 4;


		/********** Private Functions **********/		
		
        Span* spanContainingX(float p_x);
};

#endif

/*

@page cubicbezier Using the CubicBezier class

    ***** Example of creating, defining, and using a CubicBezier object, bulk method *****

    // Set the knot count, then create an array of OrderedPair objects with the size
    // KNOT_COUNT * (PTS_PER_SPAN - 1) + 1, and fill it with the control point locations
    const int KNOT_COUNT = 2;
    const int PTS_PER_SPAN = 4;
    OrderedPair[KNOT_COUNT * (PTS_PER_SPAN -1) + 1] ctrl_pts = {
        OrderedPair(0,0),
        OrderedPair(25,10),
        OrderedPair(50,60),
        OrderedPair(75,75),
        OrderedPair(150,60),
        OrderedPair(200,10),
        OrderedPair(250,72),
    }

    // Construct a new CubicBezier object by passing the array of control points and the knot count
    CubicBezier bezier = CubicBezier(ctrl_pts, KNOT_COUNT);

    // This method requires explicit initialization of the spans after object construction
    bezier.initSpans();

    // After initializing, get the position, velocity, and acceleration at an X value
    float pos = bezier.positionAtX(105)
    float vel = bezier.velocityAtX(105)
    float accel = bezier.accelAtX(105)


    ***** Example of creating, defining, and using a CubicBezier object, incremental method ***** 

    CubicBezier bezier = CubicBezier();

    // The function for setting the CubicBezier knot count 
    // will return the number of control points that need
    // to be set in order to define the object
    int n = bezier.knotCount(3);

    // Set the first span X values
    bezier.setNextX(0);
    bezier.setNextX(25);
    bezier.setNextX(50);
    bezier.setNextX(75);

    // Set the second span X values
    // The first control point is the same as the last
    // point of the previous span, so only three more
    // are needed to define this and every subsequent span
    bezier.setNextX(150);
    bezier.setNextX(200);
    bezier.setNextX(250);

    // Set the first span Y values
    bezier.setNextY(0);
    bezier.setNextY(10);
    bezier.setNextY(60);
    bezier.setNextY(75);

    // Set the second span Y values
    // Again, only three new points are needed to fully 
    // define the second span's control point Y values
    bezier.setNextY(60);
    bezier.setNextY(10);
    bezier.setNextY(75);

    // Once n X values and n Y values have been set (in any order),
    // the bezier object will automatically initialize the span
    // object represented by those points. However, do not try using
    // any of the below functions until the spans have been initialized
    
    // Get the position, velocity, and acceleration at an X value
    float pos = bezier.positionAtX(105)
    float vel = bezier.velocityAtX(105)
    float accel = bezier.accelAtX(105)

*/
