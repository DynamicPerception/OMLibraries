
/*

Motor Control Library

OpenMoco nanoMoCo Core Engine Libraries

See www.openmoco.org for more information

(c) 2008-2011 C.A. Church / Dynamic Perception LLC

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

#include <HardwareSerial.h>

#include "OMMotorFunctions.h"

// initialize static members

unsigned int OMMotorFunctions::g_curSampleRate = 200;	// How often to run the motor ISR in microseconds
unsigned int OMMotorFunctions::g_cyclesPerSpline = 100;
bool		 OMMotorFunctions::g_debug = false;
uint8_t		 OMMotorFunctions::g_plan_type = 0;


/** Constructor

  Creates a new instance of the OMMotorFunctions class. Sets all control outputs to the
  output state.

  */

OMMotorFunctions::OMMotorFunctions(int p_stp=0, int p_dir=0, int p_slp=0, int p_ms1=0, int p_ms2=0, int p_ms3=0, int p_stpreg = 0, int p_stpflg = 0) {
		// set pin states


    m_asyncSteps = 0;
    m_contSpd = 0;
    m_desiredContSpd = 1000.0;
    m_stepsMoved = 0;
    m_Steps = 0;
    m_totalSteps = 0;
    m_curSpline = 0;
    m_totalSplines = 0;
    m_curOffCycles = 0;
    m_curCycleErr = 0;
    m_curPos = 0;
    m_endPos = 0;
    m_startPos = 0;
    m_stopPos = 0;
    m_contAccelRate = 20.0;

    //variables for the next spline
    m_nextOffCycles = 0;
    m_nextCycleErr = 0;


    m_curPlanSpd = 0;
    m_curPlanErr = 0.0;
    m_curPlanSplines = 0;
    m_curPlanSpline = 0;
    m_planDir = false;

    m_asyncWasdir = false;
    m_curDir = false;
    m_switchDir = false;
    m_backCheck = false;
    m_motEn = false;
    m_motSleep = false;
    m_isRun = false;
    m_motCont = false;
    programDone(true);

    m_curMs = 1;
    m_backAdj = 0;
    m_easeType = OM_MOT_QUAD;


    m_splineOne.accel_fraction = 0.0;
    m_splineOne.decel_fraction = 0.0;
    m_splineOne.cruise_fraction = 0.0;
    m_splineOne.top_speed = 0.0;
    m_splineOne.decel_start = 0.0;
    m_splineOne.easing_coeff = 0.0;
    m_splineOne.accel_steps = 0;
    m_splineOne.decel_steps = 0;
    m_splineOne.cruise_steps = 0;

    m_splinePlanned.accel_fraction = 0.0;
    m_splinePlanned.decel_fraction = 0.0;
    m_splinePlanned.cruise_fraction = 0.0;
    m_splinePlanned.top_speed = 0.0;
    m_splinePlanned.decel_start = 0.0;
	m_splinePlanned.easing_coeff = 0.0;
    m_splinePlanned.accel_steps = 0;
    m_splinePlanned.decel_steps = 0;
    m_splinePlanned.cruise_steps = 0;

    m_refresh = true;


    m_stp = p_stp;
	m_slp = p_slp;
	m_dir = p_dir;
    m_ms1 = p_ms1;
    m_ms2 = p_ms2;
    m_ms3 = p_ms3;
    stpreg = p_stpreg;
    stpflg = p_stpflg;

    mtpc_steps        = 0;
	mtpc_dir      = false;
	mtpc_arrive       = 0;
	mtpc_accel        = 0;
	mtpc_decel        = 0;
	g_plan_type 		  = 0;
	mtpc_start	  = false;
	m_planLeadIn      = 0;
	m_planLeadOut     = 0;
	autoPause     = false;
	m_firstRun     = true;


	pinMode(m_stp, OUTPUT);
	pinMode(m_slp, OUTPUT);
	pinMode(m_dir, OUTPUT);
	pinMode(m_ms1, OUTPUT);
	pinMode(m_ms2, OUTPUT);
	pinMode(m_ms3, OUTPUT);

	m_calcMove = false;
	m_maxSpeed = 1000;

	splineReady = false;
	endOfMove = false;


void(*f_motSignal)(uint8_t) = 0;

	f_easeFunc = _quadEasing;
	f_easeCal = _qEaseCalc;
}

/** Destructor

*/



OMMotorFunctions::~OMMotorFunctions() {
}


/** Get MicroSteps

  Returns the current microstep setting.

  @return
  The current microstep setting.
  */

uint8_t OMMotorFunctions::ms() {
	return(m_curMs);
}

/** Set MicroSteps

  Sets the current microstep setting.  You may choose from the following
  values:

  1, 2, 4, 8, or 16

  @param p_Div
  Microstep setting value

  */


void OMMotorFunctions::ms( uint8_t p_Div ) {

  uint8_t s1 = false;
  uint8_t s2 = false;
  uint8_t s3 = false;

    switch( p_Div ) {
        case 1:
            break;
        case 2:
            s1 = true;
            break;
        case 4:
            s2 = true;
            break;
        case 8:
            s1 = true;
            s2 = true;
            break;
        case 16:
            s1 = true;
            s2 = true;
            s3 = true;
            break;
        default:
            return;
            break;
    }

  uint8_t wasMs = m_curMs;
  m_curMs       = p_Div;



  digitalWrite(m_ms1, s1);
  digitalWrite(m_ms2, s2);
  digitalWrite(m_ms3, s3);

    // adjust marker for home!
    if( wasMs != m_curMs ) {
        if( wasMs > m_curMs ){
            m_curPos /= (wasMs / m_curMs);
            m_startPos /= (wasMs / m_curMs);
            m_stopPos /= (wasMs / m_curMs);
            m_endPos /= (wasMs / m_curMs);
            m_backAdj /= (wasMs / m_curMs);
        } else {
            m_curPos *= (m_curMs / wasMs);
            m_startPos *= (m_curMs / wasMs);
            m_stopPos *= (m_curMs / wasMs);
            m_endPos *= (m_curMs / wasMs);
            m_backAdj *= (m_curMs / wasMs);
        }
    }

}


/** Get Current Direction

  Returns the current direction value.

  Note: if called in the middle of a move, the value returned may be
  different than the value after the move is completed, if you called move()
  in its two-argument form.

  @return
  The current direction value (true or false)
  */


uint8_t OMMotorFunctions::dir() {
	return(m_curDir);
}

/** Set Current Direction

  Sets the current direction value.

  Note: if called while a move is underway, the direction will be changed
  immediately, but backlash compensation will not be applied. Additionally,
  if this method is called during a move executed via the two-argument form
  of move(), then the direction value may not be retained after the move is
  completed.

  @param p_Dir
  Direction value (true or false)

  */

void OMMotorFunctions::dir( uint8_t p_Dir ) {

// only do anything if we're changing directions

  if( p_Dir != m_curDir ) {

    digitalWrite( m_dir, p_Dir);
   	// enable backlash compensation
    m_backCheck = true;
   	// set new dir value
    m_curDir = p_Dir;
    m_asyncWasdir = m_curDir;
  }

}


/** Set Callback Handler

 As actions start and complete, a callback is used to signal what is happening,
 without having to query the method to see if a given action is still occuring.

 By passing a function pointer to this method, that function will be executed
 at each one of these moments, and passed a single, one byte argument that
 matches the signal which is to be sent.  The template for this function is:

 @code
 void function(byte val) { }
 @endcode

 The following codes are defined:

 @code
 OM_MOT_DONE
      - The motor has stopped moving
 OM_MOT_MOVING
      - The motor has begun moving
 @endcode

  An example of an appropriate callback handler:

 @code
 #include "TimerOne.h"
#include "OMMotorFunctions.h"

OMMotorFunctions Motor = OMMotorFunctions();

void setup() {

  Motor.enable(true);
  Motor.continuous(false);
  Motor.setHandler(motorCallback);

  Serial.begin(19200);

}

void loop() {

 if( ! Motor.running() ) {
  delay(1000);
  Serial.println("Moving!");
  Motor.move(true, 1000);
 }

}

void motorCallback( byte code ) {

  if( code == OM_MOT_DONE ) {
    Serial.println("Got Done!");
    Serial.print("Moved: ");
    unsigned long smoved = Motor.stepsMoved();
    Serial.println(smoved, DEC);
  }
  else {
    Serial.println("Got Begin!");
  }

}
  @endcode

   @param p_Func
 A pointer to a function taking a single byte argument

 */


void OMMotorFunctions::setHandler( void(*p_Func)(uint8_t) ) {
	f_motSignal = p_Func;
}

void OMMotorFunctions::_fireCallback(uint8_t p_Param) {
	if( f_motSignal != 0 ) {
		f_motSignal(p_Param);
	}
}



/** Get Backlash Compensation Amount

 Returns the currently set backlash compensation amount.

 @return
 Backlash compensation amount
 */

unsigned int OMMotorFunctions::backlash() {
	return(m_backAdj);
}

/** Set Backlash Compensation Amount

  Sets the current backlash compensation amount.

  The backlash compensation allows for adjustments to moves when the
  gear train attached to your motor exhibits backlash.  If the direction
  of the motor is changed between moves, the specified amount of steps will
  be added to the next movement, to take up the slack created by the backlash.

  @param p_Back
  The backlash compensation amount, in steps

  */

void OMMotorFunctions::backlash(unsigned int p_Back) {
	m_backAdj = p_Back;
}

/** Enable Motor

  Sets the current Enable/Disable flag value.

  If the motor is not enabled, calls to move() will return immediately and
  trigger a callback with the OM_MOT_DONE value.  It will not be possible
  to move the motor in any fashion while the motor is disabled.

  @param p_En
  Enable (true) or Disable (false) the motor

  */


void OMMotorFunctions::enable(uint8_t p_En) {
	m_motEn = p_En;
}

/** Get Enable Flag Value

  Returns the current enable flag value.

  @return
  Enabled (true) or Disabled (false);

  */

uint8_t OMMotorFunctions::enable() {
	return(m_motEn);
}

