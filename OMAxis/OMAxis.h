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
    PLAN_INTERLEAVE = 0,
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

/** OMAxis provides the primary interface for controlling nanoMoCo devices on an AVR platform.
 
 Only one OMAxis instance is required to speak to any number of devices.  For most nanoMoCo-specific
 commands, you need only to specify the target() before issuing a command. Each command issued returns
 true or false indicating whether the remote device successfully received the command.
 
 The following provides a basic example of controlling a node via this class in an Arduino sketch:
 
 @code
 #include "OMMoCoBus.h"
 #include "OMMoCoMaster.h"
 #include "OMAxis.h"
 
  // RS-485 driver enable pin must be pin 5
 #define DE_PIN 5
 
 byte nodeAddr = 3;
 
 OMAxis axis = OMAxis(Serial, DE_PIN);
 
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
 
 @author Stanislav Pereplitsa, C.A. Church
 
 
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
