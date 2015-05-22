// key_frames.h

/**

@brief
KeyFrames class

The KeyFrames class stores key frame location and velocity parameters for
and arbitrary number of motor axes and returns interpolated spline values
anywhere between the first and last key frame using the HermiteSpline class.

Examples and information about creating MoCoBus node devices, see the
\ref omnode "Creating MoCoBus Node Devices" page.


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

*/

#ifndef _KEY_FRAMES_h
#define _KEY_FRAMES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class KeyFrames{

public:

	KeyFrames();										// Default constructor
	~KeyFrames();										// Default destructor

	// Assignment of KeyFrame array
	static void setAxisArray(KeyFrames* p_axis_array,	// Points the axis_array var to an existing array of key frames objects that represent the axes to be managed
		int p_axis_count);

	// Axis select functions
	static void axis(int p_axis);						// Selects the the current axis
	static int axis();									// Returns the currently selected axis	
	
	// Run-time update functions
	static void updateRate(int p_update_rate);			// Sets the velocity update rate in ms used at run-time
	static int updateRate();							// Returns the velocity update rate in ms
	
	// Data transmission functions
	static void receiveState(bool p_state);				// Set whether the NMX is currently receiving key frame input data
	static bool receiveState();							// Returns whether the NMX is currently receiving key frame input data

	// Key frame count functions
	static void setKFCount(int p_kf_count);				// Sets the key frame count and allocates memory for input vars
	static int countKF();								// Returns the key frame count
	
	// Key frame x location functions
	static void setXN(float* p_xn);						// Points xn to an existing array of values
	static void setXN(float p_input);					// Assigns xn values one at a time	
	static int countXN();								// Returns the number of xn values that have been assigned. Accurate only when assigning values one at a time.
	static void resetXN();								// Resets the xn received count
	static float getXN(int p_which);					// Returns the abscissa of the requested key frame

	// Key frame motor position functions
	void setFN(float* p_fn);							// Points fn to an existing array of values
	void setFN(float p_input);							// Assigns fn values one at a time
	int countFN();										// Returns the number of fn values that have been assigned. Accurate only when assigning values one at a time.
	void resetFN();										// Resets the fn received count
	float getFN(int p_which);							// Returns the fn value of the requested key frame

	// Key frame motor velocity functions
	void setDN(float* p_dn);							// Points dn to an existing array of values
	void setDN(float p_input);							// Assigns dn values one at a time
	int countDN();										// Returns the number of dn values that have been assigned. Accurate only when assigning values one at a time.
	void resetDN();										// Resets the dn received count
	float getDN(int p_which);							// Returns the dn value of the requested key frame

	// Interpolation functions
	float pos(float p_x);								// Returns the position rate at the given x
	float vel(float p_x);								// Returns the velocity at the given x
	float accel(float p_x);								// Returns the acceleration at the given x

	// Validation functions
	bool validateVel();								// Returns true if curve does not exceed max motor speed
	bool validateAccel();							// Returns true if curve does not exceed max motor accel
	static void setMaxVel(float p_max_vel);					// Sets the maximum velocity for validation checking
	static void setMaxAccel(float p_max_accel);				// Sets the maximum acceleration for validation checking
	
private:

	// Communication vars
	static int g_cur_axis;								// The current axis being managed
	static bool g_receiving;							// Indicates whether key frame data is currently being sent to the NMX
	static int g_xn_recieved;							// Number of abscissa values that have been received
	int m_fn_recieved;									// Number of position values that have been received
	int m_dn_recieved;									// Number of derivative values that have been received


	static int g_update_rate;							// Spline update rate in ms
	static int g_kf_count;								// Number of key frames
	static bool g_mem_allocted;							// Indicates whether memory for the input vars has been set
	static KeyFrames* g_axis_array;						// The array of key frame objects. Allow cycling through each object when allocating memory
	static int g_axis_count;							// Number of axes to be managed

	// Input / output vars
	static float* g_xn;									// Abscissas of key frame locations
	float* m_fn;										// Ordinate of current axis key frame location
	float* m_dn;										// Derivatives at key frame locations
	float m_f[1];										// Current axis curve's location at calculated point
	float m_d[1];										// Current axis curve's first derivative at calculated point
	float m_s[1];										// Current axis curve's second derivative at calculated point
	
	void updateVals(float p_x);							// Updates the output vars for the given locations

	// Validation vars
	static const int G_VALIDATION_PNT_COUNT;			// Number of points to check on spline for validation purposes
	static float g_max_vel;								// Absolute maximum velocity
	static float g_max_accel;							// Absolute maximum acceleration

	// Memory management
	void freeMemory();									// Deallocates any memory assigned to input arrays
};

#endif

/*

	@page keyframes Using the KeyFrames class

	The KeyFrames class is essentially an interface layer for the HermiteSpline class. Key frame locations
	and derivatives (that is, the velocities at the key frame locations) are stored in this class and passed
	to the HermiteSpline class to calculate the interpolated values between those locations. A particular
	sequence of operations must be followed for the class to function correctly:

	1. Create an array with a number of KeyFrame objects equal to the number of motors whose motion is to be interpolated

	2. Attach the array to class using static function KeyFrames::setAxisArray(KeyFrames* p_axis_array)

	3. Set the key frame count using static function KeyFrames::count(int p_kf_count). This will allocate the necessary
	   memory to the input variable arrays

	4. Assign the key frame abscissas either one at a time or by passing an existing array using static function
	   setXN(int p_which, float p_input) or KeyFrames::setXN(float* p_xn). These are assigned using a static function
	   because the x locations for each key frame will be the same for all axes. You must ensure that the number of assigned
	   values matches the key frame count, otherwise things may go horribly wrong. These values must be strictly sorted in
	   asceending order.

	5. Assign the key frame positions and velocities one at a time or by passing an existing array using functions
	   setFN(int p_which, float p_input) or setFN(float* p_fn), and setDN(int p_which, float p_input) or setDN(float* p_dn).
	   This must be done for each object in the KeyFrame array you've created, since each motor may have different positions
	   and velocities. You must ensure that the number of assigned values for each matches the key frame count, otherwise 
	   things may go horribly wrong.

	6. Once steps 1-5 have been completed, the position, velocity, or acceleration at any x location between the first and last
	   key frame abscissa may be retrieved with the pos(float p_x), vel(float p_x), accel(float p_x) functions.

*/