/** Enable Sleep

  Sets the current sleep between move flag value and immediately changes
  the status off the motor driver sleep line.

  When sleep is enabled, the motor driver will be put to sleep between
  moves to conserve energy and reduce the heat of both the motor and the
  driver.

  @param p_En
  Enable (true) or Disable (false) sleeps between moves

  */

void OMMotorFunctions::sleep(uint8_t p_En) {
	m_motSleep = p_En;

		// turn the sleep line off if sleep disabled, otherwise
		// turn sleep line on

	uint8_t doSleep = p_En ? OM_MOT_SSTATE : ! OM_MOT_SSTATE;
	digitalWrite(m_slp, doSleep);
}

/** Get Sleep Flag Value


  Returns the current value of the sleep between moves flag.

  @return
  Enabled (true), Disabled (false()

  */

uint8_t OMMotorFunctions::sleep() {
	return(m_motSleep);
}

/** Set Continuous Motion Speed

 Sets the current continuous motion speed, in steps per second.

 You may set the speed even while a continuous move is running, and the
 speed will be immediately changed.

 Allowable values are any valid positive range of a floating point integer
 from 0.00000001 to the currently set maximum step rate.  See maxStepRate()
 for more information.  If you attempt to set a speed higher than the current
 maxStepRate(), your input will be ignored.

 It is not recommended to call this method while executing a non-continuous move,
 or after a move has been planned.  Doing so will result in loss of control of
 the motor, or corruption of your planned move.

 @param p_Speed
 Steps per second
 */

void OMMotorFunctions::contSpeed(float p_Speed) {

	if( abs(p_Speed) > maxStepRate())
		return;

        m_desiredContSpd = p_Speed;

}

float OMMotorFunctions::desiredSpeed() {
	return(m_desiredContSpd);
}

/** Get Continuous Motion Speed

  Returns the current continuous motion speed.

  @return
  Steps per second
  */

float OMMotorFunctions::contSpeed() {
	return(m_contSpd);
}


/** Set Continuous Motion Mode

  Enables or disables continuous motion mode.

  In continuous motion mode, the motor will run continuously at the specified
  speed until either a stop() request is received, or maxSteps() are
  reached.  When continuous mode is disabled, the motor only moves the distance
  specified by steps() or passed via the two-argument form of move().

  Note: you may adjust the currently moving speed at any time, using contSpeed().

  Of additional note: when in continous mode, the callback will be executed
  immediately and passed the OM_MOT_DONE value when calling move(),
  indicating that you are free to do other activities while the motor is moving.

  Below is an example of performing a continuous motion move, with a maxSteps()
  value set.  The motor will move at a speed of 1,000 steps/second, and will
  automatically stop its self after 5,000 steps have been taken.  Once the motor
  is stopped, the steps moved value is cleared out, and we're ready to move again.

  @code
#include "TimerOne.h"
#include "OMMotorFunctions.h"

OMMotorFunctions Motor = OMMotorFunctions();

unsigned int max_steps = 5000;

void setup() {

  Motor.enable(true);
  Motor.sleep(true);

  Motor.continuous(true);
  Motor.contSpeed(1000);
  Motor.maxSteps(5000);

  Motor.setHandler(motorCallback);

  Serial.begin(19200);

}

void loop() {

 if( ! Motor.running() ) {

  delay(1000);
  Serial.println("Clearing Motor");
  Motor.clear();
  Serial.println("Starting Continuous Motion!");
  Motor.move();

 }
  else {

    Serial.print("Moved: ");
    unsigned long smoved = Motor.stepsMoved();
    Serial.println(smoved, DEC);

    delay(1000);

  }

}

void motorCallback( byte code ) {

  if( code == OM_MOT_DONE ) {
    Serial.println("Got Done!");
    Serial.print("Moved: ");
    unsigned long smoved = Motor.stepsMoved();
    Serial.println(smoved, DEC);
  }
  else {
    Serial.println("Got Begin!");
  }
}

@endcode

  @param p_En
  Enable (true) or Disable (false() continuous motion

  */

void OMMotorFunctions::continuous(uint8_t p_En) {
	m_motCont = p_En;
	//_updateContSpeed();
}

/** Get Continuous Motion Value

  Returns the current continuous motion value.

  @return
  Enabled (true), or Disabled (false)
  */


uint8_t OMMotorFunctions::continuous() {
	return(m_motCont);
}





/** Set Continuous Motion Speed Acceleration

 Sets the current continuous motion acceleration, in steps per second^2. Must be a positive float
 value greater than 0.0.


 @param p_Speed
 Steps per second^2
 */

void OMMotorFunctions::contAccel(float p_Accel) {

	if( p_Accel <= 0.0 )
		return;
    m_contAccelRate = p_Accel/(1000.0 / MS_PER_SPLINE);  //convert to steps/spline
}

/** Get Continuous Motion Speed Acceleration

 Sets the current continuous motion acceleration, in steps per second^2. Must be a positive float
 value greater than 0.0.


 @return
 Steps per second^2
 */

float OMMotorFunctions::contAccel() {

	return(m_contAccelRate * (1000.0 / MS_PER_SPLINE));
}


/** Update Continous Speed

Updates the next spline for continous speed. This is a linear acceleration and deceleration.

*/


void OMMotorFunctions::_updateContSpeed(){

    if (m_switchDir){

        //decelerate until you're within half of the m_contAccelRate step/sec of stop
        if (m_contSpd >= 0.0)
            m_contSpd -= m_contAccelRate;
        else
            m_contSpd += m_contAccelRate;

        if (m_contSpd <= (m_contAccelRate/2.0)){
            //don't want to set it zero as that will give us an infinite off_time
            m_contSpd = 0.01;
            m_switchDir = false;
            dir(!m_curDir);
        }

        //calculate the next spline's off cycles and off cycles error
        float curSpd = abs(m_contSpd) / (1000.0 / MS_PER_SPLINE);  //steps per spline

        // figure out how many cycles we delay after each step
        float off_time = g_cyclesPerSpline / (curSpd);

        m_nextOffCycles = (unsigned long) off_time;
        //multiple the error by the FLOAT_TOLERANCE to get rid of the float variable
        m_nextCycleErr = (off_time - (unsigned long) off_time)*FLOAT_TOLERANCE;

        return;

    }

    //compensate for any backlash
    if( m_backCheck == true ) {
   	   if (dir() == 0)
            m_curPos +=backlash();
       else
            m_curPos -=backlash();
   	   m_backCheck = false;
    }

    //current speed is less than the desired speed
    if (m_contSpd <= (m_desiredContSpd - m_contAccelRate)){

        m_contSpd += m_contAccelRate;

    //current speed is greater than the desired speed
    } else if (m_contSpd >= (m_desiredContSpd + m_contAccelRate)){

        m_contSpd -= m_contAccelRate;

    //we're within 25 steps/second of the desired speed, close enough to jump to desired
    } else {
        m_contSpd = m_desiredContSpd;
        if (m_desiredContSpd == 0.0 || m_contSpd == 0.0)
            stop();
    }

    //check to see if you need to reverse directions
    if(m_contSpd <= m_contAccelRate/2.0 && m_contSpd >= m_contAccelRate/-2.0){
        if(m_desiredContSpd >=0.0)
            dir(1);
        else
            dir(0);
    }


    //calculate the next spline's off cycles and off cycles error
    float curSpd = abs(m_contSpd) / (1000.0 / MS_PER_SPLINE);  //steps per spline

         // figure out how many cycles we delay after each step
    float off_time = g_cyclesPerSpline / (curSpd);

    m_nextOffCycles = (unsigned long) off_time;
    //multiple the error by the FLOAT_TOLERANCE to get rid of the float variable
    m_nextCycleErr = (off_time - (unsigned long) off_time)*FLOAT_TOLERANCE;
}

/** Motor Running?

  Returns whether or not the motor is currently moving.

  @return
  Running (true) or Stopped (false)
  */

uint8_t OMMotorFunctions::running() {
	return(m_isRun);
}

/** Set Steps per Move

 Sets the current steps per move for non-continuous motion moves.

 This method sets a default steps per move so that one may use move() with
 no arguments.

 @param p_Steps
 Steps Per Move
 */

void OMMotorFunctions::steps(unsigned long p_Steps) {
	m_Steps = p_Steps;
}

/** Get Steps Per Move

  Returns the current steps per move for non-continuous motion moves.

  @return
  Steps Per Move
  */

unsigned long OMMotorFunctions::steps() {
	return(m_Steps);
}

/** Set Maximum Steps

  Sets the maximum steps to be taken, for both continuous and non-
  continuous moves.

  Once this steps count is achieved, the motor will be stopped immediately and
  the callback will be called with the OM_MOT_DONE parameter.  It will not be
  possible to move again until the clear() method has been called.

  Set the maximum steps to 0 to disable this feature.

  @param p_Steps
  Maximum steps to move
  */

void OMMotorFunctions::maxSteps(long p_Steps) {
	m_totalSteps = p_Steps;
}

/** Get Maximum Steps

  Returns the current maximum step count.

  @return
  Maximum Steps to Move
  */

long OMMotorFunctions::maxSteps() {
	return(m_totalSteps);
}

/** Set Maximum Stepping Speed

 Sets the maximum speed at which the motor will move using the no- or two-
 argument form of move().

 This method controls the speed at which moves are made when requesting
 a move without a specified arrival time.  It does not alter the behavior of the
 step timing mechanism, but instead puts an additional limit on the time calculated
 to take a move such that the motor will be given enough time to make the move
 at the specified speed.

 This method is best used, for example, to limit the speed at which a move() is
 made, when we want that speed to fall between two usable rates, for example, 800
 steps per second is lower than the minimum rate of 1,000 steps per second.

 The following is an example of limiting a move without a specified arrival time
 to 500 steps/second maximum speed:

 @code

 void setup() {
   Motor.maxStepRate(5000);
   Motor.maxSpeed(500);
 }

 void loop() {

  static uint8_t dir = true;

  if( ! Motor.running() {
  	move(dir, 1000);
  	dir = !dir;
 }

 @endcode

 Additional benefits of the above code is that the timing loop will run at the
 maximum possible speed, allowing for the smoothest and most correct delay
 between steps, but the motor will still be limited to 800 steps per second in
 the above move.

 Note: if you specify a value greater than the currently set maxStepRate(),
 your input will be ignored.

 This method has no effect on the six-argument form of move(), plan(), planRun(),
 or continuous motion movements.

 @param p_Speed
 Maximum speed, in steps per second
*/

