/*
 
 OMAxis
 
 OpenMoco MoCoBus Core Libraries 
 
 See www.dynamicperception.com for more information
 
 (c) 2012-2013 Dynamic Perception LLC
 
 Authors: Stanislav Perepelitsa, C. A. Church
 
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

#include "OMAxis.h"
#include "OMAxisCommands.h"
#include "../OMByteBuffer/OMByteBuffer.h"

OMAxis::OMAxis(HardwareSerial& c_serObj) : OMMoCoMaster(c_serObj) {
    m_slaveAddr = 2;
}


/** Set Target Node Address for Subsequent Commands
 
 You must set a target node address before sending commands. This allows
 once instance of the OMAxis class to speak to any number of nanoMoCo
 devices.
 
 For example:
 
 @code
 // assume we have three devices at address 6, 7, and 8:
 
 for( byte i = 6; i < 9; i++ ) {
    axis.target(i);
    axis.steps(200);
 }
 @endcode

 
 If you want to start or stop all axes at once, see \ref busbcast "Sending Broadcast Commands".

 @param p_addr
 Target node address
 
 */

void OMAxis::target(uint8_t p_addr) {
    m_slaveAddr = p_addr;
}

/** Get Target Node Address for Subsequent Commands
 
 @return
 Target node address
 */

uint8_t OMAxis::target() {
    return m_slaveAddr;
}

/** Set Interval for Automatic Operation

  @param p_ms
  The interval time, in milliseconds
 
  @return
  Whether or not the command succeeded.
  
 */

bool OMAxis::interval(unsigned long p_ms) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_INTERVAL, p_ms);
}

/** Set Exposure Time for Automatic Operation
 
 @param exposure
 Exposure time, in milliseconds

 @return
 Whether or not the command succeeded.

 */
bool OMAxis::exposure(unsigned long p_ms) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_EXPOSURE, p_ms);

}

/** Set Focus Time for Automatic Operation
 
 @param focus
 Focus time, in milliseconds
 
 @return
 Whether or not the command succeeded.

 */
bool OMAxis::focus(unsigned int p_ms) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_FOCUS, p_ms);
}

/** Set Maximum Shot Count
 
 Automatic operation will stop when this count is reached.
 
 You will need to issue a stop command manually after count is
 reached to reset shot counter.
 
 @param p_count
 Maximum shot count
 
 @return 
 Whether or not command succeeded.
 
 */
bool OMAxis::maxShots(unsigned int p_count) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_MAXSHOT, p_count);

}
/** Set the Exposure Delay
 
 How long to delay after finishing an exposure (wait for camera to buffer, or
 wait for camera to finish exposure when camera controls exposure time).
 
 @param p_ms
 Milliseconds to delay
 
 @return
 Whether or not command succeeded.
 
 */
bool OMAxis::exposureDelay(unsigned int p_ms) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_DELAY, p_ms);
}

/** Tie Focus Line to Shutter Line
 
 When set to true, the focus line will always be triggered with the shutter line.
 
 Many cameras, especially Nikons, require this.
 
 @param p_en
 Enabling focus line tieing?
 
 @return
 Whether or not command succeeded.
 
 */
bool OMAxis::tie(bool p_en) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_TIE, (uint8_t)p_en);
}


/** Repeat Exposure Count
 
 Set the repeat exposure count.
 
 When repeating an exposure, the shutter line is triggered multiple times in one
 exposure cycle.  Such as for triggering bracketed shots on a camera, or taking
 multiple exposures per interval.
 
 @param p_count
 Number of times to repeat exposure
 
 @return
 Whether or not command succeeded.
 
 */
bool OMAxis::repeat(uint8_t p_count) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_CAM, SC_CAM_REPEAT, p_count);
}

/** Steps to Take Between Each Shot (Basic Automation)
 
 Between each shot, take exactly this number of steps.  Used as an alternative to
 planned moves.
 
 @param p_steps
 Steps to take between each exposure
 
 @return
 Whether or not command succeeded.
 */

bool OMAxis::steps(unsigned int p_steps) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_STEPS, p_steps);
}


/** Set Motor Direction
 
 Specify motor direction, true or false.
 
 @param p_dir
 Motor direction
 
 @return
 Whether or not command succeeded. 
 
 */
bool OMAxis::dir(bool p_dir) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_DIR, (uint8_t) p_dir);
}


