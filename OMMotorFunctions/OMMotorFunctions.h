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


#ifndef	OM_MOTOR_FUNCTIONS_H
#define OM_MOTOR_FUNCTIONS_H

#include <inttypes.h>
 // must load before wconstants to prevent issues
#include <math.h>
#include "Arduino.h"

#define OM_MOT_SSTATE	HIGH
#define OM_MOT_SAFE	10

#define OM_MOT_DONE 	 1
#define OM_MOT_MOVING 	 2
#define OM_MOT_BEGIN	 3

#define OM_MOT_LINEAR 	 4
#define OM_MOT_QUAD	     5
#define OM_MOT_QUADINV	 6

#define OM_MOT_DONE_PLAN 7

#define MS_PER_SPLINE    20

#define FLOAT_TOLERANCE  1000

#define ACCEL 0
#define CRUISE 1
#define DECEL 2

#define INCH 0
#define CM 1
#define DEG 2
#define STEPS 3


/**
  @page ommotion Motion Capabilities

  The Motor Manager class is capable of automatically making complex moves on
  your behalf.  It supports both linear and quadratic easing, with specified
  acceleration, deceleration, and arrival times.

  @section movedefs Defining A Move

  A move can be defined in a number of ways, but the most interesting move is
  one which has five attributes specified:

   <ul>
    <li>Distance to move</li>
    <li>Time to arrive at final point</li>
    <li>Time to accelerate to full speed</li>
    <li>Time to decelerate to a stop</li>
   </ul>

  Moves such as these allow you to map out complex motions without having to
  concern yourself as to how they are achieved.  The OMMotorFunctions library automatically
  performs all calculations so that it arrives at the specified destination at
  exactly the specified time, with the exact accel and decel times you specify.

  To add additional control over these complex moves, the Motor Manager class
  also provides several types of easing, to control how acceleration and
  deceleration occur in these moves.  For more information on the easing algorithms,
  see the Easing section below.


  While this class allows you to specify very complex moves, for some purposes
  you may find it advantageous to use the asynchronous motor driving without
  concerning yourself with acceleration, deceleration, or easing.  In this case,
  a two-argument form of OMMotorFunctions::move() is provided which only requires you to
  define a direction and a number of steps.   When only a direction and steps
  are provided, the class uses the linear easing algorithm
  and moves the motor at the specified maximum speed.  In the case of simple
  moves such as this, the acceleration and deceleration are fixed at a rate of
  25% of the total time it takes to make the move.

  For all moves, there is no error checking to see if you've specified a speed
  or parameters for a move that cannot actually be achieved.  The maximum speed
  of movement is controlled by OMMotorFunctions::maxStepRate() and OMMotorFunctions::maxSpeed(),
  and not direct observation of the motor.  Therefore, it is possible to set
  speeds which cannot be achieved by your specific motor.  For more information
  on how to limit the speed of your motor or control the amount of CPU time
  spent running the motor, see the \ref movespeed "Timing and Speed Controls"
  section below.

  @section movease Easing

  The Motor Manager class applies easing to all moves, allowing for control over
  how acceleration and deceleration are applied.  There are two primary algorithms
  supported for easing: linear and quadratic.  All easing methods can be applied
  at the maximum step rate of 5,000 steps/second.

  Linear easing allows for quick and highly accurate move calculations.  The
  position of the motor in space follows a smooth, natural curve - however the
  acceleration profile is entirely linear.  This can result in some moves, especially
  real-time video moves appearing "robotic" or "mechanical" in the output video.
  However, some moves will benefit from this method, especially very precise moves
  as linear easing has no error during a real-time move, whereas quadratic easing
  has a very small error rate which becomes more apparent in shorter moves.
  (This error rate is usually less than 0.05 steps past the expected position,
  meaning it has little impact on most moves.)

  Quadratic easing provides an acceleration curve which is non-linear, and can
  provide smooth transitions in speed - with a fluid curve that appears more
  like that which a human can produce rather than the linear change in speed
  produced by a computer.

  The following diagrams show the difference between linear and quadratic easing
  applied to the same move of 2,000 steps over 500 seconds, with 100 seconds in
  acceleration and 200 in deceleration:

   <img src="chart_mposprofile.png">

   <img src="chart_mspeedprofile.png">

  As we can see from the above graphs, both moves arrive at the same point and
  at the same time and reach their top speeds at the same time, but the
  quadratic acceleration profile results in a higher top speed where the
  acceleration begins slowly and then increases as it nears the crusing speed.
  Quadratic deceleration, likewise, starts decelerating slowly and increases as
  the move reaches a stop.

  While normal quadratic easing provides us a period where acceleration increases
  as we reach the top speed, there are cases where it would be advantageous to
  do the inverse - that is, invert the acceleration curve so that it starts
  at the maximum acceleration rate and the rate of acceleration decreases as the
  move nears its cruising speed. This easing mode is provided as the inverse
  quadratic easing mode.

  The following diagrams illustrate the difference between the two forms of easing
  and their effects on the placement and speed of the motor over time:


   <img src="chart_miqpos.png">

   <img src="chart_miqspeed.png">

   Here we find that the acceleration begins so sharply that the maximum speed
   is reduced.

   Using an inverse quadratic easing algorithm with acceleration and deceleration
   periods nearest to dead-center in time will result in an approximation of the
   smoothstep algorithm.  E.g.: specifying a move of 2000 steps, over 500 seconds
   with 249 seconds spent accelerating and 249 seconds spent decelerating, results
   in the following speed profile:

   <img src="chart_mqs.png">

   @section seteasing Setting the Easing Mode

   To set the type of easing applied to subsequent moves, use the OMMotorFunctions::easing()
   method.  This method accepts a single argument which should be one of the
   following constants:

   <ul>
    <li>OM_MOT_LINEAR</li>
    <li>OM_MOT_QUAD</li>
    <li>OM_MOT_QUADINV</li>
   </ul>

   Selecting between linear, quadratic, and inverse quadratic easing.

   It is not possible to change the easing mode while a move is in progress,
   and you are advised to abandon any planned move before changing the easing
   mode.

   @section planmoves Planned and Interleaved Moves

   In addition to performing complex single moves, the OMMotorFunctions class also
   supports performing the same complex moves in an interleaved fashion, such as
   with shoot-move-shoot systems, where the motor must move only between some
   activity, but you want to end at a certain position while having followed
   a natural path.  Using the OMMotorFunctions::plan() and OMMotorFunctions::planRun() methods, you can plan out
   a complex move across many intervals, and then execute each stage of the move
   as needed.

   When planning an interleaved move, the currently selected easing algorithm
   is applied to the move as a whole, and divided across the specified intervals
   such that only whole steps are taken at each execution interval.  For this
   reason, planned moves will almost always be less smooth than continuous moves
   unless there are an equivalent number of intervals in the planned move as there
   are step cycles in the continuous move.  For smoothest and most natural movement,
   continuous is preferred.

   The following chart shows how a hypothetical move is calculated across
   100 intervals, showing the distance traveled at each interval point:

   <img src="chart_mplan.png">

   Of course, each interval must have a whole number of steps, while reaching
   the destination given the planned parameters may require us to take
   fractional steps at each interval.  This error rate is accumulated and
   corrected every time we accumulate a whole step of error.  We can see this
   happening in the above image, where we occasionally take an extra step to
   catch up with our expected position.  This error-correction will always
   exhibit as a single additional step in one interval cycle at a time.

   For more information on how to plan interleaved moves, see OMMotorFunctions::plan()


   @section steptiming The Stepping Cycle

   Asynchronous motion is achieved by utilizing the Timer1 library to trigger an interrupt service
   routine every given time period, with a microsecond resolution.
   During one of these time periods, it is possible to either take a full step
   of the motor, or sit idle.  During a cycle in which a step is to be taken,
   the step pin is brought both high and back to low during that one timing cycle.

 <table border=1 width=60%>
 <tr bgcolor="#CACACA">
   <td>200uS</td><td>400uS</td><td>600uS</td><td>800uS</td><td>1000uS</td><td>1200uS</td><td>1400uS</td><td>1600uS</td>
 </tr>
 <tr>
  <td bgcolor="#FFAAAA">step</td><td bgcolor="#AAFFAA" colspan="4">idle</td><td bgcolor="#FFAAAA">step</td><td bgcolor="#AAffAA" colspan="2">idle</td>
 </tr>
 </table>

 Speed of the motor is controlled by the number of off-cycles between each step,
 which is continuously varied by re-calculating the desired position of the motor
 at set intervals.  During motion, this interval is always set to 1mS of movement,
 thus the speed of the motor is adjusted 1,000 times per second to give as fluid
 of motion as is possible while minimizing CPU utilization.

 <table border=1 width=60%>
 <tr bgcolor="#CCCCFF">
    <td colspan=5>1 mS</td><td colspan=5>2 mS</td>
 </tr>
 <tr bgcolor="#CACACA">
   <td>200uS</td><td>400uS</td><td>600uS</td><td>800uS</td><td>1000uS</td><td>1200uS</td><td>1400uS</td><td>1600uS</td>
 </tr>
 <tr>
  <td bgcolor="#FFAAAA">step</td><td bgcolor="#AAFFAA" colspan="4">idle</td><td bgcolor="#FFAAAA">step</td><td bgcolor="#AAffAA" colspan="2">idle</td>
 </tr>
 </table>

 It would seem that we could simply set the time periods extremely small,
 say 20uS, to increase the maximum stepping speed.  However, since we have to do
 actual work in these timing slices (decide whether to take a step or not, update
 position, etc.) - and not to mention that we have to perform a complex calculation
 during one cycle every millisecond to re-calculate our current speed, we must account
 for this time spent.  The worst-case observed times to perform all of these
 activities are 20uS for controlling the stepping pattern, and 170uS for
 re-calculating the current speed using the quadratic easing algorithm. (linear
 performs more quickly.)

 If we were to take longer inside of our interrupt service routine doing our
 activities than the timer is set for, our observation of time will be flawed.
 For this reason, we must allow enough time to complete all activities required
 in each time slice. This is why the maximum step rate is set to 5,000 steps
 per second, or once every 200uS.

   @section movespeed Timing and Speed Controls

   As discussed in the section \ref steptiming "The Stepping Cycle" above, there
   is a maximum rate at which stepping can occur, and CPU consumed driving the
   motor is inversely proportional to the rate specified. Therefore, if you
   have an application that is otherwise quite busy doing non-motor activities,
   such as polling sensors or reading user input, you may wish to reduce the
   rate at which stepping occurs to free up more CPU time.

   Reducing CPU consumption by decreasing the rate at which the stepping cycle
   runs reduces the accuracy of real-time speeds, the smoothness of velocity
   changes and the maximum speed at which any move can be executed.  Therefore,
   the stepping rate should only be modified lower than the maximum when
   required.

   As timing requirements limit the stepping cycle to whole microsecond periods
   greater than 200uS, you are limited to the following choices in stepping rates:
   5000, 4000, 2000, and 1000.

   To control the maximum speed of simple moves (the two-argument form of
   OMMotorFunctions::move() ) and moves executed during a planned movement, you may
   use the OMMotorFunctions:setMaxSpeed() command. Setting the maximum speed, instead
   of the maximum stepping rate, can be used to limit these types of movements
   to the actual speed at which your motor can travel when it is not one of the
   available rate options.

   Complex moves that specify all parameters are always attempted to be executed
   as requested, even if it results in a higher speed than specified by the
   maximum rate or maximum speed.  This can result in unexpected behavior from
   the motor and one should adjust their distance or arrival time accordingly.

*/