void OMMotorFunctions::maxSpeed(unsigned int p_Speed) {
	if( p_Speed > maxStepRate() || p_Speed == 0 )
		return;

	m_maxSpeed = p_Speed;
}

/** Get Maximum Stepping Speed

 Returns the current maximum stepping speed.

 Note: this value is different than that returned by maxStepRate().

 @return
 Maximum steping speed, in steps/second
 */

unsigned int OMMotorFunctions::maxSpeed() {
	return(m_maxSpeed);
}

/** Set Maximum Stepping Rate

 Sets the maximum rate at which steps can be taken.

 Due to the nature of the timing calculations, you are limited to one of the
 following choices: 5000, 4000, 2000, and 1000.  Any other input will be
 ignored and result in no change.

 Please note: the maximum rate here defines how often the step interrupt
 service routine runs.  The higher the rate you use, the greater the amount
 of CPU that will be utilized to run the stepper.  At the maximum rate, over
 50% of the available processing time of an Atmega328 is utilized at 16Mhz,
 while at the lowest rate, approximately 25% of the processing time is utilized.

 For more information on step timing, see the \ref steptiming "Stepping Cycle" section.

 This rate does not dictate the actual speed of specific moves,
 it defines the highest speed which is achievable for any kind of move.

 @param p_Rate
 The rate specified as a number of steps per second (5000, 4000, 2000, or 1000)
 */

void OMMotorFunctions::maxStepRate( unsigned int p_Rate ) {


	if(  p_Rate != 10000 && p_Rate != 5000 && p_Rate != 4000 && p_Rate != 2000 && p_Rate != 1000 )
		return;

		// convert from steps per second, to uSecond periods
	g_curSampleRate = 1000000 / (unsigned long) p_Rate;

		// timeslices are in MS_PER_SPLINE mS, so how many
		// stepping samples are there for one MS_PER_SPLINE?

	g_cyclesPerSpline = (MS_PER_SPLINE * 1000) / g_curSampleRate;

}

/** Get Current Sample Period

 Returns the current sample rate of the motor.

 @return
 Sample period in microseconds
 */

unsigned int OMMotorFunctions::curSamplePeriod() {
    return( g_curSampleRate );
}

/** Get Maximum Stepping Rate

 Returns the current maximum stepping rate in steps per second.

 @return
 Maximum rate in steps per second
 */

unsigned int OMMotorFunctions::maxStepRate() {
	return((unsigned int)( (long) 1000000 / (long) g_curSampleRate ));
}


/** Get Steps Moved

 Returns the count of steps moved since the last clear() call.

 @return
 Steps moved
 */

unsigned long OMMotorFunctions::stepsMoved() {
	return(m_stepsMoved);
}

/** Plan an Interleaved Move

 Plans out an interleaved move with acceleration and deceleration profile.

 This method plans out a complex movement that is to be made out of several
 smaller, individual movements.  This allows one to interleave motor movement
 between other activities (such as firing a camera), while maintaining
 user-controlled acceleration and deceleration curves.

 Given a number of interleaved intervals, a total number of steps to move,
 the count of intervals to accelerate to full speed over, and the count
 of intervals to decelerate back to a stop, this method performs all activities
 neccessary.

 After calling this method, you may then use planRun() at each interval to move
 the number of steps.  The number of steps used in each move are automatically
 defined to re-create the desired acceleration and deceleration curves.

 The easing algorithm to use for mapping the planned move (but not making
 Note that the current plan is reset each time you call clear().

 A planned move can result in movement that is not as smooth as that of a
 single move executed over the same period of time.

 During a planned move, the same algorithm ussed to control acceleration and
 velocity in a single move is applied to the plan to determine how many steps
 to take at each interval.  A single move run in real-time has its speed
 varied once per millisecond (see maxStepRate() for more information), however
 a planned move is only able to vary its speed at each interval.  In addition,
 a planned move may only take full steps during each transition, it is not possible
 to spread an individual step across two intervals.* The result of this is that
 for a the planned move to haave the same output smoothness as a single move, there
 must an equivalent number of intervals for the same time period.

 * While it is not possible to spread a single step across two intervals in a
 plannned move, most planned moves do not result in a whole integer of movement at
 each step.  The remainder of a step which was required to be taken but not is
 accumulated until it overflows, and an additional step is taken at this time.
 In this manner, more natural output movement is created given most input
 parameters.


 @param p_Shots
 Number of intervals in the plan.

 @param p_Dir
 Direction of travel for the plan

 @param p_Dist
 Number of steps to travel total

 @param p_Accel
 Number of intervals during which acceleration occurs

 @param p_Decel
 Number of intervals during which deceleration occurs
*/
void OMMotorFunctions::plan(unsigned long p_Shots, uint8_t p_Dir, unsigned long p_Dist, unsigned long p_Accel, unsigned long p_Decel) {

	m_curPlanSpd = 0;
	m_curPlanErr = 0.0;
	m_curPlanSplines = p_Shots;
	m_curPlanSpline = 0;
	m_planDir = p_Dir;

	// prep spline variables (using planned mode)
	_initSpline(true, p_Dist, p_Shots, p_Accel, p_Decel);

}

/** Execute the Next Iteration of the Current Plan

 Executes the next move interval as dictated by the current plan.

 If a plan has been created using plan(), this method makes the next movement
 as defined in the current plan.  This method is used to create interleaved
 movement, wherein the motor moves between some other activity.  At each call
 to planRun(), the next set of steps is executed.  The plan() method initiates
 the plan, and this method carries it out as needed.

 A movment using planRun() is always executed at the maximum speed allowed, as
 defined by maxStepRate().  If this is too fast for your motor, you must use
 maxStepRate() to set your step rate to the value nearest, and lower than that
 which can be achieved by your motor.

 If a move can be executed, asynchronous movement is initiated and the
 callback is called.  OM_MOT_BEGIN will be passed to the callback when the
 motion sequence begins.  After the motion sequence has been completed, the
 callback will be called again with the OM_MOT_DONE argument.

 If the motor is disabled, the maximum # of steps has been reached, or the
 plan has been fully executed the callback will be executed with the OM_MOT_DONE
 argument immediately, and no move will take place.

 The following is an example of a simple interleaved motion using plan() and planRun():

 @code
 #include "TimerOne.h";
 #include "OMMotorFunctions.h":

 OMMotorFunctions Motor = OMMotorFunctions();

 volatile uint8_tean okNext = true;

 unsigned int totalIntervals = 500;
 unsigned int totalSteps = 2000;
 	// 20% of time accelerating
 unsigned int accelTm = totalSteps / 5;
 	// 10% of time decelerating
 unsigned int decelTm = totalSteps / 10;

 void setup() {
 	Serial.begin(19200);

 	 // setup motor
 	Motor.enable(true);
 	Motor.ms(1);

 	Motor.setHandler(motorCB);

 	 // set max step rate
 	Motor.maxStepRate(1000);

 	// plan our interleaved move
 	Motor.plan(totalIntervals, true, totalSteps, accelTm, decelTm);
 }

 void loop() {

 	static unsigned int intervalsTaken = 0;

 	if( okNext && intervalsTaken < totalIntervals ) {
 		// do something, like fire a camera

 		intervalsTaken++;
 		okNext = false;
 		 // execute next planned moved

 		Motor.planRun();
 	}
 }

 void motorCB(byte code) {
   if( code == OM_MOT_DONE ) {
     okNext = true;
   }
 }
 @endcode

 In this example, we plan a move over 500 intervals to travel 1000 steps and
 spend 20% of the total movement accelerating, while spending 10% decelerating.
 After each activity has occurred, we call planRun() to move the motor.

 While the motor is moving, the value of okNext stays false, and when the move
 is completed, the callback named motorCB() is called.  This callback checks the
 code returned, and if it indicates the motor has completed movement, it changes
 the value of okNext to true, where the main loop then takes another activity and
 moves the motor again.

 */

void OMMotorFunctions::planRun() {
	
	// if motor is disabled, do nothing
	if( ! enable() || ( maxSteps() > 0 && stepsMoved() >= maxSteps() ) || m_curPlanSpline >= m_curPlanSplines ) {
		_fireCallback(OM_MOT_DONE);
		return;
	}

	m_curPlanSpline++;
	
	// Determine how far through the move we are
	float tmPos = ((float) m_curPlanSpline) / (float) m_curPlanSplines;
	
	// get steps to move for next movement
	f_easeFunc(true, tmPos, this); // sets m_curPlanSpd
	
	move(m_planDir, m_curPlanSpd);

}

/** Reverse the Last Iteration of the Current Plan

 Backs up one plan iteration by moving the same distance as the previous
 plan interation in the opposite direction

 */

void OMMotorFunctions::planReverse() {
	
    if( ! enable() || m_curPlanSpline == 0 ) {
        _fireCallback(OM_MOT_DONE);
		return;
    }


    // get steps to move for last movement (m_curPlanSpline is not changed)

	float tmPos = (float) m_curPlanSpline / (float) m_curPlanSplines;

	f_easeFunc(true, tmPos, this); // sets m_curPlanSpd
        // note that direction is reversed
	move(!m_planDir, m_curPlanSpd);

    // now, we move back m_curPlanSpline so we don't lose our place

    m_curPlanSpline = (m_curPlanSpline >= 1) ? m_curPlanSpline - 1 : m_curPlanSpline;

}


