
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

#include "OMMotor.h"
#include "TimerOne.h"

// initialize static members



void(*OMMotor::f_motSignal)(uint8_t) = 0;
void(*OMMotor::f_easeFunc)(bool, float) = 0;
float(*OMMotor::f_easeCal)(OMMotor::s_splineCal*, float) = 0;

unsigned long OMMotor::m_asyncSteps = 0;
float OMMotor::m_contSpd = 100.0;
volatile unsigned long OMMotor::m_stepsMoved = 0;
unsigned long OMMotor::m_Steps = 0;
unsigned long OMMotor::m_totalSteps = 0;
volatile unsigned long OMMotor::m_curSpline = 0;
unsigned long OMMotor::m_totalSplines = 0;
volatile unsigned long OMMotor::m_curOffCycles = 0;
unsigned int OMMotor::m_curSampleRate = 200;
unsigned int OMMotor::m_cyclesPerSpline = 5;
float OMMotor::m_curCycleErr = 0.0;
volatile long OMMotor::m_homePos = 0;

unsigned long OMMotor::m_curPlanSpd = 0;
float OMMotor::m_curPlanErr = 0.0;
unsigned long OMMotor::m_curPlanSplines = 0;
unsigned long OMMotor::m_curPlanSpline = 0;
bool OMMotor::m_planDir = false;

bool OMMotor::m_asyncWasdir = false;
bool OMMotor::m_curDir = false;
bool OMMotor::m_backCheck = false;
bool OMMotor::m_motEn = true;
bool OMMotor::m_motCont = false;
bool OMMotor::m_motSleep = false;
bool OMMotor::m_isRun = false;
bool OMMotor::m_clearISR = false;
bool OMMotor::m_refresh = false;

uint8_t OMMotor::m_curMs = 1;
uint8_t OMMotor::m_backAdj = 0;
uint8_t OMMotor::m_easeType = OM_MOT_LINEAR;

OMMotor::s_splineCal OMMotor::m_splineOne = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
OMMotor::s_splineCal OMMotor::m_splinePlanned = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

/** Constructor

  Creates a new instance of the OMMotor class. Sets all control outputs to the
  output state.

  */

OMMotor::OMMotor() {
        // set pin states
    pinMode(OM_MOT_DSTEP, OUTPUT);
    pinMode(OM_MOT_DDIR, OUTPUT);
    pinMode(OM_MOT_DSLP, OUTPUT);
    pinMode(OM_MOT_DMS1, OUTPUT);
    pinMode(OM_MOT_DMS2, OUTPUT);
    pinMode(OM_MOT_DMS3, OUTPUT);

    m_calcMove = false;
    m_maxSpeed = 1000;

    f_easeFunc = _linearEasing;
    f_easeCal = _qEaseCalc;
}


/** Get MicroSteps

  Returns the current microstep setting.

  @return
  The current microstep setting.
  */

uint8_t OMMotor::ms() {
    return(m_curMs);
}

/** Set MicroSteps

  Sets the current microstep setting.  You may choose from the following
  values:

  1, 2, 4, 8, or 16

  @param p_Div
  Microstep setting value

  */


