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



#ifndef OMAXIS_H_
#define OMAXIS_H_

#include "../OMMoCoMaster/OMMoCoMaster.h"

/** @file OMAxis.h 
 
 Header file for OMAxis class
 
 */

 /** Easing Types for OMAxis */
enum EasingMode { 
        /** Linear Easing */
    EASING_LINEAR = 1, 
        /** Quadratic Easing */
    EASING_QUAD, 
        /** Inverse Quadratic */
    EASING_INVQUAD 
};


 /** Stepping Engine Rate */
enum StepRateValue {
    RATE_LOW  = 1000,
    RATE_MED  = 2000,
    RATE_HI   = 4000,
    RATE_VHI  = 5000
};

 /** MicroSteps Values */
enum MicroSteps {
    MS_1   = 1,
    MS_2   = 2,
    MS_4   = 4,
    MS_8   = 8,
    MS_16  = 16
};

 /** Planned Movement Type */
enum PlanType {
        /** Interleaved Motion */
    PLAN_INTERLEAVE = 0,
        /** Continuous Motion */
    PLAN_CONTINUOUS = 1
};

 /** Common Line Setting for Direct Pulse Stepping */
enum ComLine {
        /** No Common Line is to be Used */
    COM_NONE = 0,
        /** Use COM1 */
    COM1     = 1,
        /** Use COM2 */
    COM2     = 2
    
};

/** 
 
 @brief Complete control of nanoMoCo Devices on MoCoBus
 
 
 OMAxis provides the primary interface for controlling nanoMoCo devices on an AVR platform.
 
 Only one OMAxis instance is required to speak to any number of devices.  For most nanoMoCo-specific
 commands, you need only to specify the target() before issuing a command. Each command issued returns
 true or false indicating whether the remote device successfully received the command.
 
 See \ref nanomoco "Controlling nanoMoCo Devices with OMAxis" for detailed information on how to talk to, and
 control your devices.
 
 
 The following provides a basic example of controlling a node via this class in an Arduino sketch:
 
 @code
 
    // note: include all headers due to issues with Arduino build process
 
 #include "OMMoCoBus.h"
 #include "OMMoCoMaster.h"
 #include "OMAxis.h"
 
 byte nodeAddr = 3;
 
 OMAxis axis = OMAxis(Serial);
 
 void setup() {
    Serial.begin(OM_SER_BPS);
 }
 
 void loop() {
 
    static bool isRunning = false;
 
    if( ! isRunning ) {
        axis.target(nodeAddr);
        axis.enableCamera();
        axis.interval(2);
        axis.enableMotor();
        axis.steps(50);
        axis.maxShots(100);
        axis.start();
 
        isRunning = true;
    }
 
 }
 @endcode
 
 @author 
 Stanislav Pereplitsa, C.A. Church
 
 
 Copyright (c) 2012, 2013 Dynamic Perception LLC
 
 This software is shared under the GNU GPLv3 License
 */

class OMAxis: public OMMoCoMaster {



public:
	OMAxis(HardwareSerial& c_serObj);

	//supported commands
	bool interval(unsigned long p_ms);
	bool exposure(unsigned long p_ms);
	bool focus(unsigned int p_ms);
	bool maxShots(unsigned int p_count);
	bool exposureDelay(unsigned int p_ms);
	bool tie(bool p_en);
	bool repeat(uint8_t p_count);
	bool steps(unsigned int p_steps);
	bool dir(bool p_dir);
	bool maxSteps(unsigned long p_steps);
	bool enableMotor(bool p_en);
	bool setHome(void);
	bool backlash(uint8_t p_steps);
	bool easing(EasingMode p_mode);
	bool continuous(bool p_en);
    bool continuousSpeed(float p_speed);
	bool start();
	bool pause();
    bool stop();
	bool enableCamera(bool p_en);
	bool move(bool p_dir, unsigned long p_steps);
    bool move(bool p_dir, unsigned long p_steps, unsigned long p_travel, unsigned long p_accel, unsigned long p_decel);
	bool home();
	bool expose(unsigned long p_ms);
	bool sleep(bool p_en);
	bool maxStepRate(StepRateValue p_cycles);
	bool maxSpeed(unsigned int p_steps);
	bool led(bool p_en);
	bool stopMotor();
	bool ms(MicroSteps p_micros);
	bool master(bool p_en);
	bool plan(PlanType p_type, bool p_dir, unsigned long p_steps, unsigned long p_travel, unsigned long p_accel, unsigned long p_decel);
	bool clearPlan();
    bool delayMoveStart(unsigned long p_ms);
    bool maxRunTime(unsigned long p_ms);
    bool comLinePulse(ComLine p_com);