/** Specify Maximum Steps
 
 Specify the maximum number of steps to take, upon reaching this count, the motor will stop
 no matter what other settings have been sent.
 
 You must call stop() after this count has been reached to clear the step counter, else the
 motor will no longer move again, even in manual moves.
 
 @param p_steps
 Maximum step count
 
 @return
 Whether or not command succeeded. 

 
 */
bool OMAxis::maxSteps(unsigned long p_steps) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_MAXSTEP, p_steps);
}


/** Enable/Disable Motor
 
 Enable, or disable the motor.  If motor is disabled, all move commands will be
 non-operative.
 
 @param p_en
 Enable (true) or Disable (false)
 
 @return
 Whether or not command succeeded. 
 */
bool OMAxis::enableMotor(bool p_en) {
    uint8_t comCode = p_en ? SC_MOT_ENABLE : SC_MOT_DISABLE;
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, comCode);
}



/** Set Current Position as Home
 
 @return
 Whether or not command succeeded. 
 
 */
bool OMAxis::setHome(void) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_SETHOME);
}

/** Set Backlash Correction Amount
 
 Backlash correction issues extra steps as necessary on direction changes.
 
 @param p_steps
 Amount of backlash correction, in steps
 
 @return
 Whether or not command succeeded.
 
 */
bool OMAxis::backlash(uint8_t p_steps) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_BACKLASH, p_steps);
}


/** Set Easing Mode
 
 For complex and planned moves, you can specify which easing mode to use.
 
 For more information on easing, see \ref moveease "Easing".
 
 @param p_mode
 A valid EasingMode value
 
 @return
 Whether or not command succeeded. 
 
 */
bool OMAxis::easing(EasingMode p_mode) {
    return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_EASE, (uint8_t) p_mode );
}


/** Enable or Disable Continuous Movement
 
 Normal move operation is for a specified number of steps, or from one position to another.
 
 However, it is sometimes desired to simply move continuously at a speed until, well, you
 decide not to.  This method enables this mode of operation. 
 
 Once you have enabled continuous operation, you can set the speed of this movement using
 continuousSpeed(), and you can start the movement by calling a simple move() with zero steps
 specified. Once moving, you may adjust the speed as needed using continuousSpeed().
 
 If you want to stop the motor and everything else once started, use stop(), otherwise, to
 just stop the motor, use stopMotor().
 
 For example:
 
 @code
 
 Axis.continuous(true);
 Axis.continuousSpeed(0.5);
 Axis.move(1, 0);
 
 ...
 
 Axis.stopMotor();
 @endcode
 
 @param p_en
 Enable or disable continuous movement.
 
 @return
 Whether or not command succeeded. 
 
 */
bool OMAxis::continuous(bool p_en) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_CONTEN, (uint8_t) p_en);
}

/** Set Continuous Movement Speed
 
 Sets the speed of continuous movement, in steps per second.  Can specify
 fractions.
 
 See continuous() for more information.
 
 @param p_speed
 Fractional speed in steps per second.
 
 @return
 Whether or not command succeeded. 
 
 */
 
bool OMAxis::continuousSpeed(float p_speed) {
	return command(m_slaveAddr, OM_PCODE_PDS, OM_PCODE_MOTOR, SC_MOT_CONTSPD, p_speed);
}


/** Start Executing the Current Program
 
 */
bool OMAxis::start() {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_START);
}

/** Pause the Current Program
 
 You can re-start without loss by calling start() later.
 
 */
bool OMAxis::pause() {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_PAUSE);
}


/** Stop the Current Program
 
 Stop resets all operating parameters, including the shot counter, steps moved,
 etc.  If you call start() after calling stop(), then it will be as if you are running
 your program for the first time.  See pause().
 
 */
bool OMAxis::stop() {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STOP);
}

/** Enable or Disable Camera Control
 
 If the camera is disabled, it will not be possible to trigger exposure or focus, even
 manually.
 
 @param p_en
 Enable (true) or disable (false) camera
 
 @return
 Whether or not command succeeded. 
 
 */
bool OMAxis::enableCamera(bool p_en) {
    uint8_t com = p_en ? CMD_PC_CAM_ENABLE : CMD_PC_CAM_DISABLE;
	return command(m_slaveAddr, OM_PCODE_PC, com);
}