/** Move Now

 Executes a move immediately using the provided parameters.

 This method allows you to make a complete move by specifying the distance to move,
 when to arrive at that destination, acceleration time, and deceleration time.

 This is the most simple way to execute a complete move in continuous motion
 with ease-in and -out with applied to the entire move.

 The easing algorithm used can be configured via the easing() method.

 Specify the time you to arrive at the destination position (in mS from now),
 how much of that time is spent accelerating (the shorter this time, the sharper the
 easing curve), and how much of that time is spent decelerating to a stop.  The
 motor will arrive at the given position, in the given time (with an error rate not
 to exceed 0.00024% for most moves), having eased in and out of the maximum speed
 required to complete the move in time.

 If a move can be executed, asynchronous movement is initiated and the
 callback is called.  When in continuous motion, the OM_MOT_DONE argument
 will be passed to the callback, otherwise OM_MOT_BEGIN will be passed.

 If the motor is disabled, or the maximum # of steps has been reached,
 the callback will be executed with the OM_MOT_DONE argument immediately,
 and no move will take place.


 @param p_Dir
 The direction to move

 @param p_Dist
 The distance to move (steps)

 @param p_Time
 The total time to travel to the new position (mS)

 @param p_Accel
 The travel time spent accelerating to full speed (mS)

 @param p_Decel
 The travel time spent decelerating back to a stop (mS)

 */

void OMMotorFunctions::move(uint8_t p_Dir, unsigned long p_Dist, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel) {
	
		// if motor is disabled, do nothing
	if( ! enable() || ( maxSteps() > 0 && stepsMoved() >= maxSteps() ) ) {
		_fireCallback(OM_MOT_DONE);
		return;
	}

    m_asyncWasdir = dir();
    dir( p_Dir );

    // check for backlash compensation
    if( m_backCheck == true ) {
       p_Dist += backlash();
       if (dir() == 0)
            m_curPos +=backlash();
       else
            m_curPos -=backlash();
       m_backCheck = false;
    }

	//m_totalSplines = p_Time / MS_PER_SPLINE;

		// prep spline variables
	_initSpline(false, p_Dist, p_Time, p_Accel, p_Decel);


		// we need to initialize the first spline point
	m_curSpline = 1;
	float tmPos = (float) m_curSpline / (float) m_totalSplines;

	f_easeFunc(false, tmPos, this);

	m_calcMove = true;

		// move with the spline parameters set
	move(p_Dir, p_Dist);
}

/** Resume Move

Moves the motor with the same parameters that were calculated before. This is used
to resume movement after the program was paused while in contiuous move.

*/


void OMMotorFunctions::resumeMove(){

    m_calcMove = true;
	if (m_curPos-m_stopPos != 0)
		move(dir(),abs(m_curPos-m_stopPos));

}


/** Move Now

 Immediately executes a move using saved parameters.

 This method will execute either a continuous or non-continuous move
 using the values currently saved in dir() and steps() or contSpeed(),
 depending on your current motion type.

 For continuous motion, you must specify 0 as the number of steps.  To
 limit continuous motion travel, use maxSteps() instead.

 For non-continuous motion, this form of move allows you to only specify the
 destination and direction (using saved parameters), and the motor will move as quickly as possible to
 the given destination, with acceleration and deceleration ramping occuring over
 100 steps, or 1/4 of the total steps for the move - whichever is less.  The
 speed of this form of move is controlled by the lesser of maxStepRate() or
 maxSpeed().

 If a move can be executed, asynchronous movement is initiated and the
 callback is called, with OM_MOT_BEGIN passed as the argument value.

 If the motor is disabled, or the maximum # of steps has been reached,
 the callback will be executed with the OM_MOT_DONE argument immediately,
 and no move will take place.

*/

void OMMotorFunctions::move() {
	move(dir(), steps());
}



void OMMotorFunctions::move(uint8_t p_Dir, unsigned long p_Steps) {
	move(p_Dir, p_Steps, false);
}


void OMMotorFunctions::move(uint8_t p_Dir, unsigned long p_Steps, bool p_Send) {

		// if motor is disabled, do nothing
   if( ! enable() || ( maxSteps() > 0 && stepsMoved() >= maxSteps() ) ) {
		_fireCallback(OM_MOT_DONE);
		return;
   }
   if (m_desiredContSpd == 0.0)
        return;

	// note: the check on p_Steps is required
	// to allow manual moves that are not continuous
	// when motor is set in continuous mode

   if( p_Steps == 0 && continuous() ) {


        _updateContSpeed();
       // continuous motion mode

       if( ! running() ) {
            m_asyncWasdir = dir();
            dir( p_Dir );
            m_asyncSteps = 0;
            _stepsAsync(p_Dir, (unsigned long) 0);

        //if switching directions while the motor's running
       } else if( p_Dir != m_curDir ) {
            m_switchDir = true;
            return;
       }

       _fireCallback(OM_MOT_BEGIN);
       return;
   }

   //ignore steps of 0 if not in continuous mode
   if (p_Steps == 0){
     return;
   }

   	// continuous allows higher stepping rate
   	// so clamp back down if not doing a continuous
   	// move

    if( maxStepRate() > 5000 )
   	   maxStepRate(5000);


   if( ! m_calcMove ) {

        // this move was not specified with a time to accomplish, so accomplish
        // in minimal amount of time possible

         // determine how far we'd move in our acceleration and deceleration
         // times for moves that must operate at maximum speed. We accelerate
         // and decel over 200 steps each, or 1/4 of the total steps, whichever is less

        //check to see if there's a direction change
        m_asyncWasdir = dir();
        dir( p_Dir );

        // check for backlash compensation
        if( m_backCheck == true ) {
           p_Steps += backlash();
           if (dir() == 0)
                m_curPos +=backlash();
           else
                m_curPos -=backlash();
           m_backCheck = false;
        }

            // set travel const (.travel) here before attempting to use it
        _setEasingCoeff(&m_splineOne);

        unsigned int mSpeed = abs(m_desiredContSpd);

		// Fixed acceleration/deceleration step length to use for "send to..." and SMS motor commands
		const int ACCEL_DECEL_LENGTH_SEND = 4000;
		const int ACCEL_DECEL_LENGTH_SMS = 500;
		int accel_decel_length;

		// If this is a "send to..." move (i.e. sending the motor to the start position, use a longer accel/decel length)
		if (p_Send == true)
			accel_decel_length = ACCEL_DECEL_LENGTH_SEND;
		else
			accel_decel_length = ACCEL_DECEL_LENGTH_SMS;


		// This is the fixed acceleration/deceleration step length used for the "send to" commands.
		float rampSteps = ((p_Steps / 2.0) < accel_decel_length) ? (p_Steps / 2.0) : accel_decel_length;


        rampSteps *= 2.0;

        //calculated cruise time
        float cruise_fraction = ((p_Steps - rampSteps) / mSpeed) * 1000.0;

        //calculate acceleration and deceleration time
		float adTm = ((rampSteps / mSpeed) * 1000.0) * m_splineOne.easing_coeff;

		//calculated total move time
        float mvMS = (cruise_fraction + adTm);

            // take a minimum of 50ms to make the move - to prevent over-speeding
            // and getting goofy.
        if( mvMS < 50.0 ) {
            mvMS = 50.0;
        }

            // calculated total number of splines during the move
        //m_totalSplines = (unsigned long) mvMS / MS_PER_SPLINE;

            // prep spline variables
        _initSpline(false, p_Steps, mvMS, adTm/2.0, adTm/2.0);

            // we need to initialize the first spline point
        m_curSpline = 1;
        float tmPos = (float) m_curSpline / (float) m_totalSplines;
            // calculate the first running speed
        f_easeFunc(false, tmPos, this);
   }

	// limit step cycle using this control
	// value, without mucking with max steps

   m_asyncSteps = p_Steps;

   	// re-set called from five-argument flag

   m_calcMove = false;


   _stepsAsync(p_Dir, p_Steps);
   _fireCallback(OM_MOT_MOVING);

}

void OMMotorFunctions::stop() {

        // set motors not moving in async mode
      m_isRun = false;
      m_refresh = true;
      endOfMove = false;
      splineReady = false;
      m_motCont = false;
      m_contSpd = 0.0;

        // set sleep state for drivers if needed
      if( sleep() )
        digitalWrite(m_slp, OM_MOT_SSTATE);


      	// signal completion
      _fireCallback(OM_MOT_DONE);

}

 // execute an async move, when specifying a direction
void OMMotorFunctions::_stepsAsync( uint8_t p_Dir, unsigned long p_Steps ) {


	 // is async control not already running?
	 if( ! running() ) {



         m_asyncSteps = p_Steps;
         m_firstRun = true;
         //updateSpline();

          // bring sleep pin to non-sleeping state if
          // motor kill enabled

        if( sleep() )   {
          digitalWrite(m_slp, !OM_MOT_SSTATE);
            // we don't want to start moving before its safe
            // to do so
          delay(OM_MOT_SAFE);
        }

        // set motors moving
        m_isRun = true;


	 } // end if not running

}

/** Set Easing Algorithm

 Sets the easing algorithm to be used for future moves.

 You may specify an easing type using the constants OM_MOT_LINEAR,
 OM_MOT_QUAD, and OM_MOT_QUADINV. Doing so will change the algorithm for future moves.

 You may not change the easing algorithm while a move is executing, attempting
 to do so will result in no change to the easing algorithm to prevent dangerous
 fluctuations in motor speed.

 If you attempt to specify a value for the easing type which is not supported,
 no change will be made.

 See the \ref ommotion "Motion Capabilities" section for more information about
 easing algorithms.

 @param p_easeType
 The type of easing algorithm to use, either OM_MOT_LINEAR, OM_MOT_QUAD, or OM_MOT_QUADINV.

 */

void OMMotorFunctions::easing(uint8_t p_easeType) {

	// do not attempt to change easing type while
	// executing a move
  if( running() )
  	  return;

  if( p_easeType == OM_MOT_LINEAR ) {
  	  f_easeFunc = _linearEasing;
  }
  else if( p_easeType == OM_MOT_QUAD ) {
  	  f_easeFunc = _quadEasing;
  	  f_easeCal = _qEaseCalc;
  }
  else if( p_easeType == OM_MOT_QUADINV ) {
  	  f_easeFunc = _quadEasing;
  	  f_easeCal = _qInvCalc;
  }
  else {
  	  	// unsupported type
  	  return;
  }

  m_easeType = p_easeType;
}


/** Get Easing Algorithm

 Returns the current easing alorithm for the motor.


 @return
 Easing algorithm, linear is OM_MOT_LINEAR, quad is OM_MOT_QUAD, quad inv is OM_MOT_QUADINV
 */

uint8_t OMMotorFunctions::easing() {
    return (m_easeType);
}