    void target(uint8_t p_addr);
    uint8_t target();

    bool connected();
    
    // status requests
    
    uint8_t getFWVersion();
    bool getRunning();
    unsigned long getRunTime();
    bool getCamEnabled();
    unsigned long getExposures();
    unsigned long getInterval();
    unsigned long getExposureTime();
    bool getExposing();
    bool getMotorEnabled();
    bool getMotorDir();
    unsigned long getStepsMoved();
    long getHomeDistance();
    unsigned long getMaxSteps();
    uint8_t getBacklash();
    unsigned int getSteps();
    bool getMaster();
    


private:
    // internal data
    uint8_t m_slaveAddr;
    


};

#endif /* OMAXIS_H_ */

/**
 
 @page nanomoco Controlling nanoMoCo Devices with OMAxis
 
 You can control up to 32 nanoMoCo devices at once per MoCoBus.  The OMAxis class creates a high-level interface
 to all commands the nanoMoCo can accept, including all status commands.
 
 The OMAxis class is an OMMoCoMaster-derived class, so you can perform lower-level activities if you prefer,
 however this should not be necessary.
 
 
 @section nmdevaddr Device Addressing
 
 To reduce overall memory footprint, one OMAxis instance can be used for up to 32 distinct nanoMoCo devices.
 This is achieved by passing an address to the target() method, which sets the address for subsequent commands.
 
 For example, we can send the same commands to multiple nodes in a loop:
 
 @code
 
    // note: include all headers due to issues with Arduino build process
 
 #include "OMMoCoBus.h"
 #include "OMMoCoMaster.h"
 #include "OMAxis.h"
 
 byte addresses[3] = { 4, 5, 6 };
 
 OMAxis Axis = OMAxis(Serial);
 
 void setup() {
    Serial.begin(OM_SER_BPS);
 
    sendCommands();
 }
 
 void loop() {
 
 
 }
 
 void sendCommands() {
 
    for( byte i = 0; i < sizeof(addresses) * sizeof(byte); i++ ) {
 
        Axis.target( addresses[i] );
        
        if( Axis.connected() ) {
            Axis.enableCamera(false);
        }
    }
 
 }
 
 @endcode
 
 NB: You'll see connected() used a lot in the examples.  Its use should be abundant when sending
 commands to nodes, it tells you whether or not the node is connected and responding to commands
 at that address.
 
 If you wish to set device addresses via your Arduino sketch, look at the changeAddress() method,
 otherwise you can use any other facility for setting device addresses, like Graffik.
 
 Each nanoMoCo device <b>must</b> have a unique address, otherwise communication will be impaired
 on the bus - as two devices will attempt to respond at once.
 
 
 @section nmenable Enabling Functions on the nanoMoCo
 
 Most functions of the nanoMoCo can be enabled/disabled.  You should always ensure, before starting a
 series of commands, or after having completed a programmed activity, that the functions you need 
 enabled/disabled are so.  Do not rely any expected defaults.
 
 Some high-level functions, such as motor driving and camera control, are enabled using specific
 boolean parameters to enable or disable all related functions, these include enableCamera() and
 enableMotor().
 
 Other functions, such as controlling the focus line, delaying after a shot, or exposing the camera
 are enabled or disabled by specifying how long the activity should occur for.  For example, to 
 disable focus line control, we simply set focus time to 0ms with focus().  By specifying 0 time
 to execute any given activity, we disable that activity.
 
 
 @section nminteractivesms Interactive Control with Discrete Movements
 
 The most simple form of motion control to understand is that of interactive control.  In interactive
 control, you tell each nanoMoCo device exactly what to do, and when to do it.  By 'discrete movements'
 we mean that we're going to issue commands for very specific moves to points. For example, if we wanted to 
 perform a shoot-move-shoot move for five shots with one nanoMoCo, we can use the expose() and simple
 move() methods:
 
 @code
 
 for( byte i = 0; i < 5; i++) {
 
    Axis.expose(500);
    delay(500);
 
    Axis.move(true, 100);
    delay(500);
  }
 @endcode
 
 When using the simple move() method to create discrete movements, the motor will accelerate quickly and move 
 at a speed up to the maximum speed you've set via maxSpeed().
 
 All moves, camera controls, etc. are non-blocking.  This means you may continue to send commands, and even
 perform other actions while these commands are being executed.
 
 
 @section nminteractivec Interactive Control with Continuous Movement
 
 Like moving with discrete movements (e.g. SMS), we can also do direct, interactive control with continuous
 movement.  In this case, instead of telling the nanoMoCo how many steps to take, we tell it what speed we
 want it to move at.  
 
 To execute a standard continuous move, we have to do a few things:
 
 <ul>
    <li>Tell the nanoMoCo that movement will be continuous</li>
    <li>Tell it what speed to move at</li>
    <li>Tell it to start moving</li>
 </ul>
 
 The following code shows us how to do this:
 
 @code
 
 Axis.continuous(true); // enable continuous motion
 Axis.continuousSpeed(1.5); // steps per second
 Axis.move(true, 0); // start moving
 
 @endcode
 
 The continuous speed is expressed in steps per second, and is a floating-point value.  You may, and should,
 change the speed during the move.  Note that no acceleration or deceleration is done for you, you must
 accelerate and decelerate by modulating the speed. 
 
 Our call to move() here is special: note that we told it to go 0 steps. When moving continuously, we control 
 the speed and not the total steps via the move command.
 
 The motor will continue running at the last set speed until one of the following occurs:
 
 <ul>
    <li>stop() is called</li>
    <li>stopMotor() is called</li>
    <li>maxSteps() is reached</li>
 </ul>
 
 NB: You may reset a maxSteps() counter at any time by sending a stop() command, which resets all counters.
 
 Of course, this kind of continuous motion control is both boring and a lot of work to implement.  Let's look
 at something more interesting...
 
 
 @section nminteractivecm Interactive Control with Complex Movement
 
 We can perform a complex continuous move with a specified distance to travel, time to travel, acceleration
 period, and deceleration period by using the complex move() method.  This method takes more arguments than
 the simple move() method, and allows us to create far more expressive moves with a single line of code.
 
 For more information on how the complex moves, and what the result looks like, see \ref ommotion "Motion Capabilities".
 
 For example, the following command causes a move to be executed:
 
 @code
 Axis.move(true, 5000, 60000, 3000, 2000);
 @endcode
 
 This would be a lot more work to do interactively just using a basic continuous move, plus we don't have to worry
 about calling continuous() or setting a continuousSpeed().  The nanoMoCo simply calculates for us what speed to
 be going, and how far it has gone, every tiny fraction of a second.
 
 NB: If you specify a move that cannot be made within current parameters (too far too quickly), you will not get
 an error or warning, but your motor will mvoe erratically, and may attempt to make a very long move at the end
 of the movement to catch up to the desired position.
 
 
 @section nmprogram Basic Programmed Activities
 
 The real power of the nanoMoCo shines when we get to programmed activities.  Unlike interactive control, we don't
 have to keep timing and issue each command ourselves.  Instead, we tell the nanoMoCo what we want it to do, and
 it manages what it takes to do that for us.
 
 The most basic form of this is a shoot-move-shoot move with no "output" acceleration. By this, we mean that the distance
 moved between each exposure doesn't change.  Each discrete movement has a brief acceleration and deceleration period.
 
 To achieve this, we will tell the nanoMoCo what its operating parameters are, and then to start executing them:
 
 @code
 Axis.enableMotor(true); // motor will be enabled
 Axis.enableCamera(true); // camera will be enabled
 
 Axis.interval(1000); // 1-second exposure interval
 
 Axis.steps(50); // move 50 steps between each exposure
 Axis.dir(true); // move in the 'true' direction
 
 Axis.exposure(250); // trigger exposure for 250mS
 Axis.exposureDelay(300); // delay for 300mS after exposure before moving
 
 Axis.maxShots(100); // take a maximum of 100 exposures
 
 Axis.start(); // begin executing the program
 @endcode
 
 In the above example, the axis will move 50 steps in the 'true' direction between each exposure, and there 
 will be at least 1 second between each exposure.
 
 You may pause execution of a SMS motion program at any time by using pause(), and resume it again using start().  
 You may stop the program execution entirely by using the stop() command. The program will automatically stop 
 when one of the following occurs:
 
 <ul>
    <li>maxShots() is reached</li>
    <li>maxRunTime() is reached</li>
 </ul>
 
 If maxSteps() is reached, the motor will stop moving, but the program will otherwise continue operating.
 
 
 @section nmprogramadv Expressive Programmed Moves
 
 We can achieve much more expressive programmed moves using the plan() method.  The plan() control tells the
 motor to make a complex move once the program is started using start().  While basic programmed moves only
 let us do SMS-style movements, planned movements can be either continuous or interleaved between exposures.
 
 The plan() adds one additional argument that the complex move() method does not - and that is whether the 
 move is continuous, or SMS-style. We still have to setup the camera like we would normally.
 
 @code
 Axis.enableMotor(true); // motor will be enabled
 Axis.enableCamera(true); // camera will be enabled
 
 Axis.interval(1000); // 1-second exposure interval
 
 Axis.exposure(250); // trigger exposure for 250mS
 Axis.exposureDelay(300); // delay for 300mS after exposure before moving
 
 Axis.plan(PLAN_CONTINUOUS, true, 5000, 60000, 3000, 2000); // plan a complex move 
 Axis.start(); // begin executing the program
 @endcode
 
 Now, unlike our previous programmed activity, we have expressed not only some movement, but the final target
 position, the time to arrive there, how long to accelerate, and how long to decelerate.  
 
 For continuous moves, the times used in a plan (arrival time, acceleration time, and deceleration time) are
 all expressed in milliseconds.  For interleaved (SMS) moves, the times expressed are intervals. 
 
 We can also express a delay before the movement is to start, we do this using the delayMoveStart() command. 
 Like with plan, delayMoveStart() values are milliseconds for continuous moves, or intervals for interleaved
 motion.
 
 Planned move programs follow the same stop profile as a basic programmed move. 
 
 An interleaved planned move may be paused and re-started without issue, however, it is not possible to pause
 a continuous planned move as stepper motors are not capable of starting movement at high speeds, and must
 accelerate into such speeds.
 
 
 @section nmeasing Setting Easing Mode for Expressive Programmed Moves
 
 We all know that being limited to, say, linear acceleration, can be really boring in the final output video.
 With the nanoMoCo, it is possible to choose from Linear, Quadratic, or Inverse Quadratic easing profiles to
 get that extra punch in moves.  The selected easing profile is used for any complex move() or plan(), whether
 it is continuous or interleaved.  Simply call easing() before sending the move command or starting the 
 program.
 
 @code
 Axis.easing(EASING_QUAD);
 @endcode
 
 You should not change easing profiles while a move is being executed, or a program is running. Doing so will
 result in undefined behavior.
 
 
 @section nmsynchro Synchronizing Multiple Nodes
 
 When operating with multiple nodes, it is important that some synchronization is performed between them.  In
 the case of SMS movement, this is essential to prevent the nodes that are not controlling the camera to know
 when it is safe to move.  This is achieved using the master() method.  Only one node should ever be set to
 master.
 
 The master will send a clear to move signal to the slaves using COM3 when the exposure is complete, and the
 exposure delay has expired.
 
 @code
    // set address 4 as master, all else as slaves
 Axis.target(4);
 Axis.master(true);
 
 Axis.target(5);
 Axis.master(false);
 
 Axis.target(6);
 Axis.master(false);
 @endcode
 
 The node controlling the camera must always be set as the master.  Controlling multiple cameras from slaves
 is possible, but the master or other nodes may move while they are exposing.
 
 For slave nodes, the interval value is ignored.  This means that for interleaved motion, the slave nodes will
 only move after the master has sent the slave clear signal.  For continuous motion, master is less important,
 obviously, as motors will move at their needed speeds at the right times.
 
 To start, stop, or pause multiple nodes at the same time, use the broadcast method:
 
 @code
 Axis.broadcast(OM_BCAST_START);
 
 ...
 
 Axis.broadcast(OM_BCAST_STOP);
 @endcode
 
 
 @section nmstatus Requesting Operational Status
 
 You can request operational status from any nanoMoCo node by using the methods whose names start with 'get', 
 e.g. getRunning().
 
 You should always check to see if the node is connected and responding, otherwise the results returned by these
 methods are not reliable.
 
 @code
 
 if( Axis.connected() ) {
    unsigned long runTime = Axis.getRunTime();
 }
 
 @endcode
 
 @section nmsucfail Success and Failure of Communication
 
 All communication over MoCoBus, except broadcast commands, is validated.  That is to say, when a command is sent to a node,
 the node will respond with either the command being successfully received (not necessarily executed), or the command
 resulted in an error (was not understood, incorrect arguments, etc.).  
 
 As such, all OMAxis command methods return a bool indicating whether the command was successfully received, or not.  You
 may, then, do checks like the following:
 
 @code
 if( Axis.expose(500) ) {
    // command was accepted
 }
 else {
    // command was rejected, or node is down
 }
 @endcode
 
 While the low-level command() methods provided by OMMoCoMaster differentiate between node responses that indicate errors and timeouts,
 the OMAxis methods do not.  If you wish to know the difference, check the connected() method which will always return true if the node
 is connected and operating properly, and always return false if the node is not connected or is in an inoperable state.
 
 
 @section nmconnect Connecting to Nodes and Issueing Commands
 
 It is important that your nodes be powered on and fully booted before you attempt to contact them.  While it won't
 harm the node to talk to it early, it may result in your commands not being successfully acted upon until complete
 bootup.  Note that sending a command to a node while it's in the 8-second delay period during bootup will cause the
 delay period to immediately end, and start-up to happen right away.
 
 */