/** Perform a Simple Move
 
 Moves a specified number of steps in the specified direction.  Moves at the maximum possible
 speed, and accelerates/decelerates a minimal amount.
 
 @param p_dir
 Direction to move
 
 @param p_steps
 Steps to take
 
 @return
 Whether or not command succeeded. 
 */
bool OMAxis::move(bool p_dir, unsigned long p_steps) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_MOVE_MOTOR_SMPL, p_dir, p_steps);
}

/** Send Motor to Home Immediately
 
 */
bool OMAxis::home() {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_SEND_MOTOR_HOME);
}

/** Expose Camera Now
 
 Triggers the shutter for the specified time.
 
 @param p_ms
 Exposure time, in milliseconds
 */
bool OMAxis::expose(unsigned long p_ms) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_EXPOSE_NOW, p_ms);
}

/** Enable/Disable Driver Sleep
 
 When driver sleep is enabled, the motor driver is turned off when the motor is not
 moving, reducing heat/power consumption.  When it is disabled, the motor driver stays
 on even when the motor is not moving.
 
 Note that stepper motors have little to no holding torque when their poles aren't
 energized.  For some applications, driver sleep is not recommended.
 
 @param p_en
 Enable (true) or disable (false)
 
 */
bool OMAxis::sleep(bool p_en) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_MOTOR_SLEEP, (uint8_t) p_en);
}

/** Set the Maximum Stepping Rate
 
 Set the maximum rate of the step-driving algorithm.  Specifies the maximum number
 of possible steps that can be taken, and controls how often the step cycle speed
 adjustment algorithm is run.
 
 In most cases, you should never mess with this.  In fact, if you're looking at this,
 you probably mean to be looking at maxSpeed().
 
 @param p_cycles
 A valid StepRateValue
 
 */
bool OMAxis::maxStepRate(StepRateValue p_cycles) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_MOTOR_MAX_RATE, (unsigned int) p_cycles);

}

/** Set the Maximum Speed of any Movement
 
 Sets the maximum speed, in steps per second, of any movement.  This is used for all moves, but is most
 noticable for simple moves and homing where the motor wants to move as fast as possible.  This value
 will always be below that set for maxStepRate(). 
 
 @param p_steps
 Maximum speed, in steps per second.
 
 
 */
bool OMAxis::maxSpeed(unsigned int p_steps) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_MOTOR_MAX_SPEED, p_steps);
}

/** Turn On or Off the Debug LED
 
 @param p_en
 Turn on (true) or off (false) the debug LED.
 
 */
bool OMAxis::led(bool p_en) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_DEBUG_LED, (uint8_t) p_en);
}

/** Stop Motor Immediately
 
 Stops the motor movement immediately, but does not impact other program
 operation.
 
 */
bool OMAxis::stopMotor() {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STOP_MOTOR_NOW);
}

/** Set MicroStep Amount
 
 Sets the microstep amount, if setHome() has been called, home position will
 automatically be adjusted to new microstep count.
 
 @param p_micros
 A valid MicroSteps value
 
 */
bool OMAxis::ms(MicroSteps p_micros) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_MICROSTEP_LVL, (uint8_t) p_micros);
}


/** Set Node as Timing Master or Slave
 
 While it is not required to have a timing master for all shooting modes, shoot-move-shoot style
 workflows require that all movement happens between exposures.  In this case, a "timing master" 
 must be elected.  This "timing master" will control the camera exposure, and notify other nodes
 when the exposure is complete, so that they may then perform their movements.  COM3 line is
 typically used for this communication.
 
 To be safe, you should explicitely declare the role of each node in-use, as master or slave.
 
 Do not rely on default values for this parameter.
 
 @param p_en
 Master (true) or Slave (false)
 
 
 */
bool OMAxis::master(bool p_en) {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_TIMING_MASTER, (uint8_t) p_en);
}


/** Perform a Complex Move Immediately
 
 Move the motor a specified number of steps in the specified direction.  Complete this move in the
 specified time, and accelerate and decelerate for the specified amounts of time. 
 
 This command utilizes the easing() mode currently set for the accel and decel.  For more
 information on the nature of this kind of move, see \ref movedefs "Defining a Move".
 
 It should go without saying that <i>p_travel > (p_accel + p_decel)</i>
 
 If the move specified exceeds the maximum step speed, or maximum step rate, you will receive no
 warning. The motor will behave erratically.  It is expected that the controling interface provide
 the required business logic to prevent impossible moves.
 
 
 @param p_dir
 Direction to move
 
 @param p_steps
 Total steps to move
 
 @param p_travel
 Amount of time, in milliseconds, to complete the travel
 
 @param p_accel
 Amount of time, in milliseconds, to spend accelerating
 
 @param p_decel
 Amount of time, in milliseconds, to spend decelerating
 
 */