/** Clear Steps Moved

  Clears the count of steps moved.  Additionally, if the motor is currently
  running, any movement will be immediately stopped, and the callback will
  be executed with the OM_MOT_DONE argument.

  This method does NOT reset the home location for the motor.  It does, however,
  reset any plan currently executing or previously executed.

  */

void OMMotorFunctions::clear() {

		// stop if currently running
/*
	if( running() )
		stop();
*/
	m_stepsMoved = 0;

		// wipe out plan
	m_curPlanSpd = 0;
	m_curPlanErr = 0.0;
	m_curPlanSplines = 0;
	m_curPlanSpline = 0;
}


void OMMotorFunctions::_updateMotorHome(int p_Steps) {

	// update total steps moved counter

  m_stepsMoved += p_Steps;

    // update relationship to home --
    // if going in positive direction, increase distance
    // - otherwise decrease distance

  if( m_curDir == 0 )
  	  p_Steps *= -1;

  m_curPos += p_Steps;

}

/** Set Home Position

 Sets the current position of the motor as the home position.

 */

void OMMotorFunctions::homeSet() {
    if (m_endPos != 0)
        m_endPos -= m_curPos;
    m_startPos -= m_curPos;
    m_stopPos -= m_curPos;
	m_curPos = 0;
}

/** Set End Position

 Sets the motor's end position.

 */

void OMMotorFunctions::endPos(long p_steps) {
	m_endPos = p_steps;
}

/** Get End Position

 Gets the limit position. The motor cannnot move past m_limitPos steps
 away from the m_curPos.

 @return
 A signed long, steps away from the home position that the limit of the travel is.

 */

long OMMotorFunctions::endPos() {
	return(m_endPos);
}

/** Set Start Position

 Sets the motor's start position.

 @param p_steps
 A signed long, steps away from home that the location of the start position

 */

void OMMotorFunctions::startPos(long p_steps) {
	m_startPos = p_steps;
}

/** Get Start Position

 Gets the motor's start position

 @return
 A signed long, steps away from home that the location of the start position

 */

long OMMotorFunctions::startPos() {
	return(m_startPos);
}

/** Set End Position

 Sets the motor's end position

 @param p_steps
 A signed long, steps away from home that the location of the end position

 */

void OMMotorFunctions::stopPos(long p_steps) {
	m_stopPos = p_steps;
}

/** Get End Position

 Gets the motor's end position

 @return
 A signed long, steps away from home that the location of the end position

 */

long OMMotorFunctions::stopPos() {
	return(m_stopPos);
}

/** Set Current Position

 Sets the current position in steps away from home.

*/

void OMMotorFunctions::currentPos(long p_steps) {
	m_curPos = p_steps;
}

/** Current Position

 Returns the number of steps and direction back to the home position.

 @return
 A signed long, representing the distance and direction from home.  A negative
 number represents the motor is that many steps in the false direction from home,
 whereas positive represents that many steps in the true direction from home.
*/

long OMMotorFunctions::currentPos() {
	return(m_curPos);
}


/** Send Motor Home

 Send motor home immediately.

 This method will not stop any ongoing move, but it will immediately send
 the motor back to its home position, and will result in an aborted move.

 Like other moves, this move is non-blocking and will trigger the callback
 to be executed with the OM_MOT_DONE argument when the send to home move
 is completed.
 */

void OMMotorFunctions::home() {

    moveTo(0, true);

}

/** Send Motor to Start Pos

 Send motor to start position immediately.

 This method will not stop any ongoing move, but it will immediately send
 the motor back to its start position, and will result in an aborted move.

 Like other moves, this move is non-blocking and will trigger the callback
 to be executed with the OM_MOT_DONE argument when the send to home move
 is completed.
 */

void OMMotorFunctions::moveToStart() {

    moveTo(m_startPos, true);

}

/** Send Motor to Stop Pos

 Send motor to stop position immediately.

 This method will not stop any ongoing move, but it will immediately send
 the motor back to its stop position, and will result in an aborted move.

 Like other moves, this move is non-blocking and will trigger the callback
 to be executed with the OM_MOT_DONE argument when the send to home move
 is completed.
 */

void OMMotorFunctions::moveToStop() {


    moveTo(m_stopPos, true);

}

/** Send Motor to End Pos

 Send motor to end position immediately.

 This method will not stop any ongoing move, but it will immediately send
 the motor back to its end position, and will result in an aborted move.

 Like other moves, this move is non-blocking and will trigger the callback
 to be executed with the OM_MOT_DONE argument when the send to home move
 is completed.
 */

void OMMotorFunctions::moveToEnd() {

    if (m_endPos == 0)
        return;

    moveTo(m_endPos, true);

}

/** Send Motor to Position

 Send motor to given position immediately using a simple move.

 This method will not stop any ongoing move, but it will immediately send
 the motor back to its home position, and will result in an aborted move.

 Like other moves, this move is non-blocking and will trigger the callback
 to be executed with the OM_MOT_DONE argument when the send to home move
 is completed.
 */

void OMMotorFunctions::moveTo(long p_pos) {
	moveTo(p_pos, false);
}


// Same as above, but if the third parameter is true (indicating a "send to..." move), the move will execute with a longer accel/decel
void OMMotorFunctions::moveTo(long p_pos, bool p_send) {

 if( currentPos() == p_pos )
     return;

 m_refresh = true;

 uint8_t thsDir  = false;
 long goToPos = currentPos() - p_pos;  //detemine how many steps needed

	// negative value means move in
	// positive direction

 if( goToPos < 0 ) {
	goToPos *= -1;
	thsDir = true;
 }

 move(thsDir, goToPos, p_send);

}

/** Set Plan Type

Variable to identify if a plan move is set and what type it is.
g_plan_type  = 0 (SMS), g_plan_type  = 1 (time lapse continuous), g_plan_type  = 2 (video continuous).

*/

void OMMotorFunctions::planType(uint8_t p_type){

	g_plan_type = p_type;
}

/** Get Plan Type

Variable to identify if a plan move is set and what type it is.
g_plan_type  = 0 (SMS), g_plan_type  = 1 (time lapse continuous), g_plan_type  = 2 (video continuous).

 @return
 A byte, representing the plan move type. g_plan_type  = 0 (SMS), g_plan_type  = 1 (time lapse continuous), g_plan_type  = 2 (video continuous).

*/

uint8_t OMMotorFunctions::planType(){

	return(OMMotorFunctions::g_plan_type);
}

/** Set Plan Travel Length (either shots or time depending on plan mode)

Sets the planned number of shots or the time of the movement. Depends on mt_plan and mtpc
to determine if it's the number of shots or time. If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

*/

void OMMotorFunctions::planTravelLength(unsigned long p_length){

    mtpc_arrive = p_length;
}

/** Get Plan Travel Lenth (either shots or time depending on plan mode)

Sets the planned number of shots or the time of the movement. Depends on mt_plan and mtpc
to determine if it's the number of shots or time. If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

 @return
 An unsigned long, representing the plan travel length in time (ms) or shots. If g_plan_type  = 0
 then it's shots, if g_plan_type  = 1 then it's time (ms).

*/

unsigned long OMMotorFunctions::planTravelLength(){

    return(mtpc_arrive);
}

/** Set Plan Acceleration Length (either shots or time depending on plan mode)

Sets the planned number of shots or the time of the acceleration. Depends on mt_plan and mtpc
to determine if it's the number of shots or time. If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

*/

void OMMotorFunctions::planAccelLength(unsigned long p_accel){

    mtpc_accel = p_accel;
}

/** Get Plan Acceleration Length (either shots or time depending on plan mode)

Sets the planned number of shots or the time of the acceleration. Depends on mt_plan and mtpc
to determine if it's the number of shots or time. If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

 @return
 An unsigned long, representing the plan interval acceleration in time (ms) or shots.
 If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

*/

unsigned long OMMotorFunctions::planAccelLength(){

    return(mtpc_accel);
}

/** Set Plan Deceleration Length (either shots or time depending on plan mode)

Sets the planned number of shots or the time of the deceleration. Depends on mt_plan and mtpc
to determine if it's the number of shots or time. If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

*/

void OMMotorFunctions::planDecelLength(unsigned long p_decel){

    mtpc_decel = p_decel;
}

/** Get Plan Deceleration Length (either shots or time depending on plan mode)

Sets the planned number of shots or the time of the deceleration. Depends on mt_plan and mtpc
to determine if it's the number of shots or time. If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

 @return
 An unsigned long, representing the plan interval deceleration in time (ms) or shots.
 If g_plan_type  = 0 then shots, g_plan_type  = 1 then time.

*/

unsigned long OMMotorFunctions::planDecelLength(){

    return(mtpc_decel);
}

/** Set Lead In (either shots or time depending on plan mode)

Sets the planned number of shots for lead in. This is the number of shots taken before the motor
begins its move.

*/

void OMMotorFunctions::planLeadIn(unsigned int p_leadIn){

    m_planLeadIn = p_leadIn;
}

/** Get Lead In (either shots or time depending on plan mode)

Gets the planned number of shots for lead in. This is the number of shots or time taken before the motor
begins its move.

 @return
 An unsigned int, representing the lead-in in shots or time (ms).
 If g_plan_type  = 0 or g_plan_type  = 1 then shots, if g_plan_type  = 2 then time.

*/

unsigned long OMMotorFunctions::planLeadIn(){

    return(m_planLeadIn);
}

/** Set Lead Out (either shots or time depending on plan mode)

Sets the planned number of shots for lead Out. This is the number of shots taken after the motor
finishes its move.

*/

void OMMotorFunctions::planLeadOut(unsigned int p_leadOut){

	m_planLeadOut = p_leadOut;
}

/** Get Lead Out (either shots or time depending on plan mode)

Gets the planned number of shots for lead out. This is the number of shots or time taken after the motor
finishes its move.

@return
An unsigned int, representing the lead-out in shots or time (ms).
If g_plan_type  = 0 or g_plan_type  = 1 then shots, if g_plan_type  = 2 then time.

*/

unsigned long OMMotorFunctions::planLeadOut(){

	return(m_planLeadOut);
}

/** Set Program Done

Set flag to indicate if the motor is done with the program move.

*/