void OMMotor::ms( uint8_t p_Div ) {

  bool s1 = false;
  bool s2 = false;
  bool s3 = false;

  uint8_t wasMs = m_curMs;
  m_curMs       = p_Div;

  switch( p_Div ) {
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
      break;
  }

  digitalWrite(OM_MOT_DMS1, s1);
  digitalWrite(OM_MOT_DMS2, s2);
  digitalWrite(OM_MOT_DMS3, s3);

        // adjust marker for home!

    if( wasMs != m_curMs ) {
        if( wasMs > m_curMs )
            m_homePos /= (wasMs / m_curMs);
        else
            m_homePos *= (m_curMs / wasMs);
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


bool OMMotor::dir() {
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

void OMMotor::dir( bool p_Dir ) {


  // only do anything if we're changing directions

  if( p_Dir != m_curDir ) {
   digitalWrite( OM_MOT_DDIR, p_Dir);
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
#include "OMMotor.h"

OMMotor Motor = OMMotor();

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


void OMMotor::setHandler( void(*p_Func)(uint8_t) ) {
    f_motSignal = p_Func;
}

void OMMotor::_fireCallback(uint8_t p_Param) {
    if( f_motSignal != 0 ) {
        f_motSignal(p_Param);
    }
}



/** Get Backlash Compensation Amount

 Returns the currently set backlash compensation amount.

 @return
 Backlash compensation amount
 */

uint8_t OMMotor::backlash() {
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

void OMMotor::backlash(uint8_t p_Back) {
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


void OMMotor::enable(bool p_En) {
    m_motEn = p_En;
}

/** Get Enable Flag Value

  Returns the current enable flag value.

  @return
  Enabled (true) or Disabled (false);

  */

bool OMMotor::enable() {
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

void OMMotor::sleep(bool p_En) {
    m_motSleep = p_En;

        // turn the sleep line off if sleep disabled, otherwise
        // turn sleep line on

    bool doSleep = p_En ? OM_MOT_SSTATE : ! OM_MOT_SSTATE;
    digitalWrite(OM_MOT_DSLP, doSleep);
}

/** Get Sleep Flag Value


  Returns the current value of the sleep between moves flag.

  @return
  Enabled (true), Disabled (false()

  */

bool OMMotor::sleep() {
    return(m_motSleep);
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
#include "OMMotor.h"

OMMotor Motor = OMMotor();

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

void OMMotor::continuous(bool p_En) {
    m_motCont = p_En;
}

/** Get Continuous Motion Value

  Returns the current continuous motion value.

  @return
  Enabled (true), or Disabled (false)
  */


bool OMMotor::continuous() {
    return(m_motCont);
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

void OMMotor::contSpeed(float p_Speed) {

    if( p_Speed > maxStepRate() )
        return;

    m_contSpd = p_Speed;

    float curSpd = m_contSpd / 1000;

         // figure out how many cycles we delay after each step
    float off_time = m_cyclesPerSpline / curSpd;

    m_curOffCycles = (unsigned long) off_time;
    m_curCycleErr = off_time - (unsigned long) off_time;
}

/** Get Continuous Motion Speed

  Returns the current continuous motion speed.

  @return
  Steps per second
  */

unsigned int OMMotor::contSpeed() {
    return(m_contSpd);
}

/** Motor Running?

  Returns whether or not the motor is currently moving.

  @return
  Running (true) or Stopped (false)
  */

bool OMMotor::running() {
    return(m_isRun);
}

/** Set Steps per Move

 Sets the current steps per move for non-continuous motion moves.

 This method sets a default steps per move so that one may use move() with
 no arguments.

 @param p_Steps
 Steps Per Move
 */

void OMMotor::steps(unsigned long p_Steps) {
    m_Steps = p_Steps;
}

/** Get Steps Per Move

  Returns the current steps per move for non-continuous motion moves.

  @return
  Steps Per Move
  */

unsigned long OMMotor::steps() {
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

void OMMotor::maxSteps(unsigned long p_Steps) {
    m_totalSteps = p_Steps;
}

/** Get Maximum Steps

  Returns the current maximum step count.

  @return
  Maximum Steps to Move
  */

unsigned long OMMotor::maxSteps() {
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

  static bool dir = true;

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

void OMMotor::maxSpeed(unsigned int p_Speed) {
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

unsigned int OMMotor::maxSpeed() {
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

void OMMotor::maxStepRate( unsigned int p_Rate ) {


    if(  p_Rate != 10000 && p_Rate != 5000 && p_Rate != 4000 && p_Rate != 2000 && p_Rate != 1000 )
        return;

        // convert from steps per second, to uSecond periods
    m_curSampleRate = 1000000 / (unsigned long) p_Rate;

        // timeslices are in 1mS, so how many
        // stepping samples are there for one 1mS?  This is what
        // limits us to a minimum of 500 steps per second rate

    m_cyclesPerSpline = 1000 / m_curSampleRate;

}

/** Get Maximum Stepping Rate

 Returns the current maximum stepping rate in steps per second.

 @return
 Maximum rate in steps per second
 */

unsigned int OMMotor::maxStepRate() {
    return( (long) 1000000 / (long) m_curSampleRate );
}


/** Get Steps Moved

 Returns the count of steps moved since the last clear() call.

 @return
 Steps moved
 */

unsigned long OMMotor::stepsMoved() {
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
void OMMotor::plan(unsigned long p_Shots, bool p_Dir, unsigned long p_Dist, unsigned long p_Accel, unsigned long p_Decel) {

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
 #include "OMMotor.h":

 OMMotor Motor = OMMotor();

 volatile boolean okNext = true;

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

void OMMotor::planRun() {

            // if motor is disabled, do nothing
    if( ! enable() || ( maxSteps() > 0 && stepsMoved() >= maxSteps() ) || m_curPlanSpline >= m_curPlanSplines ) {
        _fireCallback(OM_MOT_DONE);
        return;
    }


    m_curPlanSpline++;

        // get steps to move for next movement
    float tmPos = (float) m_curPlanSpline / (float) m_curPlanSplines;

    f_easeFunc(true, tmPos); // sets m_curPlanSpd
    move(m_planDir, m_curPlanSpd);

}

/** Reverse the Last Iteration of the Current Plan

 Backs up one plan iteration by moving the same distance as the previous
 plan interation in the opposite direction

 */

void OMMotor::planReverse() {
    if( ! enable() || m_curPlanSpline == 0 ) {
        _fireCallback(OM_MOT_DONE);
        return;
    }


    // get steps to move for last movement (m_curPlanSpline is not changed)

    float tmPos = (float) m_curPlanSpline / (float) m_curPlanSplines;

    f_easeFunc(true, tmPos); // sets m_curPlanSpd
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

void OMMotor::move(bool p_Dir, unsigned long p_Dist, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel) {

        // if motor is disabled, do nothing
    if( ! enable() || ( maxSteps() > 0 && stepsMoved() >= maxSteps() ) ) {
        _fireCallback(OM_MOT_DONE);
        return;
    }


    m_totalSplines = p_Time;

        // prep spline variables
    _initSpline(false, p_Dist, p_Time, p_Accel, p_Decel);

        // we need to initialize the first spline point
    m_curSpline = 1;
    float tmPos = (float) m_curSpline / (float) m_totalSplines;

    f_easeFunc(false, tmPos);

    m_calcMove = true;

        // move with the spline parameters set
    move(p_Dir, p_Dist);
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

void OMMotor::move() {
    move(dir(), steps());
}

/** Move Now

 Immediately executes a move using specified parameters.

 This method will execute either a continuous or non-continuous move
 using the values passed as arguments.  If the direction of the motor
 set by dir() is different than the value passed as an argument,
 dir() will return the value passed here as an argument until the move
 is completed or stopped.

 For continuous motion, you must specify 0 as the number of steps.  To
 limit continuous motion travel, use maxSteps() instead.

 For non-continuous motion, this form of move allows you to only specify the
 destination and direction, and the motor will move as quickly as possible to
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

void OMMotor::move(bool p_Dir, unsigned long p_Steps) {


        // if motor is disabled, do nothing
   if( ! enable() || ( maxSteps() > 0 && stepsMoved() >= maxSteps() ) ) {
        _fireCallback(OM_MOT_DONE);
        return;
   }

    // note: the check on p_Steps is required
    // to allow manual moves that are not continuous
    // when motor is set in continuous mode

   if( p_Steps == 0 && continuous() ) {
       // continuous motion mode
       if( ! running() ) {
               m_asyncSteps = 0;
               _stepsAsync(p_Dir, (unsigned long) 0);
       }

       _fireCallback(OM_MOT_BEGIN);
       return;
   }

    // continuous allows higher stepping rate
    // so clamp back down if not doing a continuous
    // move

    if( maxStepRate() > 5000 )
       maxStepRate(5000);

    // check for backlash compensation
    if( m_backCheck == true ) {
       p_Steps += backlash();
       m_backCheck = false;
    }

    // not in continuous motion mode
   // nothing to do
   if( p_Steps == 0 ) {
       _fireCallback(OM_MOT_DONE);
       return;
   }

   if( ! m_calcMove ) {

        // this move was not specified with a time to accomplish, so accomplish
        // in minimal amount of time possible

         // determine how far we'd move in our acceleration and deceleration
         // times for moves that must operate at maximum speed. We accelerate
         // and decel over 200 steps each, or 1/4 of the total steps, whichever is less

            // set travel const (.travel) here before attempting to use it

        _setTravelConst(&m_splineOne);

        unsigned int mSpeed = ( maxStepRate() > maxSpeed() ) ? maxSpeed() : maxStepRate();

        float rampSteps = (200 > (p_Steps / 4)) ? (p_Steps / 4) : 200;

        rampSteps *= 2;

        float crTm = ((p_Steps - rampSteps) / mSpeed) * 1000;
        float adTm = ((rampSteps / mSpeed) * 1000) * m_splineOne.travel;

        float mvMS = (crTm + adTm) + 1.0;

            // take a minimum of 50ms to make the move - to prevent over-speeding
            // and getting goofy.
        if( mvMS < 50.0 ) {
            mvMS = 50.0;
        }


            // one spline point per ms, like normal.
        m_totalSplines = (unsigned long) mvMS;

            // prep spline variables
        _initSpline(false, p_Steps, mvMS, adTm/2, adTm/2);

            // we need to initialize the first spline point
        m_curSpline = 1;
        float tmPos = (float) m_curSpline / (float) m_totalSplines;
            // calculate the first running speed
        f_easeFunc(false, tmPos);
   }

    // limit step cycle using this control
    // value, without mucking with max steps

   m_asyncSteps = p_Steps;

    // re-set called from five-argument flag

   m_calcMove = false;

   _stepsAsync(p_Dir, p_Steps);
   _fireCallback(OM_MOT_MOVING);

}

/** Stop Immediately

  Immediately stops any movement in progress.

  Triggers the callback with the OM_MOT_DONE argument.
  */

void OMMotor::stop() {

        // set motors not moving in async mode
      m_isRun = false;
      m_refresh = true;

        // set sleep state for drivers if needed
      if( sleep() )
        digitalWrite(OM_MOT_DSLP, OM_MOT_SSTATE);

        // let go of interrupt cycle
      Timer1.detachInterrupt();

        // re-set to original direction
      dir( m_asyncWasdir );

        // signal completion
      _fireCallback(OM_MOT_DONE);

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

void OMMotor::easing(uint8_t p_easeType) {

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

/** Clear Steps Moved

  Clears the count of steps moved.  Additionally, if the motor is currently
  running, any movement will be immediately stopped, and the callback will
  be executed with the OM_MOT_DONE argument.

  This method does NOT reset the home location for the motor.  It does, however,
  reset any plan currently executing or previously executed.

  */

void OMMotor::clear() {

        // stop if currently running

    if( running() )
        stop();

    m_stepsMoved = 0;

        // wipe out plan
    m_curPlanSpd = 0;
    m_curPlanErr = 0.0;
    m_curPlanSplines = 0;
    m_curPlanSpline = 0;
}


 // execute an async move, when specifying a direction
void OMMotor::_stepsAsync( bool p_Dir, unsigned long p_Steps ) {

    // save currently set motor direction
   m_asyncWasdir = dir();
   dir( p_Dir );

     // is async control not already running?
     if( ! running() ) {

         m_asyncSteps = p_Steps;

          // bring sleep pin to non-sleeping state if
          // motor kill enabled

        if( sleep() )   {
          digitalWrite(OM_MOT_DSLP, !OM_MOT_SSTATE);
            // we don't want to start moving before its safe
            // to do so
          delay(OM_MOT_SAFE);
        }

        // set motors moving
        m_isRun = true;

        // initialize timer1 interrupt

       Timer1.initialize(m_curSampleRate);
       Timer1.attachInterrupt(_runISR);

     } // end if not running

}


 // our ISR that is run every time Timer1 triggers, controls stepping
 // of the motor

void OMMotor::_runISR() {


    // digital pin 9
  static float cycleErrAccumulated = 0.0;
  static unsigned long cyclesLow = 0;
  static unsigned long stepsTaken = 0;
  static unsigned long totalCyclesTaken = 0;

  if( m_refresh ) {
      cyclesLow = 0;
      stepsTaken = 0;
      totalCyclesTaken = 0;
      cycleErrAccumulated = 0.0;
      m_refresh = false;
  }

  if( m_asyncSteps > 0 && totalCyclesTaken >= m_cyclesPerSpline ) {

      // we are ready for the next point in the spline,
      // run speed calculations

    if( m_curSpline == m_totalSplines ) {
                // hey, look at that - we're at the end of our spline (and
                // we haven't finished our last step either, otherwise we
                // wouldn't get here...)

      if( stepsTaken < m_asyncSteps ) {
          // we really should be taking any steps we're missing. (if asked to
          // move a specific distance.) positioning errors can add up over
          // multiple moves, and we shouldn't leave expected steps on the
          // cutting-room floor even if we have exceeded the destination time

          // TODO: multiple final steps will happen at the fastest step rate,
          // whether or not the motor can handle this!  We really shouldn't be
          // more than one step shy of our target, however, unless one specifies
          // a move that requires a step rate higher than the maximum step rate,
          // which is just painful.

                // directly access register to bring pin high
          OM_MOT_STPREG |= (1 << OM_MOT_STPFLAG);

          totalCyclesTaken--; //??????? why
          stepsTaken++;
          _updateMotorHome( (uint8_t) 1);
            // bring step pin low - this allows us to
            // hit one step per ISR run rate. (Timing is important
            // we hit minimum high pulse time in the instructions above
            // this line)
          OM_MOT_STPREG &= (B11111111 ^ (1 << OM_MOT_STPFLAG));
          return;
      }

        // we've reached the end of our anticipated time.  Go ahead and finish
        // everything up, and stop the timer. (we should only get here when a
        // last step had to be taken above.)

      stepsTaken = 0;
      m_curSpline = 0;
      totalCyclesTaken = 0;
      cycleErrAccumulated = 0.0;
      cyclesLow = 0;

      stop();
      return;
    } // end if( m_curSpline...

        // move to the next point in the current spline.
    m_curSpline++;
    float tmPos = (float) m_curSpline / (float) m_totalSplines;

        // get new off cycle timing for the next point in the spline
    f_easeFunc(false, tmPos); //goes to a new function?
    totalCyclesTaken = 0;



        // we don't stop here, as we still need to check the off-time
        // between steps, based on our current spline point

  } // end if( m_asyncSteps > 0 && totalCyclesTaken...


  totalCyclesTaken++;

  if( cycleErrAccumulated >= 1.0 ) {


        // check the error rate (fractions of a cycle that
        // were accumulated - if at least one full cycle, add
        // an additional delay cycle to get overall movement
        // timing as close to exact as is possible

        cycleErrAccumulated -= 1.0;
             // run an extra cycle low
        return;
  }

        // increase lowcycle counter after applying error correction, as we
        // don't want to take the next step too fast

  cyclesLow++;

  if( cyclesLow >= m_curOffCycles ) {
        // we've had enough low cycles, ok to trigger next step

        // set pin to trigger step
        // digital pin 9 on atmega328p
      OM_MOT_STPREG |= (1 << OM_MOT_STPFLAG);

      cyclesLow = 0;
      stepsTaken++;

      _updateMotorHome(1);

       // accumulate cycle off time errors
       // once per complete cycle (fractions of
       // off cycles are included in the calculated
       // speed)

      cycleErrAccumulated += m_curCycleErr;

        // if we hit the step count requested for this move,
        // or if we have hit the maximum stepping point,
        // stop now - don't overshoot
      if( (m_totalSteps > 0 && m_stepsMoved >= m_totalSteps) || (m_asyncSteps > 0 && stepsTaken >= m_asyncSteps) ) {
          stepsTaken = 0;
          cycleErrAccumulated = 0.0;
          cyclesLow = 0;
          stop();
      }

       // bring step pin low again, the driver will register
       // a pulse of only a few uS, so we can perform
       // a complete step in a single timing cycle,
       // allowing us to hit a speed of one step per
       // timing cycle

       OM_MOT_STPREG &= (B11111111 ^ (1 << OM_MOT_STPFLAG));


  } // end if( cyclesLow...


}



void OMMotor::_updateMotorHome(int p_Steps) {

    // update total steps moved counter

  m_stepsMoved += p_Steps;

    // update relationship to home --
    // if going in positive direction, increase distance
    // - otherwise decrease distance

  if( m_curDir == 0 )
      p_Steps *= -1;

  m_homePos += p_Steps;

}

/** Set Home

 Sets the current position of the motor as the home position.

 */

void OMMotor::homeSet() {
    m_homePos = 0;
}

/** Distance from Home

 Returns the number of steps and direction back to the home position.

 @return
 A signed long, representing the distance and direction from home.  A negative
 number represents the motor is that many steps in the false direction from home,
 whereas positive represents that many steps in the true ddirection from home.
*/

long OMMotor::homeDistance() {
    return(m_homePos);
}

/** Send Motor Home

 Send motor home immediately.

 This method will not stop any ongoing move, but it will immediately send
 the motor back to its home position, and will result in an aborted move.

 Like other moves, this move is non-blocking and will trigger the callback
 to be executed with the OM_MOT_DONE argument when the send to home move
 is completed.
 */

void OMMotor::home() {

 if( homeDistance() == 0 )
     return;

 m_refresh = true;

 bool thsDir  = false;
 long goToPos = m_homePos;

    // negative value means move in
    // positive direction

 if( goToPos < 0 ) {
    goToPos *= -1;
    thsDir = true;
 }


    // there is no need to re-set m_homePos, as travelling in the
    // specified direction will correct the recorded steps

 move(thsDir, goToPos);

}

/*

 This linear easing algorithm calculates the current delay cycles between steps,
 given a position in time, and whether or not this is part of a planned move, or
 a currently active move.

*/


void OMMotor::_linearEasing(bool p_Plan, float p_tmPos) {

  OMMotor::s_splineCal *thisSpline = p_Plan == true ? &m_splinePlanned : &m_splineOne;

  float curSpd;

  if( p_tmPos <= thisSpline->acTm ) {
    curSpd = thisSpline->topSpeed * ( p_tmPos / thisSpline->acTm);
  }
  else if( p_tmPos <= thisSpline->dcStart ) {
    curSpd = thisSpline->topSpeed;
  }
  else {
    curSpd = thisSpline->topSpeed * (1.0 - ((p_tmPos - thisSpline->acTm - thisSpline->crTm)/ thisSpline->dcTm));
  }

  if( ! p_Plan ) {
      // we only do this for non-planned (i.e. real-time) moves


     // figure out how many cycles we delay after each step
     float off_time = m_cyclesPerSpline / curSpd;

     // we can't track fractional off-cycles, so we need to have an error rate
     // which we can accumulate between steps

     m_curOffCycles = (unsigned long) off_time;
     m_curCycleErr = off_time - (unsigned long) off_time;

      // worry about the fact that floats and doubles CAN actually overflow an unsigned long
      if( m_curCycleErr > 1.0 ) {
          m_curCycleErr = 0.0;
      }


  }
  else {
        // for planned shoot-move-shoot calculations, we need whole
        // steps per shot
    m_curPlanSpd = (unsigned long) curSpd;
        // of course, this tends to leave some fractional steps on the floor
    m_curPlanErr += curSpd - (unsigned long) curSpd;

        // .. so we compensate for the error to catch up...
    if( m_curPlanErr >= 1.0 ) {
        m_curPlanErr -= 1.0;
        m_curPlanSpd++;
    }

    // TODO: correct for one step left behind in some planned calculations
    // (ending one step short b/c not enough error accumulates)
  }


}

/* Quadratic easing for planned and non-planned moves */

void OMMotor::_quadEasing(bool p_Plan, float p_tmPos) {

  OMMotor::s_splineCal *thisSpline = p_Plan == true ? &m_splinePlanned : &m_splineOne;

    // use correct quad or inv. quad calculation
  float curSpd = f_easeCal(thisSpline, p_tmPos);

  if( ! p_Plan ) {

      // we only do this for non-planned (i.e. real-time) moves

     // figure out how many cycles we delay after each step
     float off_time = m_cyclesPerSpline / curSpd;

     // we can't track fractional off-cycles, so we need to have an error rate
     // which we can accumulate between steps

     m_curOffCycles = (unsigned long) off_time;
     m_curCycleErr = off_time - m_curOffCycles;

      // worry about the fact that floats and doubles CAN actually overflow an unsigned long
     if( m_curCycleErr > 1.0 ) {
        m_curCycleErr = 0.0;
     }

  }
  else {
        // for planned shoot-move-shoot calculations, we need whole
        // steps per shot
    m_curPlanSpd = (unsigned long) curSpd;
        // of course, this tends to leave some fractional steps on the floor
    m_curPlanErr += curSpd - (unsigned long) curSpd;

        // .. so we compensate for the error to catch up...
    if( m_curPlanErr >= 1.0 ) {
        m_curPlanErr -= 1.0;
        m_curPlanSpd++;
    }

    // TODO: correct for one step left behind in some planned calculations
    // (ending one step short b/c not enough error accumulates)
  }
}


float OMMotor::_qEaseCalc(OMMotor::s_splineCal* thisSpline, float p_tmPos) {
  float curSpd;

  if( p_tmPos < thisSpline->acTm ) {
    p_tmPos = p_tmPos / thisSpline->acTm;
    curSpd = thisSpline->topSpeed * p_tmPos * p_tmPos;
  }
  else if( p_tmPos <= thisSpline->dcStart ) {
    curSpd = thisSpline->topSpeed;
  }
  else {
    p_tmPos = 1.0 - (p_tmPos - thisSpline->acTm - thisSpline->crTm) / thisSpline->dcTm;
    curSpd = thisSpline->topSpeed * p_tmPos * p_tmPos;
  }

  return(curSpd);

}

float OMMotor::_qInvCalc(OMMotor::s_splineCal* thisSpline, float p_tmPos) {
  float curSpd;

  if( p_tmPos < thisSpline->acTm ) {
    p_tmPos = 1.0 - (p_tmPos / thisSpline->acTm);
    curSpd = thisSpline->topSpeed - (thisSpline->topSpeed * p_tmPos * p_tmPos);
  }
  else if( p_tmPos <= thisSpline->dcStart ) {
    curSpd = thisSpline->topSpeed;
  }
  else {
    p_tmPos = (p_tmPos - thisSpline->acTm - thisSpline->crTm) / thisSpline->dcTm;
    curSpd = thisSpline->topSpeed - (thisSpline->topSpeed * p_tmPos * p_tmPos);
  }

  return(curSpd);

}

 // pre-calculate spline values to optimize execution time when requesting the
 // velocity at a certain point

void OMMotor::_initSpline(bool p_Plan, float p_Steps, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel) {

   OMMotor::s_splineCal *thisSpline = &m_splineOne;
   unsigned long totSplines = m_totalSplines;

   if( p_Plan == true ) {
        // work with plan parameters
       thisSpline = &m_splinePlanned;
       totSplines = m_curPlanSplines;
   }

   _setTravelConst(thisSpline);

    // pre-calculate values for spline interpolation
   thisSpline->acTm = (float) p_Accel / (float) p_Time;
   thisSpline->dcTm = (float) p_Decel / (float) p_Time;
   thisSpline->crTm = 1.0 - (thisSpline->acTm + thisSpline->dcTm);
   thisSpline->dcStart = thisSpline->acTm + thisSpline->crTm;

   float velocity = p_Steps / (thisSpline->acTm/thisSpline->travel + thisSpline->crTm + thisSpline->dcTm/thisSpline->travel);
   thisSpline->topSpeed = (velocity * thisSpline->crTm) / ( totSplines * thisSpline->crTm );


}

void OMMotor::_setTravelConst(OMMotor::s_splineCal* thisSpline) {

             // for linear easing, we always travel an average of 1/2 the distance during
        // an acceleration period that we would travel during the same cruise period
   thisSpline->travel = 2.0;

        // for quadratic easing, we travel slightly shorter or further...
        // note: these values can likely be tuned further

   if( m_easeType == OM_MOT_QUAD )
       thisSpline->travel = 2.9999985;
   else if( m_easeType == OM_MOT_QUADINV )
       thisSpline->travel = 1.5000597;
}