bool OMAxis::move(bool p_dir, unsigned long p_steps, unsigned long p_travel, unsigned long p_accel, unsigned long p_decel) {
    
	OMByteBuffer sendBuf;
	uint8_t _buf[21];
    
	sendBuf.assign(_buf, sizeof(_buf));
    
	sendBuf.append(CMD_PC_MOVE_COMPLEX);
	sendBuf.append(p_dir);
	sendBuf.append(p_steps);
	sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_travel);
	sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_accel);
	sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_decel);
    
	return command(m_slaveAddr, OM_PCODE_PC, sendBuf.getBuf(), sendBuf.size());
    

}

/** Plan a Complex Move
 
 A planned complex move allows you to specify a complex move to happen at a later point (such as, after starting
 the execution of the program).  You can specify that this complex move be a continuous movement (movement happens
 while the camera is firing) or an interleaved move (i.e. 'shoot-move-shoot', movement happens between exposures).
 
 Like a regular complex move, you specify the five parameters, the direction, the steps to travel, the time to complete
 the travel, the acceleration time, and the deceleration time.
 
 For continuous moves, all times are in milliseconds.
 
 For interleaved moves, all times are in intervals (e.g. move over 500 exposures)
 
 For even more powerful movement planning capabilities, add in delayMoveStart() to cause the movement to begin at a point
 later than the start of the program.
 
 For example, the following code plans a movement that goes 7,000 steps in 60 seconds, with 5 seconds of acceleration
 and deceleration, where the move starts 30 seconds after the program starts running:
 
 @code
 Axis.plan(PLAN_CONTINUOUS, true, 7000, 60000, 5000, 5000);
 Axis.delayMoveStart(30000);
 Axis.start();
 @endcode
 
 @param p_type
 The type of movement, either continuous or interleaved
 
 @param p_dir
 The direction of travel
 
 @param p_steps
 The total number of steps to move
 
 @param p_travel
 The amount of time (in milliseconds for continuous, or intervals for interleaved) to complete the travel in
 
 @param p_accel
 The amount of time (in milliseconds for continuous, or intervals for interleaved) to accelerate to full speed
 
 @param p_decel
 The amount of time (in milliseconds for continuous, or intervals for interleaved) to decelerate to stop

 */
bool OMAxis::plan(PlanType p_type, bool p_dir, unsigned long p_steps, unsigned long p_travel, unsigned long p_accel, unsigned long p_decel) {
    
	OMByteBuffer sendBuf;
	uint8_t _buf[22];
    
	sendBuf.assign(_buf, sizeof(_buf));
    
	sendBuf.append(CMD_PC_PLAN);
	sendBuf.append( (uint8_t) p_dir);
    sendBuf.append( (uint8_t) p_type);
    sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_steps);
	sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_travel);
	sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_accel);
	sendBuf.append(OM_PDU_ESC);
	sendBuf.append(p_decel);
    
	return command(m_slaveAddr, OM_PCODE_PC, sendBuf.getBuf(), sendBuf.size());

}
/** Clear a Planned Complex Move
 
 Wipes out a planned move, allows for non-planned activity once a plan has been defined.
 
 */
bool OMAxis::clearPlan() {
	return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_CLEAR_PLAN);
}

/** Delay Planned Move
 
 Sets a delay before a planned move is to begin (after start is issued)
 
 @param p_ms
 Time to delay move start, in milliseconds
 */

bool OMAxis::delayMoveStart(unsigned long p_ms) {
    return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_MAX_RUN, p_ms);
}


/** Watch a COM Line for Stepping Pulses
 
 The nanoMoCo supports the ability to monitor one of the common lines for a 
 step and direction signal, which can be used to manually pulse out individual
 steps (such as with a manual pulse generator to nudge a motor into position).
 
 When triggering a manual pulse, the given COM line will be brought low for a brief
 period of time.  The act of bringing the line low will trigger a step, and the
 amount of time it is brought low for will indicate the direction of the step.
 
 A step in direction 0 will be 50 uS long low pulse, and a step in direction 1 will
 be a 100 uS long low pulse.
 
 @param p_com
 The COM line to monitor, use COM_NONE to disable monitoring
 */