void OMMotorFunctions::programDone(uint8_t p_En){
    m_programDone = p_En;
}

/** Get Program Done

Get flag to indicate if the motor is done with the program move.

*/

uint8_t OMMotorFunctions::programDone(){
    return(m_programDone);
}

/**Reset Program Move

Resets the start of the program move, causes the program to recalculate the move.

*/

void OMMotorFunctions::resetProgramMove(){
    mtpc_start = false;
}


/** Do a program move

Move from the current position to stopPos using the planned moved parameters.

*/

void OMMotorFunctions::programMove(){


    if( m_curPos == m_stopPos ){    //already at stop point
        programDone(true);
        return;
    }

    uint8_t thsDir  = false;
    long goToPos = m_curPos - m_stopPos;  //detemine how many steps needed

	// negative value means move in
	// positive direction
     if( goToPos < 0 ) {
        goToPos *= -1;
        thsDir = true;
     }


	 if (OMMotorFunctions::g_plan_type == 0){
        if( mtpc_start == false){
            plan(mtpc_arrive, thsDir, goToPos, mtpc_accel, mtpc_decel);
            mtpc_start = true;
        }
        planRun();
    }
	else {
        if( mtpc_start == false ) {
           // a planned continuous move has not been started...
           mtpc_start = true;
           move(thsDir, goToPos, mtpc_arrive, mtpc_accel, mtpc_decel);
       }
    }

}


/*

 This linear easing algorithm calculates the current delay cycles between steps,
 given a position in time, and whether or not this is part of a planned move, or
 a currently active move.
 
 @p_SMS:
 Boolean value indicating whether this is an SMS or a continuous move
 
 @p_move_percent: 
	A value from 0.0-1.0 indicating how far thorugh the move the motor is. Note that
	this includes only the portion of the program during which the motor is moving and
	does include lead-in or lead-outs in the calculation.

 @theFunctions: 
	A pointer to an OMMotorFunctions object. In practice, the motor that is getting 
	updated is the object getting passed into the function. It seems like this could be
	eliminated by changing _linearEasing to a non-static function, though I had some problems
	with type mismatches when I tried that.

*/


void OMMotorFunctions::_linearEasing(uint8_t p_SMS, float p_move_percent, OMMotorFunctions* theFunctions) {

	// Is this a plan move? If yes, use planned spline, otherwise use default start spline
	OMMotorFunctions::s_splineCal *thisSpline = p_SMS == true ? &theFunctions->m_splinePlanned : &theFunctions->m_splineOne;

	// Current continuous speed (steps/s) (CONT) or current SMS move steps (SMS)
	float speed_steps = 0;

	// SMS moves
	if (p_SMS){
		// Determine whether we're currently in the accel/constant/decel phase,
		// then assign move steps / speed based the current spline within the SMS moves
		if (p_move_percent <= thisSpline->accel_fraction) {
			speed_steps = thisSpline->accel_coeff * theFunctions->m_curPlanSpline;
		}
		else if (p_move_percent < thisSpline->decel_start) {
			speed_steps = thisSpline->top_speed;
		}
		else {
			// This should count down from the total decel moves to 1.
			// Need to add 1, otherwise each move count is off by one and the last move is 0, which causes things to come out funky. 
			// Basically, when that happens, what should be the first decel move ends up as the final move, which is way bad.
			speed_steps = thisSpline->decel_coeff * (thisSpline->decel_moves - (theFunctions->m_curPlanSpline - thisSpline->accel_moves - thisSpline->cruise_moves) + 1);
		}

		// Calculate the error for SMS moves (and adjust movement to whole number)
		_SMSErrorCalc(p_move_percent, speed_steps, thisSpline, theFunctions);
	}

	// Continuous moves
	else{
		// Determine whether we're currently in the accel/constant/decel phase,
		// then assign move steps / speed based upon current position within that phase
		if (p_move_percent <= thisSpline->accel_fraction)
			speed_steps = thisSpline->top_speed * (p_move_percent / thisSpline->accel_fraction);
		else if (p_move_percent < thisSpline->decel_start)
			speed_steps = thisSpline->top_speed;
		else 
			speed_steps = thisSpline->top_speed * (1.0 - ((p_move_percent - thisSpline->accel_fraction - thisSpline->cruise_fraction) / thisSpline->decel_fraction));

		// Calculate the error for continuous moves
		_contErrorCalc(p_move_percent, speed_steps, theFunctions);
	}	
}

/* 
This quadratic / inverse quadratic easing algorithm calculates the current delay cycles 
between steps, given a position in time, and whether or not this is part of a planned move, 
or a currently active move.

@p_SMS:
Boolean value indicating whether this is an SMS or a continuous move

@p_move_percent:
A value from 0.0-1.0 indicating how far thorugh the move the motor is. Note that
this includes only the portion of the program during which the motor is moving and
does include lead-in or lead-outs in the calculation.

@theFunctions:
A pointer to an OMMotorFunctions object. In practice, the motor that is getting
updated is the object getting passed into the function. It seems like this could be
eliminated by changing _quadEasing to a non-static function, though I had some problems
with type mismatches when I tried that.

*/

void OMMotorFunctions::_quadEasing(uint8_t p_SMS, float p_move_percent, OMMotorFunctions* theFunctions) {

  // Select planned or default spline
  OMMotorFunctions::s_splineCal *thisSpline = p_SMS == true ? &theFunctions->m_splinePlanned : &theFunctions->m_splineOne;
  
  // Current continuous speed (steps/s) (CONT) or current SMS move steps (SMS)
  float speed_steps = 0;	

  // use correct quad or inv. quad calculation
  speed_steps = (theFunctions->f_easeCal)(thisSpline, p_move_percent, theFunctions, p_SMS);

  // Calculate the error for continuous moves
  if( ! p_SMS )
	  _contErrorCalc(p_move_percent, speed_steps, theFunctions);

  // Calculate the error for SMS moves (and adjust movement to whole number)
  else
	  _SMSErrorCalc(p_move_percent, speed_steps, thisSpline, theFunctions);
}


/*

This function modifies a motor's error values for continuous moves.

@p_move_percent:
	A reference to a variable of the same name in the _linearEasing or _quadEasing functions. This is how far through
	the total program the motor is currently

@p_cur_spd:
	A reference to a variable of the same name in the _linearEasing or _quadEasing functions. This is the just-calculated
	speed, in steps/second of the current continuous move.

@theFunctions:
	A pointer to the current motor object, passed through from either the _linearEasing or _quadEasing function..

*/

void OMMotorFunctions::_contErrorCalc(const float& p_move_percent, float& p_cur_spd, OMMotorFunctions *theFunctions){
	
	float off_time = 1000000.0;		// 1 second worth of microseconds

	// If curSpd is very small, set to a small number to prevent dividing by 0
	if (p_cur_spd <= 0.000001){
		p_cur_spd = 0.000001;
	}
	// Otherwise, figure out how many cycles to delay after each step
	else {
		off_time = theFunctions->g_cyclesPerSpline / p_cur_spd;
	}

	// we can't track fractional off-cycles, so we need to have an error rate
	// which we can accumulate between steps
	theFunctions->m_nextOffCycles = (unsigned long)off_time;
	if (theFunctions->m_nextOffCycles < 1){
		theFunctions->m_nextOffCycles = 1;
	}

	// multiply the error by the FLOAT_TOLERANCE in order to get rid of the float varaible
	theFunctions->m_nextCycleErr = (off_time - theFunctions->m_nextOffCycles) * FLOAT_TOLERANCE;

	// worry about the fact that floats and doubles CAN actually overflow an unsigned long
	if (theFunctions->m_nextCycleErr >= FLOAT_TOLERANCE) {
		theFunctions->m_nextCycleErr = 0.0;
	}
}


/*

This function changes the current SMS move length to an integer (since the motor can't move fractional steps),
then adds the trucated decimal to motor's error value. If the error value exceeds 1, an extra step is added to the
upcoming SMS move.

@p_move_percent:
	A reference to a variable of the same name in the _linearEasing or _quadEasing functions. This is how far through
	the total program the motor is currently

@p_cur_move_steps:
	A reference to a variable of the same name in the _linearEasing or _quadEasing functions. This length, in steps,
	of the just-calculated SMS move. This is the value that is being adjusted to an whole number and whose floating
	point portion is added to the motor's error.

@thisSpline:
	A pointer to the motor's current spline, passed through from either the _linearEasing or _quadEasing function.

@theFunctions:
	A pointer to the current motor object, passed through from either the _linearEasing or _quadEasing function..

*/

void OMMotorFunctions::_SMSErrorCalc(const float& p_move_percent, float& p_cur_move_steps, s_splineCal *thisSpline, OMMotorFunctions *theFunctions){


	byte phase = 0;

	// Determine the current ramping phase
	if (p_move_percent <= thisSpline->accel_fraction)
		phase = ACCEL;
	else if (p_move_percent <= thisSpline->decel_start)
		phase = CRUISE;
	else
		phase = DECEL;

	// If the calculated move would be greater than the steps remaining in the current phase 
	// (or the movement completion equals the phase's fraction), just use the current remaining as the move length
	switch (phase){

	case ACCEL:
		if ((unsigned long)p_cur_move_steps >= thisSpline->accel_steps || p_move_percent == thisSpline->accel_fraction)
			p_cur_move_steps = thisSpline->accel_steps;
		break;

	case CRUISE:

		if ((unsigned long)p_cur_move_steps > thisSpline->cruise_steps || p_move_percent == thisSpline->decel_start)
			p_cur_move_steps = thisSpline->cruise_steps;
		break;

	case DECEL:
		if ((unsigned long)p_cur_move_steps > thisSpline->decel_steps || p_move_percent == 1.0)
			p_cur_move_steps = thisSpline->decel_steps;
		break;

	}

	// Drop the floating point from the calculated steps...
	theFunctions->m_curPlanSpd = (unsigned long)p_cur_move_steps;
	// ...then add the dropped decimal places to the error variable..
	theFunctions->m_curPlanErr += (p_cur_move_steps - theFunctions->m_curPlanSpd);
	// ... and if at least 1 full step of error has accumulated, add it to the SMS move length
	if (theFunctions->m_curPlanErr >= 1.0) {
		theFunctions->m_curPlanErr -= 1.0;
		theFunctions->m_curPlanSpd++;
	}

	// Adjust the remaining steps in the current phase
	switch (phase){

	case ACCEL:
		thisSpline->accel_steps -= theFunctions->m_curPlanSpd;

		break;

	case CRUISE:
		thisSpline->cruise_steps -= theFunctions->m_curPlanSpd;

		break;

	case DECEL:
		thisSpline->decel_steps -= theFunctions->m_curPlanSpd;

		break;
	}
}


