/*
 * SimpleDevMaster.cpp
 *
 *  Created on: 25.04.2012
 *      Author: perepelitsa
 */

#include "SimpleDevMaster.h"
#include "OMMoCoCommands.h"

//extern SimpleDevMaster g_simpleDevMaster;

SimpleDevMaster::SimpleDevMaster(HardwareSerial& c_serObj, uint8_t c_dePin)
  : OMMoCoMaster(c_serObj, c_dePin) {
	sendBuf.assign(sendBufStore, sizeof(sendBufStore));
}

SimpleDevMaster::~SimpleDevMaster() {
}

/**!
 * Command
 **/
bool SimpleDevMaster::cdInterval(uint16_t interval) {
	//
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_INTERVAL);
	sendBuf.append(interval);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cdExposure(uint32_t exposure) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_EXPOSURE);
	sendBuf.append(exposure);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cdFocus(uint16_t focus) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_FOCUS);
	sendBuf.append(focus);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cdMaxShots(uint16_t count) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_MAX_SHOTS);
	sendBuf.append(count);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cdExposureDelay(uint16_t delay) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_EXPOSURE_DLY);
	sendBuf.append(delay);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cdFocusWShutter(bool isOn) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_FOCUS_WSHUTTER);
	sendBuf.append(isOn);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cdRepeatCycles(uint8_t count) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_CD_REPEAT_CYCLES);
	sendBuf.append(count);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdStepsEachInterval(uint16_t steps) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_STEPS_EACH);
	sendBuf.append(steps);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdNOOP(void) {
 return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdDirection(bool dir) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_DIRECTION);
	sendBuf.append(dir);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdMaximumTravelSteps(uint32_t steps) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_MAX_TRAVEL_STEPS);
	sendBuf.append(steps);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdMotorEnable(void) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_MOTOR_ENABLE);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdMotorDisable(void) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_MOTOR_DISABLE);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdSetHomePositionHere(void) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_SET_HOME_HERE);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdBackslashAmount(uint8_t steps) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_BACKSLASH_AMO);
	sendBuf.append(steps);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdEasingMode(uint8_t mode) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_EASING_MODE);
	sendBuf.append(mode);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::mdContinuousMode(bool isOn) {
	sendBuf.reset();
	sendBuf.append((uint16_t) CMD_MD_CONTINUOUS_MODE);
	sendBuf.append(isOn);
	int res = command(slave_addr, OM_PCODE_PDS, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::nop() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_NOOP);
}

/**!
 * Command
 **/
bool SimpleDevMaster::start() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_START);
}

/**!
 * Command
 **/
bool SimpleDevMaster::pause() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_PAUSE);
}

/**!
 * Command
 **/
bool SimpleDevMaster::cameraEnable() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_CAM_ENABLE);
}

/**!
 * Command
 **/
bool SimpleDevMaster::cameraDisable() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_CAM_DISABLE);

}

/**!
 * Command
 **/
bool SimpleDevMaster::moveMotorSimple(bool dir, uint16_t steps) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_MOVE_MOTOR_SMPL);
	sendBuf.append(dir);
	sendBuf.append(steps);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::sendMotorHome() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_SEND_MOTOR_HOME);
}

/**!
 * Command
 **/
bool SimpleDevMaster::exposeNow(uint32_t exposure) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_EXPOSE_NOW);
	sendBuf.append(exposure);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::motorSleep(bool isSleep) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_MOTOR_SLEEP);
	sendBuf.append(isSleep);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::motorMaxStepRate(uint16_t stepsPerSecond) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_MOTOR_MAX_RATE);
	sendBuf.append(stepsPerSecond);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::motorMaxStepSpeed(uint16_t stepsPerSecond) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_MOTOR_MAX_SPEED);
	sendBuf.append(stepsPerSecond);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::nop1() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_NOOP);
}
/**!
 * Command
 **/
bool SimpleDevMaster::nop2() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_NOOP);
}

/**!
 * Command
 **/
bool SimpleDevMaster::nop3() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_NOOP);
}

bool SimpleDevMaster::debugLed(bool isOn) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_DEBUG_LED);
	sendBuf.append(isOn);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}

/**!
 * Command
 **/
bool SimpleDevMaster::stopMotorNow() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_STOP_MOTOR_NOW);
}

/**!
 * Command
 **/
bool SimpleDevMaster::microStepLevel(uint8_t level) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_MICROSTEP_LVL);
	sendBuf.append(level);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::timingMaster(bool isMaster) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_TIMING_MASTER);
	sendBuf.append(isMaster);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::moveMotorComplex(bool dir, uint32_t steps,
		uint32_t totalTravelTime, uint32_t accelTime, uint32_t decelTime) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_MOVE_MOTOR_CPLX);
	sendBuf.append(dir);
	sendBuf.append(steps);
	sendBuf.append(totalTravelTime);
	sendBuf.append(accelTime);
	sendBuf.append(decelTime);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::planInterleavedTravel(bool dir, uint32_t steps,
		uint32_t totalTravelIntervals, uint32_t accelIntervals,
		uint32_t decelIntervals) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_PLAN_INTER_TRAVEL);
	sendBuf.append(dir);
	sendBuf.append(steps);
	sendBuf.append(totalTravelIntervals);
	sendBuf.append(accelIntervals);
	sendBuf.append(decelIntervals);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(),
			sendBuf.size());
	if (res == 0)
		return true;
	else
		return false;
}
/**!
 * Command
 **/
bool SimpleDevMaster::cleanPlannedTravel() {
	return command(slave_addr, OM_PCODE_PC, (uint8_t) CMD_PC_CLEAR_PLAN_TRAVEL);
}

/**!
 * Command
 **/
bool SimpleDevMaster::statusRequest(uint8_t statusType, unsigned char* blob) {
	sendBuf.reset();
	sendBuf.append((uint8_t) CMD_PC_STATUS_REQ);
	sendBuf.append(statusType);
	int res = command(slave_addr, OM_PCODE_PC, sendBuf.getBuf(), sendBuf.size());
	if (res == 0) {
		memcpy(blob, buffer(), bufferLen());
		return true;
	}
	else
		return false;
}