bool OMAxis::comLinePulse(ComLine p_com) {
    return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_COMLINE, (uint8_t) p_com);
}

/** Determine if Node is Connected and Responding
 
 Runs sends a NOOP command to the node, and determines whether or not
 the node can be reached at the address, and is responding to commands.
 
 Should be used before issuing status requests to ensure the node is connected
 and responding properly, otherwise status results may be incorrect.
 */

bool OMAxis::connected() {
    return command(m_slaveAddr, OM_PCODE_PC, CMD_PC_NOOP);
}


/** Status: Firmware Version
 
 Retrieve the firmware version.
 
 @return
 The firmware version #
 */
uint8_t OMAxis::getFWVersion() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_FWVER);
    
    uint8_t ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;
}


/** Status: Program Running
 
 @return
 Whether or not the program is running
 */

bool OMAxis::getRunning() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_RUN);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;
    
}


/** Status: Program Runtime
 
 Get program run time, in milliseconds
 
 @return
 Run time in milliseconds
 */

unsigned long OMAxis::getRunTime() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_RUNTIME);
    
    unsigned long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoul(data);
    }
    
    return ret;

}


/** Status: Camera Enabled
 
 @return 
 Whether or not camera is enabled
 */


bool OMAxis::getCamEnabled() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_CAMEN);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;

}

/** Status: Exposure Count
 
 Get exposure counter value
 
 @return
 Exposure counter value
 */

unsigned long OMAxis::getExposures() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_SHOTS);
    
    unsigned long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoul(data);
    }
    
    return ret;
   
}

/** Status: Interval
 
 Get interval time, in milliseconds
 
 @return
 Interval time, in milliseconds
 */

unsigned long OMAxis::getInterval() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_INTERVAL);
    
    unsigned long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoul(data);
    }
    
    return ret;

}

/** Status: Exposure Time
 
 Get exposure time, in milliseconds
 
 @return
 Exposure time, in milliseconds
 */

unsigned long OMAxis::getExposureTime() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_EXPTM);
    
    unsigned long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoul(data);
    }
    
    return ret;
    
}

/** Status: Currently Exposing
 
 Is the camera currently exposing?
 
*/


bool OMAxis::getExposing() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_EXPOSING);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;
        
}

/** Status: Motor Enabled
 
 @return
 Whether or not the motor is enabled
 */

bool OMAxis::getMotorEnabled() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_MOTOREN);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;
        
}

/** Status: Current Motor Direction
 
 @return
 Current motor direction
 */

bool OMAxis::getMotorDir() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_MOTORDIR);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;
    
}

/** Status: Steps Moved
 
 Get the steps moved counter
 
 @return
 Steps moved since last counter reset
 */

unsigned long OMAxis::getStepsMoved() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_MOVED);
    
    unsigned long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoul(data);
    }
    
    return ret;    
}

/** Status: Distance from Home
 
 Returns the step distance from home, if a move in a positive direction is
 required to reach home, the value will be negative, otherwise it will be
 positive.

 @return
 Distance from home, signed long
 */
 
long OMAxis::getHomeDistance() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_HOMEDIST);
    
    long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntol(data);
    }
    
    return ret;      
}

/** Status: Maximum Steps Setting
 
 @return
 Max Steps value
 */

unsigned long OMAxis::getMaxSteps() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_MAXSTEPS);
    
    unsigned long ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 3 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoul(data);
    }
    
    return ret;        
}

/** Status: Backlash Correction
 
 @return
 Current backlash correction value
 */

uint8_t OMAxis::getBacklash() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_BACKLASH);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;
   
}

/** Status: Steps per Exposure
 
 @return
 Steps per exposure value
 */

unsigned int OMAxis::getSteps() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_STEPS);
    
    unsigned int ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 1 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = ntoui(data);
    }
    
    return ret;        
 
}

/** Status: Timing Master
 
 @return
 Timing master value
 */
bool OMAxis::getMaster() {
    int res = command(m_slaveAddr, OM_PCODE_PC, CMD_PC_STATUS_REQ, OM_STAT_MASTER);
    
    bool ret = 0;
    
    if( res && responseType() != -1 && responseLen() > 0 ) {
        uint8_t* data = (uint8_t*) responseData();
        ret = data[0];
    }
    
    return ret;    
}