float OMMotorFunctions::_qEaseCalc(OMMotorFunctions::s_splineCal* thisSpline, float p_move_percent, OMMotorFunctions* theFunctions, uint8_t p_SMS) {
  float curSpd;

	// For SMS moves
    if (p_SMS){
		// Accel phase
		if (p_move_percent < thisSpline->accel_fraction) {
			p_move_percent = theFunctions->m_curPlanSpline;
			curSpd = thisSpline->accel_coeff * p_move_percent * p_move_percent;		// y = ax^2, where y = curSpd, x = p_move_percent and a = steps per calculation unit
		}
		// Constant phase
		else if (p_move_percent <= thisSpline->decel_start) {
            curSpd = thisSpline->top_speed;
        } 
		// Decel phase
		else {
			p_move_percent = (theFunctions->m_curPlanSplines - (theFunctions->m_curPlanSpline - 1));
			curSpd = thisSpline->decel_coeff * p_move_percent * p_move_percent;
        }

    } 
	
	// For continuous moves
	else {
		// Accel phase
		if (p_move_percent < thisSpline->accel_fraction) {
			p_move_percent = p_move_percent / thisSpline->accel_fraction;
			curSpd = thisSpline->top_speed * p_move_percent * p_move_percent;
		}
		// Constant phase
		else if (p_move_percent < thisSpline->decel_start) {
            curSpd = thisSpline->top_speed;
        } 
		// Decel phase
		else {
			p_move_percent = 1.0 - (p_move_percent - thisSpline->accel_fraction - thisSpline->cruise_fraction) / thisSpline->decel_fraction;
			curSpd = thisSpline->top_speed * p_move_percent * p_move_percent;
        }
    }

  return(curSpd);

}

float OMMotorFunctions::_qInvCalc(OMMotorFunctions::s_splineCal* thisSpline, float p_move_percent, OMMotorFunctions* theFunctions, uint8_t p_SMS) {
  float curSpd;

	// For SMS moves
    if (p_SMS){
		// Accel phase
		if (p_move_percent < thisSpline->accel_fraction) {
			p_move_percent = theFunctions->mtpc_accel - (theFunctions->m_curPlanSpline - 1);
			curSpd = thisSpline->top_speed - thisSpline->accel_coeff * p_move_percent * p_move_percent;
		}
		// Constant phase
		else if (p_move_percent <= thisSpline->decel_start) {
            curSpd = thisSpline->top_speed;
        } 
		// Decel phase
		else {
			p_move_percent = (theFunctions->m_curPlanSpline) - (theFunctions->m_curPlanSplines - theFunctions->mtpc_decel);
			curSpd = thisSpline->top_speed - (thisSpline->decel_coeff * p_move_percent * p_move_percent);
        }
    } 
	
	// For continuous moves
	else {
		// Accel phase
		if (p_move_percent < thisSpline->accel_fraction) {
			p_move_percent = 1.0 - (p_move_percent / thisSpline->accel_fraction);
			curSpd = thisSpline->top_speed - (thisSpline->top_speed * p_move_percent * p_move_percent);
		}
		// Constant phase
		else if (p_move_percent < thisSpline->decel_start) {
            curSpd = thisSpline->top_speed;
        } 
		// Decel phase
		else {
			p_move_percent = (p_move_percent - thisSpline->accel_fraction - thisSpline->cruise_fraction) / thisSpline->decel_fraction;
			curSpd = thisSpline->top_speed - (thisSpline->top_speed * p_move_percent * p_move_percent);
        }
    }

  return(curSpd);

}


/*

	Returns the partial sum for k from 1 to n.
	i.e. 1 + 2 + 3 + ... n

*/

unsigned long OMMotorFunctions::_partialSum(unsigned long p_input){
	
	return (p_input * (p_input + 1)) / 2;

}


/*

	Returns the partial sum of squares for k^2 from 1 to n.
	i.e. 1^2 + 2^2 + 3^2 + ... n

*/

unsigned long OMMotorFunctions::_partialSumOfSquares(unsigned long p_input){

	return (p_input * (p_input + 1) * (2 * p_input + 1)) / 6;

}

 /* pre-calculate spline values to optimize execution time when requesting the
	velocity at a certain point
	
	@p_SMS:
		Boolean value indicating whether this is for a planned move (true) or manual move (false)
	
	@p_Steps:
		Total steps to travel during this overall movement
	
	@p_Travel:
		SMS mode: Total movement increments OR Continuous: total movement time in milliseconds
	
	@p_Accel:
		SMS mode: Accel movement increments OR Continuous: accel time in milliseconds

	@p_Decel:
		SMS mode: Decel movement increments OR Continuous: Decel time in milliseconds

  */

void OMMotorFunctions::_initSpline(uint8_t p_SMS, float p_Steps, unsigned long p_Travel, unsigned long p_Accel, unsigned long p_Decel) {

   OMMotorFunctions::s_splineCal *thisSpline = &m_splineOne;
   m_totalSplines = p_Travel / MS_PER_SPLINE;
   unsigned long totSplines = m_totalSplines;	// Total number of SMS moves
   
   if( p_SMS == true ) {
   	   	// work with plan parameters
   	   thisSpline = &m_splinePlanned;
   	   totSplines = m_curPlanSplines;	

	   // Save the count of physical moves for each phase
	   thisSpline->accel_moves = p_Accel;
	   thisSpline->cruise_moves = p_Travel - p_Accel - p_Decel;
	   thisSpline->decel_moves = p_Decel;
   }

   _setEasingCoeff(thisSpline);

   // pre-calculate values for spline interpolation
   thisSpline->accel_fraction = (float)p_Accel / (float)p_Travel;
   thisSpline->decel_fraction = (float)p_Decel / (float)p_Travel;
   thisSpline->cruise_fraction = 1.0 - (thisSpline->accel_fraction + thisSpline->decel_fraction);
   thisSpline->decel_start = thisSpline->accel_fraction + thisSpline->cruise_fraction;
   
   // Step count that equals continuous speed * travel time (CONT_VID OR CONT_TL) OR cruise phase movement length * SMS movements (SMS)
   float length_at_cruise = p_Steps / (thisSpline->accel_fraction / thisSpline->easing_coeff + thisSpline->cruise_fraction + thisSpline->decel_fraction / thisSpline->easing_coeff);

	// SMS mode
    if (p_SMS == true){

        unsigned long ac_movement_units = 0;		// Total of step units required to accelerate
		unsigned long dc_movement_units = 0;		// Total of step units required to decelerate


		if (m_easeType == OM_MOT_LINEAR){
			ac_movement_units = _partialSum(p_Accel);	// Partial sum equation for 1 + 2 + 3 + ... n
			dc_movement_units = _partialSum(p_Decel);
		}
		else {
			ac_movement_units = _partialSumOfSquares(p_Accel);	// Partial sum of squares equation for 1^2 + 2^2 + 3^2 + ... n^2
			dc_movement_units = _partialSumOfSquares(p_Decel);	
		}

        //calculate step interval
        if (m_easeType == OM_MOT_QUADINV){

			const float QUAD_TRAVEL_COEFF = 3.0;

            //temporarily set travel distance as if it was OM_MOT_QUAD, this is to quantize the inverse quad function
			thisSpline->accel_steps = (unsigned long)((length_at_cruise * thisSpline->accel_fraction) / QUAD_TRAVEL_COEFF);
			thisSpline->decel_steps = (unsigned long)((length_at_cruise * thisSpline->decel_fraction) / QUAD_TRAVEL_COEFF);

            //Calculate step size difference
			thisSpline->accel_coeff = (float)thisSpline->accel_steps / ((float)ac_movement_units);
			thisSpline->decel_coeff = (float)thisSpline->decel_steps / ((float)dc_movement_units);

            //recalculate distance
			thisSpline->accel_steps = (unsigned long)((length_at_cruise * thisSpline->accel_fraction) / thisSpline->easing_coeff);
			thisSpline->decel_steps = (unsigned long)((length_at_cruise * thisSpline->decel_fraction) / thisSpline->easing_coeff);

        }
		else {
            //calculate distance required for each acceleration/deceleration
			thisSpline->accel_steps = (unsigned long)((length_at_cruise * thisSpline->accel_fraction) / thisSpline->easing_coeff);
			thisSpline->decel_steps = (unsigned long)((length_at_cruise * thisSpline->decel_fraction) / thisSpline->easing_coeff);
             //Calculate step size difference
			thisSpline->accel_coeff = (float)thisSpline->accel_steps / ((float)ac_movement_units);
			thisSpline->decel_coeff = (float)thisSpline->decel_steps / ((float)dc_movement_units);

        }
        //calculate desired travel length for the cruise section
		thisSpline->cruise_steps = (unsigned long)(p_Steps - thisSpline->accel_steps - thisSpline->decel_steps);
		thisSpline->top_speed = (length_at_cruise) / ((float)totSplines);
    }

	// Continuous mode
	else {
		thisSpline->top_speed = (length_at_cruise) / ((float)totSplines);	// steps / spline (default 20ms)
    }

	// This is referenced by the motor validation routine in the NMX firmware
	m_top_speed = thisSpline->top_speed;

}

void OMMotorFunctions::_setEasingCoeff(OMMotorFunctions::s_splineCal* thisSpline) {

	// for linear easing, we always travel an average of 1/2 the distance during
	// an acceleration period that we would travel during the same cruise period
	thisSpline->easing_coeff = 2.0; 

	// for quadratic easing, we travel slightly shorter or further...
	// note: these values can likely be tuned further

	if( m_easeType == OM_MOT_QUAD )
		thisSpline->easing_coeff = 3.0;//2.9999985;
	else if( m_easeType == OM_MOT_QUADINV )
		thisSpline->easing_coeff = 1.5;//1.5000597;
}