/**

  @brief
  Motor Manager

  The Motor Manager class provides the ability to run a stepper motor on
  the nanoMoCo platform in either continuous motion or on-demand motion
  using asynchronous (non-blocking) control.

  All actions performed by the Motor Manager are non-blocking, and
  this library uses the Timer1 library to achieve this.  For this reason,
  you may only have one such object instantiated in your program, and under
  no circumstances should you attempt to use Timer1 in another part of
  your program while a motor action is being performed.

  Status reporting from the Motor Manager is handled via a callback function,
  which you may specify.  This function will be called at specified times
  with a special code to indicate the status.  For more information on this,
  see setHandler().

  It is not possible to specify the motor control lines at this time, and the
  library will only work for other Arduino-compatible devices when they use
  the same pins for the stepper driver.

  For information on complex movements and motion profiles with the OMMotorFunctions
  library, see the \ref ommotion "Motion Capabilities" section.

  Example of a Simple Stepper Program:

  @code
   // note: You -always- have to include the TimerOne library in an arduino
   // sketch before including the OMMotorFunctions header

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

  @author C. A. Church

  (c) 2008-2011 C. A. Church / Dynamic Perception

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


class OMMotorFunctions {


public:


    OMMotorFunctions(int, int, int, int, int, int, int, int, int);
    ~OMMotorFunctions();

    void ms(uint8_t);
    uint8_t ms();

    void dir(uint8_t);
    uint8_t dir();

    void backlash(unsigned int);
    unsigned int backlash();

    void setHandler(void(*)(uint8_t));

    unsigned int curSamplePeriod();

    void maxStepRate(unsigned int);
    unsigned int maxStepRate();

    unsigned int maxSpeed(unsigned int);
    unsigned int maxSpeed();

    void enable(uint8_t);
    uint8_t enable();

    void continuous(uint8_t);
    uint8_t continuous();

    float contAccel(float);
    float contAccel();

    void contSpeed(float);
    float contSpeed();
    float desiredSpeed();

    uint8_t running();

    void sleep(uint8_t);
    uint8_t sleep();

    void stealth(uint8_t);
    uint8_t stealth();

    void steps(unsigned long);
    unsigned long steps();

    void maxSteps(long);
    long maxSteps();

    unsigned long stepsMoved();

    void stop();
    void clear();

    void move();
    void move(uint8_t, unsigned long);
    void move(uint8_t, unsigned long, bool);
    void move(uint8_t, unsigned long, unsigned long, unsigned long, unsigned long);
    void resumeMove();

    void plan(unsigned long, uint8_t, unsigned long, unsigned long, unsigned long);
    void planRun();		//used move
    void planReverse(); //used move

    void homeSet();
    void endPos(long);
    long endPos();
    void startPos(long);
    long startPos();
    void stopPos(long);
    long stopPos();
    void currentPos(long);
    long currentPos();

    void moveTo(long);
    void moveTo(long, bool);
    void moveToStart();
    void moveToStop();
    void moveToEnd();
    void home();

    uint8_t programDone();
    void programDone(uint8_t);

    static void planType(uint8_t);
    static uint8_t planType();
    void planTravelLength(unsigned long);
    unsigned long planTravelLength();
    void planAccelLength(unsigned long);
    unsigned long planAccelLength();
    void planDecelLength(unsigned long);
    unsigned long planDecelLength();
    void programMove();

    void easing(uint8_t);
    uint8_t easing();

    void resetProgramMove();

    void checkRefresh();
    uint8_t checkStep();

    void planLeadIn(unsigned int);
    void planLeadOut(unsigned int);
    unsigned long planLeadIn();
    unsigned long planLeadOut();

    int stpreg;
    int stpflg;

    uint8_t autoPause;

    unsigned long motor_steps_max;
    unsigned long mtpc_steps;
    uint8_t       mtpc_dir;
    unsigned long mtpc_arrive;		// SMS: Number of travel shots -- Continuous: Total movement time in milliseconds
    unsigned long mtpc_accel;		// SMS: Number of accel shots -- Continuous: Accel movement time in milliseconds
    unsigned long mtpc_decel;		// SMS: Number of accel shots -- Continuous: Decel movement time in milliseconds	

    uint8_t mt_plan;

    void updateSpline();
    volatile uint8_t splineReady;
    uint8_t endOfMove;

    float getTopSpeed();

    // Setting and retrieving of motor units code
    void units(int);
    int units();
    void gboxRatio(float);
    float gboxRatio();
    void platRatio(float);
    float platRatio();

    uint8_t programBackCheck();
    void programBackCheck(uint8_t p_setFlag);

    // Key frame assignment functions
    void keyDest(uint8_t, long);
    void keyTime(uint8_t, unsigned long);
    void keyAccel(uint8_t, unsigned long);
    void keyDecel(uint8_t, unsigned long);
    void keyLead(uint8_t, unsigned long);

    // Key frame queries
    long keyDest(uint8_t);
    unsigned long keyTime(uint8_t);
    unsigned long keyAccel(uint8_t);
    unsigned long keyDecel(uint8_t);
    unsigned long keyLead(uint8_t);

    static void debugOutput(bool p_state);
    static bool debugOutput();

    void restoreLastMs();
    int lastMs();
    void setSending(boolean);
    boolean isSending();

    // For getting the motor number within the motor class
    uint8_t id();


private:
        // variables for pre-calculated
        // spline formula values


    int m_stp;
    int m_slp;
    int m_dir;
    int m_ms1;
    int m_ms2;
    int m_ms3;

    uint8_t m_programDone;

    struct s_splineCal {
        float accel_fraction;			// Acceleration percentage of total move
        float decel_fraction;			// Deceleration percentage of total move
        float cruise_fraction;			// Constant speed percentage of toal move
        float top_speed;				// SMS: max steps required during any single movement interval -- Continuous: top speed in steps / sec
        float decel_start;				// Percentage of total move when deceleration begins
        float easing_coeff;				// 1/travel = distance covered during accel/decel compared to same time at continuous speed
        float accel_coeff;				// Steps (SMS) or steps/s (CONT) per acceleration calculation unit. This is actually the coefficient of the power function y = ax^2 or linear function y = mx
        float decel_coeff;				// Steps (SMS) or steps/s (CONT) per deceleration calculation unit. This is actually the coefficient of the power function y = ax^2 or linear function y = mx
        unsigned long accel_steps;		// Number of acceleration steps remaining
        unsigned long decel_steps;		// Number of deceleration steps remaining
        unsigned long cruise_steps;		// Number of cruising steps remaining
        unsigned long accel_moves;		// Number of SMS acceleration moves
        unsigned long cruise_moves;		// Number of SMS cruise moves
        unsigned long decel_moves;		// Number of SMS deceleration moves
    };


    static uint8_t g_plan_type;	// Plan type: 0 = SMS, 1 = CONT_TL, 2 = CONT_VID

    s_splineCal m_splineOne;
    s_splineCal m_splinePlanned;

    void _stepsAsync(uint8_t, unsigned long);

    void _updateMotorHome(int);
    void _fireCallback(uint8_t);

    void _initSpline(uint8_t, float, unsigned long, unsigned long, unsigned long);
    static void _linearEasing(uint8_t, float, OMMotorFunctions*);
    static void _quadEasing(uint8_t, float, OMMotorFunctions*);
    static void _SMSErrorCalc(const float&, float&, s_splineCal*, OMMotorFunctions*);
    static void _contErrorCalc(const float&, float&, OMMotorFunctions*);
    static unsigned long _partialSum(unsigned long);
    static unsigned long _partialSumOfSquares(unsigned long);


    void _setEasingCoeff(OMMotorFunctions::s_splineCal*);

    static float _qEaseCalc(OMMotorFunctions::s_splineCal*, float, OMMotorFunctions*, uint8_t);
    static float _qInvCalc(OMMotorFunctions::s_splineCal*, float, OMMotorFunctions*, uint8_t);

    void _updateContSpeed();

    unsigned int m_maxSpeed;

    uint8_t m_backCheck;
    uint8_t m_motEn;
    uint8_t m_motSleep;
    uint8_t m_motStealth;
    uint8_t m_isRun;
    uint8_t m_asyncWasdir;
    uint8_t m_curDir;
    uint8_t m_switchDir;
    uint8_t m_refresh;
    uint8_t m_motCont;
    uint8_t mtpc_start;
    uint8_t m_firstRun;
    uint8_t m_id;

    //movement variables
    volatile int m_cycleErrAccumulated;
    volatile unsigned long m_cyclesLow;
    volatile unsigned long m_stepsTaken;
    volatile unsigned long m_totalCyclesTaken;

    unsigned long m_asyncSteps;					// Number of steps in a specified move
    unsigned long m_asyncDo;
    volatile unsigned long m_stepsMoved;		
    unsigned long m_totalSteps;


    unsigned long m_nextOffCycles;
    unsigned int m_nextCycleErr;



    volatile unsigned long m_curOffCycles;      //
    volatile int m_curCycleErr;                 //
    static unsigned int g_curSampleRate;        //
    static unsigned int g_cyclesPerSpline;      //
    volatile unsigned long m_curSpline;         //
    unsigned long m_totalSplines;               // 

    volatile unsigned long m_curPlanSpd;
    unsigned long m_curPlanSplines;             // SMS mode: number of movement intervals -- Continuous: number of arbitrary time increments (?)
    unsigned long m_curPlanSpline;
    volatile float m_curPlanErr;
    uint8_t m_planDir;                          // Planned move direction

    unsigned long m_Steps;                      // Number of steps since start of programmed move. This is not used in the NMX firmware, could be depreciated
    float m_contSpd;                            // Motor's current continuous speed (steps/s)
    float m_desiredContSpd;                     // Motor's target continuous speed (steps/s)
    float m_contAccelRate;                      // Motors continuous acceleration rate (steps/s^2)

    volatile long m_curPos;                     // Distance in current microsteps of present location from home position
    long m_endPos;                              // Distance in current microsteps of end limit from home position
    long m_startPos;                            // Distance in current microsteps of program start from home position
    long m_stopPos;                             // Distance in current microsteps of program stop from home position

    unsigned long m_planLeadIn;                 // Motor's lead-in, in shots (SMS, time lapse continuous modes) or milliseconds (video continuous)
    unsigned long m_planLeadOut;                // Motor's lead-in, in shots (SMS, time lapse continuous modes) or milliseconds (video continuous)

    uint8_t m_curMs;                            // Current microstepping value
    unsigned int m_backAdj;                     // Backlash adjustment steps (must be adjusted when microstepping changes)
    uint8_t m_easeType;                         // Easing mode: linear, quadratic, or inv. quadratic

    float m_top_speed;

    /* Code indicating the units of measurement used for this motor. This only a reference value for master devices and is not use in firmware.
        0 == inches, 1 == centimeters, 2 == degrees, 3 == steps
    */
    int m_unitCode;
    float m_gbox_ratio;
    float m_plat_ratio;

    void(*f_motSignal)(uint8_t);
    void(*f_easeFunc)(uint8_t, float, OMMotorFunctions*);
    float(*f_easeCal)(OMMotorFunctions::s_splineCal*, float, OMMotorFunctions*, uint8_t);

    uint8_t m_programBackCheck;

    uint8_t m_calcMove;

    // Key Frame variables
    struct key_frame {
        long dest[10];
        unsigned long time[10];
        unsigned long accel[10];
        unsigned long decel[10];
        unsigned long lead_in[10];
    };

    key_frame key_frame;

    static bool g_debug;

    // "Send to" vars
    int m_lastMs;
    boolean m_isSending;

    // We switch to 4th stepping for sending home, so stealth mode needs
    // to be deactivated for this and reactivated when the sending is done,
    // so this keeps track of the previous stealth state.
    boolean m_wasStealth;  

};

#endif