/** getTopSpeed

	Calculates the planned move variables and report back the top speed in 16th microsteps / second that will be used during the move when
	in time lapse continuous or video continuous mode, or 16th microsteps / move during "cruising" portion of SMS mode.

*/

float OMMotorFunctions::getTopSpeed() {

	// Don't proceed with this function if this motor is running since calling _initSpline during a move could be bad
	// Return -1 to indicate error
	if (m_isRun)
		return(-1);

	// Determine the length and distance required for _initSpline()
	// compensate for any backlash
	long dist;
	if (m_backCheck == true)
		dist =abs(m_stopPos - m_startPos + m_backAdj);
	else
		dist =abs(m_stopPos - m_startPos);

	// For time lapse SMS mode
	if (OMMotorFunctions::g_plan_type == 0) {

		// Determine the total splines based upon the travel time
		m_curPlanSplines = (unsigned long)mtpc_arrive;

		// Initialize the planned move variables to calculate the m_top_speed variable
		_initSpline(true, dist, mtpc_arrive, mtpc_accel, mtpc_decel);

	}

	// For time lapse continuous and video continuous modes
	else if (OMMotorFunctions::g_plan_type == 1 || OMMotorFunctions::g_plan_type == 2) {

		// Initialize the planned move variables to calculate the m_top_speed variable
		_initSpline(false, dist, mtpc_arrive, mtpc_accel, mtpc_decel);

		const float MILLIS_P_SECOND = 1000.0;

		// Convert to steps/spline to steps/second
		m_top_speed *= MILLIS_P_SECOND / MS_PER_SPLINE;
	}

	// Convert to 16th steps/move (SMS) or 16th steps/second (continuous), based on current microstepping value
	m_top_speed *= (16.0 / (float) m_curMs);
	return(m_top_speed);
}

/** checkRefresh

Check to see if the ISR variables need to be reset and if so resets them

*/

void OMMotorFunctions::checkRefresh(){

      if( m_refresh ) {
          m_cyclesLow = 0;
          m_stepsTaken = 0;
          m_totalCyclesTaken = 0;
          m_cycleErrAccumulated = 0;
          m_refresh = false;
      }
}


void OMMotorFunctions::updateSpline(){
	
    if (splineReady == false){
        
		//If it's in continuous mode accel/decel until desired speed
        if (continuous()){
            _updateContSpeed();
        } 
		
		//Calculate next spline while not in continous mode
		else { 
            if( m_curSpline >= m_totalSplines ) {
                        // hey, look at that - we're at the end of our spline (and
                        // we haven't finished our last step either, otherwise we
                        // wouldn't get here...)

                endOfMove = true;
            } 
			
			else {

                // move to the next point in the current spline.
				float move_percent = ((float)m_curSpline + 1.0) / (float)m_totalSplines;

                // Get new off cycle timing for the next point in the spline.
				// This function is a callback to either _linearEasing or _quadEasing
				f_easeFunc(false, move_percent, this);

                endOfMove = false;
            }
        }
        splineReady = true;
    }



}




/** checkStep

Check to see if the motor needs to take a step

*/



uint8_t OMMotorFunctions::checkStep(){//uint8_t p_endOfMove){

    if (m_firstRun == true){ //run the first time the ISR is run, this populates the variables
        m_curOffCycles = m_nextOffCycles;
        m_curCycleErr = m_nextCycleErr;
        m_totalCyclesTaken = 0;
        splineReady = false;
        m_firstRun = false;
    }



    if( m_totalCyclesTaken >= g_cyclesPerSpline) {
        if(splineReady == false && !continuous()){
            updateSpline();
        }

        if (endOfMove){

            if( m_stepsTaken < m_asyncSteps ) {
              // we really should be taking any steps we're missing. (if asked to
              // move a specific distance.) positioning errors can add up over
              // multiple moves, and we shouldn't leave expected steps on the
              // cutting-room floor even if we have exceeded the destination time

              // TODO: multiple final steps will happen at the fastest step rate,
              // whether or not the motor can handle this!  We really shouldn't be
              // more than one step shy of our target, however, unless one specifies
              // a move that requires a step rate higher than the maximum step rate,
              // which is just painful.

              m_totalCyclesTaken--; //??????? why
              m_stepsTaken++;
              _updateMotorHome(1);
                // bring step pin low - this allows us to
                // hit one step per ISR run rate. (Timing is important
                // we hit minimum high pulse time in the instructions above
                // this line)
              return (true);
          }

            // we've reached the end of our anticipated time.  Go ahead and finish
            // everything up, and stop the timer. (we should only get here when a
            // last step had to be taken above.)

          m_stepsTaken = 0;
          m_curSpline = 0;
          m_totalCyclesTaken = 0;
          m_cycleErrAccumulated = 0;
          m_cyclesLow = 0;

          stop();
          return (false);

        }


        //update spline data
        m_curOffCycles = m_nextOffCycles;
        m_curCycleErr = m_nextCycleErr;
        m_curSpline++;
        m_totalCyclesTaken = 0;
        splineReady = false;


    }

    m_totalCyclesTaken++;

    //compare against the FLOAT_TOLERANCE because the error variable isn't a float and all
    //the errors should have been multipled by the FLOAT_TOLERANCE
    if( m_cycleErrAccumulated >= FLOAT_TOLERANCE ) {


            // check the error rate (fractions of a cycle that
            // were accumulated - if at least one full cycle, add
            // an additional delay cycle to get overall movement
            // timing as close to exact as is possible
            m_cycleErrAccumulated -= FLOAT_TOLERANCE;


                 // run an extra cycle low
            return (false);
    }


            // increase lowcycle counter after applying error correction, as we
            // don't want to take the next step too fast

    m_cyclesLow++;

    if( m_cyclesLow >= m_curOffCycles ) {

            // we've had enough low cycles, ok to trigger next step

            // if we hit the step count requested for this move,
            // or if we have hit the maximum stepping point,
            // stop now - don't overshoot

		boolean limitViolation = false;
          
		  if ((m_endPos < 0 && ((m_curPos <= m_endPos && m_curDir == 0) || (m_curPos >= 0 && m_curDir == 1)))){
			  if (g_debug)
				USBSerial.println("Limit violation 0");
			  limitViolation = true;
		  }
		  else if ((m_endPos > 0 && ((m_curPos >= m_endPos  && m_curDir == 1) || (m_curPos <= 0 && m_curDir == 0)) )){
			  if (g_debug)
				USBSerial.println("Limit violation 1");
			  limitViolation = true;
		  }
		  else if ((m_asyncSteps > 0 && m_stepsTaken >= m_asyncSteps)){
			  if (g_debug)
				USBSerial.println("Limit violation 2");
			  limitViolation = true;
		  }

		  if (limitViolation){

              m_stepsTaken = 0;
              m_cycleErrAccumulated = 0;
              m_cyclesLow = 0;

              stop();
          } else {

              m_cyclesLow = 0;
              m_stepsTaken++;

              _updateMotorHome(1);

               // accumulate cycle off time errors
               // once per complete cycle (fractions of
               // off cycles are included in the calculated
               // speed)

              m_cycleErrAccumulated += m_curCycleErr;

              return(true);

          }



    } // end if( cyclesLow...

    return(false);
}

/**

Sets the flag indicating whether the motor has backlash to be taken up before a program move

*/
void OMMotorFunctions::programBackCheck(uint8_t p_setFlag) {

	m_programBackCheck = p_setFlag;
	m_backCheck = p_setFlag;

}

/**

Returns whether the motor has backlash to be taken up before a program move

*/
uint8_t OMMotorFunctions::programBackCheck() {

	return(m_programBackCheck);

}


/**

Read/write functions for motor's key frame varaibles

*/

void OMMotorFunctions::keyDest(uint8_t p_which, long p_input) {

	key_frame.dest[p_which] = p_input;
}

void OMMotorFunctions::keyTime(uint8_t p_which, unsigned long p_input) {

	key_frame.time[p_which] = p_input;
}

void OMMotorFunctions::keyAccel(uint8_t p_which, unsigned long p_input) {

	key_frame.accel[p_which] = p_input;
}

void OMMotorFunctions::keyDecel(uint8_t p_which, unsigned long p_input) {

	key_frame.decel[p_which] = p_input;
}

void OMMotorFunctions::keyLead(uint8_t p_which, unsigned long p_input) {

	key_frame.lead_in[p_which] = p_input;
}

long OMMotorFunctions::keyDest(uint8_t p_which) {

	return(key_frame.dest[p_which]);
}

unsigned long OMMotorFunctions::keyTime(uint8_t p_which) {

	return(key_frame.time[p_which]);
}

unsigned long OMMotorFunctions::keyAccel(uint8_t p_which) {

	return(key_frame.accel[p_which]);
}

unsigned long OMMotorFunctions::keyDecel(uint8_t p_which) {

	return(key_frame.decel[p_which]);
}

unsigned long OMMotorFunctions::keyLead(uint8_t p_which) {

	return(key_frame.lead_in[p_which]);
}


/** void debugOutput(bool p_switch)

This method handles turning on and off the USB debug output flag.

@param
p_switch = (true, false)

*/

void OMMotorFunctions::debugOutput(bool p_state) {

	g_debug = p_state;
}

/** bool debugOutput()

This method handles retuning the state of the USB debug output flag.

*/

bool OMMotorFunctions::debugOutput() {

	return g_debug;
}

/** void units(int p_unitCode)

The unit code indicates the units in which the motion of the platform
to which the motor is connected will be measured int. This value is not
used in the firmware and is simply a reference value for master devices
for computing travel distances.

0 == inch
1 == centimeter
2 == deg
3 == steps

*/
void OMMotorFunctions::units(int p_unitCode){
	m_unitCode = p_unitCode;
}

/** void units()

Returns the unit code for this motor. 

The unit code indicates the units in which the motion of the platform
to which the motor is connected will be measured int. This value is not
used in the firmware and is simply a reference value for master devices
for computing travel distances.

0 == inch
1 == centimeter
2 == deg
3 == steps

*/
int OMMotorFunctions::units(){
	return m_unitCode;
}
